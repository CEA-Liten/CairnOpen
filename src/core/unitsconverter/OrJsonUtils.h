#pragma once
#include <vector>
#include <string>
#include <sstream>
#include "json.hpp"
using json = nlohmann::json;

namespace orjson {
    template <typename T>
    inline bool json2double(const json& j, const std::string& a_key, T& a_value)
    {
        std::string vStr;
        j.at(a_key).get_to(vStr);
        char* end{};
        a_value = std::strtod(vStr.c_str(), &end);
        return (vStr.c_str() != end);
    }

    template <typename T>
    inline bool json2str(const json& j, const std::string& a_key, T& a_value)
    {
        bool vRet = false;
        int iValue = 0;
        try
        {
            j.at(a_key).get_to(iValue);
            std::istringstream stream(std::to_string(iValue));
            stream >> a_value;
            vRet = true;
        }
        catch (const std::exception&)
        {
            double vValue = 0.0;
            j.at(a_key).get_to(vValue);
            std::istringstream stream(std::to_string(vValue));
            stream >> a_value;
            vRet = true;
        }
        return vRet;
    }



    template <typename T>
    inline bool json2int(const json& j, const std::string& a_key, T& a_value)
    {
        std::string vStr;
        j.at(a_key).get_to(vStr);
        char* end{};
        a_value = (int)std::strtol(vStr.c_str(), &end, 10);
        return (vStr.c_str() != end);
    }

    template <typename T>
    inline bool json2bool(const json& j, const std::string& a_key, T& a_value)
    {
        bool vRet = false;
        try
        {
            std::string vStr;
            j.at(a_key).get_to(vStr);
            char* end{};
            int vValue = std::strtol(vStr.c_str(), &end, 10);
            if (vStr.c_str() != end) {
                // cas "1" ou "0"
                a_value = (vValue == 1);
                vRet = true;
            }
            else {
                // cas "true", "false"
                std::transform(vStr.begin(), vStr.end(), vStr.begin(), ::tolower);
                std::istringstream is(vStr);
                is >> std::boolalpha >> a_value;
                vRet = true;
            }
        }
        catch (const json::exception&)
        {
            // cas 1 ou 0
            double vValue = 0;
            j.at(a_key).get_to(vValue);
            a_value = (vValue == 1.0);
            vRet = true;
        }
        return vRet;
    }

    template <typename T>
    inline bool from_json(const json& j, const std::string& a_key, T& a_value, bool a_mandatory = false)
    {
        bool vRet = false;
        if (j.contains(a_key)) {
            try
            {
                j.at(a_key).get_to(a_value); // essai dans le type attendu
                vRet = true;
            }
            catch (const json::exception&)
            {
                try
                {
                    // essai de conversion
                    if (std::is_same_v<std::string, T>) {
                        vRet = json2str(j, a_key, a_value);
                    }
                    else if (std::is_same_v<double, T>) {
                        vRet = json2double(j, a_key, a_value);
                    }
                    else if (std::is_same_v<int, T> || std::is_same_v<unsigned int, T>) {
                        vRet = json2int(j, a_key, a_value);
                    }
                    else if (std::is_same_v<bool, T>) {
                        vRet = json2bool(j, a_key, a_value);
                    }
                }
                catch (const json::exception&)
                {
                    vRet = false;
                }
            }
        }
        if (a_mandatory && !vRet) {
            // un paramètre obligatoire n'a pas été initialisé
            throw std::range_error("Mandatory parameter " + a_key + " not initialized");
        }
        return vRet;
    }

    template <typename T>
    inline bool from_json(const json& j, const std::string& a_key, std::vector<T>& a_value, bool a_mandatory = false)
    {
        bool vRet = false;
        if (j.contains(a_key)) {
            try
            {
                j.at(a_key).get_to(a_value);
                vRet = true;
            }
            catch (const json::exception&)
            {
                if (std::is_same_v<std::string, T>) {
                    std::vector<double> vStrs;
                    try
                    {
                        j.at(a_key).get_to(vStrs);
                        vRet = true;
                        std::vector<std::string>& vValues = (std::vector<std::string>&)(a_value);
                        for (auto& vElem : vStrs) {
                            vValues.push_back(std::to_string(vElem));
                        }
                    }
                    catch (const std::exception&)
                    {

                    }
                }
                else if (std::is_same_v<double, T>) {
                    std::vector<std::string> vStrs;
                    try
                    {
                        j.at(a_key).get_to(vStrs);
                        vRet = true;
                        std::vector<double>& vValues = (std::vector<double>&)(a_value);
                        for (const std::string& vElem : vStrs) {
                            char* vEnd = (char*)vElem.c_str();
                            vValues.push_back(std::strtod(vElem.c_str(), &vEnd));
                            vRet &= (vElem.c_str() != vEnd);
                        }
                    }
                    catch (const std::exception&)
                    {

                    }
                }
                else if (std::is_same_v<int, T>) {
                    std::vector<std::string> vStrs;
                    try
                    {
                        j.at(a_key).get_to(vStrs);
                        vRet = true;
                        std::vector<int>& vValues = (std::vector<int>&)(a_value);
                        for (const std::string& vElem : vStrs) {
                            char* vEnd = (char*)vElem.c_str();
                            vValues.push_back(std::strtol(vElem.c_str(), &vEnd, 10));
                            vRet &= (vElem.c_str() != vEnd);
                        }
                    }
                    catch (const std::exception&)
                    {

                    }
                }
            }
        }
        if (a_mandatory && !vRet) {
            // un paramètre obligatoire n'a pas été initialisé
            throw std::range_error("Mandatory parameter " + a_key + " not initialized");
        }
        return vRet;
    }

}