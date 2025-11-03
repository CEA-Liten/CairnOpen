
#include "StudyPathManager.h"
#include "CairnLogger.h"
#include <filesystem>
namespace fs = std::filesystem;



void StudyPathManager::setStudyName(const std::string& aStudyName)
{
    fs::path vStudy(aStudyName);
    mStudyName = vStudy.stem().string();

    vStudy.remove_filename();
    mProjectDir = vStudy.string();

    if (mResultsDir == "" || mResultFile == "") {
        setResultsDir(mProjectDir);
    }
}

std::string StudyPathManager::archFile()     const
{
    fs::path vStudy(mProjectDir);
    return (vStudy / (mStudyName + ".json")).string();
}

void StudyPathManager::setResultsDir(const std::string& aResultsDir)
{
    if (aResultsDir == "") {
        mResultsDir = mProjectDir;        
    }    
    else {
        mResultsDir = aResultsDir;
    }
    fs::path vResultsPath(mResultsDir);
    if (mResultsDir != mProjectDir) {
        // scenario
        if (vResultsPath.is_relative()) {
            // relatif to project file
            vResultsPath = mProjectDir / vResultsPath;
            mResultsDir = vResultsPath.string();
        }
        // create         
        if (!fs::exists(vResultsPath)) {
            std::error_code ec;
            if (!fs::create_directory(vResultsPath, ec)) {
                std::string vErrMsg = "Cannot create directory " + vResultsPath.string() + ", " + ec.message();
                cCritical() << vErrMsg;
            }
        }        
    }
        
    vResultsPath /= mStudyName + "_results";
    mResultFile = vResultsPath.string();
}

void StudyPathManager::setResultFile(const std::string& aResultFile) {
    mResultFile = aResultFile;
    if (mResultFile != "") {
        fs::path vResultsPath(mResultFile);
        vResultsPath.remove_filename();
        mResultsDir = vResultsPath.string();
    }
}

void StudyPathManager::setScenarioName(const std::string& aScenarioName) {
    mScenarioName = aScenarioName;
}

std::string StudyPathManager::getScenarioFile(const std::string& aSuffixe, int aNSol, bool aPrefixResults) const
{   
    fs::path vResultsPath(mResultsDir);
    if (mScenarioName != "") {
        vResultsPath /= mScenarioName;
    }
    std::string vFileName = mStudyName;
    if (aPrefixResults) {
        vFileName += "_results";
    }
    if (aNSol > 0) {
        vFileName += "_" + std::to_string(aNSol);
    }
    vFileName += aSuffixe;
    vResultsPath /= vFileName;
    return vResultsPath.string();
}

void StudyPathManager::copyFileToScenarioDir(const std::string& aFilePath)
{
    if (mResultsDir != mProjectDir) {
        fs::path vResultsPath(mResultsDir);
        if (mScenarioName != "") {
            vResultsPath /= mScenarioName;
        }

        fs::path vFilePath(aFilePath);
        std::string vFileName = vFilePath.filename().string();
        vResultsPath /= vFileName;

        //Remove file if already exists
        if (fs::exists(vResultsPath)) {
            std::error_code ec;
            if (!fs::remove(vResultsPath, ec)) {
                std::string vErrMsg = "Error while deleting already existing file " + vResultsPath.string() + ", " + ec.message();
                cCritical() << vErrMsg;
            }
        }

        //Copy file to scenarioPath
        std::error_code ec;
        fs::copy(vFilePath, vResultsPath, ec);
        if (ec.value()) {
            std::string vErrMsg = "Error while copying " + vFileName + " to " + vResultsPath.remove_filename().string();
            cError() << vErrMsg;
        }
    }
}