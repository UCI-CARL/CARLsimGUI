//SpikeStream includes
#include "PluginManager.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDir>
#include <QtDebug>
#include <QLibrary>


/*! Constructor */
PluginManager::PluginManager(QString& pluginFolder) throw(SpikeStreamException){
	//Store widget and plugin folder
	this->pluginFolder = pluginFolder;
	this->pluginFilter = "*";
}

/*! Constructor with filter */
PluginManager::PluginManager(QString& pluginFolder, QString& pluginFilter) throw(SpikeStreamException) {
	//Store widget and plugin folder
	this->pluginFolder = pluginFolder;
	this->pluginFilter = pluginFilter;
}

/*! Destructor */
PluginManager::~PluginManager(){
}


/*-------------------------------------------------------------------*/
/*--------                  PUBLIC METHODS                    -------*/
/*-------------------------------------------------------------------*/

/*! Returns a list of names of the available plugins */
QStringList PluginManager::getPluginNames() throw(SpikeStreamException){
	return pluginFunctionMap.keys();
}


/*! Returns plugin as  QWidget */
QWidget* PluginManager::getPlugin(QString pluginName) throw(SpikeStreamException){
	if(pluginFunctionMap.contains(pluginName)){
		QWidget* tempWidget = pluginFunctionMap[pluginName]();
		qDebug()<<"Getting new QWidget plugin with name "<<pluginName;
		//printf("Getting new QWidget plugin with name %s\n", pluginName.toStdString().c_str()); 
		return tempWidget;
	}
	throw SpikeStreamException("ClassLoader: CANNOT FIND OR RESOLVE CLASS: ");
}


/*! Loads up all of the plugins in the plugin directory */
void PluginManager::loadPlugins(){
	//Get a list of files in the plugins directory
	QDir pluginDirectory(this->pluginFolder);

	//Filter plugin appropriately depending on the operating system
	QStringList filters;
	#ifdef LINUX32_SPIKESTREAM
		filters << "*.so";
	#endif
	#ifdef WIN32_SPIKESTREAM
		filters << pluginFilter + ".dll"; // enable filtering of specific plugins
	#endif
	#ifdef MAC32_SPIKESTREAM
		filters << "*.dylib";
	#endif
	pluginDirectory.setNameFilters(filters);

	//Get list of plugin files
	QStringList fileList = pluginDirectory.entryList();
	//qDebug()<<__FUNCTION__<<"("<< __FILE__<<"#"<<__LINE__<<")";
	qDebug()<<"Available plugins in"<<pluginFolder<<":"<< fileList;
	//printf("Available plugins in%s\n", pluginFolder.toStdString().c_str());

	//Load functions pointing to each plugin
	QString failureMsg;
	for(QList<QString>::iterator fileIter = fileList.begin(); fileIter != fileList.end(); ++fileIter){
		QString filePath = pluginDirectory.absolutePath() + "/" + *fileIter;
		qDebug()<<"Loading:"<<filePath;
		//printf("Available plugins in%s\n", filePath.toStdString().c_str());

		//Open the library
		QLibrary tmpLib(filePath);
		//Get functions to create the widget and to get the widget's name
		CreatePluginFunctionType createPluginFunction = (CreatePluginFunctionType) tmpLib.resolve("getClass");
    //qDebug()<<"Resolved CreatePlugingFunction:" << createPluginFunction;
	//qDebug() << "tmpLib isLoaded: " << tmpLib.isLoaded();
		GetPluginNameFunctionType  getPluginNameFunction = (GetPluginNameFunctionType) tmpLib.resolve("getName");
    //qDebug()<<"Resolved GetPluginNameFunction:"<< getPluginNameFunction;

		//Store the function to create the widget
		if (createPluginFunction  && getPluginNameFunction) {

			//Get the name of the plugin
			QString tmpPluginName = getPluginNameFunction();

	    qDebug()<<"Loading plugin:"<<tmpPluginName;
		//printf("Loading plugin:%s\n", tmpPluginName.toStdString().c_str());

	// Experimental:
			if(tmpPluginName == "TestPlugin") {

				GetPluginNameCstrFunctionType getPluginNameCstrFunction = (GetPluginNameCstrFunctionType) tmpLib.resolve("getNameCstr");
				const char* tmpPluginNameCstr = getPluginNameCstrFunction();
				qDebug() << "cstr: " << tmpPluginNameCstr;
				//printf("cstr: %s\n", tmpPluginNameCstr);
			}

			//Store function in map
			pluginFunctionMap[tmpPluginName] = createPluginFunction;
		}
		else{
			failureMsg += *fileIter + " ";
		}
	}

	//Throw exception if we have failed to load some or all of the plugins.
	if(!failureMsg.isEmpty()){
		throw SpikeStreamException("Failed to obtain required functions from librarie(s): " + failureMsg);
	}
}



