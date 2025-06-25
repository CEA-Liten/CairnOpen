#ifndef  TIMESERIESMANAGER_H
#define TIMESERIESMANAGER_H
#include "MilpData.h"
#include <QStringList>
#include "ZEVariables.h"
#include "TimeSeriesReader.h"
#include "OrUnitsConverter.h"
#include "StudyPathManager.h"

class TimeSeriesManager {
public:
	TimeSeriesManager(MilpData &aMilpData, const std::string &a_ReaderKind = "csv");
	~TimeSeriesManager();

	void importTS(const QStringList& aTSfileList, const QMap<QString, ZEVariables*>& aListSubscribedVariables, const int& iShift = 0, bool isCheckTimeSeriesUnits = false);
	void importTS(const QMap<QString, ZEVariables*>& aListSubscribedVariables);

	class OrCheckUnits CheckUnits(const QString& a_FileUnit, const QString& a_Units, bool a_Check = true);

protected:
	void importTS(const QString& aTSfile, const QMap<QString, ZEVariables*>& aListSubscribedVariables, const int& iShift, QStringList& aListNotFoundNames, bool isCheckTimeSeriesUnits = false);
	void readTimes(const QString& aTSfile, const int& iShift, std::vector<double>& aTimes);
	
	void extrapolation(const QString& aTSfile, const int& iShift, const TimeSeriesReader::TimeSeriesDescrp &aHeader, const std::vector<double>& aTimes, std::vector<double>& aValues);
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
