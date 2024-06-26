//SpikeStream includes
#include "Globals.h"
#include "PluginManager.h"
#include "PluginsDialog.h"
using namespace spikestream;

//Qt includes
#include <QComboBox>
#include <QLayout>
#include <QStackedWidget>


/*! Constructor */
PluginsDialog::PluginsDialog(QWidget* parent, const QString pluginFolder, const QString title) : QDialog(parent){
	
	// FIXED Warning: QWindowsWindow::setGeometry: Unable to set geometry 1
	//https://stackoverflow.com/questions/31230975/qt-setgeometry-unable-to-set-geometry/31231069#31231069
	//https://stackoverflow.com/questions/54307407/why-am-i-getting-qwindowswindowsetgeometry-unable-to-set-geometry-warning-wit
	//https://stackoverflow.com/questions/31230975/qt-setgeometry-unable-to-set-geometry
	setMinimumSize(250, 30);
	
	//Set caption
	this->setWindowTitle(title);

	//Create vertical box to organize layout
	QVBoxLayout* mainVerticalBox = new QVBoxLayout(this);

	//Get list of available plugins - some plugins may fail to load and throw an exception
	QString pluginPath = Globals::getSpikeStreamRoot() + pluginFolder;
	PluginManager* pluginManager = new PluginManager(pluginPath);
	try{
		pluginManager->loadPlugins();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}

	//Load up plugins that worked.
	try{
		QStringList pluginList = pluginManager->getPluginNames();

		//Add list to combo box
		QComboBox* pluginsCombo = new QComboBox(this);
		pluginsCombo->addItems(pluginList);

		//Add combo to layout
		QHBoxLayout *comboBox = new QHBoxLayout();
		comboBox->addWidget(new QLabel("Available plugins: "));
		comboBox->addWidget(pluginsCombo);
		comboBox->addStretch(5);
		mainVerticalBox->addLayout(comboBox);

		//Add the widgets to a stacked widget
		QStackedWidget* stackedWidget = new QStackedWidget();
		for(QList<QString>::iterator iter = pluginList.begin(); iter != pluginList.end(); ++iter){
			stackedWidget->addWidget(pluginManager->getPlugin(*iter));
		}

		//Add stacked widget to layout
		mainVerticalBox->addWidget(stackedWidget);

		//Connect combo changed signal to slot loading appropriate analysis widget
		connect(pluginsCombo, SIGNAL(currentIndexChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)) );
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Destructor */
PluginsDialog::~PluginsDialog(){
}




