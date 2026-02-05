#ifndef  TIMESERIESREADER_H
#define TIMESERIESREADER_H

#include "ZEVariables.h"

class TimeSeriesReader
{
public:
	static TimeSeriesReader* NewReader(const std::string& a_ReaderType);
	virtual ~TimeSeriesReader() {};


	struct  TimeSeriesDescrp
	{
		std::string Name;
		std::string Unit;
		int Index;
	};
	virtual bool open(const std::string& aTSfile) = 0;	
	virtual bool readTimes(std::vector<double>& aTimes, const double& aStartTime, const double& aEndTime, const double& aTimeStep) = 0;
	virtual void readHeader(const t_mapExchange& aListSubscribedVariables, std::vector<TimeSeriesDescrp>& aHeader) = 0;
	virtual void readValues(int a_index, std::vector<double>& aTimes) = 0;
	virtual void close() = 0;

	virtual int getNumLine(int aNumLine) { return aNumLine; };

protected:
	TimeSeriesReader() {};
};

#endif