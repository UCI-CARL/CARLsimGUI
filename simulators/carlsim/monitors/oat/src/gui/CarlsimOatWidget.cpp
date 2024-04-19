//SpikeStream includes
#include "Globals.h"
#include "CarlsimOatWidget.h"
#include "CarlsimOatTableView.h"
#include "CarlsimOatXmlLoader.h"
#include "CarlsimOatModel.h"
#include "CarlsimOatMonitor.h"
#include "CarlsimWrapper.h"
#include "CarlsimOatSpikeReportDialog.h"
#include "CarlsimOatGroupReportDialog.h"
#include "CarlsimOatConnectionReportDialog.h"
#include "CarlsimOatNeuronReportDialog.h"
#include "Util.h"

using namespace spikestream::carlsim_monitors;

//Qt includes
#include <QLabel>
#include <QLayout>
#include <QGroupBox>

// \sa CARLsimCNS 
//Functions for dynamic library loading
extern "C" {
	/*! Creates an QWidget class when library is dynamically loaded. */
	_MSC_DLL_API QWidget* getClass(){
		return new CarlsimOatWidget();
	}

	/*! Returns a descriptive name for this widget */
	_MSC_DLL_API QString getName(){
		return QString("CARLsim OAT");  // QString("OAT (Offline Analysis Tool)");
	}
}



/*! Constructor */
CarlsimOatWidget::CarlsimOatWidget(QWidget* parent) : AbstractMonitorWidget(parent){

	//Vertical box to organize layout
	QVBoxLayout* verticalBox = new QVBoxLayout(this);


	//Construct table view and model
	oatModel = new CarlsimOatModel();
	QTableView* oatMonitorView = new CarlsimOatTableView(this, oatModel);
	verticalBox->addWidget(oatMonitorView);

	connect(oatMonitorView, SIGNAL(monitorPrint(int)), this, SLOT(printMonitor(int)));

	// Save Simulation with Synapses
	QHBoxLayout *buttonBox = new QHBoxLayout();


	QPushButton* defaultsButton = new QPushButton("Save Simulation");
	buttonBox->addWidget(defaultsButton);
	connect (defaultsButton, SIGNAL(clicked()), this, SLOT(saveSimulation()));

	synapsesCheckBox = new QCheckBox("synaptic weights");
	buttonBox->addWidget(synapsesCheckBox);

	// align buttons left
	buttonBox->addStretch(1);

	verticalBox->addLayout(buttonBox);

}

/*! Destructor */
CarlsimOatWidget::~CarlsimOatWidget(){
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Sets the wrapper that will be used in the experiments.
	The void pointer must point to an object of class CarlsimWrapper */
void CarlsimOatWidget::setWrapper(void *wrapper){
	this->carlsimWrapper = (CarlsimWrapper*)wrapper;
	oatModel->setWrapper(wrapper);

	connect(carlsimWrapper, SIGNAL(timeStepChanged(unsigned int)), this, SLOT(emit_timeStepChanged(unsigned int)));
	connect(carlsimWrapper, SIGNAL(timeStepChanged(unsigned int, const QList<unsigned>)), this, SLOT(emit_timeStepChanged(unsigned int)));

	connect(this, SIGNAL(monitorsLoaded()), carlsimWrapper, SLOT(handleMonitorsLoaded()));
}




/*----------------------------------------------------------*/
/*------                 PUBLIC SLOTS                ------*/
/*----------------------------------------------------------*/

// type = unqualified class name OatSpikeMonitor
void CarlsimOatWidget::findMonitor(OatMonitor*& monitor, QString type, QString name) {

	assert(oatModel); // must be defined

	for (int i = 0; i < oatModel->rowCount(); i++)
	{
		auto tmpMonitor = oatModel->getMonitor(i);  
		const char* monitorClass = tmpMonitor->metaObject()->className();

		if (QString::compare(monitorClass, "spikestream::carlsim_monitors::" + type) == 0)
		{
			monitor = tmpMonitor;
			return;
		}
		else {
			monitor = nullptr;
		}
	}
}

/*----------------------------------------------------------*/
/*------                 PRIVATE SLOTS                ------*/
/*----------------------------------------------------------*/

void CarlsimOatWidget::emit_timeStepChanged(unsigned int step) {

	emit timeStepChanged(step);

}

void CarlsimOatWidget::saveSimulation() {
	bool synapses = synapsesCheckBox->isChecked();
	auto config = carlsimWrapper->getCarlsimConfig();

	//carlsimWrapper->carlsim->saveSimulation("DEFAULT", synapses);
	std::string fileName("results/sim_" + config->netName + ".dat");
	carlsimWrapper->carlsim->saveSimulation(fileName, synapses);
}

void CarlsimOatWidget::loadMonitors() {
	qDebug("load CARLsim OAT-Monitors");

	ConfigLoader* configLoader = new ConfigLoader();

	if (!Util::getBool(configLoader->getParameter("carlsim_monitors_oat_autoload", "false")))
		return;

	auto xmlConfigPath = configLoader->getParameter("carlsim_monitors_oat_config", "CarlsimWidgetMonitors.xml");

	qDebug() << "load from " << xmlConfigPath;

	CarlsimOatXmlLoader xmlLoader(carlsimWrapper, oatModel);

	QFile configFile(xmlConfigPath);
	if (!configFile.exists()) {
		QString rootDirectory = Util::getRootDirectory();
		configFile.setFileName(rootDirectory + "/" + xmlConfigPath);
		if (!configFile.exists()) {
			throw SpikeStreamException("Cannot find xml config file " + xmlConfigPath + " in directory " + rootDirectory + " or as absolute path.");
		}
	}
	qDebug() << "Load OAT-Monitors from " << configFile.fileName() << endl;
	xmlLoader.loadFromPath(configFile.fileName());

	emit monitorsLoaded();
}


/*! when simulation is unloaded, the internal references need to be set to nullptrs
*/
void CarlsimOatWidget::unloadMonitors() {

	oatModel->unloadOatMonitors();

	emit monitorsUnloaded();
}

//! for Connection mon
void CarlsimOatWidget::setup() {
	for (int i = 0; i<oatModel->rowCount(); i++)
		oatModel->getMonitor(i)->setup();
}

void CarlsimOatWidget::printMonitor(int index) {
	CarlsimOatReportDialog *dialog = NULL;
	auto monitor = oatModel->getMonitor(index);    
	const char* monitorClass = monitor->metaObject()->className(); 
	if (QString::compare(monitorClass, "spikestream::carlsim_monitors::OatSpikeMonitor") == 0)
		dialog = new CarlsimOatSpikeReportDialog(this, monitor);
	else
	if (QString::compare(monitorClass, "spikestream::carlsim_monitors::OatGroupMonitor") == 0)
		dialog = new CarlsimOatGroupReportDialog(this, monitor);
	else
	if (QString::compare(monitorClass, "spikestream::carlsim_monitors::OatNeuronMonitor") == 0)
		dialog = new CarlsimOatNeuronReportDialog(this, monitor);
	else
	if (QString::compare(monitorClass, "spikestream::carlsim_monitors::OatConnectionMonitor") == 0) 
		dialog = new CarlsimOatConnectionReportDialog(this, monitor);
	else
		throw SpikeStreamException("Unsupported Monitor");

	dialog->show();
	dialog->raise();
	dialog->activateWindow();
}

void CarlsimOatWidget::monitorStartRecording(unsigned step) {
	// delegate to model
	//printf("CarlsimOatWidget::monitorStartRecording(unsigned step=%d)\n", step);
	oatModel->monitorStartRecording(step);
}

void CarlsimOatWidget::monitorStopRecording(unsigned step) {
	//printf("CarlsimOatWidget::monitorStopRecording(unsigned step=%d)\n", step);
	oatModel->monitorStopRecording(step);
}


/*----------------------------------------------------------*/
/*------              PROTECTED METHODS               ------*/
/*----------------------------------------------------------*/



/*----------------------------------------------------------*/
/*------                PRIVATE METHODS               ------*/
/*----------------------------------------------------------*/


