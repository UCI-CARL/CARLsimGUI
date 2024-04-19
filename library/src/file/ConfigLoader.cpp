//SpikeStream includes
#include "ConfigLoader.h"
#include "SpikeStreamIOException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>

//Other includes
#include <iostream>
using namespace std;


QString ConfigLoader::configFilePath = "spikestream.config";   

/*! Returns the working directory which is opened when loading projects, import files etc. */
const QString& ConfigLoader::getConfigFilePath() {
	return configFilePath;
}


/*! Sets the config file */
void ConfigLoader::setConfigFilePath(const QString& path) {
	configFilePath = path;
}


/*! Constructor loads up the configuration data from the given file path. */
ConfigLoader::ConfigLoader(){
	QString rootDirectory = Util::getRootDirectory();

	QFile configFile(configFilePath); 
	if (!configFile.exists()) {
		//cout << "DEBUG: configFilePath " << configFilePath.toStdString() << " not found as absolute path." << endl;
		configFile.setFileName(rootDirectory + "/" + configFilePath);
		if (!configFile.exists()) {
			QDir tmpDir(rootDirectory);
			//cout << "ERROR: configFilePath " << configFilePath.toStdString() << " does not exist realtiv to " << tmpDir.absolutePath().toStdString() << endl;
			throw SpikeStreamIOException("Cannot find config file " + configFilePath + " in directory " + tmpDir.absolutePath() + " or as absolute path.");
		}
	}
	cout << "Loading configuration " << configFilePath.toStdString() << endl;

	//Load the config file into the map
	if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
		throw SpikeStreamIOException("Cannot open file for reading: " + configFile.fileName());

	QTextStream in(&configFile);
	QString line;
	while (!in.atEnd()) {
		line = in.readLine();
		setParameter(line);
	}
	configFile.close();
}

/*! Constructor loads up the configuration data from the given file path. */
ConfigLoader::ConfigLoader(const QString& configFilePath) {

	cout << configFilePath.toStdString() << endl;

	//Create config file
	QFile configFile(configFilePath);


	if (!configFile.exists()) {
		throw SpikeStreamIOException("Cannot find config file " + configFilePath);
	}

	//Load the config file into the map
	if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
		throw SpikeStreamIOException("Cannot open file for reading: " + configFile.fileName());

	QTextStream in(&configFile);
	QString line;
	QString &prev = QString(""); 
	while (!in.atEnd()) {
		line = in.readLine();
		//cout << line.toStdString() << endl;
		setParameter(line, prev); // support multi line param value 
	}
	configFile.close();
}



/*! Destructor. */
ConfigLoader::~ConfigLoader(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Extracts the configuration parameter as a QString. */
QString ConfigLoader::getParameter(const QString& paramName){
	if(!configMap.contains(paramName))
		throw SpikeStreamException("Configuration file does not contain a parameter with name: " + paramName);
	return configMap[paramName];
}


/*! Extracts the configuration parameter as a QString.  */
QString ConfigLoader::getParameter(const QString& paramName, const QString& paramDefault){
	if(!configMap.contains(paramName))
		return paramDefault;
	return configMap[paramName];
}



/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Debug method for printing out the loaded configuration parameters. */
void ConfigLoader::printConfig(){
	cout<<"-----------  SpikeStream Configuration  -----------"<<endl;
	for (QHash<QString, QString>::iterator iter = configMap.begin(); iter != configMap.end(); ++iter) {
		cout << "Key: " << iter.key().toStdString() << "; "<< "Value: " << iter.value().toStdString()<< endl;
	}
}


/*! Sets the parameter in the line from the config map using the value in the
	parameter map if it exists. */
void ConfigLoader::setParameter(const QString& configFileLine){
	//Do nothing if line is a comment or is empty
	if(configFileLine.isEmpty() || configFileLine.at(0) == '#')
		return;

	//Extract the parameter for this line
	QString paramName = configFileLine.section("=", 0, 0, QString::SectionSkipEmpty).trimmed();
	QString paramValue = configFileLine.section("=", 1, 1, QString::SectionSkipEmpty).trimmed();

	if(configMap.contains(paramName))
		throw SpikeStreamException("Duplicate parameter in config file: " + paramName);

	configMap[paramName] = paramValue;
}

/*! Sets the parameter in the line from the config map using the value in the
	parameter map if it exists. */
void ConfigLoader::setParameter(const QString& configFileLine, QString& prevParam) {

	//Do nothing if line is a comment or is empty
	if (configFileLine.isEmpty() || configFileLine.at(0) == '#' )
		return;

	//Check if it is a multi value parameter value
	auto len = configFileLine.size();
	bool isMultilineValue = len >= 2 && configFileLine[len - 1] == '\\' && configFileLine[len - 2] == '\\'; 

	//Extract the parameter for this line
	QString paramName;
	QString paramValue;
	if (configFileLine.contains("=")) {
		paramName = configFileLine.section("=", 0, 0, QString::SectionSkipEmpty).trimmed();
		paramValue = configFileLine.section("=", 1, 1, QString::SectionSkipEmpty).trimmed();
		if(isMultilineValue) {
			//Define a new multiline param
			prevParam = paramName;
			//cout << "Define new multiline param " << prevParam.toStdString() << endl;
		}
	} else {
		paramValue = configFileLine.trimmed(); // is multiline
	}
	
	//cout << "Extracted Param: " << paramName.toStdString() << " = " << paramValue.toStdString() << endl;

	if (isMultilineValue || !prevParam.isEmpty()) {

		if (isMultilineValue) {
			//Trim the multiline suffix, but not for the concluding, last line
			paramValue.chop(2);
		}
		
		if (paramName.isEmpty()) {
			//Extend the existing param 
			configMap[prevParam] = configMap[prevParam] + QString("\n") + paramValue;
			//cout << "Extend multiline param: " << prevParam.toStdString() << " = " << paramValue.toStdString() << endl;
			return;
		}
		
	}

	if (configMap.contains(paramName))
		throw SpikeStreamException("Duplicate parameter in config file: " + paramName);

	configMap[paramName] = paramValue;

	return;
}


