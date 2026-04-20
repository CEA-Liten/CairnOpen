#ifndef  TIMESERIESREADERPEGASE_H
#define TIMESERIESREADERPEGASE_H
#include "TimeSeriesReader.h"

class TimeSeriesReaderPegase
	: public TimeSeriesReader
{
public:
	TimeSeriesReaderPegase();

	bool open(const std::wstring& aTSfile);
	void readHeader(const t_mapExchange& aListSubscribedVariables, std::vector<TimeSeriesDescrp>& aHeader);
	bool readTimes(std::vector<double>& aTimes, const double& aStartTime, const double& aEndTime, const double& aTimeStep);
	void readValues(int a_index, std::vector<double>& aTimes);
	void close();	
protected:
};

#endif