//SpikeStream includes
#include "SimulationLoaderWidget.h"
#include "Globals.h"
#include "PluginManager.h"
#include "ConfigLoader.h"
using namespace spikestream;

//Qt includes
#include <QLabel>
#include <QDebug>


/*! Constructor */
SimulationLoaderWidget::SimulationLoaderWidget(QWidget* parent) : QWidget(parent) {
	//Create vertical box to organize layout
	mainVerticalBox = new QVBoxLayout(this);

	try{
		//Get list of available analysis plugins
		QString pluginPath = Globals::getSpikeStreamRoot() + "/plugins/simulation";


		ConfigLoader* configLoader = new ConfigLoader();
		QString simulatorPlugin = configLoader->getParameter("simulator_plugin", "*");


		//qDebug() << " this string crashses release version ";
		//qDebug() << QString(" this string crashses release version ");

		//qInfo() << " this string crashes in the release version ";
		//printf("simulator plugin %s\n", simulatorPlugin.toStdString().c_str());
		
		qDebug() << "simulator plugin " << simulatorPlugin;  // Release version crashes due qDebug


		PluginManager* pluginManager = new PluginManager(pluginPath, simulatorPlugin);

		pluginManager->loadPlugins();
		QStringList pluginList = pluginManager->getPluginNames();

		//Add list to combo box
		QComboBox* pluginsCombo = new QComboBox(this);
		pluginsCombo->addItems(pluginList);

		//Add combo to layout
		QHBoxLayout *comboBox = new QHBoxLayout();
		//comboBox->addWidget(new QLabel("Simulation plugins: "));
		comboBox->addWidget(new QLabel("Plugin: "));
		comboBox->addWidget(pluginsCombo);
		comboBox->addStretch(5);
		mainVerticalBox->addLayout(comboBox);

		//Add the widgets to a stacked widget
		stackedWidget = new QStackedWidget();
		for(QList<QString>::iterator iter = pluginList.begin(); iter != pluginList.end(); ++iter){
			QWidget* tmpWidget = pluginManager->getPlugin(*iter);
			pluginWidgetMap[*iter] = stackedWidget->addWidget(tmpWidget);
		}

		//Add stacked widget to layout
		mainVerticalBox->addWidget(stackedWidget);

		//Connect combo changed signal to slot loading appropriate analysis widget
		connect(pluginsCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(showSimulationWidget(int)));
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
	catch(...){
		qCritical()<<"An unknown exception occurred.";
	}
}


/*! Destructor */
SimulationLoaderWidget::~SimulationLoaderWidget(){
}


/*-------------------------------------------------------------------*/
/*----------             PRIVATE SLOTS                  -------------*/
/*-------------------------------------------------------------------*/

/*! Requests current widget to hide its results - particularly in the network viewer.
	Sets the current analysis widget */
void SimulationLoaderWidget::showSimulationWidget(int layerID){
	if(layerID != stackedWidget->currentIndex()){
		stackedWidget->setCurrentIndex(layerID);
	}
}

