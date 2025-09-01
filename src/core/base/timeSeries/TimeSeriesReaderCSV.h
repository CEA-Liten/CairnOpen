#ifndef  TIMESERIESREADERCSV_H
#define TIMESERIESREADERCSV_H
#include "TimeSeriesReader.h"

class TimeSeriesReaderCSV
	: public TimeSeriesReader
{
public:
	TimeSeriesReaderCSV();

	bool open(const std::string& aTSfile);
	void readHeader(const t_mapExchange& aListSubscribedVariables, std::vector<TimeSeriesDescrp> &aHeader);
	bool readTimes(std::vector<double>& aTimes);
	void readValues(int a_index, std::vector<double>& aTimes);
	void close();

	int getNumLine(int aNumLine);

protected:
	static int iSkipHead;

	std::vector<std::vector<std::string>> m_Data;

	void readColumn(int aCol, int aRowSkipped, std::vector<double> &aValues);
};

#endif



