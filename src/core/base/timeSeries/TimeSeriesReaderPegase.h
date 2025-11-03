#ifndef  TIMESERIESREADERPEGASE_H
#define TIMESERIESREADERPEGASE_H
#include "TimeSeriesReader.h"

class TimeSeriesReaderPegase
	: public TimeSeriesReader
{
public:
	TimeSeriesReaderPegase();

	bool open(const std::string& aTSfile);
	void readHeader(const t_mapExchange& aListSubscribedVariables, std::vector<TimeSeriesDescrp>& aHeader);
	bool readTimes(std::vector<double>& aTimes);
	void readValues(int a_index, std::vector<double>& aTimes);
	void close();	
protected:
};

#endif