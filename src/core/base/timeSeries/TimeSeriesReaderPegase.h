#ifndef  TIMESERIESREADERPEGASE_H
#define TIMESERIESREADERPEGASE_H
#include "TimeSeriesReader.h"
#include <QList>
#include <QStringList>
#include <QVector>

class TimeSeriesReaderPegase
	: public TimeSeriesReader
{
public:
	TimeSeriesReaderPegase();

	bool open(const QString& aTSfile);
	void readHeader(const QMap<QString, class ZEVariables*>& aListSubscribedVariables, std::vector<TimeSeriesDescrp>& aHeader);
	bool readTimes(std::vector<double>& aTimes);
	void readValues(int a_index, std::vector<double>& aTimes);
	void close();	
protected:
	std::vector< QVector<float>* > m_Data;
};

#endif