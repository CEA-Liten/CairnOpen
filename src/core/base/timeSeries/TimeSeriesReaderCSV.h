#ifndef  TIMESERIESREADERCSV_H
#define TIMESERIESREADERCSV_H
#include "TimeSeriesReader.h"

class TimeSeriesReaderCSV
	: public TimeSeriesReader
{
public:
	TimeSeriesReaderCSV();

	bool open(const std::wstring& aTSfile);
	void readHeader(const t_mapExchange& aListSubscribedVariables, std::vector<TimeSeriesDescrp> &aHeader);
	bool readTimes(std::vector<double>& aTimes, const double& aStartTime, const double& aEndTime, const double& aTimeStep);
	void readValues(int a_index, std::vector<double>& aTimes);
	void close();

	int getNumLine(int aNumLine);

protected:
	static int iSkipHead;

	int m_startRow{ -1 };
	int m_endRow{ -1 };

	std::vector<std::vector<std::string>> m_Data;

	void readTimeColumn(const double& aStartTime, const double& aEndTime, const double& aTimeStep, const int& aRowSkipped, std::vector<double>& aValues);
	void readTSColumn(int aCol, int aRowSkipped, std::vector<double> &aValues);
};

#endif



