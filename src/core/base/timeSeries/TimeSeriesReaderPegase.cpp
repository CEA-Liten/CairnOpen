#include "TimeSeriesReaderPegase.h"
#include "ZEVariables.h"

TimeSeriesReaderPegase::TimeSeriesReaderPegase()
{
}

bool TimeSeriesReaderPegase::open(const std::string& aTSfile)
{    
    return true;
}

void TimeSeriesReaderPegase::readHeader(const t_mapExchange& aListSubscribedVariables, std::vector<TimeSeriesDescrp>& aHeader)
{
    m_Data.clear();       
    for (auto& iSubscribedVariable : aListSubscribedVariables) {
        ZEVariables* var = iSubscribedVariable.second;
        aHeader.push_back({ var->Name().toStdString() , var->Unit().toStdString(), (int)aHeader.size()});
        m_Data.push_back(var->ptrVariable());
    }    
}

bool TimeSeriesReaderPegase::readTimes(std::vector<double>& aTimes)
{ 
    return false; // no times
}

void TimeSeriesReaderPegase::readValues(int a_index, std::vector<double>& aValues)
{
    if (a_index >= 0 && a_index < m_Data.size()) {
        const QVector<float>& vValues = *m_Data[a_index];
        aValues.resize(vValues.size());
        for (size_t i = 0;i < vValues.size(); i++) {
            aValues[i] = vValues[i];
        }
    }
}

void TimeSeriesReaderPegase::close()
{    
}