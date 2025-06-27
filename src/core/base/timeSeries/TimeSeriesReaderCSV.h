#ifndef  TIMESERIESREADERCSV_H
#define TIMESERIESREADERCSV_H
#include "TimeSeriesReader.h"
#include <QList>
#include <QStringList>

class TimeSeriesReaderCSV
	: public TimeSeriesReader
{
public:
	TimeSeriesReaderCSV();

	bool open(const QString& aTSfile);
	void readHeader(const QMap<QString, class ZEVariables*>& aListSubscribedVariables, std::vector<TimeSeriesDescrp> &aHeader);
	bool readTimes(std::vector<double>& aTimes);
	void readValues(int a_index, std::vector<double>& aTimes);
	void close();

	int getNumLine(int aNumLine);

protected:
	static int iSkipHead;

	QList<QStringList> m_Data;

	void readColumn(int aCol, int aRowSkipped, std::vector<double> &aValues);
};

#endif



