//SpikeStream includes
#include "MonitorLoaderWidget.h"
#include "Globals.h"
#include "PluginManager.h"
using namespace spikestream;

//Qt includes
#include <QLabel>
#include <QDebug>


/*! Constructor */
MonitorLoaderWidget::MonitorLoaderWidget(QString pluginPath, QWidget* parent) : QWidget(parent) {
	//Create vertical box to organize layout
	mainVerticalBox = new QVBoxLayout(this);

	try{
		//Get list of available analysis plugins
		PluginManager* pluginManager = new PluginManager(pluginPath);
		pluginManager->loadPlugins();
		QStringList pluginList = pluginManager->getPluginNames();

		//Add list to combo box
		QComboBox* pluginsCombo = new QComboBox(this);
		pluginsCombo->addItems(pluginList);

		//Add combo to layout
		QHBoxLayout *comboBox = new QHBoxLayout();
		comboBox->addWidget(new QLabel("Plugin: "));
		comboBox->addWidget(pluginsCombo);
		comboBox->addStretch(5);
		mainVerticalBox->addLayout(comboBox);

		//Add the widgets to a stacked widget
		stackedWidget = new QStackedWidget();
		for(QList<QString>::iterator iter = pluginList.begin(); iter != pluginList.end(); ++iter){
			QWidget* tmpWidget = pluginManager->getPlugin(*iter);
			abstractMonitorList.append((AbstractMonitorWidget*)tmpWidget);
			pluginWidgetMap[*iter] = stackedWidget->addWidget(tmpWidget);
		}

		//Add stacked widget to layout
		mainVerticalBox->addWidget(stackedWidget);

		//Connect combo changed signal to slot loading appropriate analysis widget
		connect(pluginsCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(showMonitorWidget(int)) );
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Destructor */
MonitorLoaderWidget::~MonitorLoaderWidget(){
}


/*------------------------------------------------------------*/
/*------               PRIVATE SLOTS                    ------*/
/*------------------------------------------------------------*/

/*! Sets the current visible device widget */
void MonitorLoaderWidget::showMonitorWidget(int layerID){
	if(layerID != stackedWidget->currentIndex()){
		stackedWidget->setCurrentIndex(layerID);
	}
}



