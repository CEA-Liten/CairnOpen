#include "CairnUtils.h"
#include "GlobalSettings.h"
#include <sstream>
#include <fstream>

namespace CairnUtils {

    int generateTimeStamp() {
        std::chrono::high_resolution_clock::time_point time_stamp = std::chrono::high_resolution_clock::now();
        long sec_timeStamp = std::chrono::duration_cast<std::chrono::seconds>(time_stamp.time_since_epoch()).count();
        return sec_timeStamp;
    }

    std::string addTimeStampToFileName(std::string aFullFileName)
	{
        fs::path fullFileName(aFullFileName);

        //Get a time stamp and rename the file
        std::string extenstion = fullFileName.extension().string();
        std::string baseName = fullFileName.stem().string() + std::string("_") + std::to_string(generateTimeStamp()) + extenstion;
        fullFileName.replace_filename(baseName);

        return fullFileName.string();
    }

    bool openFileForWriting(std::fstream& aFileOut, const std::string& a_FileName, std::ios::openmode a_openMode) {

        aFileOut.open(a_FileName, a_openMode );
        if (!aFileOut.is_open()) {
            cWarning() << "Couldn't open " + a_FileName + " for writing!";
            aFileOut.close(); //for safety!
            //Add a time stamp to the file name
            std::string vFileName = addTimeStampToFileName(a_FileName);
            cInfo() << "A timestamp has been added to the filename: " + vFileName;

            aFileOut.open(vFileName, a_openMode);
            if (!aFileOut.is_open()) {
                cWarning() << "Couldn't open " + vFileName + " for writing!";
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

    void outputIndicator(std::fstream& out, const std::string compoName, const std::string indicatorName, const double value, 
        const std::string unit, const std::string alias, const std::string Description, const std::vector<std::string>& labels)
    {
        out << CairnUtils::simplified(compoName) << ";" << CairnUtils::simplified(indicatorName) << ";";
        if (!value)  out << "0";
        else {
            if (value == value)
                out << value;
            else
                out << "nan";
        }        
        out << ";" << CairnUtils::simplified(unit) << ";" << CairnUtils::simplified(alias);
        if (Description != "N/A") out << ";" << Description;
        for (auto const& vlabel : labels) {
            out << ";" << vlabel;
        }
        out << "\n";
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

    std::string simplified(const std::string& a_string)
    {
        std::string vRet = a_string;
        ltrim(vRet);
        rtrim(vRet);
        vRet = CairnUtils::replace(vRet, "  ", " ");
        vRet = CairnUtils::replace(vRet, "\t", "");
        vRet = CairnUtils::replace(vRet, "\r", "");
        vRet = CairnUtils::replace(vRet, "\n", "");
        return vRet;
    }


    std::string toUpper(const std::string& a_string)
    {
        std::string vTmp(a_string);
        std::transform(a_string.begin(), a_string.end(), vTmp.begin(), ::toupper);
        return vTmp;
    }
    std::vector<std::string> split(const std::string& a_string, const char& a_separator)
    {        
        return split(a_string, std::string{ a_separator });
    }

    std::vector<std::string> split(const std::string& a_string, const std::string& a_separator)
    {
        size_t pos_start = 0, pos_end, delim_len = a_separator.length();
        std::string token;
        std::vector<std::string> res;

        while ((pos_end = a_string.find(a_separator, pos_start)) != std::string::npos) {
            token = a_string.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        res.push_back(a_string.substr(pos_start));
        return res;
    }

    std::string BuildFileName(const std::string &aFileName)
    {
        if (aFileName == "") return aFileName;

        fs::path filename(aFileName);
        if (filename.has_filename()) {
            if (filename.is_relative())
            {
                filename = fs::current_path() / filename;
            }
            return filename.string();
        }
        else
            return "";
    }
    std::vector<std::vector<std::string>> readFromCsvFile(const std::string& aFileName, const std::string& sep)
    {
        std::vector<std::vector<std::string>>  data_Inputs;
        std::string filename = BuildFileName(aFileName);
        if (filename == "") return data_Inputs;
        
        fs::path vPath(filename);
        if (!fs::exists(vPath))
        {
            return data_Inputs;
        }
        else
        {
            cInfo() << " Reading csv file " << filename;
        }
        data_Inputs = readToList(filename, sep);
        return data_Inputs;
    }
    std::vector<std::vector<std::string>> readToList(const std::string& Full_File_Name, const std::string& Separator)
    {
        std::vector<std::vector<std::string>> data_input;        
        std::vector<std::string> fields;
        std::fstream File(Full_File_Name, std::ios_base::in);

        if (!File.is_open())
        {
            //cInfo() << "Error CSV File could not be opened for reading " ;
            //return {{"ERROR"}};
            Cairn_Exception error("Error CSV File could not be opened for reading: " + Full_File_Name, -1);
            throw error;
        }

        int k = 1;
        std::string line;
        while (std::getline(File, line)) 
        {
            fields = split(line, Separator);
            if (contains(fields, "Error\n"))           
            {
                cInfo() << "Error reading line " << line;
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

    std::string upperCase(const std::string& str)
    {
        std::string upper_case_str = "";
        for (int i = 0; i < str.length(); i++) {
            upper_case_str += std::toupper(str[i]);
        }
        return upper_case_str;
    }

  

}

