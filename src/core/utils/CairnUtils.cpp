#include "CairnUtils.h"
#include "GlobalSettings.h"
#include <sstream>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

namespace CairnUtils {

    int generateTimeStamp() {
        std::chrono::high_resolution_clock::time_point time_stamp = std::chrono::high_resolution_clock::now();
        long sec_timeStamp = std::chrono::duration_cast<std::chrono::seconds>(time_stamp.time_since_epoch()).count();
        return sec_timeStamp;
    }

    std::string addTimeStampToFileName(std::string aFileName)
	{
        fs::path fileNameInfo(aFileName);        
        fs::path fileDir = fileNameInfo.root_path();

        std::string extenstion = fileNameInfo.extension().string();
        std::string baseName = fileNameInfo.stem().string() + std::string("_") + std::to_string(generateTimeStamp()) + extenstion;
        
        //Get a time stamp and rename the file
        fs::path  filename_ts = fileDir / fs::path(baseName);
        return filename_ts.string();
    }

    bool openFileForWriting(std::fstream& aFileOut, const std::string& a_FileName, std::ios::openmode a_openMode) {

        aFileOut.open(a_FileName, a_openMode );
        if (!aFileOut.is_open()) {
            qWarning() << "Couldn't open " + QString(a_FileName.c_str()) + " for writing!";
            aFileOut.close(); //for safety!
            //Add a time stamp to the file name
            std::string vFileName = addTimeStampToFileName(a_FileName);
            qInfo() << "A timestamp has been added to the filename: " + QString(vFileName.c_str());
            aFileOut.open(vFileName, a_openMode);
            if (!aFileOut.is_open()) {
                qWarning() << "Couldn't open " + QString(vFileName.c_str()) + " for writing!";
                return false;  
            }
            else {
                return true;
            }
        }
        else {
            return true;
        }
    }

    void outputIndicator(std::fstream& out, const QString compoName, const QString indicatorName, const double value, const QString unit, const QString alias) {
        out << compoName.simplified().toStdString() << ";" << indicatorName.simplified().toStdString() << ";" << QString::number(value).toStdString() << ";" << unit.simplified().toStdString() << ";" << alias.simplified().toStdString() << "\n";
    }

    void outputIndicator(std::fstream& out, const QString compoName, const QString indicatorName, const QVector<double> list_values, const QString unit, const QString alias) {
        out << compoName.simplified().toStdString() << ";" << indicatorName.simplified().toStdString() << ";";
        for (int i = 0; i < list_values.size(); i++) {
            out << list_values.at(i);
            if(i < list_values.size()-1) out << ",";
        }
        out << ";" << unit.simplified().toStdString() << ";" << alias.simplified().toStdString() << "\n";
    }

    void printInfoParam(const QString& aParamName, const bool IsBlocking, const QString& aUnit, const QString& aDescription)
    {
        if (GS::iVerbose > 0)
        {
            QFile logFile(QDir::currentPath() + "/UnitParam.log");
            logFile.open(QIODevice::Append | QIODevice::Text);
            QTextStream txtLogFile(&logFile);
            txtLogFile.setFieldAlignment(QTextStream::FieldAlignment::AlignLeft);
            txtLogFile << aParamName << "- Mandatory : " << IsBlocking << "- Unit : " << aUnit << "- Description : " << aDescription << "\n";

            //    Q_ASSERT(! aDescription.contains("(") && ! aDescription.contains(")") && ! aDescription.contains(",")) ;
            if (aDescription.contains("(") || aDescription.contains(")") || aDescription.contains(","))
            {
                QString ErrorMsg("Description MUST NOT include any parenthesis nor commas for parameter: ");
                ErrorMsg.append(aParamName);
                //        qFatal (ErrorMsg.toStdString().c_str()) ;
                qWarning("%s", ErrorMsg.toStdString().c_str());
            }
            logFile.close();
        }
    }

    void printMissingParam(const QString& aParamName, const QString &aValue)
    {
        if (GS::iVerbose > 1)
        {
            QFile qfLogFile(QDir::currentPath() + "/DefaultInputParam.log");
            qfLogFile.open(QIODevice::Append | QIODevice::Text);
            QTextStream missLogFile(&qfLogFile);
            missLogFile.setFieldAlignment(QTextStream::FieldAlignment::AlignLeft);
            missLogFile << "Missing value in the file for parameter " << aParamName << " - get default value from Submodel and Component : " << aValue << "\n";
            qfLogFile.close();
        }
    }

    void resetInfoParam()
    {
        if (GS::iVerbose > 0)
        {
            QFile logFile(QDir::currentPath() + "/UnitParam.log");
            logFile.open(QIODevice::WriteOnly | QIODevice::Text);
            QTextStream txtLogFile(&logFile);
            txtLogFile.setFieldAlignment(QTextStream::FieldAlignment::AlignLeft);
            txtLogFile << "";
            logFile.close();
        }
    }

    void resetMissingParam()
    {
        if (GS::iVerbose > 1)
        {
            QFile qfLogFile(QDir::currentPath() + "/DefaultInputParam.log");
            qfLogFile.open(QIODevice::WriteOnly | QIODevice::Text);
            QTextStream missLogFile(&qfLogFile);
            missLogFile.setFieldAlignment(QTextStream::FieldAlignment::AlignLeft);
            missLogFile << "";
            qfLogFile.close();
        }
    }

    bool contains(const std::vector< std::string>& a_List, const std::string &a_Find)
    {
        std::vector<std::string>::const_iterator vIter = find(a_List.begin(), a_List.end(), a_Find);
        return (vIter != a_List.end());
    }

    bool contains(const std::string& a_string, const std::string& a_Find, bool a_toUpper)
    {
        if (a_toUpper) {
            std::string vTmp(a_string);
            std::transform(a_string.begin(), a_string.end(), vTmp.begin(), ::toupper);
            return vTmp.find(a_Find) != std::string::npos;
        }
        else
            return a_string.find(a_Find) != std::string::npos;
    }

    bool contains(const std::string& a_string, const std::vector<std::string>& a_FindOneInList, bool a_toUpper)
    {
        for (auto& vFind : a_FindOneInList) {
            if (contains(a_string, vFind, a_toUpper))
                return true;
        }
        return false;
    }


    std::string join(const std::vector< std::string> & a_List) 
    {
        if (a_List.size()) {
            std::string vRet = "";
            std::string vSep = "[";

            for (auto& vElem : a_List) {
                vRet += vSep + vElem;
                vSep = ",";
            }
            return vRet + "]";
        }
        else
            return "[]";
    }

    std::string replace(std::string& a_string , const std::string& a_find, const std::string& a_replace, bool a_toUpper)
    {       
        if (a_toUpper) {            
            std::transform(a_string.begin(), a_string.end(), a_string.begin(), ::toupper);            
        }
        if (!a_find.empty())
            for (size_t pos = 0; (pos = a_string.find(a_find, pos)) != std::string::npos; pos += a_replace.size())
                a_string.replace(pos, a_find.size(), a_replace);
        return a_string;
    }

    std::string toUpper(const std::string& a_string)
    {
        std::string vTmp(a_string);
        std::transform(a_string.begin(), a_string.end(), vTmp.begin(), ::toupper);
        return vTmp;
    }
    std::vector<std::string> split(const std::string& a_string, const char& a_separator)
    {
        std::vector<std::string> vRet;
        std::istringstream iss(a_string);
        std::string item;
        while (std::getline(iss, item, a_separator)) {
            vRet.push_back(item);
        }
        return vRet;
    }

    std::string BuildFileName(const std::string &aFileName)
    {
        fs::path vPath(aFileName);
        if (vPath.filename().string() == aFileName)
            return (fs::current_path() / vPath).string();
        else
            return aFileName;
    }
    std::vector<std::vector<std::string>> readFromCsvFile(const std::string& aFileName, const std::string& sep)
    {
        std::vector<std::vector<std::string>>  data_Inputs;

        std::string filename = BuildFileName(aFileName);
        fs::path vPath(filename);
        if (!fs::exists(vPath))
        {
            return data_Inputs;
        }
        else
        {
            qInfo() << " Reading csv file " << QString(filename.c_str());
        }
        data_Inputs = readToList(filename, sep);
        return data_Inputs;
    }
    std::vector<std::vector<std::string>> readToList(const std::string& Full_File_Name, const std::string& Separator)
    {
        std::vector<std::vector<std::string>> data_input;
        if (Separator.size() != 1) return data_input;
        std::vector<std::string> fields;
        std::fstream File(Full_File_Name, std::ios_base::in);

        if (!File.is_open())
        {
            //qInfo() << "Error CSV File could not be opened for reading " ;
            //return {{"ERROR"}};
            Cairn_Exception error("Error CSV File could not be opened for reading: " + Full_File_Name, -1);
            throw error;
        }

        int k = 1;
        std::string line;
        while (std::getline(File, line)) 
        {
            fields = split(line, Separator[0]);
            if (contains(fields, "Error\n"))           
            {
                qInfo() << "Error reading line " << QString(line.c_str());
            }
            if (k > 4) {//data lines 
                //Verify if the used separator is correct and that comma is not used for decimals.
                for (int i = 0; i < fields.size(); i++) {
                    if (contains(fields[i], ",") || contains(fields[i], ";")) {
                        std::string errorMessage = "Error while importing input time series: " + Full_File_Name + "\nPlease, verify that the correct separator(" + Separator + ") is used and that comma is not used for decimal values.";
                        if (i == 0) {
                            errorMessage += " Line: " + fields[0];
                        }
                        else {
                            errorMessage += " Value: " + fields[i];
                        }
                        Cairn_Exception error(errorMessage, -1);
                        throw error;
                    }
                }
            }
            data_input.push_back(fields);
            k++;
        }
        return data_input;
    }

    std::vector<double> getDataArray(const std::vector<std::vector<std::string>>& data_Inputs, int aCol, int iskipHead)
    {
        std::vector<double> lu;
        std::string value;

        for (int i = iskipHead; i < data_Inputs.size(); ++i)
        {
            value = (data_Inputs.at(i)).at(aCol);

            if (value == "")
            {                
                break;
            }
            else
            {
                lu.push_back(std::stod(value));
            }

        }
        return lu;
    }

}

