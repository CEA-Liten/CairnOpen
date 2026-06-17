#ifndef  TIMESERIESVALUES_H
#define TIMESERIESVALUES_H
#include "TimeSeriesReader.h"

class TimeSeriesValues
	: public TimeSeriesReader
{
public:
	TimeSeriesValues();

	void addTS(const t_dict& a_TS);
	bool checkTS(std::string& a_ErrMsg);

	bool open(const std::wstring& aTSfile);
	void readHeader(const t_mapExchange& aListSubscribedVariables, std::vector<TimeSeriesDescrp>& aHeader);
	bool readTimes(std::vector<double>& aTimes, const double& aStartTime, const double& aEndTime, const double& aTimeStep);
	void readValues(int a_index, std::vector<double>& aTimes);
	void close();

protected:
	bool checkTS(const t_dict &a_TS, std::string& a_ErrMsg, bool a_withTime = false);
	bool checkMap(const t_dict& a_TS, const std::string &a_Field, std::string& a_ErrMsg);
	bool checkMapVector(const t_dict& a_TS, const std::string& a_Field, std::string& a_ErrMsg, size_t &a_Size);

	size_t get_Size(const t_value* ap_Values);
	double get_Value(const t_value* ap_Values, size_t i);
	int m_startRow{ -1 };
	int m_endRow{ -1 };
	t_value* p_Time{ nullptr };
	std::vector<t_dict> m_TSList;
};

#endif