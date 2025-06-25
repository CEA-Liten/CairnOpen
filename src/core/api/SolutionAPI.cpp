#include "CairnAPI.h"
#include "CairnCore.h"
#include "CairnAPIUtils.h"
using namespace CairnAPIUtils;

std::string CairnAPI::SolutionAPI::s_Time = "Time";

CairnAPI::SolutionAPI::SolutionAPI()
{
	m_Problem = nullptr;
}

void CairnAPI::SolutionAPI::set_Problem(OptimProblem* ap_Problem)
{
	m_Problem = ap_Problem;	
}

void CairnAPI::SolutionAPI::set_Results(int a_Step)
{
	if (m_Problem) {
		CairnCore* vCairn = (CairnCore*)m_Problem->parent();
		int previousSize = 0;
		int ts = vCairn->npdtFutur();
		QMap<QString, ZEVariables*> ListPublishedVariables = m_Problem->ListPublishedVariables();
		if (!a_Step) {
			// initialisation
			m_timeSeries.clear();
			m_timeSeries[s_Time] = t_values(ts);
			QMapIterator<QString, ZEVariables*> iPublishedVariable(ListPublishedVariables);
			while (iPublishedVariable.hasNext())
			{
				iPublishedVariable.next();
				ZEVariables* var = iPublishedVariable.value();
				m_timeSeries[var->Name().toStdString()] = t_values(ts);				
			}
		}
		else {
			// next step
			previousSize = m_timeSeries[s_Time].size();
			for (auto& vTS : m_timeSeries) {
				vTS.second.resize(previousSize + ts);
			}
		}
		int pdt = vCairn->getTimeData()->pdt();
		int npdtPast = vCairn->getTimeData()->npdtPast();
		for (size_t j = 0; j < ts; j++)	{
			m_timeSeries["Time"][j+previousSize] = ((int)j + 1 + ts * a_Step) * pdt;
			
			QMapIterator<QString, ZEVariables*> iPublishedVariable2(ListPublishedVariables);
			while (iPublishedVariable2.hasNext())
			{
				iPublishedVariable2.next();
				ZEVariables* var = iPublishedVariable2.value();
				QString zeVarName = var->Name();
				if (var->ptrVariable()->size() > 0)	{
					m_timeSeries[var->Name().toStdString()][j+previousSize] = var->ptrVariable()->at(j + npdtPast);					
				}
			}			
		}
	}		
}

void CairnAPI::SolutionAPI::set_Status(int a_status)
{
	m_status = a_status; // int status
}

const double* CairnAPI::SolutionAPI::getOptimalSolution(int a_numSol) const
{
	if (m_Problem) {
		Solver* vSolver = m_Problem->getSolver();
		if (vSolver) {
			return vSolver->getOptimalSolution(a_numSol);
		}
		else
			CairnAPIUtils::setError(errNotFound, "Solver");
	}
	else
		CairnAPIUtils::setError(noCairn);
	return nullptr;
}

std::string CairnAPI::SolutionAPI::get_Status() const
{
	// return string status
	if (m_Problem)
		return m_Problem->getOptimisationStatus().toStdString();
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return "";
}

int CairnAPI::SolutionAPI::get_NbSolutions() const
{
	if (m_Problem)
		return m_Problem->getNumberOfSolutions();
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return 0;
}

void CairnAPI::SolutionAPI::exportTimeSeries(const std::string& a_path, int a_numSol)
{
	if (m_Problem) {
		if (m_status >= 0) {
			CairnCore* vCairn = (CairnCore*)m_Problem->parent();
			QString vTSFileName = QString(a_path.c_str());
			if (vTSFileName == "") {
				vTSFileName = vCairn->StudyName() + "_Results.csv";
			}
			// export OUTPUT Time series					
			if (vCairn->exportTS(vTSFileName)) {
				CairnAPIUtils::setError(errWrite, vTSFileName.toStdString());
			}
		}
		else {
			CairnAPIUtils::setError(errRun);
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
}

void CairnAPI::SolutionAPI::exportPLAN(const std::string& a_path, int a_numSol)
{
}

void CairnAPI::SolutionAPI::exportHIST(const std::string& a_path, int a_numSol)
{
}

void CairnAPI::SolutionAPI::exportIndicators(const std::string& a_path, int a_numSol)
{
}

t_dictValues CairnAPI::SolutionAPI::get_TimeSeries(int a_numSol)
{
	t_dictValues vRet;
	if (m_Problem) {
		if (m_status >= 0) {
			return m_timeSeries;
		}
		else {
			CairnAPIUtils::setError(errRun);
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}	
	return vRet;
}

t_values CairnAPI::SolutionAPI::get_Times(int a_numSol)
{
	if (m_Problem) {
		if (m_status >= 0) {
			return m_timeSeries[s_Time];
		}
		else {
			CairnAPIUtils::setError(errRun);
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return t_values();
}

t_values CairnAPI::SolutionAPI::get_Values(const std::string& a_Name, int a_numSol)
{
	if (m_Problem) {
		if (m_status >= 0) {
			t_dictValues::iterator vIter = m_timeSeries.find(a_Name);
			if (vIter != m_timeSeries.end()) {
				return m_timeSeries[a_Name];
			}
			else {
				CairnAPIUtils::setError(errNotFound, "timeSeries " + a_Name);
			}
		}
		else {
			CairnAPIUtils::setError(errRun);
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return t_values();
}


