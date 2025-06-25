#include "TimeSeriesReader.h"
#include "TimeSeriesReaderCSV.h"
#include "TimeSeriesReaderPegase.h"

TimeSeriesReader* TimeSeriesReader::NewReader(const std::string& a_ReaderType)
{
	TimeSeriesReader* vRet = nullptr;
	if (a_ReaderType == "csv") {
		vRet = new TimeSeriesReaderCSV();
	}
	else if (a_ReaderType == "pegase") {
		vRet = new TimeSeriesReaderPegase();
	}
	return vRet;
}
