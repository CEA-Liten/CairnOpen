#include "TimeSeriesReaderPegase.h"
#include "ZEVariables.h"

TimeSeriesReaderPegase::TimeSeriesReaderPegase()
{
}

bool TimeSeriesReaderPegase::open(const QString& aTSfile)
{    
    return true;
}

void TimeSeriesReaderPegase::readHeader(const QMap<QString, ZEVariables*>& aListSubscribedVariables, std::vector<TimeSeriesDescrp>& aHeader)
{
    m_Data.clear();
    QMapIterator<QString, ZEVariables*> iSubscribedVariable(aListSubscribedVariables);    
    while (iSubscribedVariable.hasNext())
    {
        iSubscribedVariable.next();
        ZEVariables* var = iSubscribedVariable.value();
        aHeader.push_back({ var->Name() , var->Unit(), (int) aHeader.size()});
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