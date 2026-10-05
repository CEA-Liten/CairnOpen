#include "TimeSeriesValues.h"
#include "CairnAPI.h"
#include "Cairn_Exception.h"

TimeSeriesValues::TimeSeriesValues()
{
}

void TimeSeriesValues::addTS(const t_dict& a_TS)
{
    if (a_TS.empty())
        m_TSList.clear();
    else
        m_TSList.push_back(a_TS);
}

bool TimeSeriesValues::checkTS(std::string& a_ErrMsg)
{
    // return false if no TS (ErrMsg is empty)
    // return false with ErrMsg if the definition of TS is incomplete
    bool vRet = false;
    if (m_TSList.size() == 1) {
        vRet = checkTS(m_TSList[0], a_ErrMsg, true);
        if (vRet) {
            t_dict::iterator vIter = m_TSList[0].find("Times");            
            p_Time = &vIter->second;
        }
    }
    else {
        for (auto& vTS : m_TSList) {
            vRet = checkTS(vTS, a_ErrMsg);
            if (!vRet) break;

            t_dict::iterator vIter = vTS.find("Name");
            if (const std::string* pSrc = std::get_if<std::string>(&vIter->second)) {
                if (*pSrc == "Times" || *pSrc == "Time") {
                    vIter = vTS.find("Values");
                    p_Time = &vIter->second;
                }
            }
        }
        if (vRet && !p_Time) {
            vRet = false;
            a_ErrMsg = "No time in TS";
        }
    }    
    return vRet;
}

bool TimeSeriesValues::checkTS(const t_dict& a_TS, std::string& a_ErrMsg, bool a_withTime)
{
    // dict format:
    /*  "Name":"H2_Load.LoadMassFlowrate",
        "Unit" : "kg/h",                     (optionnel ? )
        "Description" : "charge debit",      (optionnel ? )   
        "Values" : [1, 2, 3] ,
        "Times" : [3600, 7200, 10800] ,
        "StartTime" : "01/01/2012 00:00"     (optionnel ? )
    */
    bool vRet = checkMap(a_TS, "Name", a_ErrMsg);
    size_t vSize = 0;
    if (vRet) vRet = checkMapVector(a_TS, "Values", a_ErrMsg, vSize);
    if (a_withTime)
        if (vRet) vRet = checkMapVector(a_TS, "Times", a_ErrMsg, vSize);
    return vRet;
}

bool TimeSeriesValues::checkMap(const t_dict& a_TS, const std::string& a_Field, std::string& a_ErrMsg)
{
    bool vRet = false;
    t_dict::const_iterator vIter = a_TS.find(a_Field);
    if (vIter != a_TS.end()) {
        if (const std::string* pSrc = std::get_if<std::string>(&vIter->second)) {
            vRet = true;
        }  
        else {
            a_ErrMsg = "Parameter " + a_Field + " must be a string";
        }
    }
    else {
        a_ErrMsg = "Parameter " + a_Field + " must be assigned";
    }
    return vRet;
}

bool TimeSeriesValues::checkMapVector(const t_dict& a_TS, const std::string& a_Field, std::string& a_ErrMsg, size_t &a_Size)
{
    bool vRet = false;
    t_dict::const_iterator vIter = a_TS.find(a_Field);
    if (vIter != a_TS.end()) {
        size_t vSize = 0;
        if (const std::vector<double>* pSrc = std::get_if<std::vector<double>>(&vIter->second)) {            
            vSize = pSrc->size();
            vRet = true;
        }
        else if (const std::vector<int>* pSrc = std::get_if<std::vector<int>>(&vIter->second)) {
            vSize = pSrc->size();
            vRet = true;
        }
        else if (const std::vector<std::string>* pSrc = std::get_if<std::vector<std::string>>(&vIter->second)) {
            vSize = pSrc->size();
            vRet = true;
        }
        else {
            a_ErrMsg = "Parameter " + a_Field + " must be a vector";
        }
        if (vRet && a_Size) {
            if (vSize != a_Size) {
                a_ErrMsg = "Parameter " + a_Field + " must be of size " + std::to_string(a_Size) + ", not " + std::to_string(vSize);
                vRet = false;
            }                         
        }
        a_Size = vSize;
    }
    else {
        a_ErrMsg = "Parameter " + a_Field + " must be assigned";
    }
    return vRet;
}

size_t TimeSeriesValues::get_Size(const t_value* ap_Values)
{
    size_t vSize = 0;
    if (ap_Values) {
        if (const std::vector<double>* pSrc = std::get_if< std::vector<double>>(ap_Values)) {
            vSize = pSrc->size();
        }
        else if (const std::vector<int>* pSrc = std::get_if< std::vector<int>>(ap_Values)) {
            vSize = pSrc->size();
        }  
        else if (const std::vector<std::string>* pSrc = std::get_if< std::vector<std::string>>(ap_Values)) {
            vSize = pSrc->size();
        }
    }
    return vSize;
}

double TimeSeriesValues::get_Value(const t_value* ap_Values, size_t i)
{
    double vRet = std::nan("1");
    if (ap_Values) {
        if (const std::vector<double>* pSrc = std::get_if< std::vector<double>>(ap_Values)) {
            if (i >= 0 && i < pSrc->size()) {
                vRet = (*pSrc)[i];
            }
        }
        else if (const std::vector<int>* pSrc = std::get_if< std::vector<int>>(ap_Values)) {
            if (i >= 0 && i < pSrc->size()) {
                vRet = (double)(*pSrc)[i];
            }
        }   
        else if (const std::vector<std::string>* pSrc = std::get_if< std::vector<std::string>>(ap_Values)) {
            if (i >= 0 && i < pSrc->size()) {
                try {
                    vRet = std::stod((*pSrc)[i]);                    
                }
                catch (...) {                    
                }                
            }
        }
    }
    return vRet;
}

bool TimeSeriesValues::open(const std::wstring& aTSfile)
{
    return true;
}

void TimeSeriesValues::readHeader(const t_mapExchange& aListSubscribedVariables, std::vector<TimeSeriesDescrp>& aHeader)
{
    size_t vSize = m_TSList.size();
    aHeader.resize(vSize);
    for (size_t i = 0; i < vSize; i++) {
        const t_dict& vTS = m_TSList[i];
        t_dict::const_iterator vIter = vTS.find("Name");
        if (vIter != vTS.end()) {
           aHeader[i].Name = *std::get_if< std::string>(&vIter->second);
        }
        aHeader[i].Unit = "";
        vIter = vTS.find("Unit");
        if (vIter != vTS.end()) {
            if (std::holds_alternative<std::string>(vIter->second)) {
                try
                {
                    aHeader[i].Unit = std::get< std::string>(vIter->second);
                }
                catch (const std::exception&)
                {
                }
            }                        
        }                
        aHeader[i].Index = i;
        cDebug() << "read time series: " << aHeader[i].Name << ", unit: " << aHeader[i].Unit;
    }
}

bool TimeSeriesValues::readTimes(std::vector<double>& aTimes, const double& aStartTime,
    const double& aEndTime, const double& aTimeStep)
{
    if (!m_TSList.size()) {
        return false; // no time series
    }
    if (aEndTime > 0 && aEndTime < aStartTime) {
        throw Cairn_Exception("End time (" + std::to_string(aEndTime) + ") is less than start time ("
            + std::to_string(aStartTime) + ")", -1);
    }


    size_t vNb = get_Size(p_Time);
    aTimes.reserve(vNb);
    
    double firstTime = aTimeStep;
    for (std::size_t i = 0; i < vNb; i++)
    {        
        double dvalue = get_Value(p_Time, i);
        
        if (!std::isfinite(dvalue)) {
            throw Cairn_Exception("Non-finite time (NaN/Inf) at " + std::to_string(i + 1), -1);
        }

        // ------- Add value and set star and end rows -------
        if (i == 0 && firstTime < 0.0) {
            firstTime = dvalue;
        }

        if (dvalue < aStartTime) {
            continue; // skip until we reach start time
        }
        else if (m_startRow < 0) {
            m_startRow = static_cast<int>(i); // set only once
        }

        if (aEndTime > 0) { // if aEndTime < 0, read until the end
            if (m_endRow < 0 && dvalue >= aEndTime) {
                m_endRow = static_cast<int>(i); // set only once
            }
            if (dvalue > aEndTime) {
                break; // past the range
            }
        }

        // Normalize: dvalue mapped to [aStartTime -> firstTime]
        const double normalizedValue = (aStartTime > 0) ? dvalue - aStartTime + firstTime : dvalue;
        aTimes.push_back(normalizedValue);
    }
    return true; 
}

void TimeSeriesValues::readValues(int a_index, std::vector<double>& aValues)
{
    const t_dict& vTS = m_TSList[a_index];
    t_dict::const_iterator vIter = vTS.find("Values");    
    const t_value * pValues = (&vIter->second);
    
    aValues.clear();

    // Normalize start/end rows
    const std::size_t startRow = (m_startRow < 0 ? 0 : static_cast<std::size_t>(m_startRow));
    
    const std::size_t maxRow = get_Size(pValues);
    const std::size_t endRow = (m_endRow > 0)
        ? std::min(static_cast<std::size_t>(m_endRow), maxRow)
        : maxRow;

    if (startRow >= endRow) {
        cDebug() << "readValues: startRow (" << startRow << ") >= endRow (" << endRow << "), nothing to read.";
        return;
    }

    // Reserve for performance
    aValues.reserve(endRow - startRow);

    for (int i = startRow; i < endRow; ++i)
    {
        double dValue = get_Value(pValues, i);
        
        if (!std::isfinite(dValue)) {
            throw Cairn_Exception("Non-finite value (NaN/Inf) at row " + std::to_string(i + 1), -1);
        }

        aValues.push_back(dValue);
    }
}

void TimeSeriesValues::close()
{
    m_TSList.clear();
    m_startRow = -1;
    m_endRow = -1 ;
    p_Time = nullptr;
}