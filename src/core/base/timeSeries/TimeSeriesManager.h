#ifndef  TIMESERIESMANAGER_H
#define TIMESERIESMANAGER_H
#include "MilpData.h"
#include "ZEVariables.h"
#include "TimeSeriesReader.h"
#include "OrUnitsConverter.h"
#include "StudyPathManager.h"

class TimeSeriesManager {
public:
	TimeSeriesManager(MilpData &aMilpData, const std::string &a_ReaderKind = "csv");
	~TimeSeriesManager();

	void importTS(const std::vector<std::string>& aTSfileList, const t_mapExchange& aListSubscribedVariables, const int& iShift = 0, bool isCheckTimeSeriesUnits = false);
	void importTS(const t_mapExchange& aListSubscribedVariables);

	class OrCheckUnits CheckUnits(const std::string& a_FileUnit, const std::string& a_Units, bool a_Check = true);

protected:
	void importTS(const std::string& aTSfile, const t_mapExchange& aListSubscribedVariables, const int& iShift, std::vector<std::string>& aListNotFoundNames, bool isCheckTimeSeriesUnits = false);
	void readTimes(const std::string& aTSfile, const int& iShift, std::vector<double>& aTimes);
	
	void extrapolation(const std::string& aTSfile, const int& iShift, const TimeSeriesReader::TimeSeriesDescrp &aHeader, const std::vector<double>& aTimes, std::vector<double>& aValues);
	void conversion(const OrCheckUnits& checkUnits, std::vector<double>& aValues);

	void importZEVarInterpolation(ZEVariables* var, std::vector<double> aVec, std::vector<double> pdtVec, const int& iShift);
	void importZEVarAverage(ZEVariables* var, std::vector<double> aVec, std::vector<double> pdtVec, const int& iShift);

	void Average(ZEVariables* var, double aTimeStepIn, const std::vector<double> &aTimeStepsOut, uint aNpdtPast);

	MilpData &r_MilpData;
	TimeSeriesReader* p_Reader{ nullptr };

	int m_rowShift{ 0 };
	int m_npdtFutur{ 0 };

	std::string m_ReaderKind;
};

#endif // ! TIMESERIESMANAGER_H
