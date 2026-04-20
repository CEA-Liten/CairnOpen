#include "TimeSeriesReaderCSV.h"
#include "GlobalSettings.h"
#include "CairnUtils.h"
#include "Cairn_Exception.h"

int TimeSeriesReaderCSV::iSkipHead = 4;// à déterminer autrement !

TimeSeriesReaderCSV::TimeSeriesReaderCSV()
{
}

bool TimeSeriesReaderCSV::open(const std::wstring& aTSfile)
{
    m_Data = CairnUtils::readFromCsvFile_W(aTSfile, ";");

    return m_Data.size() > 0;
}

void TimeSeriesReaderCSV::readHeader(const t_mapExchange& aListSubscribedVariables, std::vector<TimeSeriesDescrp>& aHeader)
{
    size_t vSize = m_Data.at(0).size();
    aHeader.resize(vSize);
    for (size_t i = 0;i < vSize;i++) {
        aHeader[i].Name = m_Data.at(0).at(i);
        if (m_Data.at(2).size() > i) aHeader[i].Unit = m_Data.at(2).at(i);
        else aHeader[i].Unit = "";
        aHeader[i].Index = i;
    }  
}

bool TimeSeriesReaderCSV::readTimes(std::vector<double>& aTimes, const double& aStartTime, 
    const double& aEndTime, const double& aTimeStep)
{    
   readTimeColumn(aStartTime, aEndTime, aTimeStep, iSkipHead, aTimes);
   return true;
}

void TimeSeriesReaderCSV::readValues(int a_index, std::vector<double>& aValues)
{    
    readTSColumn(a_index, iSkipHead, aValues);

    // TODO ?
    //if (zeVarName_Inputs.size() != time_Inputs.size()) {
    //    cInfo() << "The number of lines read for " + zeVarName + " is " + std::to_string(zeVarName_Inputs.size()) + ". For each column, the reading stops when an empty cell is found.";
    //    //Resize to discard all values beyond Time column
    //    cInfo() << "Resize the data of " + zeVarName + " to the length of the column Time (discard all the lines beyond the last considered value on column Time).";
    //    zeVarName_Inputs.resize(time_Inputs.size());
    //}

}

void TimeSeriesReaderCSV::close()
{
    m_Data.clear();
}

int TimeSeriesReaderCSV::getNumLine(int aNumLine)
{
    return iSkipHead+aNumLine;
}

void TimeSeriesReaderCSV::readTimeColumn(const double& aStartTime, const double& aEndTime, 
    const double& aTimeStep, const int& aRowSkipped, std::vector<double>& aValues)
{
    aValues.clear();

    const std::size_t rowSkipped = (aRowSkipped > 0 ? static_cast<std::size_t>(aRowSkipped) : 0);
    const std::size_t maxRow = m_Data.size();
    if (rowSkipped >= maxRow) {
        throw Cairn_Exception("Row skip (" + std::to_string(aRowSkipped) + ") is beyond available rows (" 
            + std::to_string(maxRow) + ")", -1);
    }

    if (aEndTime > 0 && aEndTime < aStartTime) {
        throw Cairn_Exception("End time (" + std::to_string(aEndTime) + ") is less than start time (" 
            + std::to_string(aStartTime) + ")", -1);
    }

    aValues.reserve(maxRow - rowSkipped);

    const std::size_t col = 0;
    double firstTime = aTimeStep;
    for (std::size_t i = rowSkipped; i < maxRow; ++i)
    {
        const auto& row = m_Data[i];
        if (row.size() <= col) continue; //skip line

        const std::string& value = row[col];
        if (value.empty()) {
            throw Cairn_Exception("Empty time value at row " + std::to_string(i + 1) + ", col " + std::to_string(col), -1);
        }

        double dvalue{};
        try {
            dvalue = std::stod(value);
        }
        catch (const std::invalid_argument&) {
            throw Cairn_Exception("Non-numeric time '" + value + "' at row " +
                std::to_string(i + 1) + ", col " + std::to_string(col), -1);
        }
        catch (const std::out_of_range&) {
            throw Cairn_Exception("Out-of-range time '" + value + "' at row " +
                std::to_string(i + 1) + ", col " + std::to_string(col), -1);
        }

        if (!std::isfinite(dvalue)) {
            throw Cairn_Exception("Non-finite time (NaN/Inf) at row " +
                std::to_string(i + 1) + ", col " + std::to_string(col), -1);
        }

        // ------- Add value and set star and end rows -------
        if (i == rowSkipped && firstTime < 0.0) {
            firstTime = dvalue;
        }

        if (dvalue < aStartTime) {
            continue; // skip until we reach start time
        }
        else if (m_startRow < 0) {
            m_startRow = static_cast<int>(i - rowSkipped); // set only once
        }

        if (aEndTime > 0) { // if aEndTime < 0, read until the end
            if (m_endRow < 0 && dvalue >= aEndTime) {
                m_endRow = static_cast<int>(i - rowSkipped); // set only once
            }
            if (dvalue > aEndTime) {
                break; // past the range
            }
        }

        // Normalize: dvalue mapped to [aStartTime -> firstTime]
        const double normalizedValue = (aStartTime > 0) ? dvalue - aStartTime + firstTime : dvalue;
        aValues.push_back(normalizedValue);
    }
}

void TimeSeriesReaderCSV::readTSColumn(int aCol, int aRowSkipped, std::vector<double> &aValues)
{
    aValues.clear();

    if (aCol < 0) {
        throw Cairn_Exception("Negative column index: " + std::to_string(aCol), -1);
    }

    // Normalize start/end rows
    const std::size_t baseStart = (m_startRow < 0 ? 0 : static_cast<std::size_t>(m_startRow));
    const std::size_t rowSkipped = (aRowSkipped > 0 ? static_cast<std::size_t>(aRowSkipped) : 0);
    const std::size_t startRow = baseStart + rowSkipped;

    const std::size_t maxRow = m_Data.size();
    const std::size_t endRow = (m_endRow > 0)
        ? std::min(static_cast<std::size_t>(m_endRow) + rowSkipped, maxRow)
        : maxRow;

    if (startRow >= endRow) {
        cDebug() << "readTSColumn: startRow (" << startRow << ") >= endRow (" << endRow << "), nothing to read.";
        return;
    }

    // Reserve for performance
    aValues.reserve(endRow - startRow);

    for (int i = startRow; i < endRow; ++i)
    {
        const auto& row = m_Data[i];
        if (aCol >= static_cast<int>(row.size())) {
            throw Cairn_Exception("Column " + std::to_string(aCol) + " out of range at row " + std::to_string(i + 1) + "; a cell is missing!", -1);
        }

        const std::string& value = row[aCol];
        if (value.empty()) {
            throw Cairn_Exception("Empty value at row " + std::to_string(i + 1) + ", col " + std::to_string(aCol), -1);
        }

        double dValue{};
        try {
            dValue = std::stod(value);
        }
        catch (const std::invalid_argument&) {
            throw Cairn_Exception("Non-numeric value '" + value + "' at row " + std::to_string(i + 1) + ", col " + std::to_string(aCol), -1);
        }
        catch (const std::out_of_range&) {
            throw Cairn_Exception("Out-of-range value '" + value + "' at row " + std::to_string(i + 1) + ", col " + std::to_string(aCol), -1);
        }

        if (!std::isfinite(dValue)) {
            throw Cairn_Exception("Non-finite value (NaN/Inf) at row " + std::to_string(i + 1) + ", col " + std::to_string(aCol), -1);
        }

        aValues.push_back(dValue);
    }
}
