#include "TimeSeriesReaderCSV.h"
#include "GlobalSettings.h"
#include "CairnUtils.h"

int TimeSeriesReaderCSV::iSkipHead = 4;// à déterminer autrement !

TimeSeriesReaderCSV::TimeSeriesReaderCSV()
{
}

bool TimeSeriesReaderCSV::open(const std::string& aTSfile)
{
    m_Data = CairnUtils::readFromCsvFile(aTSfile, ";");

    return m_Data.size()>0;
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

bool TimeSeriesReaderCSV::readTimes(std::vector<double>& aTimes)
{    
   readColumn(0, iSkipHead, aTimes);  
   return true;
}

void TimeSeriesReaderCSV::readValues(int a_index, std::vector<double>& aValues)
{    
    readColumn(a_index, iSkipHead, aValues);

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

void TimeSeriesReaderCSV::readColumn(int aCol, int aRowSkipped, std::vector<double> &aValues)
{
    aValues.clear();
    for (int i = aRowSkipped; i < m_Data.size(); ++i)
    {
        std::string value = (m_Data.at(i)).at(aCol);
        if (value == "")
        {
            break;
        }
        else if (std::isnan(std::stod(value))) {
            cDebug() << "A NAN value is found at row" << i + 1 << ", column" << aCol << "while reading the input time series.";
            break;
        }
        else
        {
            aValues.push_back(std::stod(value));
        }
    }
}
