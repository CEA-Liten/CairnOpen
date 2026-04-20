#include "TimeSeriesReaderPegase.h"
#include "ZEVariables.h"

TimeSeriesReaderPegase::TimeSeriesReaderPegase()
{
}

bool TimeSeriesReaderPegase::open(const std::wstring& aTSfile)
{    
    return true;
}

void TimeSeriesReaderPegase::readHeader(const t_mapExchange& aListSubscribedVariables, std::vector<TimeSeriesDescrp>& aHeader)
{      
    for (auto& iSubscribedVariable : aListSubscribedVariables) {
        ZEVariables* var = iSubscribedVariable.second;
        aHeader.push_back({ var->Name() , var->Unit(), (int)aHeader.size()});
    }    
}

bool TimeSeriesReaderPegase::readTimes(std::vector<double>& aTimes, const double& aStartTime, 
    const double& aEndTime, const double& aTimeStep)
{ 
    return false; // no times
}

void TimeSeriesReaderPegase::readValues(int a_index, std::vector<double>& aValues)
{
  // nothing to do
}

void TimeSeriesReaderPegase::close()
{    
}