#include "TimeSeriesManager.h"
#include "Cairn_Exception.h"
#include "GlobalSettings.h"

TimeSeriesManager::TimeSeriesManager(MilpData& aMilpData, const std::string& a_ReaderKind)
    : r_MilpData(aMilpData)
{
    m_ReaderKind = a_ReaderKind;
}

TimeSeriesManager::~TimeSeriesManager()
{
    if (p_Reader)
        delete p_Reader;
}

void TimeSeriesManager::importTS(const QMap<QString, ZEVariables*>& aListSubscribedVariables)
{
    // Reader without files (Pegase)
    importTS({ "" }, aListSubscribedVariables);
}

void TimeSeriesManager::importTS(const QStringList& aTSfileList, const QMap<QString, ZEVariables*> &aListSubscribedVariables, const int& iShift, bool isCheckTimeSeriesUnits)
{
    //Generate a list of required input timeseries names
    QStringList listNotFoundNames = {};    

    QMapIterator<QString, ZEVariables*> iSubscribedVariable(aListSubscribedVariables);
    while (iSubscribedVariable.hasNext())
    {
        iSubscribedVariable.next();
        ZEVariables* var = iSubscribedVariable.value();
        if (!var->IsMPC())
        {   //Add to the list if it is not a controlled variable
            listNotFoundNames.push_back(var->Name());
        }
    }

    //Read all input timeseries files
    QListIterator<QString> vTSfile(aTSfileList);
    while (vTSfile.hasNext())
    {
        QString file = vTSfile.next();

        //import a timeseries file
        try {
            importTS(file, aListSubscribedVariables, iShift, listNotFoundNames, isCheckTimeSeriesUnits);
        }
        catch (Cairn_Exception cairn_error) {
            throw cairn_error;
        }
    }

    //Verify if all input timeseries are found
    if (listNotFoundNames.size() > 0) {
        QString errorMessage = "ERROR: The following input dataseries are misssing : ";
        for (int i = 0; i < listNotFoundNames.size(); i++) {
            if (i > 0) errorMessage += ", ";
            errorMessage += listNotFoundNames[i];
            Cairn_Exception cairn_error(errorMessage, -1);
            throw cairn_error;
        }
    }
}

void TimeSeriesManager::importTS(const QString& aTSfile, const QMap<QString, ZEVariables*>& aListSubscribedVariables, const int& iShift, QStringList& aListNotFoundNames, bool isCheckTimeSeriesUnits)
{
    if (p_Reader)
        delete p_Reader;
    
    p_Reader = TimeSeriesReader::NewReader(m_ReaderKind);

    struct SUnitErr {
        QString zeVar; QString dataUnit; QString zeUnit;
        SUnitErr(const QString& aVar, const QString& aUnit, const QString& aDataUnit)
        {
            zeVar = aVar; dataUnit = aDataUnit; zeUnit = aUnit;
        };
    };
    std::vector<SUnitErr> vUnitErrs;

    if (p_Reader->open(aTSfile)) {
        // read and analyze Times        
        std::vector<double> vTimes;
        readTimes(aTSfile, iShift, vTimes);

        // read Header (names, units)
        std::vector<TimeSeriesReader::TimeSeriesDescrp> vHeaders;
        p_Reader->readHeader(aListSubscribedVariables, vHeaders);
        
        //Read timeseries data        
        QMapIterator<QString, ZEVariables*> iSubscribedVariable(aListSubscribedVariables);        
        while (iSubscribedVariable.hasNext())
        {
            iSubscribedVariable.next();
            ZEVariables* var = iSubscribedVariable.value();
            QString zeVarName = var->Name();
            bool ifound = false;

            for (auto& vHeader : vHeaders) {    
                if (vHeader.Name == zeVarName) {
                    QString zeVarUnit = var->Unit();
                    OrCheckUnits checkUnits = CheckUnits(vHeader.Unit, zeVarUnit, true);
                    if (!checkUnits.isConsistency) {
                        vUnitErrs.push_back(SUnitErr(zeVarName, zeVarUnit, vHeader.Unit));
                    }
                    
                    // Read values
                    std::vector<double> vValues;
                    p_Reader->readValues(vHeader.Index, vValues);
                   
                    // Extrapolation
                    // Apply time shift
                    extrapolation(aTSfile, iShift, vHeader, vTimes, vValues);

                    // Conversion 
                    conversion(checkUnits, vValues);
                    
                    // Transform + Apply : vValues -> var                 
                    if (vTimes.size()) {
                        if (r_MilpData.readingMode() == "Interpolation")
                            importZEVarInterpolation(var, vValues, vTimes, iShift);
                        else
                            importZEVarAverage(var, vValues, vTimes, iShift);
                    }
                    else {
                        QVector<float>& vZEValues = *var->ptrVariable();                        
                        vZEValues.resize(vValues.size());
                        for (size_t i = 0;i < vValues.size(); i++) {
                            vZEValues.replace(i, vValues[i]);
                        }
                        // Average for multi timesteps: var                    
                        Average(var, r_MilpData.pdtHeure(), r_MilpData.TimeSteps(), r_MilpData.npdtPast());
                    }                                                            
                    ifound = true;
                    break;
                }
            }
            if (ifound) {
                //Remove zeVarName from list of not found names
                aListNotFoundNames.removeAll(zeVarName);
            }
        }
        p_Reader->close();
    }
    


    if (vUnitErrs.size()) {
        QString vErrMsg = "Error while importing: " + aTSfile;
        for (auto& vErr : vUnitErrs) {
            vErrMsg += "\n\nUnits inconsistency for the variable " + vErr.zeVar + ", the reading unit is " + vErr.dataUnit + ", the unit defined is " + vErr.zeUnit;
        }
        
        if (isCheckTimeSeriesUnits) {
            //Throw an Exception and stop the simulation
            Cairn_Exception cairn_error(vErrMsg, -1);
            throw cairn_error;
        }
        else {
            //Write a warnning in the log but continue the simulation
            qWarning() << vErrMsg;
        }
    }
}


void TimeSeriesManager::readTimes(const QString& aTSfile, const int& iShift, std::vector<double>& aTimes)
{
    aTimes.clear();

    //compute m_npdtFutur from TimeSteps
    m_rowShift = iShift;
    m_npdtFutur = 0;
    for (int i = 0; i < r_MilpData.TimeSteps().size(); i++) {
        m_npdtFutur += int(r_MilpData.TimeStep(i) / r_MilpData.TimeStep(0));
    }

    std::vector<double> vReadTimes;
    if (p_Reader->readTimes(vReadTimes)) {
        // times exists for this reader
        //Verify that the first time value is not 0
        if (vReadTimes[0] == 0) {
            Cairn_Exception cairn_error("Error while importing: " + aTSfile + "\n\nThe first time value cannot be 0; assuming that a variable value is 0 at time=0.", -1);
            throw cairn_error;
        }
        else if (vReadTimes[0] < 3600 * r_MilpData.TimeStep(0)) {
            qWarning() << "The first time value is " + QString::number(vReadTimes[0]) + " which is less than TimeStep=" + QString::number(3600 * r_MilpData.TimeStep(0)) + ".";
            qWarning() << "The first point is at time=TimeStep." + r_MilpData.readingMode() + "will be applied in this case.";

        }
        //Find the first row where time is greater than or equal to 3600 * r_MilpData.TimeStep(0) * iShift
    //The difference between time values inside .csv might not be equal to TimeStep    
        int k_periodic = 0;
        if (m_rowShift > 0) {
            int k = 0;
            //for (int i = 0; i < vReadTimes.size(); i++) {
            while (true) {
                if (fabs(vReadTimes[k] + k_periodic * vReadTimes[vReadTimes.size() - 1] - 3600 * r_MilpData.TimeStep(0) * iShift) < 10e-6)
                {
                    if (k == vReadTimes.size() - 1) {
                        k_periodic += 1;
                        m_rowShift = 0;
                    }
                    else
                        m_rowShift = k + 1;
                    break;
                }
                else if (vReadTimes[k] + k_periodic * vReadTimes[vReadTimes.size() - 1] > 3600 * r_MilpData.TimeStep(0) * iShift)
                {
                    m_rowShift = k;
                    if (k == vReadTimes.size() - 1)
                        k_periodic += 1;
                    break;
                }
                else if (k == vReadTimes.size() - 1) {
                    if (r_MilpData.rollingMode() == "Periodic" || r_MilpData.rollingMode() == "Persistent") {
                        if (k_periodic == 0) {
                            qWarning() << "Importing: " + aTSfile;
                            qWarning() << "The TimeShift used is beyond the Time values! Reading recursively - Rolling Mode is : " + r_MilpData.rollingMode();
                            qDebug() << "Number of lines = " << vReadTimes.size() << ", current TimeShift in TimeStep = " << iShift;
                        }
                        k = 0;
                        k_periodic += 1;
                        continue;
                    }
                    else {
                        Cairn_Exception cairn_error("Error while importing: " + aTSfile + "\n\nThe TimeShift used is beyond the Time values! Rolling Mode is : " + r_MilpData.rollingMode(), -1);
                        qDebug() << "Number of lines = " << vReadTimes.size() << ", FutureSize in TimeStep = " << m_npdtFutur << ", current TimeShift in TimeStep = " << iShift << "TimeStep = " << r_MilpData.TimeStep(0);
                        throw cairn_error;
                    }
                }
                k += 1;
            }
        }

        //Prepare time vector and apply TimeShift     
        double vTimeStep = r_MilpData.TimeStep(0);
        double time = 3600.0 * iShift * vTimeStep;
        int l = 0;
        int r = 0;
        double delta = 0.0;
        //Add points until time == (m_npdtFutur + iShift)*TimeStep.
        while (time < 3600 * vTimeStep * (iShift + m_npdtFutur)) {
            if (l + m_rowShift < vReadTimes.size())
                aTimes.push_back(k_periodic * vReadTimes[vReadTimes.size() - 1] + vReadTimes[l + m_rowShift]);
            else {
                if (r_MilpData.rollingMode() == "Periodic" || r_MilpData.rollingMode() == "Persistent") {//loop from the beginning not timeshift
                    if (r > vReadTimes.size() - 1) {
                        r = 0;
                    }
                    if (r > 0)
                        delta = vReadTimes[r] - vReadTimes[r - 1];
                    else
                        delta = vReadTimes[0];
                    if (aTimes.size() > 0)
                        aTimes.push_back(aTimes[aTimes.size() - 1] + delta);
                    else
                        aTimes.push_back(k_periodic * vReadTimes[vReadTimes.size() - 1] + delta);
                    r += 1;
                }
                else
                {
                    Cairn_Exception cairn_error("Error while importing: " + aTSfile + "\n\nThe number of lines for Time column is not enough!", -1);
                    qDebug() << "Number of lines = " << vReadTimes.size() << ", FutureSize in TimeStep = " << m_npdtFutur << ", current TimeShift in TimeStep = " << iShift << "TimeStep = " << r_MilpData.TimeStep(0);
                    throw cairn_error;
                }
            }

            if (isnan(aTimes[l])) {
                Cairn_Exception cairn_error("Error while importing: " + aTSfile + "\n\nThe time value at line " + QString::number(p_Reader->getNumLine(l + 1)) + " is NAN!", -1);
                throw cairn_error;
            }

            //Verify that time is strictly increasing
            if (l == 0) {
                if (aTimes[l] < 0) {
                    Cairn_Exception cairn_error("Error while importing: " + aTSfile + "\n\nThe Time value at line " + QString::number(p_Reader->getNumLine(0)) + " is negative (first data line)!", -1);
                    throw cairn_error;
                }
            }
            else {
                if (aTimes[l] <= aTimes[l - 1]) {
                    Cairn_Exception cairn_error("Error while importing: " + aTSfile + "\n\nThe Time value at line " + QString::number(p_Reader->getNumLine(l + 1)) + " is less than or equal to a previous value.", -1);
                    throw cairn_error;
                }
            }

            time = aTimes[l];
            l += 1;
        }
    }
}

void TimeSeriesManager::extrapolation(const QString& aTSfile, const int& iShift, const TimeSeriesReader::TimeSeriesDescrp& aHeader, const std::vector<double>& aTimes, std::vector<double>& aValues)
{
    if (aTimes.size()) {
        std::vector<double> vValues(aValues.size());
        vValues.assign(aValues.begin(), aValues.end());
        aValues.resize(aTimes.size());

        int r = 0;
        for (int i = 0; i < aValues.size(); i++) {
            if (i + m_rowShift < vValues.size()) {
                if (m_rowShift)
                    aValues[i] = vValues[i + m_rowShift];
            }
            else {
                if (i + m_rowShift == vValues.size()) {
                    qWarning() << "Last point for " + aHeader.Name + " has been reached!";
                    qInfo() << r_MilpData.rollingMode() + " mode will be applied for " + aHeader.Name;
                }
                if (r_MilpData.rollingMode() == "Periodic") {//loop from the beginning not timeshift
                    if (r > vValues.size() - 1) {
                        r = 0;
                    }
                    aValues[i] = vValues[r];
                    r += 1;
                }
                else if (r_MilpData.rollingMode() == "Persistent")
                {
                    if (i > 0)
                        aValues[i] = aValues[i - 1];
                    else
                        //This happens when vValues size is exactly equal to mNpdt and mNpdt == mNpdtPast == mTimeShift
                        //Not ideal: Should save the last aValues value from the previous cycle (e.g. if average with timeStep != 3600 is used)
                        aValues[i] = vValues[vValues.size() - 1];
                }
                else // if (rollingMode() == "Stop")
                {
                    Cairn_Exception cairn_error("Error while importing: " + aTSfile + "\n\nThe number of lines for variable " + aHeader.Name + " in the input CSV file is not enough!", -1);
                    qDebug() << "Number of lines = " << vValues.size() << ", FutureSize in TimeStep = " << m_npdtFutur << ", current TimeShift in TimeStep = " << iShift << "TimeStep = " << r_MilpData.TimeStep(0);
                    throw cairn_error;
                }
            }

            if (isnan(aValues[i])) {
                Cairn_Exception cairn_error("Error while importing: " + aTSfile + "\n\nThe value of " + aHeader.Name + " at index " + QString::number(i) + " is NAN!", -1);
                throw cairn_error;
            }
        }
    }    
}

void TimeSeriesManager::conversion(const OrCheckUnits& checkUnits, std::vector<double>& aValues)
{    
    if (!checkUnits.isSame) {
        OrUnitsConverter::OrDefUnit vSrcUnit(checkUnits.keyUnit1);
        OrUnitsConverter::OrDefUnit vDestUnit(checkUnits.keyUnit2);

        // conversion
        for (size_t i=0;i<aValues.size();i++)
            aValues[i] = UnitsConverter::Convert(aValues[i], vSrcUnit, vDestUnit);
    }
}

OrCheckUnits TimeSeriesManager::CheckUnits(const QString& a_FileUnit, const QString& a_Units, bool a_Check)
{
    OrCheckUnits vRet;
    if (a_Check && (a_FileUnit != a_Units)) {
        OrUnitsConverter::OrDefUnit vUnit1(a_FileUnit.toStdString());
        if (a_Units.contains(";")) {
            QStringList listUnit = a_Units.split(";", QString::SkipEmptyParts);
            for (auto& vUnit : listUnit) {
                UnitsConverter::CheckUnits(vUnit1, OrUnitsConverter::OrDefUnit(vUnit.toStdString()), &vRet);
                if (vRet.isConsistency)
                    break;
            }
        }
        else if (a_Units == "") {
            UnitsConverter::CheckUnits(vUnit1, OrUnitsConverter::OrDefUnit("-"), &vRet);
        }
        else {
            UnitsConverter::CheckUnits(vUnit1, OrUnitsConverter::OrDefUnit(a_Units.toStdString()), &vRet);
        }
    }
    else {
        vRet.isSame = true;
        vRet.isConsistency = true;
    }

    return vRet;
}

void TimeSeriesManager::importZEVarInterpolation(ZEVariables* var, std::vector<double> aVec, std::vector<double> pdtVec, const int& iShift)
{
    double time = 3600 * (iShift + 1) * r_MilpData.TimeStep(0);
    int iRow = 0;

    for (size_t j = r_MilpData.npdtPast(); j < r_MilpData.npdtTot(); j++)
    {
        while (iRow < pdtVec.size() - 1)
        {
            if (fabs(time - pdtVec[iRow]) < 10e-6 || time < pdtVec[iRow])
                break;
            iRow++;
        }

        if (fabs(time - pdtVec[iRow]) < 10e-6) {
            var->ptrVariable()->replace(j, aVec[iRow]);
        }
        else {
            double interVal = 0.0;
            if (iRow == 0)//case where first time value in .csv is greater than TimeStep
                interVal = aVec[iRow] * time / pdtVec[iRow];
            else
                interVal = ((aVec[iRow] - aVec[iRow - 1]) * (time - pdtVec[iRow - 1]) / (pdtVec[iRow] - pdtVec[iRow - 1])) + aVec[iRow - 1];

            if (isnan(interVal)) {
                Cairn_Exception cairn_error("Error while importing the input data series: NAN value found for " + var->Name() + " at time " + QString::number(time) + ", row: " + QString::number(iRow), -1);
                throw cairn_error;
            }

            //Values for variables that are not temperature or price cannot be negative 
            if (interVal < 0)
            {
                if (var->Unit().toLower() == "degc" || var->Unit().toLower() == "degk" || var->Unit().toLower() == "k"
                    || var->Unit().toLower().contains("eur") || var->Unit().toLower().contains("currency"))
                {
                    //temperature or price : do nothing
                }
                else if (fabs(interVal) < 1.e-5) //Correction for negligible negative values that might result from sum computation 
                    interVal = 0.0;
                else
                    qDebug() << " ABNORMAL NEGATIVE VALUE !! " << var->Name() << var->Unit() << iRow << time << pdtVec[iRow] << interVal << aVec[iRow];
            }

            var->ptrVariable()->replace(j, interVal);
        }

        time = time + 3600. * (r_MilpData.TimeStep(j - r_MilpData.npdtPast()));
    }
}

void TimeSeriesManager::importZEVarAverage(ZEVariables* var, std::vector<double> aVec, std::vector<double> pdtVec, const int& iShift)
{
    double time = 3600 * iShift * r_MilpData.TimeStep(0);
    int iRow = 0;
    int previRow = -1;

    double sumValue = 0.0;
    double sumTime = 0.0;
    double valInterp = 0.0;

    for (size_t j = r_MilpData.npdtPast(); j < r_MilpData.npdtTot(); j++)
    {
        time = time + 3600. * (r_MilpData.TimeStep(j - r_MilpData.npdtPast()));

        while (iRow < pdtVec.size() - 1) // pdtVec.size() == npdtTot() - npdtPast()
        {
            if (fabs(time - pdtVec[iRow]) < 10e-6 || time < pdtVec[iRow]) {
                // resynchro due to loss of precision
                if (previRow > 0) //Is this correction really needed?!
                    if (time <= pdtVec[previRow])
                        time = pdtVec[previRow];
                break;
            }

            if (iRow > 0) {
                sumTime += (pdtVec[iRow] - pdtVec[iRow - 1]);
                sumValue += aVec[iRow] * (pdtVec[iRow] - pdtVec[iRow - 1]);
            }
            else { //iRow == 0
                sumTime += pdtVec[iRow] - 3600 * iShift * r_MilpData.TimeStep(0);
                sumValue += aVec[iRow] * (pdtVec[iRow] - 3600 * iShift * r_MilpData.TimeStep(0));
                if (sumTime < 0) {
                    Cairn_Exception cairn_error("Error while importing input time series.\n\nNegative time value! Something went wrong!", -1);
                    throw cairn_error;
                }
            }

            iRow++;
        }

        //Don't take average value for temperature and price
        if (var->Unit().toLower() == "degc" || var->Unit().toLower() == "degk" || var->Unit().toLower() == "k"
            || var->Unit().toLower().contains("eur") || var->Unit().toLower().contains("currency"))
        {
            if (time - pdtVec[iRow] < 10e-6) {
                var->ptrVariable()->replace(j, aVec[iRow]);
            }
            else {
                double sval = 0.0;
                if (iRow == 0)//case where first time value in .csv is greater than TimeStep
                    sval = aVec[iRow] * time / pdtVec[iRow];
                else
                    sval = ((aVec[iRow] - aVec[iRow - 1]) * (time - pdtVec[iRow - 1]) / (pdtVec[iRow] - pdtVec[iRow - 1])) + aVec[iRow - 1];

                if (isnan(sval)) {
                    Cairn_Exception cairn_error("Error while importing the input data series: NAN value found for " + var->Name() + " at time " + QString::number(time) + ", row: " + QString::number(iRow), -1);
                    throw cairn_error;
                }

                var->ptrVariable()->replace(j, sval);
            }

            if (iRow < pdtVec.size() - 1) iRow++;
        }
        else // Not temperature or price
        {
            if (iRow == previRow + 1)  //To have a better precision for the basic case
            {
                if (fabs(time - pdtVec[iRow]) < 10e-6) {
                    var->ptrVariable()->replace(j, aVec[iRow]);
                }
                else {
                    double sval = 0.0;
                    if (iRow == 0)//case where first time value in .csv is greater than TimeStep
                        sval = aVec[iRow] * time / pdtVec[iRow];
                    else
                        sval = ((aVec[iRow] - aVec[iRow - 1]) * (time - pdtVec[iRow - 1]) / (pdtVec[iRow] - pdtVec[iRow - 1])) + aVec[iRow - 1];

                    if (isnan(sval)) {
                        Cairn_Exception cairn_error("Error while importing the input data series: NAN value found for " + var->Name() + " at time " + QString::number(time) + ", row: " + QString::number(iRow), -1);
                        throw cairn_error;
                    }

                    //Values for variables that are not temperature or price cannot be negative 
                    if (sval < 0)
                    {
                        //Correction for negligible negative values that might result from sum computation 
                        if (fabs(sval) < 1.e-5)
                            sval = 0.0;
                        else
                            qDebug() << " ABNORMAL NEGATIVE VALUE !! " << var->Name() << var->Unit() << iRow << time << pdtVec[iRow] << sval << aVec[iRow];
                    }

                    var->ptrVariable()->replace(j, sval);
                }

                sumTime = 0.0;
                sumValue = 0.0;

                if (iRow < pdtVec.size() - 1) {
                    previRow = iRow;
                    iRow++;
                }
            }
            else
            {
                valInterp = ((aVec[iRow] - aVec[iRow - 1]) * (time - pdtVec[iRow - 1]) / (pdtVec[iRow] - pdtVec[iRow - 1])) + aVec[iRow - 1];
                //Do we really need this?!
                if (fabs(time - pdtVec[iRow - 1]) < 1.e-6) {
                    time = pdtVec[iRow - 1];
                }

                sumTime += (time - pdtVec[iRow - 1]);
                sumValue += (time - pdtVec[iRow - 1]) * valInterp;

                if (isnan(sumValue / sumTime)) {
                    Cairn_Exception cairn_error("Error while importing the input data series: NAN value found for " + var->Name() + " at time " + QString::number(time) + ", row: " + QString::number(iRow), -1);
                    throw cairn_error;
                }

                //Values for variables that are not temperature or price cannot be negative 
                if (sumValue < 0)
                {
                    //Correction for negligible negative values that might result from sum computation 
                    if (fabs(sumValue / sumTime) < 1.e-5)
                        sumValue = 0.0;
                    else
                        qDebug() << " ABNORMAL NEGATIVE VALUE !! " << var->Name() << var->Unit() << iRow << time << pdtVec[iRow] << (sumValue / sumTime) << aVec[iRow];
                }

                var->ptrVariable()->replace(j, (sumValue / sumTime));

                sumTime = (pdtVec[iRow] - time);
                sumValue = (pdtVec[iRow] - time) * valInterp;

                if (iRow < pdtVec.size() - 1) {
                    previRow = iRow;
                    iRow++;
                }
            }
        }
    }
}


void TimeSeriesManager::Average(ZEVariables* var, double aTimeStepIn, const std::vector<double> &aTimeStepsOut, uint aNpdtPast)
{
    var->IsExt(true);
    QVector<float>& vFineIn = *var->ptrVariable();
    const uint64_t aSizeFine = vFineIn.size(); //pastSize+futurSize
    const uint64_t aSizeCoarse = aTimeStepsOut.size() + aNpdtPast; //pastSize+ComputationfuturSize

    std::vector<double> TimeStepsIn(aSizeFine);
    std::vector<double> localIn(aSizeFine);
    double tmpFine = 0.;
    double tmpCoarse = 0.;

    if (aSizeFine < aSizeCoarse)
    {
        qCritical() << "aSizeFine = " << aSizeFine;
        qCritical() << "aSizeCoarse = " << aSizeCoarse;
        qCritical() << "ANOMALIE ! " << aTimeStepIn << aNpdtPast;
    }
    QVector<float>& vCoarseOut = *var->ptrOutVariable();            
    vCoarseOut.fill(0.0, aSizeCoarse);  // raz Out  
    
    if (aSizeFine == 0)
    {
        qCritical() << "Abnormal missing allocation aSizeFine = " << aSizeFine;        
        return;
    }
    TimeStepsIn.assign(aSizeFine, aTimeStepIn);

    uint64_t icoarse = 0;
    double dt = 0.;

    //initialize past.
    for (uint64_t ifine = 0; ifine < aNpdtPast; ifine++)
    {
        vCoarseOut[icoarse] += vFineIn[ifine];
        localIn[ifine] = vFineIn[ifine];
        icoarse++;
    }
    for (uint64_t ifine = aNpdtPast; ifine < aSizeFine; ifine++)
    {
        if (dt <= aTimeStepsOut[icoarse - aNpdtPast])  // dt est la periode de moyenne, composee d'un nombre entier de pas de temps
        {
            dt += TimeStepsIn[ifine];
            vCoarseOut[icoarse] += vFineIn[ifine] * TimeStepsIn[ifine];
            localIn[ifine] = vFineIn[ifine];
            tmpFine += TimeStepsIn[ifine];
            //qDebug() << "vFineIn ifine " << ifine << aSizeFine << vFineIn[ifine];
        }
        if (dt >= aTimeStepsOut[icoarse - aNpdtPast])
        {
            vCoarseOut[icoarse] = vCoarseOut[icoarse] / dt;
            //qDebug() << "vCoarseOut icoarse " << icoarse << aSizeCoarse << vCoarseOut[icoarse];
            tmpCoarse += aTimeStepsOut[icoarse - aNpdtPast];
            dt = 0.;
            icoarse++;
        }
    }
    if (tmpFine != tmpCoarse || icoarse != aSizeCoarse)
    {
        qCritical() << "Bad timesteps definition : the sum of coarse timesteps " << tmpCoarse << " must be equal to the sum of constant fine timestep " << tmpFine;
        qCritical() << "Bad timesteps definition : icoarse " << icoarse << " must be equal to aSizeCoarse " << aSizeCoarse;

        vCoarseOut[aSizeCoarse - 1] = vCoarseOut[aSizeCoarse - 1] / aTimeStepsOut[aTimeStepsOut.size() - 1];
        qCritical() << "Final value of coarse timeseries will be biased : " << aSizeCoarse << vCoarseOut[aSizeCoarse - 1];
    }
}