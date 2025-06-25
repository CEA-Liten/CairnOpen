#ifndef CAIRNUTILS_H
#define CAIRNUTILS_H

#include <QDebug>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QIODevice>
#include <QTextStream>

namespace CairnUtils {
	//Generate a timestamp.
	int generateTimeStamp(); 

	//Add a timestamp to aFileName. 
	QString addTimeStampToFileName(QString aFileName); 

	//Open aFileOut for writing. If aFileOut cannot be accessed, add a timestamp to the file name.
	bool openFileForWriting(QFile& aFileOut, QIODevice::OpenMode openMode=QIODevice::WriteOnly);

	//write a line in PLAN or HIST file
	//scalar 
	void outputIndicator(QTextStream& out, const QString compoName, const QString indicatorName, const double value, const QString unit, const QString alias);
	//list
	void outputIndicator(QTextStream& out, const QString compoName, const QString indicatorName, const QVector<double> list_values, const QString unit, const QString alias);

	void printInfoParam(const QString& aParamName, const bool IsBlocking, const QString& aUnit, const QString& aDescription);
	void printMissingParam(const QString& aParamName, const QString& aValue);
	void resetInfoParam();
	void resetMissingParam();

	//void jsonSaveGUIModelParams(QJsonArray& paramArray, const QMap <QString, QString>& paramMapAll, const QList<QString>& paramNamesList);
}

#endif //CAIRNUTILS_H