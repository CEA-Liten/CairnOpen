#ifndef  TIMESERIESREADER_H
#define TIMESERIESREADER_H
#include <QString>
#include <QMap>
#include <vector>

class TimeSeriesReader
{
public:
	static TimeSeriesReader* NewReader(const std::string& a_ReaderType);
	virtual ~TimeSeriesReader() {};


	struct  TimeSeriesDescrp
	{
		QString Name;
		QString Unit;
		int Index;
	};
	virtual bool open(const QString& aTSfile) = 0;	
	virtual bool readTimes(std::vector<double>& aTimes) = 0;
	virtual void readHeader(const QMap<QString, class ZEVariables*>& aListSubscribedVariables, std::vector<TimeSeriesDescrp>& aHeader) = 0;
	virtual void readValues(int a_index, std::vector<double>& aTimes) = 0;
	virtual void close() = 0;

	virtual int getNumLine(int aNumLine) { return aNumLine; };

protected:
	TimeSeriesReader() {};
};

#endif