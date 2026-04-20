#include "TEST_CairnCore.h"
#include <iostream>
#include "Utils.h"
#include "UtilsJson.h"

#include <filesystem>
namespace fs = std::filesystem;
using namespace std;

int test(const fs::path& a_casepath, const fs::path& a_case)
{	
	bool vIsSampling = fs::exists(a_casepath / "sampling.csv");

	fs::path vStudy((a_casepath / a_case).replace_extension(".json"));
	
	fs::path vTimeSeries = a_case;
	vTimeSeries+= "_dataseries";
	fs::path vData((a_casepath / vTimeSeries).replace_extension(".csv"));
		
	CairnAPI m_Cairn;
	CairnAPI::OptimProblemAPI m_Problem;
	std::string vFileName = vStudy.string();

	TESTAPI("read study file from the file path: " + vFileName,
		m_Problem = m_Cairn.read_Study(vFileName)
	)
	std::string vTimeseriesFileName = vData.string();
	TESTAPI("Read the Timeseries from the file path: " + vTimeseriesFileName,
		m_Problem.add_TimeSeries(vTimeseriesFileName)
	)

	fs::path vResults = a_case;
	vResults += "_results_Results";
	fs::path vResultsFile = (a_casepath / vResults).replace_extension(".csv");
	fs::remove(vResultsFile);


	CairnAPI::SolutionAPI vSolution;

	TESTAPI("Run",
		vSolution = m_Problem.run()
	)
		
	TESTAPI2FALSE("Results exists: " + vResultsFile.string(), fs::exists(vResultsFile))

	return noError;
}

int main()
{
	int vRet = noError;
	
	fs::path vRootPath(TEST_DATA + (std::string)"/../../models/");
	//test(vRootPath / "geometryModel", "geo_wo_relax.json");

	for (auto const& dir_entry : fs::directory_iterator{ vRootPath }) {		
		if (dir_entry.is_directory()) {			
			for (auto const& f : fs::directory_iterator{ dir_entry }) {
				if (!f.is_directory()) {
					if (f.path().extension() == ".json") {
						int vTest = test(dir_entry, f.path().stem());
						if (vTest != noError) vRet = vTest;						
					}
				}
			}
		}
	}

	return vRet;
}


