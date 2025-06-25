#ifndef STUDYPATHMANAGER_H
#define STUDYPATHMANAGER_H

#include <string>


class StudyPathManager {
public:
    void setStudyName(const std::string& aStudyName);
    void setResultsDir(const std::string& aResultsDir);
    void setResultFile(const std::string& aResultFile);
    void setScenarioName(const std::string& aScenarioName);

    const std::string& StudyName() { return mStudyName; }
    const std::string& resultFile()   const { return mResultFile; }
    const std::string& projectDir()     const { return mProjectDir; }
    const std::string& resultsDir()     const { return mResultsDir; }
    std::string archFile()     const;    // projectDir + StudyName + .json
    const std::string& scenarioName() const { return mScenarioName; };

    std::string getScenarioFile(const std::string& aSuffixe, int aNSol = 0, bool aPrefixResults = true) const;
    void copyFileToScenarioDir(const std::string& aFilePath);
private:
    std::string mStudyName; // only name
    std::string mResultFile; // prefix for resultFiles
    std::string mScenarioName;

    std::string mProjectDir;   // absolute path of project (configuration files)   
    std::string mResultsDir;   // absolute path of results (csv, logs)
};

#endif