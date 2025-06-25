#include "CairnUtils.h"
#include "GlobalSettings.h"

namespace CairnUtils {

    int generateTimeStamp() {
        std::chrono::high_resolution_clock::time_point time_stamp = std::chrono::high_resolution_clock::now();
        long sec_timeStamp = std::chrono::duration_cast<std::chrono::seconds>(time_stamp.time_since_epoch()).count();
        return sec_timeStamp;
    }

    QString addTimeStampToFileName(QString aFileName)
	{
        QFileInfo fileNameInfo = QFileInfo(aFileName);
        QString fileDir = fileNameInfo.absoluteDir().path();
        QString baseName = fileNameInfo.baseName();
        QString extenstion = fileNameInfo.suffix();
        //Get a time stamp and rename the file
        QString filename_ts = fileDir + "/" + baseName + QString("_") + QString::number(generateTimeStamp()) + QString(".") + extenstion;
        return filename_ts;
    }

    bool openFileForWriting(QFile& aFileOut, QIODevice::OpenMode openMode) {
        QString aFileName = aFileOut.fileName();
        if (!aFileOut.open(openMode | QIODevice::Text)) {
            qWarning() << "Couldn't open " + aFileName + " for writing!";
            aFileOut.close(); //for safety!
            //Add a time stamp to the file name
            aFileName = addTimeStampToFileName(aFileName);
            qInfo() << "A timestamp has been added to the filename: " + aFileName;
            aFileOut.setFileName(aFileName);
            if (!aFileOut.open(openMode | QIODevice::Text)) {
                qWarning() << "Couldn't open " + aFileName + " for writing!";
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

    void outputIndicator(QTextStream& out, const QString compoName, const QString indicatorName, const double value, const QString unit, const QString alias) {
        out << compoName.simplified() << ";" << indicatorName.simplified() << ";" << QString::number(value) << ";" << unit.simplified() << ";" << alias.simplified() << "\n";
    }

    void outputIndicator(QTextStream& out, const QString compoName, const QString indicatorName, const QVector<double> list_values, const QString unit, const QString alias) {
        out << compoName.simplified() << ";" << indicatorName.simplified() << ";";
        for (int i = 0; i < list_values.size(); i++) {
            out << list_values.at(i);
            if(i < list_values.size()-1) out << ",";
        }
        out << ";" << unit.simplified() << ";" << alias.simplified() << "\n";
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
}

