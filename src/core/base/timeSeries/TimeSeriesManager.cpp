#include "TimeSeriesManager.h"
#include "Cairn_Exception.h"
#include "CairnUtils.h"
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

void TimeSeriesManager::addTS(const std::wstring& a_fileName)
{
    // si filename = "" , efface la liste	
    if (a_fileName == L"") {
        m_TSfileList.clear();
    }
    else {
        // ajoute un fichier time series dans la liste des fichiers
        m_TSfileList.push_back(a_fileName);
    }    
}

void TimeSeriesManager::addTS(const t_dict& a_TS)
{
    m_TSValues.addTS(a_TS);
}

bool TimeSeriesManager::checkTS(string& a_ErrMsg)
{
    bool vRet = true;
    // Verify timeseries files
    for (auto& vFileName : m_TSfileList) {
        fs::path vTimeStepFile(vFileName);
        std::error_code vErrFile;
        if (!fs::exists(vTimeStepFile, vErrFile)) {
            const std::string strTSfile = CairnUtils::toUTF8String(vFileName);
            a_ErrMsg =  "Error : timeseries file: " + strTSfile + " does not exist";
            vRet = false;
        }
    }
    if (vRet) {
        vRet = m_TSValues.checkTS(a_ErrMsg);
        if (!vRet && a_ErrMsg == "") {
            // no TS in m_TSValues
            if (!m_TSfileList.size()) {
                // no files
                a_ErrMsg = "Please, defines timeseries before running";
            }
            else
                vRet = true;
        }
    }    
    return vRet;
}

void TimeSeriesManager::clearTS()
{
    addTS(L"");
    m_TSValues.addTS({});
}

void TimeSeriesManager::importTS(const std::vector<std::wstring>& aTSfileList, const t_mapExchange& aListSubscribedVariables, bool isCoSim, const int& iShift, bool isCheckTimeSeriesUnits)
{
    addTS(L"");
    for (auto& vFile : aTSfileList)
        addTS(vFile);
    importTS(aListSubscribedVariables, isCoSim, iShift, isCheckTimeSeriesUnits);
}

void TimeSeriesManager::importTS(const t_mapExchange& aListSubscribedVariables,
    bool isCoSim, const int& iShift, bool isCheckTimeSeriesUnits)
{
    // Collect names of non-MPC variables 
    std::unordered_set<std::string> notFoundNames;
    for (auto& [key, var] : aListSubscribedVariables) {
        if (!var->IsMPC())
            notFoundNames.insert(var->Name());
    }

    if (!isCoSim) {
        std::string errMsg;
        if (!checkTS(errMsg))
            throw Cairn_Exception(errMsg, -1);
    }

    bool anyFound = false; // at least one timeseries (file) found! 

    // import timeseries from input files
    for (auto& vFile : m_TSfileList) {
        // TODO: use unique_ptr ?
        delete p_Reader;
        p_Reader = TimeSeriesReader::NewReader(m_ReaderKind);
        anyFound |= importTS(*p_Reader, vFile, aListSubscribedVariables, iShift, notFoundNames, isCheckTimeSeriesUnits);
    }

    // read the values of added timeseries; if any 
    anyFound |= importTS(m_TSValues, L"Time series values", aListSubscribedVariables, iShift, notFoundNames, isCheckTimeSeriesUnits);

    if (!isCoSim && !anyFound)
        throw Cairn_Exception("Error: at least one timeseries must be provided!", -1);

    if (!notFoundNames.empty()) {
        std::string errorMessage = "ERROR: The following input dataseries are missing: ";
        bool first = true;
        for (auto& name : notFoundNames) {
            if (!first) errorMessage += ", ";
            errorMessage += name;
            first = false;
        }
        throw Cairn_Exception(errorMessage, -1);
    }
}

bool TimeSeriesManager::importTS(TimeSeriesReader& a_Reader, const std::wstring& aTSfile,
    const t_mapExchange& aListSubscribedVariables, const int& iShift,
    std::unordered_set<std::string>& aNotFoundNames, bool isCheckTimeSeriesUnits)
{
    struct SUnitErr {
        std::string zeVar, dataUnit, zeUnit;
    };

    const std::string strTSfile = CairnUtils::toUTF8String(aTSfile);

    if (!a_Reader.open(aTSfile)) {
        cError() << "Timeseries file not found: " << strTSfile;
        return false;
    }

    // read and analyze Times 
    std::vector<double> vTimes;
    readTimes(a_Reader, aTSfile, iShift, vTimes);

    // read Header (names, units)
    std::vector<TimeSeriesReader::TimeSeriesDescrp> vHeaders;
    a_Reader.readHeader(aListSubscribedVariables, vHeaders);

    // read timeseries data 
    std::vector<SUnitErr> vUnitErrs;

    for (auto& [key, var] : aListSubscribedVariables) {
        const std::string zeVarName = var->Name();

        auto headerIt = std::find_if(vHeaders.begin(), vHeaders.end(),
            [&](const TimeSeriesReader::TimeSeriesDescrp& h) {
                return CairnUtils::compareStrings(h.Name, zeVarName);
            });

        // timeseries name was not found; continue, as it may be in another file
        if (headerIt == vHeaders.end())
            continue; 

        // timeseries name found
        const auto& vHeader = *headerIt;  
        const std::string zeVarUnit = var->Unit();
        OrCheckUnits checkUnits = CheckUnits(vHeader.Unit, zeVarUnit, true);

        if (!checkUnits.isConsistency)
            vUnitErrs.push_back({ zeVarName, vHeader.Unit, zeVarUnit });

        std::vector<double> vValues;
        a_Reader.readValues(vHeader.Index, vValues);

        extrapolation(aTSfile, iShift, vHeader, vTimes, vValues);
        conversion(checkUnits, vValues);

        if (!vTimes.empty()) {
            if (r_MilpData.readingMode() == "Interpolation")
                importZEVarInterpolation(var, vValues, vTimes, iShift);
            else
                importZEVarAverage(var, vValues, vTimes, iShift);
        }
        else {
            Average(var, r_MilpData.pdtHeure(), r_MilpData.TimeSteps(), r_MilpData.npdtPast());
        }

        aNotFoundNames.erase(zeVarName); 
    }

    a_Reader.close();

    if (!vUnitErrs.empty()) {
        std::string errMsg = "Error while importing: " + strTSfile;
        for (auto& e : vUnitErrs)
            errMsg += "\n\nUnits inconsistency for variable " + e.zeVar
            + ", file unit: " + e.dataUnit
            + ", expected unit: " + e.zeUnit;

        if (isCheckTimeSeriesUnits)
            throw Cairn_Exception(errMsg, -1);
        else
            cInfo() << errMsg;
    }

    return true;
}

void TimeSeriesManager::readTimes(TimeSeriesReader& a_Reader, const std::wstring& aTSfile, const int& iShift, std::vector<double>& aTimes)
{
    aTimes.clear();

    //compute m_npdtFutur from TimeSteps
    m_rowShift = iShift;
    m_npdtFutur = 0;
    for (int i = 0; i < r_MilpData.TimeSteps().size(); i++) {
        m_npdtFutur += int(r_MilpData.TimeStep(i) / r_MilpData.pdtHeure());
    }

    const std::string strTSfile = CairnUtils::toUTF8String(aTSfile);

    std::vector<double> vReadTimes;
    const double startTime = r_MilpData.pdt() * r_MilpData.startTime(); //in seconds
    const double endTime = (r_MilpData.endTime() > 0) ? r_MilpData.pdt() * r_MilpData.endTime() : -1.0; //in seconds
    if (a_Reader.readTimes(vReadTimes, startTime, endTime, r_MilpData.pdt())) {
        // times exists for this reader
        //Verify that the first time value is not 0
        if (vReadTimes[0] == 0) {
            Cairn_Exception cairn_error("Error while importing: " + strTSfile + "\n\nThe first time value cannot be 0; assuming that a variable value is 0 at time=0.", -1);
            throw cairn_error;
        }
        else if (vReadTimes[0] < r_MilpData.pdt()) {
            cWarning() << "The first time value is " << vReadTimes[0] << " which is less than TimeStep=" << r_MilpData.pdt() << ". " 
                       << "The first point is at time=TimeStep." + r_MilpData.readingMode() + "will be applied in this case.";
        }

        //Find the first row where time is greater than or equal to r_MilpData.pdt() * iShift
        //The difference between time values inside .csv might not be equal to TimeStep    
        int k_periodic = 0;
        if (m_rowShift > 0) {
            int k = 0;
            //for (int i = 0; i < vReadTimes.size(); i++) {
            while (true) {
                if (fabs(vReadTimes[k] + k_periodic * vReadTimes[vReadTimes.size() - 1] - r_MilpData.pdt() * iShift) < 10e-6)
                {
                    if (k == vReadTimes.size() - 1) {
                        k_periodic += 1;
                        m_rowShift = 0;
                    }
                    else
                        m_rowShift = k + 1;
                    break;
                }
                else if (vReadTimes[k] + k_periodic * vReadTimes[vReadTimes.size() - 1] > r_MilpData.pdt() * iShift)
                {
                    m_rowShift = k;
                    if (k == vReadTimes.size() - 1)
                        k_periodic += 1;
                    break;
                }
                else if (k == vReadTimes.size() - 1) {
                    if (r_MilpData.rollingMode() == "Periodic" || r_MilpData.rollingMode() == "Persistent") {
                        if (k_periodic == 0) {
                            cInfo() << "Importing: " + strTSfile << ". "
                                       << "The TimeShift used is beyond the Time values! " 
                                       << "Reading recursively - Rolling Mode is: " + r_MilpData.rollingMode();
                            cDebug() << "Number of lines = " << vReadTimes.size() << ", current TimeShift in TimeStep = " << iShift;
                        }
                        k = 0;
                        k_periodic += 1;
                        continue;
                    }
                    else {
                        Cairn_Exception cairn_error("Error while importing: " + strTSfile + "\n\nThe TimeShift used is beyond the Time values! Rolling Mode is : " + r_MilpData.rollingMode(), -1);
                        cDebug() << "Number of lines = " << vReadTimes.size() << ", FutureSize in TimeStep = " << m_npdtFutur << ", current TimeShift in TimeStep = " << iShift << "TimeStep = " << r_MilpData.pdtHeure();
                        throw cairn_error;
                    }
                }
                k += 1;
            }
        }

        //Prepare time vector and apply TimeShift     
        double time = iShift * r_MilpData.pdt();
        int l = 0;
        int r = 0;
        double delta = 0.0;
        //Add points until time == (m_npdtFutur + iShift)*TimeStep.
        while (time < r_MilpData.pdt() * (iShift + m_npdtFutur)) {
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
                    Cairn_Exception cairn_error("Error while importing: " + strTSfile + "\n\nThe number of lines for Time column is not enough!", -1);
                    cDebug() << "Number of lines = " << vReadTimes.size() << ", FutureSize in TimeStep = " << m_npdtFutur << ", current TimeShift in TimeStep = " << iShift << "TimeStep = " << r_MilpData.pdt();
                    throw cairn_error;
                }
            }

            if (std::isnan(aTimes[l])) {
                Cairn_Exception cairn_error("Error while importing: " + strTSfile + "\n\nThe time value at line " + std::to_string(a_Reader.getNumLine(l + 1)) + " is NAN!", -1);
                throw cairn_error;
            }

            //Verify that time is strictly increasing
            if (l == 0) {
                if (aTimes[l] < 0) {
                    Cairn_Exception cairn_error("Error while importing: " + strTSfile + "\n\nThe Time value at line " + std::to_string(a_Reader.getNumLine(0)) + " is negative (first data line)!", -1);
                    throw cairn_error;
                }
            }
            else {
                if (aTimes[l] <= aTimes[l - 1]) {
                    Cairn_Exception cairn_error("Error while importing: " + strTSfile + "\n\nThe Time value at line " + std::to_string(a_Reader.getNumLine(l + 1)) + " is less than or equal to a previous value.", -1);
                    throw cairn_error;
                }
            }

            time = aTimes[l];
            l += 1;
        }
    }
}

void TimeSeriesManager::extrapolation(const std::wstring& aTSfile, const int& iShift, const TimeSeriesReader::TimeSeriesDescrp& aHeader, const std::vector<double>& aTimes, std::vector<double>& aValues)
{
    const std::string strTSfile = CairnUtils::toUTF8String(aTSfile);

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
                    cInfo() << "Last point for " + aHeader.Name + " has been reached!";
                    cInfo() << r_MilpData.rollingMode() + " mode will be applied for " + aHeader.Name;
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
                    Cairn_Exception cairn_error("Error while importing: " + strTSfile + "\n\nThe number of lines for variable " + aHeader.Name + " in the input CSV file is not enough!", -1);
                    cDebug() << "Number of lines = " << vValues.size() << ", FutureSize in TimeStep = " << m_npdtFutur << ", current TimeShift in TimeStep = " << iShift << "TimeStep = " << r_MilpData.pdt();
                    throw cairn_error;
                }
            }

            if (std::isnan(aValues[i])) {
                Cairn_Exception cairn_error("Error while importing: " + strTSfile + "\n\nThe value of " + aHeader.Name + " at index " + std::to_string(i) + " is NAN!", -1);
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
        for (size_t i = 0; i < aValues.size(); i++) {
            aValues[i] = UnitsConverter::Convert(aValues[i], vSrcUnit, vDestUnit);
        }
    }
}

OrCheckUnits TimeSeriesManager::CheckUnitConsistency(const std::string& a_FileUnit, const std::string& a_Unit, bool a_Check)
{
    OrCheckUnits vRet;
    if (a_Check && (a_FileUnit != a_Unit)) 
    {
        // Check first if there is a possible conversion for the entire unit
        UnitsConverter::CheckUnits(OrUnitsConverter::OrDefUnit(a_FileUnit), OrUnitsConverter::OrDefUnit(a_Unit), &vRet);
        if (vRet.isSame || vRet.isConsistency) {
            return vRet;
        }

        // Is it a concatenation of units ?
        if (CairnUtils::contains(a_Unit, "/")) {
            // All concatenated units should be the same one by one
            t_list listFileUnit = CairnUtils::split(a_FileUnit, '/');
            t_list listUnit = CairnUtils::split(a_Unit, '/');
            if (listFileUnit.size() == listUnit.size()) {
                for (size_t i = 0; i < listUnit.size(); i++) {
                    OrUnitsConverter::OrDefUnit vUnitPart1(listFileUnit[i]);
                    OrUnitsConverter::OrDefUnit vUnitPart2(listUnit[i]);
                    UnitsConverter::CheckUnits(vUnitPart1, vUnitPart2, &vRet);
                    if (!vRet.isSame) {
                        vRet.isConsistency = false;
                        break; // if there is a unit that is not the same (identical or an alias) => stop!
                    }
                }
            }
            else {
                vRet.isSame = false;
                vRet.isConsistency = false;
            }
        }
    }
    else {
        vRet.isSame = true;
        vRet.isConsistency = true;
    }
    return vRet;
}


OrCheckUnits TimeSeriesManager::CheckUnits(const std::string& a_FileUnit, const std::string& a_Units, bool a_Check)
{
    OrCheckUnits vRet;
    if (a_Check && (a_FileUnit != a_Units)) {
        // Several units (separated by ;) are possible
        if (CairnUtils::contains(a_Units, ";")) {
            t_list listUnit = CairnUtils::split(a_Units, ';');
            for (auto& vUnit : listUnit) {
                vRet = CheckUnitConsistency(a_FileUnit, vUnit, a_Check);
                if (vRet.isConsistency)
                    break;
            }
        }
        else if (a_Units == "") {
            UnitsConverter::CheckUnits(OrUnitsConverter::OrDefUnit(a_FileUnit), OrUnitsConverter::OrDefUnit("-"), &vRet);
        }
        else {
            vRet = CheckUnitConsistency(a_FileUnit, a_Units, a_Check);
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
    double time = (iShift + 1) * r_MilpData.pdt();
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
            (*var->ptrVariable())[j] =  aVec[iRow];
        }
        else {
            double interVal = 0.0;
            if (iRow == 0)//case where first time value in .csv is greater than TimeStep
                interVal = aVec[iRow] * time / pdtVec[iRow];
            else
                interVal = ((aVec[iRow] - aVec[iRow - 1]) * (time - pdtVec[iRow - 1]) / (pdtVec[iRow] - pdtVec[iRow - 1])) + aVec[iRow - 1];

            if (std::isnan(interVal)) {
                Cairn_Exception cairn_error("Error while importing the input data series: NAN value found for " + var->Name() + " at time " + std::to_string(time) + ", row: " + std::to_string(iRow), -1);
                throw cairn_error;
            }

            //Values for variables that are not temperature or price cannot be negative 
            if (interVal < 0)
            {
                std::string vUpperUnit = CairnUtils::toUpper(var->Unit());
                if (vUpperUnit == "DEGC" || vUpperUnit == "DEGK" || vUpperUnit == "K"
                    || CairnUtils::contains(vUpperUnit, "EUR") || CairnUtils::contains(vUpperUnit, "CURRENCY"))
                {
                    //temperature or price : do nothing
                }
                else if (fabs(interVal) < 1.e-5) //Correction for negligible negative values that might result from sum computation 
                    interVal = 0.0;
                else
                    cDebug() << " ABNORMAL NEGATIVE VALUE !! " << var->Name() << var->Unit() << iRow << time << pdtVec[iRow] << interVal << aVec[iRow];
            }

            (*var->ptrVariable())[j] = interVal;
        }

        time = time + 3600. * (r_MilpData.TimeStep(j - r_MilpData.npdtPast()));
    }
}

void TimeSeriesManager::importZEVarAverage(ZEVariables* var, std::vector<double> aVec, std::vector<double> pdtVec, const int& iShift)
{
    double time = iShift * r_MilpData.pdt();
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
                sumTime += pdtVec[iRow] - iShift * r_MilpData.pdt();
                sumValue += aVec[iRow] * (pdtVec[iRow] - iShift * r_MilpData.pdt());
                if (sumTime < 0) {
                    Cairn_Exception cairn_error((std::string)"Error while importing input time series. Negative time value! Something went wrong!", -1);
                    throw cairn_error;
                }
            }

            iRow++;
        }

        //Don't take average value for temperature and price
        std::string vUpperUnit = CairnUtils::toUpper(var->Unit());
        if (vUpperUnit == "DEGC" || vUpperUnit == "DEGK" || vUpperUnit == "K"
            || CairnUtils::contains(vUpperUnit, "EUR") || CairnUtils::contains(vUpperUnit, "CURRENCY"))
        {        
            if (time - pdtVec[iRow] < 10e-6) {
                (*var->ptrVariable())[j] = aVec[iRow];
            }
            else {
                double sval = 0.0;
                if (iRow == 0)//case where first time value in .csv is greater than TimeStep
                    sval = aVec[iRow] * time / pdtVec[iRow];
                else
                    sval = ((aVec[iRow] - aVec[iRow - 1]) * (time - pdtVec[iRow - 1]) / (pdtVec[iRow] - pdtVec[iRow - 1])) + aVec[iRow - 1];

                if (std::isnan(sval)) {
                    Cairn_Exception cairn_error("Error while importing the input data series: NAN value found for " + var->Name() + " at time " + std::to_string(time) + ", row: " + std::to_string(iRow), -1);
                    throw cairn_error;
                }

                (*var->ptrVariable())[j] = sval;
            }

            if (iRow < pdtVec.size() - 1) iRow++;
        }
        else // Not temperature or price
        {
            if (iRow == previRow + 1)  //To have a better precision for the basic case
            {
                if (fabs(time - pdtVec[iRow]) < 10e-6) {
                    (*var->ptrVariable())[j] = aVec[iRow];
                }
                else {
                    double sval = 0.0;
                    if (iRow == 0)//case where first time value in .csv is greater than TimeStep
                        sval = aVec[iRow] * time / pdtVec[iRow];
                    else
                        sval = ((aVec[iRow] - aVec[iRow - 1]) * (time - pdtVec[iRow - 1]) / (pdtVec[iRow] - pdtVec[iRow - 1])) + aVec[iRow - 1];

                    if (std::isnan(sval)) {
                        Cairn_Exception cairn_error("Error while importing the input data series: NAN value found for " + var->Name() + " at time " + std::to_string(time) + ", row: " + std::to_string(iRow), -1);
                        throw cairn_error;
                    }

                    //Values for variables that are not temperature or price cannot be negative 
                    if (sval < 0)
                    {
                        //Correction for negligible negative values that might result from sum computation 
                        if (fabs(sval) < 1.e-5)
                            sval = 0.0;
                        else
                            cDebug() << " ABNORMAL NEGATIVE VALUE !! " << var->Name() << var->Unit() << iRow << time << pdtVec[iRow] << sval << aVec[iRow];
                    }

                    (*var->ptrVariable())[j] = sval;
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

                if (std::isnan(sumValue / sumTime)) {
                    Cairn_Exception cairn_error("Error while importing the input data series: NAN value found for " + var->Name() + " at time " + std::to_string(time) + ", row: " + std::to_string(iRow), -1);
                    throw cairn_error;
                }

                //Values for variables that are not temperature or price cannot be negative 
                if (sumValue < 0)
                {
                    //Correction for negligible negative values that might result from sum computation 
                    if (fabs(sumValue / sumTime) < 1.e-5)
                        sumValue = 0.0;
                    else
                        cDebug() << " ABNORMAL NEGATIVE VALUE !! " << var->Name() << var->Unit() << iRow << time << pdtVec[iRow] << (sumValue / sumTime) << aVec[iRow];
                }

                (*var->ptrVariable())[j] = (sumValue / sumTime);

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
    std::vector<double>& vFineIn = *var->ptrVariable();
    const uint64_t aSizeFine = vFineIn.size(); //pastSize+futurSize
    const uint64_t aSizeCoarse = aTimeStepsOut.size() + aNpdtPast; //pastSize+ComputationfuturSize

    std::vector<double> TimeStepsIn(aSizeFine);
    std::vector<double> localIn(aSizeFine);
    double tmpFine = 0.;
    double tmpCoarse = 0.;

    if (aSizeFine < aSizeCoarse)
    {
        cCritical() << "aSizeFine = " << aSizeFine;
        cCritical() << "aSizeCoarse = " << aSizeCoarse;
        cCritical() << "ANOMALIE ! " << aTimeStepIn << aNpdtPast;
    }
    std::vector<double>& vCoarseOut = *var->ptrOutVariable();      
    vCoarseOut.clear();
    vCoarseOut.resize(aSizeCoarse, 0.0); // raz Out      
    
    if (aSizeFine == 0)
    {
        cCritical() << "Abnormal missing allocation aSizeFine = " << aSizeFine;        
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
            //cDebug() << "vFineIn ifine " << ifine << aSizeFine << vFineIn[ifine];
        }
        if (dt >= aTimeStepsOut[icoarse - aNpdtPast])
        {
            vCoarseOut[icoarse] = vCoarseOut[icoarse] / dt;
            //cDebug() << "vCoarseOut icoarse " << icoarse << aSizeCoarse << vCoarseOut[icoarse];
            tmpCoarse += aTimeStepsOut[icoarse - aNpdtPast];
            dt = 0.;
            icoarse++;
        }
    }
    if (tmpFine != tmpCoarse || icoarse != aSizeCoarse)
    {
        cCritical() << "Bad timesteps definition : the sum of coarse timesteps " << tmpCoarse << " must be equal to the sum of constant fine timestep " << tmpFine;
        cCritical() << "Bad timesteps definition : icoarse " << icoarse << " must be equal to aSizeCoarse " << aSizeCoarse;

        vCoarseOut[aSizeCoarse - 1] = vCoarseOut[aSizeCoarse - 1] / aTimeStepsOut[aTimeStepsOut.size() - 1];
        cCritical() << "Final value of coarse timeseries will be biased : " << aSizeCoarse << vCoarseOut[aSizeCoarse - 1];
    }
}