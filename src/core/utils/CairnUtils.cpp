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

    bool openFileForWriting(std::fstream& file,
        const std::string& fileName,
        std::ios::openmode mode)
    {
        file.open(fileName, mode);
        if (file.is_open()) {
            return true;
        }

        cWarning() << "Couldn't open " << fileName << " for writing!";
        file.close(); // safety

        const std::string stampedName = addTimeStampToFileName(fileName);
        cInfo() << "A timestamp has been added to the filename: " << stampedName;

        file.open(stampedName, mode);
        if (file.is_open()) {
            return true;
        }

        cWarning() << "Couldn't open " << stampedName << " for writing!";
        return false;
    }

    std::string parseIndicatorName(const std::string& indicatorName, const bool isSizeOptimized)
    {
        static const std::unordered_map<std::string, std::string> optimizedNames = {
            {"Installed Size",     "Installed Optimal Size"},
            {"Component Weight",   "Component Optimal Weight"},
            {"Storage Capacity",   "Storage Optimal Capacity"}
        };

        std::string name = indicatorName;
        auto it = optimizedNames.find(name);
        if (it != optimizedNames.end() && isSizeOptimized) {
            name = it->second;
        }

        return trim(name);
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

    std::string toUpper(const std::string& a_string) {
        std::string vTmp(a_string);
        std::transform(a_string.begin(), a_string.end(), vTmp.begin(), ::toupper);
        return vTmp;
    }

    std::string toLower(const std::string& a_string) {
        std::string vTmp(a_string);
        std::transform(a_string.begin(), a_string.end(), vTmp.begin(), ::tolower);
        return vTmp;
    }

    std::string remove_spaces(const std::string& a_string) {
        std::string vRet(a_string);
        vRet.erase(std::remove(vRet.begin(), vRet.end(), ' '), vRet.end());
        return vRet;
    }

    std::string trim(const std::string& a_string) {
        std::string vRet = a_string;
        ltrim(vRet);
        rtrim(vRet);
        return vRet;
    }

    std::string simplified(const std::string& a_string) {
        std::string vRet = a_string;
        ltrim(vRet);
        rtrim(vRet);
        vRet = CairnUtils::replace(vRet, "  ", " ");
        vRet = CairnUtils::replace(vRet, "\t", "");
        vRet = CairnUtils::replace(vRet, "\r", "");
        vRet = CairnUtils::replace(vRet, "\n", "");
        return vRet;
    }

    std::string to_string_trim(const double& num, int sig) {
        char buf[64];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf),
            num, std::chars_format::general, sig);
        if (ec != std::errc{}) return {}; // handle error if needed
        return std::string(buf, ptr);
    }

    std::string replace(std::string& a_string, const std::string& a_find, const std::string& a_replace, bool a_toUpper) {
        if (a_toUpper) {
            std::transform(a_string.begin(), a_string.end(), a_string.begin(), ::toupper);
        }
        if (!a_find.empty())
            for (size_t pos = 0; (pos = a_string.find(a_find, pos)) != std::string::npos; pos += a_replace.size())
                a_string.replace(pos, a_find.size(), a_replace);
        return a_string;
    }

    std::string joinStrings(const std::vector< std::string>& a_List, const std::string& a_separator) {
        if (a_List.empty()) {
            return "[]";
        }

        std::string vRet = "[" + a_List[0];
        for (size_t i = 1; i < a_List.size(); ++i) {
            vRet += a_separator + a_List[i];
        }
        return vRet + "]";
    }

    bool contains(const std::vector< std::string>& a_List, const std::string &a_Find) {
        std::vector<std::string>::const_iterator vIter = find(a_List.begin(), a_List.end(), a_Find);
        return (vIter != a_List.end());
    }

    bool contains(const std::string& a_string, const std::string& a_Find, bool a_toUpper) {
        if (a_toUpper) {
            std::string vTmp(a_string);
            std::transform(a_string.begin(), a_string.end(), vTmp.begin(), ::toupper);
            return vTmp.find(a_Find) != std::string::npos;
        }
        else
            return a_string.find(a_Find) != std::string::npos;
    }

    bool contains(const std::string& a_string, const std::vector<std::string>& a_FindOneInList, bool a_toUpper) {
        for (auto& vFind : a_FindOneInList) {
            if (contains(a_string, vFind, a_toUpper))
                return true;
        }
        return false;
    }

    void removeMatchingSubstring(std::vector<std::string>& list, const std::string& substring) {
        list.erase(
            std::remove_if(list.begin(), list.end(), [&](std::string s) { return contains(s, substring); }),
            list.end()
        );
    }


    std::vector<std::string> split(const std::string& a_string, const char& a_separator) {        
        return split(a_string, std::string{ a_separator });
    }

    std::vector<std::string> split(const std::string& a_string, const std::string& a_separator) {
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

    std::string BuildFileName(const std::string& aFileName) {
        return BuildFileName_W(toWString(aFileName));
    }

    std::string BuildFileName_W(const std::wstring& aFileName) {
        if (aFileName.empty()) return "";

        fs::path filename(aFileName);
        if (filename.has_filename()) {
            if (filename.is_relative()) {
                filename = fs::current_path() / filename;
            }
            return filename.string();
        }
        return "";
    }

    std::vector<std::vector<std::string>> readFromCsvFile(const std::string& aFileName, const std::string& sep) 
    {
        return readFromCsvFile_W(toWString(aFileName), sep);
    }

    std::vector<std::vector<std::string>> readFromCsvFile_W(const std::wstring& aFileName, const std::string& sep) 
    {
        std::vector<std::vector<std::string>>  data_Inputs;
        std::string filename = BuildFileName_W(aFileName);

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

        std::ifstream File(Full_File_Name, std::ios::binary); // open in binary to detect BOM
        if (!File.is_open()) {
            throw Cairn_Exception("Error CSV File could not be opened for reading: " + Full_File_Name, -1);
        }

        // Detect and skip BOM
        const std::string encoding = detectBOM(File);
        cInfo() << "CSV encoding detected:" << encoding;

        int k = 1;
        std::string line;
        while (std::getline(File, line))
        {
            // Strip \r for Windows CR LF files
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            std::vector<std::string> fields = split(line, Separator);
            for (auto& elem : fields) {
                elem = simplified(elem);
            }

            if (k > 4) // data lines
            {
                for (int i = 0; i < (int)fields.size(); i++)
                {
                    if (contains(fields[i], ",") || contains(fields[i], ";"))
                    {
                        std::string errorMessage = "Error while importing input time series: "
                            + Full_File_Name
                            + "\nPlease verify that the correct separator (" + Separator + ") is used"
                            + " and that comma is not used for decimal values."
                            + (i == 0 ? " Line: " + fields[0] : " Value: " + fields[i]);
                        throw Cairn_Exception(errorMessage, -1);
                    }
                }
            }

            data_input.push_back(std::move(fields)); // move instead of copy
            k++;
        }

        return data_input;
    }

    std::vector<double> getDataArray(const std::vector<std::vector<std::string>>& data_Inputs, int aCol, int iskipHead) {
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

    /* Methods related to the levelization and discount factor computations */

    int Newton(const double& aValue, const uint& aIarg, uint aOffset,
        const double aExtrapolateOverYear, double& X_Set,
        double (*Y_func)(const double&, const uint&, unsigned int, const double),
        bool (*Y_Test)(const double&, const double&))
    {
        /*
         * Newton's method - secant (no analytical derivative calculation available)
         * + quadratic convergence
         * - 3 function calls at each iteration to handle the minimum and maximum thresholds at the boundaries of the domain
         *
         * Search for X_Set such that Y_Set(X_Set) < Epsilon using an approximated Newton scheme
         *
         * @X_Set  : starting point of Newton
         * @Y_func : function to solve
         * @Y_Test : testing function to stop Newton iterative process
         *
         * Output : int
         *     0 if converged, negative value if not converged (equal to -iteration number after non convergence)
        */

        const int    kMaxIter = 20;
        const double kEpsX = 1e-2;
        const double kMinDer = 1e-20;

        int    iter = 0;
        double Y = 0.0;
        double dX = 0.0;

        while (true)
        {
            if (++iter > kMaxIter) {
                cDebug() << "Newton failed to converge after" << iter
                    << "iterations. Y=" << Y
                    << "X=" << X_Set
                    << "dX=" << dX;
                return -iter;
            }

            // Central finite difference derivative
            const double Ym = Y_func(X_Set - kEpsX, aIarg, aOffset, aExtrapolateOverYear);
            const double Yp = Y_func(X_Set + kEpsX, aIarg, aOffset, aExtrapolateOverYear);
            const double dY = (Yp - Ym) / (2.0 * kEpsX);

            Y = Y_func(X_Set, aIarg, aOffset, aExtrapolateOverYear);

            if (std::fabs(dY) > kMinDer) {
                dX = -(Y - aValue) / dY;
            }
            else {
                dX = 0.0; // derivative too small : freeze step
            }

            if (Y_Test(Y, aValue)) {
                return 0; // converged
            }

            X_Set += dX;
        }
    }

    bool levelization_test(const double& aSum, const double& aValue)
    {
        if (abs(aSum - aValue) < 1.e-4)
            return true;
        else
            return false;
    }

    double levelization(const double& aDiscountRate, const unsigned int& Nyear, unsigned int aOffset, const double aExtrapolateOverYear)
    {
        double sum = 0.;

        if (Nyear <= 1) return 1.;

        for (unsigned int i = 0 + aOffset; i < Nyear + aOffset; i++)
        {
            sum += 1. / pow((1. + aDiscountRate), i);
        }

        return sum * aExtrapolateOverYear;
    }

    double discountRate(const double& aDiscountFactor, const unsigned int& aNyear, unsigned int aOffset, const double aExtrapolateOverYear)
    {
        double discountRate = 0.2;

        if (aDiscountFactor > 0.) {
            int iconv = Newton(aDiscountFactor, aNyear, aOffset, aExtrapolateOverYear, discountRate, &levelization, &levelization_test);

            if (iconv < 0) cWarning() << "Non convergence in discountrate computation" << iconv;
        }
        else {
            discountRate = 0.;
        }

        return discountRate;
    }

    double levelization(const double  aDiscountRate, const unsigned int Nyear, const unsigned int aAbsoluteCurrentTimeStep,
        const unsigned int aNbYearInput, const unsigned int aLeapYearPos, unsigned int aOffset, const double aExtrapolateOverYear)
    {
        if (Nyear <= 1) return 1.0;

        const double extra = (aNbYearInput > 1) ? 1.0 : aExtrapolateOverYear;

        // --- Build cumulative hours per year ---
        int hours = 0;
        std::vector<int> cumulHoursPerYear(aNbYearInput);

        for (unsigned int i = 0; i < aNbYearInput; ++i) {
            hours += (i == aLeapYearPos - 1) ? 8784 : 8760;
            cumulHoursPerYear[i] = hours;
        }

        // --- Determine which simulated year we are in ---
        unsigned int NbYearSimu = 0;

        while (NbYearSimu < aNbYearInput &&
            aAbsoluteCurrentTimeStep >= cumulHoursPerYear[NbYearSimu]) {
            ++NbYearSimu;
        }

        // --- Compute levelized discount factors ---
        const unsigned int maxBlock = std::ceil(double(Nyear + aOffset) / aNbYearInput);
        std::vector<double> levelizedFactors(aNbYearInput + 1, 0.0);

        for (unsigned int j = 0; j < levelizedFactors.size(); ++j) {
            double sum = 0.0;
            for (unsigned int i = aOffset; i < maxBlock; ++i) {
                const unsigned int exponent = aNbYearInput * i + j;
                sum += 1.0 / std::pow(1.0 + aDiscountRate, exponent);
            }
            levelizedFactors[j] = sum;
        }

        return levelizedFactors[NbYearSimu] * extra;
    }

    std::vector<double> levelizationTable(const double  aDiscountRate, const unsigned int Nyear,
        const unsigned int aNbYearInput, const unsigned int aLeapYearPos, unsigned int aOffset,
        const double aExtrapolateOverYear)
    {
        if (Nyear <= 1) return std::vector<double>(1, 1.0);

        const double extra = (aNbYearInput > 1) ? 1.0 : aExtrapolateOverYear;

        // --- Build cumulative hours per input year ---
        std::vector<int> cumulHoursPerYear(aNbYearInput);
        int hours = 0;
        unsigned int leapPos = aLeapYearPos;

        for (unsigned int i = 0; i < aNbYearInput; ++i) {
            if (i == leapPos - 1) {
                hours += 8784;
                leapPos += 4; // next leap year
            }
            else {
                hours += 8760;
            }
            cumulHoursPerYear[i] = hours;
        }

        // --- Compute levelized discount factors ---
        std::vector<double> levelizedFactors(aNbYearInput, 0.0);

        for (unsigned int j = 0; j < aNbYearInput; ++j) {
            double sum = 0.0;
            // accumulate discount factors while within simulation horizon
            unsigned int exponent = j;
            while (exponent < Nyear) {
                sum += 1.0 / std::pow(1.0 + aDiscountRate, exponent);
                exponent += aNbYearInput;
            }
            levelizedFactors[j] = sum * extra;
        }

        return levelizedFactors;
    }

    std::vector<double> yearHourTable(unsigned int aNbYearInput, unsigned int aLeapYearPos)
    {
        std::vector<double> cumulHoursPerYear(aNbYearInput, 0.0);

        int hours = 0;
        for (unsigned int i = 0; i < aNbYearInput; ++i) {
            const bool isLeapYear = (i == aLeapYearPos - 1);
            hours += isLeapYear ? 8784 : 8760;
            cumulHoursPerYear[i] = hours;
        }

        return cumulHoursPerYear;
    }

    /* Methods related to gradient matrix computations */

    double Energy(MatrixXf* positions, MatrixXf* distances) {
        const int n = distances->rows();
        const float maxD = distances->maxCoeff();
        if (maxD == 0.f) return 0.0;   // avoid division by zero

        MatrixXf D = (*distances) / maxD;

        MatrixXf distXY(n, n);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                const float dx = (*positions)(i, 0) - (*positions)(j, 0);
                const float dy = (*positions)(i, 1) - (*positions)(j, 1);
                distXY(i, j) = std::sqrt(dx * dx + dy * dy);
            }
        }

        double E = 0.0;
        const float invSqrt2 = 1.0f / std::sqrt(2.0f);

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (i == j) continue;

                const float diff = distXY(i, j) - (*distances)(i, j);
                E += invSqrt2 * (diff * diff) / D(i, j);
            }
        }

        return E;
    }

    MatrixXf gradient( double (*Y_func)(MatrixXf*, MatrixXf*),
        MatrixXf* pos, MatrixXf* param, double* dx ) 
    {
        const int n = pos->rows();
        MatrixXf gradY(n, 2);

        // Precompute the base energy once
        const double baseY = Y_func(pos, param);

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < 2; ++j) {
                MatrixXf Xdx = *pos; 
                Xdx(i, j) += *dx;
                const double y = Y_func(&Xdx, param);
                gradY(i, j) = (y - baseY) / (*dx);
            }
        }

        return gradY;
    }

    GradDescResult GradientDescent( double (*Y_func)(MatrixXf*, MatrixXf*),
        MatrixXf* init, MatrixXf* param, int nbMaxIterations, double gapStop, 
        double dx, double alpha)
    {
        // Initialize trajectory
        std::vector<MatrixXf> X;
        std::vector<double>   Y;

        X.reserve(nbMaxIterations + 1);
        Y.reserve(nbMaxIterations + 1);

        X.push_back(*init);
        Y.push_back(Y_func(&X[0], param));

        int iter = 1;
        double gap = std::numeric_limits<double>::infinity();
        bool cond = true;

        MatrixXf grad = gradient(Y_func, init, param, &dx);

        const auto t0 = std::chrono::high_resolution_clock::now();
        unsigned int elapsed = 0;

        while (std::abs(gap) > gapStop &&
            iter < nbMaxIterations &&
            cond &&
            elapsed < 120)
        {
            // Time update
            const auto t1 = std::chrono::high_resolution_clock::now();
            elapsed = std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count();

            // Gradient descent step
            X.push_back(X.back() - alpha * grad);

            // Evaluate new energy
            Y.push_back(Y_func(&X.back(), param));

            // Compute gap
            gap = Y.back() - Y[Y.size() - 2];

            // Stop if energy increases
            if (Y.back() > Y[Y.size() - 2])
                cond = false;

            // Update gradient
            grad = gradient(Y_func, &X.back(), param, &dx);

            ++iter;
        }

        cDebug() << "Gradient stops:" << gap << "iter" << iter << "cond" << cond;
        cDebug() << "time:" << elapsed;

        GradDescResult result;
        result.X = std::move(X);
        result.Y = std::move(Y);
        result.gap = gap;
        result.iteration = iter;
        result.condition = cond;
        return result;
    }
    /* ------------------------------------------------------ */

    void collectParameters(
        std::vector<ParameterRow>& rows,
        const std::string& componentName,
        const std::map<std::string, ModelParam*>& paramMap,
        const std::map<std::string, bool>& optionsMap,
        const std::map<std::string, std::string>& timeSeriesNames,
        const std::vector<std::string>& labelList,
        const std::map<std::string, std::string>& labelValueMap)
    {
        auto getOption = [&](const std::string& key, bool defaultValue) {
            auto it = optionsMap.find(key);
            return (it != optionsMap.end()) ? it->second : defaultValue;
        };

        bool onlyIsUsed = getOption("OnlyUsedParams", false);

        for (auto const& [key, param] : paramMap) {
            // Filter: only used parameters
            if (!onlyIsUsed || param->IsUsed() ||
                (timeSeriesNames.find(key) != timeSeriesNames.end() &&
                    timeSeriesNames.at(key) != ""))
            {
                ParameterRow row;
                row.component = componentName;
                row.parameter = key;
                row.unit = param->getUnit();
                row.description = param->getDescription();
                row.mandatory = param->IsBlocking();

                // Determine value
                auto tsIt = timeSeriesNames.find(key);
                if (tsIt != timeSeriesNames.end()) {
                    row.value = tsIt->second;
                }
                else {
                    row.value = param->toString();
                }

                // Collect label values
                for (const auto& label : labelList) {
                    auto labelIt = labelValueMap.find(label);
                    row.labels[label] = (labelIt != labelValueMap.end()) ? labelIt->second : "";
                }

                rows.push_back(row);
            }
        }
    }

    void writeParameterDataToCSV(
        std::ostream& out,
        const ExportParameterData& data,
        const std::map<std::string, bool>& optionsMap)
    {
        auto getOption = [&](const std::string& key, bool defaultValue) {
            auto it = optionsMap.find(key);
            return (it != optionsMap.end()) ? it->second : defaultValue;
        };

        const bool showMandatory = getOption("ShowMandatory", true);
        const bool showLabels = getOption("ShowLabels", true);

        // Write header
        out << "Component;Parameter;Value;Unit;Description";

        if (showMandatory) {
            out << ";Mandatory";
        }

        if (showLabels) {
            for (const auto& label : data.labelHeaders) {
                out << ";" << label;
            }
        }

        // Write extra headers
        for (const auto& header : data.extraHeaders) {
            out << ";" << header;
        }

        out << "\n";

        // Write rows
        for (const auto& row : data.rows) {
            out << row.component << ";"
                << row.parameter << ";"
                << row.value << ";"
                << row.unit << ";"
                << row.description;

            if (showMandatory) {
                out << ";" << (row.mandatory ? "true" : "false");
            }

            if (showLabels) {
                for (const auto& label : data.labelHeaders) {
                    auto it = row.labels.find(label);
                    std::string val = (it != row.labels.end()) ? it->second : "";
                    out << ";" << val;
                }
            }

            // Write extra data
            for (const auto& header : data.extraHeaders) {
                auto it = row.extraData.find(header);
                std::string val = (it != row.extraData.end()) ? it->second : "";
                out << ";" << val;
            }

            out << "\n";
        }
    }

}

