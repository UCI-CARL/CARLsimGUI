//SpikeStream includes
#include "CarlsimWidget.h"
#include "DeviceLoaderWidget.h"
#include "ExperimentLoaderWidget.h"
#include "MonitorLoaderWidget.h"
#include "InjectorLoaderWidget.h"
#include "Globals.h"
#include "GlobalVariables.h"
#include "CarlsimParametersDialog.h"
#include "ModelParametersDialog.h"
#include "NeuronParametersDialog.h"
#include "NeuronGroupSelectionDialog.h"
#include "Pattern.h"
#include "PatternManager.h"
#include "SpikeStreamSimulationException.h"
#include "SynapseParametersDialog.h"
#include "Util.h"

#include "TimePanelWidget.h"
#include "NcMergedDockWidget.h"

using namespace spikestream;


//Qt includes
#include <QButtonGroup>
#include <QDebug>
#include <QFileDialog>
#include <QLayout>
#include <QMessageBox>
#include <QMutexLocker>
#include <QLocale>
#include <QTranslator>
#include <QMainWindow>
#include <QAction>


/*! String user selects to load a pattern */
#define LOAD_PATTERN_STRING "Load Pattern"

#define MONITOR_NEURONS_OFF 0
#define MONITOR_NEURONS_FIRING 1
#define MONITOR_NEURONS_MEMBRANE 2


//Functions for dynamic library loading
extern "C" {
	/*! Creates a CarlsimWidget class when library is dynamically loaded. */
	_MSC_DLL_API QWidget* getClass(){
		return new CarlsimWidget();
	}

	/*! Returns a descriptive name for this widget */
	_MSC_DLL_API QString getName(){
		return QString("CARLsim CUDA Simulator");
	}
}


/*! Constructor */
CarlsimWidget::CarlsimWidget(QWidget* parent) :
#ifdef NC_EXTENSIONS
	translator(NULL),
#endif
	QWidget(parent) {


	// Register types to enable signals and slots to work
	qRegisterMetaType< QList<unsigned> >("QList<unsigned>");
	qRegisterMetaType< QHash<unsigned, float> >("QHash<unsigned, float>");
 
#ifdef NC_EXTENSIONS
	translator = new QTranslator;
	QLocale locale; 
	if(!translator->load(":/translation/"+ locale.name())) {
		qDebug() << "Could not load translations " << locale.name() << ". Please check the LANG environment variable.";
		delete translator;
		translator = NULL;
	} else {
		QApplication::installTranslator(translator); 
	}
#endif

	//Create layout for the entire widget
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	mainVBox->setMargin(0); // Prevent waste of space by inset

	mainGroupBox = new QGroupBox(this);
	mainGroupBox->setObjectName("CarlsimWidget_mainGroupBox");
	mainGroupBox->setStyleSheet("#CarlsimWidget_mainGroupBox {border-style: none;}"); 

	controlsWidget = new QWidget();
	controlsWidget->setEnabled(false);
	QVBoxLayout* controlsVBox = new QVBoxLayout(controlsWidget);
	controlsVBox->setMargin(0); // Prevent waste of space by inset

	//Add load, unload and parameters buttons
	loadButton = new QPushButton(" Load Simulation ");
	connect(loadButton, SIGNAL(clicked()), this, SLOT(loadSimulation()));
	unloadButton = new QPushButton("Unload");
	connect(unloadButton, SIGNAL(clicked()), this, SLOT(unloadSimulation()));
	unloadButton->setEnabled(false);
	neuronParametersButton = new QPushButton(" Neuron Parameters ");
	connect(neuronParametersButton, SIGNAL(clicked()), this, SLOT(setNeuronParameters()));
	synapseParametersButton = new QPushButton(" Synapse Parameters ");
	connect(synapseParametersButton, SIGNAL(clicked()), this, SLOT(setSynapseParameters()));
	carlsimParametersButton = new QPushButton(" CARLsim Parameters ");
	connect(carlsimParametersButton, SIGNAL(clicked()), this, SLOT(setCarlsimParameters()));
#ifdef NC_EXTENSIONS_CARLSIM
	modelParametersButton = new QPushButton(" Model Parameters ");
	connect(modelParametersButton, SIGNAL(clicked()), this, SLOT(setModelParameters()));
#endif
	QHBoxLayout* loadLayout = new QHBoxLayout();
	loadLayout->addWidget(loadButton);
	loadLayout->addWidget(unloadButton);
	loadLayout->addWidget(neuronParametersButton);
	loadLayout->addWidget(synapseParametersButton);
	loadLayout->addWidget(carlsimParametersButton);
#ifdef NC_EXTENSIONS_CARLSIM
	loadLayout->addWidget(modelParametersButton);
#endif
	mainVBox->addLayout(loadLayout);

#ifdef NC_EXTENSIONS_CARLSIM

	// Add the tool bar at the MainWindow
	toolBar = getToolBar();	
	toolBar->setWindowTitle(tr("Simulation"));
	toolBar->setObjectName("simulation:ToolBar");
	Globals::getMainWindow()->addToolBar(toolBar);

#endif

#ifdef NC_EXTENSIONS_CARLSIM
	// New TimePanel with Custom Dock
	timePanel = new TimePanelWidget(this);
	timePanelDock = new NcMergedDockWidget(timePanel, "TimePanelDock", tr("Time-Panel"), this); 
	controlsVBox->addWidget(timePanelDock);
#endif

	//Group box for monitoring controls
	monitorGroupBox = new QGroupBox("Monitor", controlsWidget);
	//monitorGroupBox = new QGroupBox("Monitor", this);
	QVBoxLayout* monitorVBox = new QVBoxLayout();

	//Add widget to control live monitoring of neurons
	QHBoxLayout* monitorNeuronsBox = new QHBoxLayout();
	monitorNeuronsBox->addWidget(new QLabel("Monitor neurons: "));
	QButtonGroup* monitorNeuronsButtonGroup = new QButtonGroup();
	connect(monitorNeuronsButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(monitorNeuronsStateChanged(int)));
	noMonitorNeuronsButton = new QRadioButton("Off");
	noMonitorNeuronsButton->setChecked(true);
	monitorNeuronsButtonGroup->addButton(noMonitorNeuronsButton, MONITOR_NEURONS_OFF);
	monitorNeuronsBox->addWidget(noMonitorNeuronsButton);
	monitorFiringNeuronsButton = new QRadioButton("Firing");
	monitorNeuronsButtonGroup->addButton(monitorFiringNeuronsButton, MONITOR_NEURONS_FIRING);
	monitorNeuronsBox->addWidget(monitorFiringNeuronsButton);
	rasterButton = new QPushButton("Raster");
	connect(rasterButton, SIGNAL(clicked()), this, SLOT(rasterButtonClicked()));
	rasterButton->setMaximumSize(60,20);
	rasterButton->setEnabled(false);
	monitorNeuronsBox->addWidget(rasterButton);
	monitorMemPotNeuronsButton = new QRadioButton("Membrane potential");
	monitorNeuronsButtonGroup->addButton(monitorMemPotNeuronsButton, MONITOR_NEURONS_MEMBRANE);
	monitorNeuronsBox->addWidget(monitorMemPotNeuronsButton);
	memPotGraphButton = new QPushButton("Graph");
	connect(memPotGraphButton, SIGNAL(clicked()), this, SLOT(memPotGraphButtonClicked()));
	memPotGraphButton->setMaximumSize(60,20);
	memPotGraphButton->setEnabled(false);
	monitorNeuronsBox->addWidget(memPotGraphButton);
	monitorNeuronsBox->addStretch(5);
	monitorVBox->addLayout(monitorNeuronsBox);
	monitorVBox->addSpacing(5);

	//Add widgets to monitor, save and reset the weights
	QHBoxLayout* saveWeightsBox = new QHBoxLayout();
	monitorWeightsCheckBox = new QCheckBox("Monitor weights");
	connect(monitorWeightsCheckBox, SIGNAL(clicked(bool)), this, SLOT(setMonitorWeights(bool)));
	saveWeightsBox->addWidget(monitorWeightsCheckBox);
	resetWeightsButton = new QPushButton("Reset Weights");
	connect(resetWeightsButton, SIGNAL(clicked()), this, SLOT(resetWeights()));
	saveWeightsBox->addWidget(resetWeightsButton);
	saveWeightsButton = new QPushButton("Save Weights");
	connect(saveWeightsButton, SIGNAL(clicked()), this, SLOT(saveWeights()));
	saveWeightsBox->addWidget(saveWeightsButton);
	saveWeightsBox->addStretch(5);
	monitorVBox->addLayout(saveWeightsBox);

#ifdef CARLSIM_AXONPLAST
	//Add widgets to monitor, save and reset the weights
	QHBoxLayout* saveDelaysBox = new QHBoxLayout();
	monitorDelaysCheckBox = new QCheckBox("Monitor delays");
	monitorDelaysCheckBox->setChecked(false);
	connect(monitorDelaysCheckBox, SIGNAL(clicked(bool)), this, SLOT(setMonitorDelays(bool)));
	saveDelaysBox->addWidget(monitorDelaysCheckBox);
	resetDelaysButton = new QPushButton("Reset Delays");
	connect(resetDelaysButton, SIGNAL(clicked()), this, SLOT(resetDelays()));
	saveDelaysBox->addWidget(resetDelaysButton);
	saveDelaysButton = new QPushButton("Save Delays");
	connect(saveDelaysButton, SIGNAL(clicked()), this, SLOT(saveDelays()));
	saveDelaysBox->addWidget(saveDelaysButton);
	saveDelaysBox->addStretch(5);
	monitorVBox->addLayout(saveDelaysBox);
#endif

	//Add widgets to control archiving
	archiveCheckBox = new QCheckBox("Archive.");
	connect(archiveCheckBox, SIGNAL(stateChanged(int)), this, SLOT(archiveStateChanged(int)));
	archiveDescriptionEdit = new QLineEdit("Undescribed");
	archiveDescriptionEdit->setEnabled(false);
	setArchiveDescriptionButton = new QPushButton("Set Description");
	setArchiveDescriptionButton->setEnabled(false);
	connect(setArchiveDescriptionButton, SIGNAL(clicked()), this, SLOT(setArchiveDescription()));
	QHBoxLayout* archiveLayout = new QHBoxLayout();
	archiveLayout->addWidget(archiveCheckBox);
	archiveLayout->addWidget(archiveDescriptionEdit);
	archiveLayout->addWidget(setArchiveDescriptionButton);
	monitorVBox->addLayout(archiveLayout);

	//Add monitor group box to layout
	monitorGroupBox->setLayout(monitorVBox);
	controlsVBox->addWidget(monitorGroupBox);

	//Group box for injection controls
	injectGroupBox = new QGroupBox("Inject", controlsWidget);
	QVBoxLayout* injectVBox = new QVBoxLayout();

	//Add widgets to enable injection of patterns
	QHBoxLayout* injectPatternBox = new QHBoxLayout();
	injectPatternNeurGrpCombo = new QComboBox();
	injectPatternNeurGrpCombo->setMinimumSize(200, 20);
	injectPatternBox->addWidget(injectPatternNeurGrpCombo);
	patternCombo = new QComboBox();
	patternCombo->setMinimumSize(200, 20);
	patternCombo->addItem("");
	patternCombo->addItem(LOAD_PATTERN_STRING);
	connect(patternCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(loadPattern(QString)));
	patternCurrentCombo = new QComboBox();
	fillPatternCurrentCombo();
	injectPatternBox->addWidget(patternCurrentCombo);
	injectPatternBox->addWidget(patternCombo);
	injectPatternButton = new QPushButton("Inject Pattern");
	injectPatternButton->setEnabled(false);
	injectPatternButton->setMinimumHeight(22);
	connect(injectPatternButton, SIGNAL(clicked()), this, SLOT(injectPatternButtonClicked()));
	injectPatternBox->addWidget(injectPatternButton);
	sustainPatternChkBox = new QCheckBox("Sustain");
	sustainPatternChkBox->setEnabled(false);
	connect(sustainPatternChkBox, SIGNAL(clicked(bool)), this, SLOT(sustainPatternChanged(bool)));
	injectPatternBox->addWidget(sustainPatternChkBox);
	injectVBox->addSpacing(5);
	injectVBox->addLayout(injectPatternBox);

	//Add inject group box to layout
	injectGroupBox->setLayout(injectVBox);
	controlsVBox->addWidget(injectGroupBox);


	//Group box for plugins, such as experiments and devices
	QGroupBox* pluginsGroupBox = new QGroupBox(controlsWidget);
	pluginsGroupBox->setObjectName("CarlsimWidget_pluginsGroupBox"); 
	pluginsGroupBox->setStyleSheet("#CarlsimWidget_pluginsGroupBox {border-style: none;}");
	QVBoxLayout* pluginsVBox = new QVBoxLayout();
	pluginsVBox->setMargin(0);
	QTabWidget* pluginsTabWidget = new QTabWidget();


	//Add widget that loads monitors
	MonitorLoaderWidget* monitorLoaderWidget = new MonitorLoaderWidget(Globals::getSpikeStreamRoot() + "/plugins/simulation/carlsimmonitors", this);
	monitorLoaderWidget->setMinimumSize(600, 400);
	pluginsTabWidget->addTab(monitorLoaderWidget, "Monitors");

	//Add widget that injectors 
	InjectorLoaderWidget* injectorLoaderWidget = new InjectorLoaderWidget(Globals::getSpikeStreamRoot() + "/plugins/simulation/carlsiminjectors", this);
	injectorLoaderWidget->setMinimumSize(600, 400);
	pluginsTabWidget->addTab(injectorLoaderWidget, "Injectors");


	//Add widget that loads devices
	DeviceLoaderWidget* deviceLoaderWidget = new DeviceLoaderWidget(Globals::getSpikeStreamRoot() + "/plugins/simulation/carlsimdevices");  
	deviceLoaderWidget->setMinimumSize(600, 400);
	pluginsTabWidget->addTab(deviceLoaderWidget, "Devices");

	//Add widget that loads experiments
	ExperimentLoaderWidget* exptLoaderWidget = new ExperimentLoaderWidget(Globals::getSpikeStreamRoot() + "/plugins/simulation/carlsimexperiments");
	exptLoaderWidget->setMinimumSize(600, 400);
	pluginsTabWidget->addTab(exptLoaderWidget, "Experiments");

	//Add experiment group box to layout
	pluginsVBox->addWidget(pluginsTabWidget);
	pluginsGroupBox->setLayout(pluginsVBox);
	controlsVBox->addWidget(pluginsGroupBox);

	//Put layout into enclosing box
	mainVBox->addWidget(controlsWidget);
	mainGroupBox->setLayout(mainVBox);
	this->setMinimumSize(680, 800);

#ifdef NC_EXTENSIONS_CARLSIM

	ConfigLoader* configLoader = new ConfigLoader();
	//ConfigLoader* configLoader = new ConfigLoader(Globals::getConfigFile());
	if(Util::getBool(configLoader->getParameter("carlsimWidget_autoloadSimulation", "false"))) 
		connect(this, SIGNAL(networkChangedProcessed()), this, SLOT(loadSimulation()));  


	// marshall to local event router
	if (Util::getBool(configLoader->getParameter("carlsimWidget_autoloadInjectors", "false"))) {
		connect(this, SIGNAL(simulationLoaded()), Globals::getEventRouter(), SLOT(carlsimSimulationLoadedSlot()));  
	}

	// marshall to global event router
	if(Util::getBool(configLoader->getParameter("carlsimWidget_autoloadDevices", "false")))
		connect(this, SIGNAL(simulationLoaded()), Globals::getEventRouter(), SLOT(carlsimSimulationLoadedSlot()));  

	if(Util::getBool(configLoader->getParameter("carlsimWidget_autostartSimulation", "false")))
		connect(Globals::getEventRouter(), SIGNAL(finspikesDeviceLoadedSignal()), this, SLOT(startSimulation()));

	carlsimWrapper = new CarlsimWrapper(configLoader);
	delete configLoader;
#else 
	carlsimWrapper = new CarlsimWrapper();
#endif 

	connect(carlsimWrapper, SIGNAL(finished()), this, SLOT(carlsimWrapperFinished()));
	connect(carlsimWrapper, SIGNAL(progress(int,int)), this, SLOT(updateProgress(int, int)), Qt::QueuedConnection);
	connect(carlsimWrapper, SIGNAL(simulationStopped()), this, SLOT(simulationStopped()), Qt::QueuedConnection);
	connect(carlsimWrapper, SIGNAL(timeStepChanged(unsigned)), this, SLOT(updateTimeStep(unsigned)), Qt::QueuedConnection);
	connect(carlsimWrapper, SIGNAL(timeStepChanged(unsigned, const QList<unsigned>&)), this, SLOT(updateTimeStep(unsigned, const QList<unsigned>&)), Qt::QueuedConnection);
	connect(carlsimWrapper, SIGNAL(timeStepChanged(unsigned, const QHash<unsigned, float>&)), this, SLOT(updateTimeStep(unsigned, const QHash<unsigned, float>&)), Qt::QueuedConnection);

	// Enable CarlsimWrapper to request loading the simulation, e.g. for a new simulation run
	connect(carlsimWrapper, SIGNAL(loadSimulationRequired()), this, SLOT(loadSimulation()));

	//! Wire the timepanel with the CarlsimWrapper
	connect(
		carlsimWrapper, SIGNAL(timeChanged(unsigned, unsigned long long, QDateTime, QDateTime, QDateTime, QDateTime, unsigned long long)),
		timePanel, SLOT(updateTime(unsigned, unsigned long long, QDateTime, QDateTime, QDateTime, QDateTime, unsigned long long)), 
		Qt::QueuedConnection);  //  Queued
	connect(carlsimWrapper, SIGNAL(modelSpeedChanged(double)), timePanel, SLOT(setSpeed(double)));
	connect(carlsimWrapper, SIGNAL(modelStepMsChanged(unsigned)), timePanel, SLOT(setStepMs(unsigned)));
	connect(timePanel, SIGNAL(speedChanged(double)), carlsimWrapper, SLOT(setModelSpeed(double)));
	connect(timePanel, SIGNAL(stepMsChanged(unsigned)), carlsimWrapper, SLOT(setModelStepMs(unsigned)));

	//Pass device managers to CarlsimWrapper
	QList<AbstractDeviceWidget*> deviceWidgetList = deviceLoaderWidget->getAbstractDeviceWidgets();
	foreach(AbstractDeviceWidget* tmpDevWidget, deviceWidgetList){
		carlsimWrapper->addDeviceManager(tmpDevWidget->getDeviceManager());
	}

	//Set up link between experiment widgets and wrapper and signals/slots
	QList<AbstractExperimentWidget*> exptWidgetList = exptLoaderWidget->getAbstractExperimentWidgets();
	foreach(AbstractExperimentWidget* tmpExptWidget, exptWidgetList){
		tmpExptWidget->setWrapper(carlsimWrapper);
		connect(tmpExptWidget, SIGNAL(experimentStarted()), this, SLOT(experimentStarted()));
		connect(tmpExptWidget, SIGNAL(experimentEnded()), this, SLOT(experimentEnded()));
		connect(carlsimWrapper, SIGNAL(monitorsLoaded()), tmpExptWidget, SLOT(assignOatMonitor()));
		connect(tmpExptWidget, SIGNAL(requiresMonitorWidget(AbstractMonitorWidget*&, QString)), this, SLOT(provideMonitorWidget(AbstractMonitorWidget*&, QString)));
	}

	//Set up link between injector widgets and CARLsim wrapper and signals/slots  // 
	QList<AbstractInjectorWidget*> injectorWidgetList = injectorLoaderWidget->getAbstractInjectorWidgets();
	foreach(AbstractInjectorWidget* tmpInjectorWidget, injectorWidgetList){
		tmpInjectorWidget->setWrapper(carlsimWrapper); // direct link backwards, not good
		//carlsimWrapper->addInjectorWidget(tmpInjectorWidget); // direct link forward, since the Threat / step calls the injection 
		//variant encapsulated
		//connect(this, SIGNAL(simulationLoaded()), tmpInjectorWidget, SLOT(loadInjectors())); //!too late, violates the callback WP
		connect(carlsimWrapper, SIGNAL(carlsimConfigStateNetworkBuilt()), tmpInjectorWidget, SLOT(loadInjectors()), Qt::DirectConnection); 
		connect(carlsimWrapper, SIGNAL(stepCurrentInjection()), tmpInjectorWidget, SLOT(inject()), Qt::DirectConnection);
		// When emitted, the signal is immediately delivered to the slot., in the same thread 
	}

	// Set up link between monitor widgets and CARLsim wrapper and signals / slots   
	monitorWidgetList = monitorLoaderWidget->getAbstractMonitorWidgets();  // The abstract monitor widget can be down cast to CarlsimOatWidget
	foreach(AbstractMonitorWidget* tmpMonitorWidget, monitorWidgetList) {
		tmpMonitorWidget->setWrapper(carlsimWrapper); // direct link backwards
		connect(carlsimWrapper, SIGNAL(carlsimConfigStateNetworkBuilt()), tmpMonitorWidget, SLOT(loadMonitors()), Qt::DirectConnection);
		connect(carlsimWrapper, SIGNAL(carlsimSetupState()), tmpMonitorWidget, SLOT(setup()), Qt::DirectConnection);
		connect(carlsimWrapper, SIGNAL(monitorStartRecording(unsigned)), tmpMonitorWidget, SLOT(monitorStartRecording(unsigned)), Qt::DirectConnection);
		connect(carlsimWrapper, SIGNAL(monitorStopRecording(unsigned)), tmpMonitorWidget, SLOT(monitorStopRecording(unsigned)), Qt::DirectConnection);

		// any state
		connect(carlsimWrapper, SIGNAL(carlsimUnloaded()), tmpMonitorWidget, SLOT(unloadMonitors()), Qt::DirectConnection);
	}


	//Listen for network changes
	connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(networkChanged()));

	//Listen for simulation control events
	connect(Globals::getEventRouter(), SIGNAL(startStopSimulationSignal()), this, SLOT(startStopSimulation()));
	connect(Globals::getEventRouter(), SIGNAL(stepSimulationSignal()), this, SLOT(stepSimulation()));

	//Set initial state of tool bar
	checkWidgetEnabled();

	//Initialise variables
	progressDialog = NULL;
	rasterDialogCtr = 0;
#ifdef NC_EXTENSIONS
	// implemented above
#endif 

}


/*! Destructor */
CarlsimWidget::~CarlsimWidget(){
#ifdef NC_EXTENSIONS
	if(translator!=NULL)
	{
		QApplication::removeTranslator(translator);  // does not delete the translator, see
		delete translator; 
	}
#endif

	for(QHash<int, RGBColor*>::iterator iter = heatColorMap.begin(); iter != heatColorMap.end(); ++iter)
		delete iter.value();
	heatColorMap.clear();
}


 bool CarlsimWidget::event(QEvent *event)
 {
     return QWidget::event(event);
 }





 /*! */
 void CarlsimWidget::provideMonitorWidget(AbstractMonitorWidget*& monitorWidget, QString className) {

	 if (monitorWidgetList.size() <= 0) {
		 qCritical() << "Widgets missing" << endl;
		 monitorWidget = nullptr;
		 return;
	 }

	 monitorWidget = monitorWidgetList[0];

	 const char* monitorClass = monitorWidget->metaObject()->className();

	 if (QString::compare(monitorClass, "spikestream::carlsim_monitors::" + className) != 0) {
		 qCritical() << "Inappropriate Widget" << endl;
		 monitorWidget = nullptr;
	 }


 }


/*----------------------------------------------------------*/
/*------                PRIVATE SLOTS                 ------*/
/*----------------------------------------------------------*/

 #ifdef NC_EXTENSIONS_CARLSIM

 void CarlsimWidget::resetSimtime() {

	// Update GUI when not yet started (for visual consistency)
	if(carlsimWrapper->getModelStart() == CarlsimWrapper::REALTIME) {
		QDateTime current = QDateTime::currentDateTime();
		carlsimWrapper->setModelTime0(current.addMSecs( - (int) carlsimWrapper->getModelLagMs() ));
		carlsimWrapper->setRealTime0(current);
	}

	carlsimWrapper->resetTime(); 

}
#endif

/*! Switches the archiving of the simulation on or off */
void CarlsimWidget::archiveStateChanged(int state){
	if(state == Qt::Checked){
		archiveDescriptionEdit->setEnabled(true);
		setArchiveDescriptionButton->setEnabled(true);
		if(archiveDescriptionEdit->text().isEmpty())
			archiveDescriptionEdit->setText("Undescribed");
		carlsimWrapper->setArchiveMode(true, archiveDescriptionEdit->text());
	}
	else{
		archiveDescriptionEdit->setEnabled(false);
		setArchiveDescriptionButton->setEnabled(false);
		carlsimWrapper->setArchiveMode(false);
	}
}


/*! Checks for progress with the loading. Has to be done with a timer because the thread is kept
	running ready for playing or stepping the simulation. Updates to the progress bar are done
	by the updateProgress method */
void CarlsimWidget::checkLoadingProgress(){
	//Check for errors during loading
	if(carlsimWrapper->isError()){
		loadingTimer->stop();
		progressDialog->setValue(progressDialog->maximum());
		delete progressDialog;
		progressDialog = NULL;
		qCritical()<<"Error occurred loading simulation: '"<<carlsimWrapper->getErrorMessage()<<"'.";
		return;
	}

	//Check for cancelation - stop timer and abort operation
	else if(progressDialog->wasCanceled()){
		loadingTimer->stop();
		carlsimWrapper->cancelLoading();
		delete progressDialog;
		progressDialog = NULL;
		return;
	}

	//If simulation has not been loaded return with loading timer still running
	else if(!carlsimWrapper->isSimulationLoaded()){
		return;
	}

	else if (updatingProgress){
		return;
	}

	//If we have reached this point, loading is complete
	loadingTimer->stop();
	progressDialog->setValue(progressDialog->maximum());
	delete progressDialog;
	progressDialog = NULL;

	//Adjust buttons
	loadButton->setEnabled(false);
	unloadButton->setEnabled(true);
	synapseParametersButton->setEnabled(false);
	carlsimParametersButton->setEnabled(false);
	controlsWidget->setEnabled(true);
	playAction->setEnabled(true);
	stopAction->setEnabled(false);
	stepAction->setEnabled(true);
	monitorWeightsCheckBox->setChecked(carlsimWrapper->isMonitorWeights());
#ifdef CARLSIM_AXONPLAST
	monitorDelaysCheckBox->setChecked(carlsimWrapper->isMonitorDelays());
#endif
	archiveCheckBox->setChecked(false);//Single archive associated with each simulation run

	//Cannot save weights or archive if the network is not fully saved in the database
	if(!Globals::getNetwork()->isSaved()){
		archiveCheckBox->setEnabled(false);
		setArchiveDescriptionButton->setEnabled(false);
		archiveDescriptionEdit->setEnabled(false);
	}
	else{
		archiveCheckBox->setEnabled(true);
		setArchiveDescriptionButton->setEnabled(true);
		archiveDescriptionEdit->setEnabled(true);
	}

	//Set wrapper as the simulation in global scope
	Globals::setSimulation(carlsimWrapper);

#ifdef NC_EXTENSIONS_CARLSIM
	emit simulationLoaded();
#endif
}


/*! Checks for progress with the resetting of weights*/
void CarlsimWidget::checkResetWeightsProgress(){
	//Check for errors during resetting weights
	if(carlsimWrapper->isError()){
		heavyTaskTimer->stop();
		progressDialog->setValue(progressDialog->maximum());
		delete progressDialog;
		progressDialog = NULL;
		qCritical()<<"Error occurred resetting of weights: '"<<carlsimWrapper->getErrorMessage()<<"'.";
		return;
	}

	//Check for cancelation - stop timer and abort operation
	else if(progressDialog->wasCanceled()){
		heavyTaskTimer->stop();
		carlsimWrapper->cancelResetWeights();
		delete progressDialog;
		progressDialog = NULL;
		return;
	}

	//If save weights is not complete return with timer running
	else if(!carlsimWrapper->isWeightsReset()){
		return;
	}

	else if (updatingProgress){
		return;
	}

	//If we have reached this point, loading is complete
	heavyTaskTimer->stop();
	progressDialog->setValue(progressDialog->maximum());
	delete progressDialog;
	progressDialog = NULL;
}


/*! Checks for progress with the saving of weights*/
void CarlsimWidget::checkSaveWeightsProgress(){
	//Check for errors during loading
	if(carlsimWrapper->isError()){
		heavyTaskTimer->stop();
		progressDialog->setValue(progressDialog->maximum());
		delete progressDialog;
		progressDialog = NULL;
		qCritical()<<"Error occurred saving of weights: '"<<carlsimWrapper->getErrorMessage()<<"'.";
		return;
	}

	//Check for cancelation - stop timer and abort operation
	else if(progressDialog->wasCanceled()){
		heavyTaskTimer->stop();
		carlsimWrapper->cancelSaveWeights();
		delete progressDialog;
		progressDialog = NULL;
		return;
	}

	//If simulation has not been loaded return with loading timer still running
	else if(!carlsimWrapper->isWeightsSaved()){
		return;
	}

	//Make sure that progress dialog is not being redrawn
	else if (updatingProgress){
		return;
	}

	//If we have reached this point, loading is complete
	heavyTaskTimer->stop();
	progressDialog->setValue(progressDialog->maximum());
	delete progressDialog;
	progressDialog = NULL;
}


/*! Deletes the raster plot dialog that invoked this slot.
	Should be triggered when the plot dialog is closed.  */
void CarlsimWidget::deleteMembranePotentialGraphDialog(int){
	unsigned tmpID = sender()->objectName().toUInt();
	qDebug()<<"Deleting Membrane potential graph dialog with ID: "<<tmpID;
}


/*! Deletes the raster plot dialog that invoked this slot.
	Should be triggered when the plot dialog is closed.  */
void CarlsimWidget::deleteRasterPlotDialog(int){
	unsigned tmpID = sender()->objectName().toUInt();
	qDebug()<<"Deleting Raster plot dialog with ID: "<<tmpID;
	if(!rasterDialogMap.contains(tmpID))
		throw SpikeStreamException("Raster dialog not found: ID=" + QString::number(tmpID));
	rasterDialogMap.remove(tmpID);
}


/*! Called when an experiment completes. Returns control to the user. */
void CarlsimWidget::experimentEnded(){
	toolBar->setEnabled(true);
	monitorGroupBox->setEnabled(true);
	injectGroupBox->setEnabled(true);
}


/*! Called when an experiment starts. Takes control away from the user. */
void CarlsimWidget::experimentStarted(){

	qInfo() << "CarlsimWidget::experimentStarted did not take the control away." << endl;
}



/*! Called when inject pattern button is clicked.
	Sets the injection of the selected pattern for the next time step in the wrapper */
void CarlsimWidget::injectPatternButtonClicked(){
	setInjectionPattern(false);
}


/*! If the string is set to LOAD_PATTERN_STRING, a dialog is displayed
	for the user to load up a new pattern into memory. */
void CarlsimWidget::loadPattern(QString comboStr){
	//Return if we are setting the pattern to another pattern.
	if(comboStr != LOAD_PATTERN_STRING){
		return;
	}

	//Reset pattern combo
	patternCombo->setCurrentIndex(0);

	//Select file containing new pattern.
	QString filePath = getFilePath("*.pat");
	if(filePath.isEmpty())
		return;
	if(!QFile::exists(filePath)){
		qCritical()<<"Selected file '"<<filePath<<"' does not exist.";
		return;
	}

	try{
		//Load up the new pattern
		Pattern newPattern;
		PatternManager::load(filePath, newPattern);

		//Add name to combo if it does not already exist
		if(!patternMap.contains(filePath)){
			patternCombo->insertItem(patternCombo->count() - 1, newPattern.getName());
		}

		//Replace pattern stored for this name with new pattern or add it if it doesn't exist.
		patternMap[filePath] = newPattern;

		//Set combo to display loaded item
		if(patternCombo->itemText(0) == "")
			patternCombo->removeItem(0);
		patternCombo->setCurrentIndex(patternCombo->count() - 2);

		//Enable injection and sustain buttons
		injectPatternButton->setEnabled(true);
		sustainPatternChkBox->setEnabled(true);
	}
	catch(SpikeStreamException* ex){
		qCritical()<<ex->getMessage();
	}
}


/*! Instructs the CARLsim wrapper to load the network from the database */
void CarlsimWidget::loadSimulation(){
	//Run some checks
	if(!Globals::networkLoaded()){
		qCritical()<<"Cannot load simulation: no network loaded.";
		return;
	}
	if(carlsimWrapper->isRunning()){
		qCritical()<<"CARLsim wrapper is already running - cannot load simulation.";
		return;
	}
	if(carlsimWrapper->isSimulationLoaded()){
		qCritical()<<"Simulation is already loaded - you must unload the current simulation before loading another.";
		return;
	}

	//Load the current neuron groups into control widgets
	loadNeuronGroups();

	try{
		//Store the current neuron colour for monitoring
		neuronColor = Globals::getNetworkDisplay()->getSimulationFiringNeuronColor();

		//Start loading of simulation
		taskCancelled = false;
		progressDialog = new QProgressDialog(tr("Loading simulation"), tr("Cancel"), 0, 100, this, Qt::CustomizeWindowHint);
		progressDialog->setWindowModality(Qt::WindowModal);
		progressDialog->show();
		updatingProgress = false;
		carlsimWrapper->start();//Load carried out by run method

		//Wait for loading to finish and update progress dialog
		loadingTimer  = new QTimer(this);
		connect(loadingTimer, SIGNAL(timeout()), this, SLOT(checkLoadingProgress()));
		loadingTimer->start(200);

	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}



/*! Called when the raster button is clicked.
	Launches a dialog to select the  neuron groups to monitor and then launches a raster plot dialog. */
void CarlsimWidget::memPotGraphButtonClicked(){
	//Get ID of current neuron
	neurid_t neurID = Globals::getNetworkDisplay()->getSingleNeuronID();
	if(neurID < START_NEURON_ID){
		qCritical()<<"No neuron selected. You must select a neuron to plot a membrane potential graph.";
		return;
	}

	try{
		//Create raster dialog
		MembranePotentialGraphDialog* memPotDlg = new MembranePotentialGraphDialog(neurID, this);
		memPotDlg->setObjectName(QString::number(neurID));
		connect(memPotDlg, SIGNAL(finished(int)), this, SLOT(deleteMembranePotentialGraphDialog(int)));
		memPotDlg->show();

	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}



/*! Controls whether the time step is updated each time step. */
void CarlsimWidget::monitorChanged(int state){
	try{
		if(state == Qt::Checked){
			carlsimWrapper->setMonitor(true);
			if(carlsimWrapper->isSimulationLoaded()){
			}
			noMonitorNeuronsButton->setEnabled(true);
			monitorFiringNeuronsButton->setEnabled(true);
			monitorMemPotNeuronsButton->setEnabled(true);
			monitorWeightsCheckBox->setEnabled(true);
#ifdef CARLSIM_AXONPLAST
			monitorDelaysCheckBox->setEnabled(true); 
#endif
		}
		else{
			carlsimWrapper->setMonitor(false);
			noMonitorNeuronsButton->setEnabled(false);
			monitorFiringNeuronsButton->setEnabled(false);
			monitorMemPotNeuronsButton->setEnabled(false);
			monitorWeightsCheckBox->setEnabled(false);
#ifdef CARLSIM_AXONPLAST
			monitorDelaysCheckBox->setEnabled(false);
#endif
		}
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Switches the monitoring of the simulation on or off */
void CarlsimWidget::monitorNeuronsStateChanged(int monitorType){
	try{
		//No monitoring at all
		if(monitorType == MONITOR_NEURONS_OFF){
			carlsimWrapper->setMonitorNeurons(false, false);
			rasterButton->setEnabled(false);
			memPotGraphButton->setEnabled(false);
		}
		//Monitoring firing neurons
		else if(monitorType == MONITOR_NEURONS_FIRING){
			carlsimWrapper->setMonitorNeurons(true, false);
			rasterButton->setEnabled(true);
			memPotGraphButton->setEnabled(false);
		}
		//Monitoring membrane potential
		else if(monitorType == MONITOR_NEURONS_MEMBRANE){
			carlsimWrapper->setMonitorNeurons(false, true);
			rasterButton->setEnabled(false);
			memPotGraphButton->setEnabled(true);
		}
		//Unrecognized value
		else{
			qCritical()<<"Monitor neuron type not recognized: "<<monitorType;
			return;
		}

		//Clear current highlights
		QHash<unsigned int, RGBColor*>* newHighlightMap = new QHash<unsigned int, RGBColor*>();
		Globals::getNetworkDisplay()->setNeuronColorMap(newHighlightMap);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Called when the wrapper thread exits.
	Unloads simulation and resets everything. */
void CarlsimWidget::carlsimWrapperFinished(){
	checkForErrors();
	unloadSimulation(false);
}


/*! Called when the network is changed.
	Shows an error if this simulation is running, otherwise enables or disables the toolbar. */
void CarlsimWidget::networkChanged(){
	if(carlsimWrapper->isSimulationRunning())
		qCritical()<<"Network should not be changed while simulation is running";

	//Unload simulation if it is loaded
	unloadSimulation(false);

	//Fix enabled status of toolbar
	checkWidgetEnabled();

#ifdef NC_EXTENSIONS_CARLSIM
	emit networkChangedProcessed();
#endif
}


/*! Called when the raster button is clicked.
	Launches a dialog to select the  neuron groups to monitor and then launches a raster plot dialog. */
void CarlsimWidget::rasterButtonClicked(){
	//Select neuron groups to monitor
	NeuronGroupSelectionDialog selectDlg(Globals::getNetwork(), this);
	if(selectDlg.exec() == QDialog::Accepted){
		QList<NeuronGroup*> neurGrpList = selectDlg.getNeuronGroups();

		//Create raster dialog
		SpikeRasterDialog* rasterDlg = new SpikeRasterDialog(neurGrpList, this);
		rasterDlg->setObjectName(QString::number(rasterDialogCtr));
		connect(rasterDlg, SIGNAL(finished(int)), this, SLOT(deleteRasterPlotDialog(int)));
		rasterDlg->show();

		//Store details so that we can update it
		rasterDialogMap[rasterDialogCtr] = rasterDlg;
		++rasterDialogCtr;
	}
}


/*! Instructs wrapper to reset weights to
	stored values at the next time step */
void CarlsimWidget::resetWeights(){
	//Check user wants to save weights.
	int response = QMessageBox::warning(this, "Reset Weights?", "Are you sure that you want to reset the weights.\nThis will overwrite the trained weights and cannot be undone.", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
	if(response != QMessageBox::Ok)
		return;

	try{

		//Start resetting of weights
		updatingProgress = false;
		taskCancelled = false;
		progressDialog = new QProgressDialog("Resetting weights", "Cancel", 0, 100, this);
		progressDialog->setWindowModality(Qt::WindowModal);
		progressDialog->setMinimumDuration(1000);

		//Instruct wrapper thread to start saving weights
		carlsimWrapper->resetWeights();

		//Wait for loading to finish and update progress dialog
		heavyTaskTimer  = new QTimer(this);
		connect(heavyTaskTimer, SIGNAL(timeout()), this, SLOT(checkResetWeightsProgress()));
		heavyTaskTimer->start(200);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}

/*! Instructs wrapper to save weights to
	weight field in database at the next time step */
void CarlsimWidget::saveWeights(){
	//Check user wants to save weights.
	int response = QMessageBox::warning(this, "Save Weights?", "Are you sure that you want to save weights.\nThis will overwrite the current weights in the loaded network.", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
	if(response != QMessageBox::Ok)
		return;

	//Double check that network does not have analyses
	if(Globals::getAnalysisDao()->networkHasAnalyses(Globals::getNetwork()->getID())){
		qCritical()<<"Network is linked to analyses - weights cannot be saved until analyses are deleted.";
		return;
	}

	try{
		//Start saving of weights
		updatingProgress = false;
		taskCancelled = false;
		progressDialog = new QProgressDialog("Saving weights", "Cancel", 0, 100, this, Qt::CustomizeWindowHint);
		progressDialog->setWindowModality(Qt::WindowModal);
		progressDialog->setMinimumDuration(0);
		progressDialog->setAutoClose(false);
		progressDialog->show();

		//Instruct wrapper thread to start saving weights
		carlsimWrapper->saveWeights();

		//Wait for loading to finish and update progress dialog
		heavyTaskTimer  = new QTimer(this);
		connect(heavyTaskTimer, SIGNAL(timeout()), this, SLOT(checkSaveWeightsProgress()));
		heavyTaskTimer->start(200);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Instructs wrapper to monitor weights by saving them to
	 temporary weight field in network at each time step */
void CarlsimWidget::setMonitorWeights(bool enable){
	carlsimWrapper->setMonitorWeights(enable);
}


#ifdef CARLSIM_AXONPLAST

/*! Checks for progress with the resetting of weights*/
void CarlsimWidget::checkResetDelaysProgress() {
	//Check for errors during resetting delays
	if (carlsimWrapper->isError()) {
		heavyTaskTimer->stop();
		progressDialog->setValue(progressDialog->maximum());
		delete progressDialog;
		progressDialog = NULL;
		qCritical() << "Error occurred resetting of weights: '" << carlsimWrapper->getErrorMessage() << "'.";
		return;
	}

	//Check for cancelation - stop timer and abort operation
	else if (progressDialog->wasCanceled()) {
		heavyTaskTimer->stop();
		carlsimWrapper->cancelResetDelays();
		delete progressDialog;
		progressDialog = NULL;
		return;
	}

	//If save weights is not complete return with timer running
	else if (!carlsimWrapper->isDelaysReset()) {
		return;
	}

	else if (updatingProgress) {
		return;
	}

	//If we have reached this point, loading is complete
	heavyTaskTimer->stop();
	progressDialog->setValue(progressDialog->maximum());
	delete progressDialog;
	progressDialog = NULL;
}


/*! Checks for progress with the saving of weights*/
void CarlsimWidget::checkSaveDelaysProgress() {
	//Check for errors during loading
	if (carlsimWrapper->isError()) {
		heavyTaskTimer->stop();
		progressDialog->setValue(progressDialog->maximum());
		delete progressDialog;
		progressDialog = NULL;
		qCritical() << "Error occurred saving of delays: '" << carlsimWrapper->getErrorMessage() << "'.";
		return;
	}

	//Check for cancelation - stop timer and abort operation
	else if (progressDialog->wasCanceled()) {
		heavyTaskTimer->stop();
		carlsimWrapper->cancelSaveDelays();
		delete progressDialog;
		progressDialog = NULL;
		return;
	}

	//If simulation has not been loaded return with loading timer still running
	else if (!carlsimWrapper->isDelaysSaved()) {
		return;
	}

	//Make sure that progress dialog is not being redrawn
	else if (updatingProgress) {
		return;
	}

	//If we have reached this point, loading is complete
	heavyTaskTimer->stop();
	progressDialog->setValue(progressDialog->maximum());
	delete progressDialog;
	progressDialog = NULL;
}


/*! Instructs wrapper to reset weights to
	stored values at the next time step */
void CarlsimWidget::resetDelays() {
	//Check user wants to save weights.
	int response = QMessageBox::warning(this, "Reset Delays?", "Are you sure that you want to reset the weights.\nThis will overwrite the trained weights and cannot be undone.", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
	if (response != QMessageBox::Ok)
		return;

	try {

		//Start resetting of weights
		updatingProgress = false;
		taskCancelled = false;
		progressDialog = new QProgressDialog("Resetting delays", "Cancel", 0, 100, this);
		progressDialog->setWindowModality(Qt::WindowModal);
		progressDialog->setMinimumDuration(1000);

		//Instruct wrapper thread to start saving weights
		carlsimWrapper->resetDelays();

		//Wait for loading to finish and update progress dialog
		heavyTaskTimer = new QTimer(this);
		connect(heavyTaskTimer, SIGNAL(timeout()), this, SLOT(checkResetDelaysProgress()));
		heavyTaskTimer->start(200);
	}
	catch (SpikeStreamException& ex) {
		qCritical() << ex.getMessage();
	}
}

/*! Instructs wrapper to save weights to
	weight field in database at the next time step */
void CarlsimWidget::saveDelays() {
	//Check user wants to save weights.
	int response = QMessageBox::warning(this, "Save Delays?", "Are you sure that you want to save weights.\nThis will overwrite the current weights in the loaded network.", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
	if (response != QMessageBox::Ok)
		return;

	//Double check that network does not have analyses
	if (Globals::getAnalysisDao()->networkHasAnalyses(Globals::getNetwork()->getID())) {
		qCritical() << "Network is linked to analyses - delays cannot be saved until analyses are deleted.";
		return;
	}

	try {
		//Start saving of weights
		updatingProgress = false;
		taskCancelled = false;
		progressDialog = new QProgressDialog("Saving delays", "Cancel", 0, 100, this, Qt::CustomizeWindowHint);
		progressDialog->setWindowModality(Qt::WindowModal);
		progressDialog->setMinimumDuration(0);
		progressDialog->setAutoClose(false);
		progressDialog->show();

		//Instruct wrapper thread to start saving weights
		carlsimWrapper->saveDelays();

		//Wait for loading to finish and update progress dialog
		heavyTaskTimer = new QTimer(this);
		connect(heavyTaskTimer, SIGNAL(timeout()), this, SLOT(checkSaveDelaysProgress()));
		heavyTaskTimer->start(200);
	}
	catch (SpikeStreamException& ex) {
		qCritical() << ex.getMessage();
	}
}


/*! Instructs CARLsim wrapper to monitor delays by saving them to
	 temporary delay field in network at each time step */
void CarlsimWidget::setMonitorDelays(bool enable) {
	carlsimWrapper->setMonitorDelays(enable);
}
#endif


/*! Sets the archive description using the contents of the archiveDescriptionEdit text field */
void CarlsimWidget::setArchiveDescription(){
	if(archiveDescriptionEdit->text().isEmpty())
		archiveDescriptionEdit->setText("Undescribed");
	if(carlsimWrapper->getArchiveID() == 0){
		qCritical()<<"Attempting to set archive description when no archive is in use by CARLsim.";
		return;
	}
	Globals::getArchiveDao()->setArchiveProperties(carlsimWrapper->getArchiveID(), archiveDescriptionEdit->text());
	Globals::getEventRouter()->archiveListChangedSlot();
}


/*! Sets the parameters of the neurons in the network */
void  CarlsimWidget::setNeuronParameters(){
	NeuronParametersDialog* dialog = new NeuronParametersDialog(this);
	dialog->exec();
	delete dialog;
}


/*! Sets the parameters */
void CarlsimWidget::setCarlsimParameters(){
	try{
		CarlsimParametersDialog dialog(carlsimWrapper->carlsim, carlsimWrapper->getCarlsimConfig(), carlsimWrapper->getSTDPFunctionID(), this);
		if(dialog.exec() == QDialog::Accepted){
			carlsimWrapper->setCarlsimConfig(dialog.getCarlsimConfig());
		}
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}

#ifdef NC_EXTENSIONS_CARLSIM
/*! Sets the parameters of Model */
void CarlsimWidget::setModelParameters(){
	try{
		ModelParametersDialog dialog(
			carlsimWrapper->getModelTime0().addMSecs( carlsimWrapper->getModelLagMs()), 
			carlsimWrapper->getModelSpeed(), 
			carlsimWrapper->getModelStepMs(), 
			(enum ModelParametersDialog::ModelStart) carlsimWrapper->getModelStart(), 
			(enum ModelParametersDialog::ModelAutosync) carlsimWrapper->getModelAutosync(), 
			carlsimWrapper->getModelLagMs(),			
			this);
		if(dialog.exec() == QDialog::Accepted){
			carlsimWrapper->setModelTime0(dialog.getSimtime().addMSecs( - (int) dialog.getLag_ms()) );  // Caution, needs to be converted to signed first
			carlsimWrapper->setModelStart((enum CarlsimWrapper::ModelStart) dialog.getStart());
			carlsimWrapper->setModelAutosync((enum CarlsimWrapper::ModelAutosync) dialog.getAutosync());
			carlsimWrapper->setModelStart((enum CarlsimWrapper::ModelStart) dialog.getStart());
			carlsimWrapper->setModelSpeed(dialog.getSpeed());
			carlsimWrapper->setModelStepMs(dialog.getStep_ms());
			carlsimWrapper->setModelLagMs(dialog.getLag_ms());
		}
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}
#endif

/*! Sets the parameters of the synapses in the network */
void CarlsimWidget::setSynapseParameters(){
	SynapseParametersDialog* dialog = new SynapseParametersDialog(this);
	dialog->exec();
	delete dialog;
}


/*! Called when CARLsim stops playing */
void CarlsimWidget::simulationStopped(){
	saveWeightsButton->setEnabled(true);
	playAction->setEnabled(true);
	stopAction->setEnabled(false);
	stepAction->setEnabled(true);
	unloadButton->setEnabled(true);
	neuronParametersButton->setEnabled(true);
}


/*! Called when the simulation has advanced one time step
	This version of the method only updates the time step. */
void CarlsimWidget::updateTimeStep(unsigned int ){

	//Allow simulation to proceed on to next step
	carlsimWrapper->clearWaitForGraphics();
}


/*! Called when the simulation has advanced one time step.
	This version of the method updates the time step and firing neuron IDs. */
void CarlsimWidget::updateTimeStep(unsigned int timeStep, const QList<unsigned>& neuronIDList){

	//Fill map with neuron ids
	QHash<unsigned int, RGBColor*>* newHighlightMap = new QHash<unsigned int, RGBColor*>();
	QList<unsigned>::const_iterator endList = neuronIDList.end();
	for(QList<unsigned>::const_iterator iter = neuronIDList.begin(); iter != endList; ++iter){
		(*newHighlightMap)[*iter] = neuronColor;
	}

	//Set highlight map in network display
	Globals::getNetworkDisplay()->setNeuronColorMap(newHighlightMap);

	//Update spike rasters
	for(QHash<unsigned, SpikeRasterDialog*>::iterator iter = rasterDialogMap.begin(); iter != rasterDialogMap.end(); ++iter)
		iter.value()->addData(neuronIDList, timeStep);

	//Allow simulation to proceed on to next step
	carlsimWrapper->clearWaitForGraphics();
}


/*! Called when the simulation has advanced one time step.
	This version of the method updates the time step and membrane potential. */
void CarlsimWidget::updateTimeStep(unsigned int /*timeStep*/, const QHash<unsigned, float>& membranePotentialMap){	

	//Fill map with appropriate colours depending on membrane potential.
	float tmpMemPot = 0.0f;
	QHash<unsigned int, RGBColor*>* newHighlightMap = new QHash<unsigned int, RGBColor*>();
	QHash<unsigned, float>::const_iterator endMap = membranePotentialMap.end();
	for(QHash<unsigned, float>::const_iterator iter = membranePotentialMap.begin(); iter != endMap; ++iter){
		//Update 3D display
		tmpMemPot = iter.value();
		if(tmpMemPot < -89.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[-1];
		else if(tmpMemPot < -78.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[0];
		else if(tmpMemPot < -67.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[1];
		else if(tmpMemPot < -56.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[2];
		else if(tmpMemPot < -45.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[3];
		else if(tmpMemPot < -34.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[4];
		else if(tmpMemPot < -23.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[5];
		else if(tmpMemPot < -12.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[6];
		else if(tmpMemPot < -1.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[7];
		else if(tmpMemPot < 10.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[8];
		else if(tmpMemPot < 21.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[9];
		else
			(*newHighlightMap)[iter.key()] = heatColorMap[10];

	}

	//Set network display
	Globals::getNetworkDisplay()->setNeuronColorMap(newHighlightMap);

	//Allow simulation to proceed on to next step
	carlsimWrapper->clearWaitForGraphics();
}


/*! Called when the rate of the simulation has been changed by the user. */
void CarlsimWidget::simulationRateChanged(int){
	if(simulationRateCombo->currentText() == "Max")
		carlsimWrapper->setFrameRate(0);
	else{
		unsigned int frameRate = Util::getUInt(simulationRateCombo->currentText());
		carlsimWrapper->setFrameRate(frameRate);
	}
}


/*! Starts the simulation running */
void CarlsimWidget::startSimulation(){
	//Do nothing if there is no simulation loaded or if wrapper is busy
	if(!carlsimWrapper->isSimulationLoaded()){
		return;
	}
	try{
		carlsimWrapper->playSimulation();
		playAction->setEnabled(false);
		stopAction->setEnabled(true);
		stepAction->setEnabled(false);
		unloadButton->setEnabled(false);
		neuronParametersButton->setEnabled(false);
		saveWeightsButton->setEnabled(false);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Starts simulation if it is not already running or stops it. */
void CarlsimWidget::startStopSimulation(){
	//Do nothing if there is no simulation loaded or if wrapper is busy
	if(!carlsimWrapper->isSimulationLoaded()){
		return;
	}
	if(carlsimWrapper->getCurrentTask() == CarlsimWrapper::NO_TASK_DEFINED)
		startSimulation();
	else if(carlsimWrapper->getCurrentTask() == CarlsimWrapper::RUN_SIMULATION_TASK)
		stopSimulation();
}


/*! Advances the simulation by one time step */
void CarlsimWidget::stepSimulation(){
	//Do nothing if there is no simulation loaded or if wrapper is busy
	if(!carlsimWrapper->isSimulationLoaded() || carlsimWrapper->getCurrentTask() != CarlsimWrapper::NO_TASK_DEFINED){
		return;
	}

	//Step simulation
	try{
		carlsimWrapper->stepSimulation();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Stops the simulation */
void CarlsimWidget::stopSimulation(){
	//Do nothing if there is no simulation loaded
	if(!carlsimWrapper->isSimulationLoaded()){
		return;
	}

	carlsimWrapper->stopSimulation();
}


/*! Switches between pattern injection controlled by button and
	continuous pattern injection at every time step */
void CarlsimWidget::sustainPatternChanged(bool enabled){
	if(enabled){
		injectPatternButton->setEnabled(false);
		injectPatternNeurGrpCombo->setEnabled(false);
		patternCombo->setEnabled(false);
		patternCurrentCombo->setEnabled(false);
		setInjectionPattern(true);
	}
	else{
		injectPatternButton->setEnabled(true);
		injectPatternNeurGrpCombo->setEnabled(true);
		patternCombo->setEnabled(true);
		patternCurrentCombo->setEnabled(true);

		//Switch off sustain pattern - the pattern will be automatically deleted on next step.
		carlsimWrapper->setSustainPattern(false);
	}
}


/*! Instructs the wrapper to discard the current simulation */
void CarlsimWidget::unloadSimulation(bool confirmWithUser){
	//Double check that user wants to unload simulation
	if(confirmWithUser){
		int response = QMessageBox::warning(this, "Unload?", "Are you sure that you want to unload the simulation?\nAny unsaved weight changes will be lost.", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
		if(response != QMessageBox::Ok)
			return;
	}

	//Unload the simulation
	carlsimWrapper->unloadSimulation();
	Globals::setSimulation(NULL);

	//Clear network display
	Globals::getNetworkDisplay()->setNeuronColorMap(new QHash<unsigned int, RGBColor*>());

	//Set buttons appropriately
	loadButton->setEnabled(true);
	unloadButton->setEnabled(false);
	synapseParametersButton->setEnabled(true);
	carlsimParametersButton->setEnabled(true);
	controlsWidget->setEnabled(false);
	archiveDescriptionEdit->setText("Undescribed");
	archiveCheckBox->setChecked(false);//Single archive associated with each simulation run


	//Inject patterns, current etc.
	sustainPatternChkBox->setChecked(false);
	injectPatternButton->setEnabled(true);
	injectPatternNeurGrpCombo->setEnabled(true);
	patternCombo->setEnabled(true);
	patternCurrentCombo->setEnabled(true);


	//Clean up any raster plots
	for(QHash<unsigned, SpikeRasterDialog*>::iterator iter = rasterDialogMap.begin(); iter != rasterDialogMap.end(); ++iter){
		delete iter.value();
	}
	rasterDialogMap.clear();

}


/*! Updates progress with loading the simulation */
void CarlsimWidget::updateProgress(int stepsCompleted, int totalSteps){
	//Set flag to avoid multiple calls to progress dialog while it is redrawing
	if(updatingProgress)
		return;
	updatingProgress = true;

	if(progressDialog == NULL)
		return;

	//Check numbers are sensible
	if(stepsCompleted > totalSteps){
		qCritical()<<"Progress update error: Number of steps completed is greater than the number of possible steps.";
		return;
	}

	//Update progress
	if(stepsCompleted < totalSteps){
		progressDialog->setValue(stepsCompleted);
		progressDialog->setMaximum(totalSteps);
	}

	//Clear updating progress flag
	updatingProgress = false;
}


/*----------------------------------------------------------*/
/*------               PRIVATE METHODS                ------*/
/*----------------------------------------------------------*/

/*! Checks the wrapper for errors */
bool CarlsimWidget::checkForErrors(){
	if(carlsimWrapper->isError()){
		qCritical()<<"CarlsimWrapper error: '"<<carlsimWrapper->getErrorMessage()<<"'.";
		return true;
	}
	return false;
}


/*! Checks to see if network is been loaded and sets enabled status of toolbar appropriately */
void CarlsimWidget::checkWidgetEnabled(){
	if(Globals::networkLoaded())   
		mainGroupBox->setEnabled(true);
	else
		mainGroupBox->setEnabled(false);

}


/*! Builds a set of colours for use when displaying the membrane potential. */
void CarlsimWidget::createMembranePotentialColors(){
		heatColorMap[-1] = new RGBColor(0.0f, 0.0f, 0.0f);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[-1]);
		heatColorMap[0] = new RGBColor(HEAT_COLOR_0);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[0]);
		heatColorMap[1] = new RGBColor(HEAT_COLOR_1);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[1]);
		heatColorMap[2] = new RGBColor(HEAT_COLOR_2);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[2]);
		heatColorMap[3] = new RGBColor(HEAT_COLOR_3);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[3]);
		heatColorMap[4] = new RGBColor(HEAT_COLOR_4);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[4]);
		heatColorMap[5] = new RGBColor(HEAT_COLOR_5);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[5]);
		heatColorMap[6] = new RGBColor(HEAT_COLOR_6);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[6]);
		heatColorMap[7] = new RGBColor(HEAT_COLOR_7);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[7]);
		heatColorMap[8] = new RGBColor(HEAT_COLOR_8);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[8]);
		heatColorMap[9] = new RGBColor(HEAT_COLOR_9);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[9]);
		heatColorMap[10] = new RGBColor(HEAT_COLOR_10);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[10]);
}


/*! Fills the pattern current combo with appropriate values */
void CarlsimWidget::fillPatternCurrentCombo(){
	patternCurrentCombo->addItem("Fire");
	for(int i=0; i<=100; i+=10)
		patternCurrentCombo->addItem(QString::number(i));
}

/*! Enables user to enter a file path */
QString CarlsimWidget::getFilePath(QString fileFilter){
	QFileDialog dialog(this);
	dialog.setDirectory(Globals::getWorkingDirectory());
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter( QString("Configuration files (" + fileFilter + ")") );
	dialog.setViewMode(QFileDialog::Detail);
	QStringList fileNames;
	if (dialog.exec())
		fileNames = dialog.selectedFiles();
	if(fileNames.size() > 0)
		return fileNames[0];
	else
		return QString("");
}


/*! Extracts the neuron group id from the supplied string. */
unsigned CarlsimWidget::getNeuronGroupID(QString neurGrpStr){
	QRegExp regExp("[()]");
	return Util::getUInt(neurGrpStr.section(regExp, 1, 1));
}


/*! The pattern map uses the file path as the key; the pattern combo uses the name of the pattern.
	This method looks up the file path corresponding to a pattern name, returning the first result
	that is found. Exception is thrown if pattern name does not exist in the patterns in the pattern
	map. */
QString CarlsimWidget::getPatternKey(const QString& patternComboText){
	if(patternComboText == LOAD_PATTERN_STRING){
		throw SpikeStreamException("Cannot inject pattern: no patterns added.");
	}

	//Find the key to the pattern in the pattern map
	QString patternKey = "";
	for(QHash<QString, Pattern>::iterator iter = patternMap.begin(); iter != patternMap.end(); ++iter){
		if(iter.value().getName() == patternComboText){
			patternKey = iter.key();
			break;
		}
	}

	//Check that key has been found
	if(patternKey.isEmpty()){
		throw SpikeStreamException("Pattern not found: " + patternComboText);
	}

	return patternKey;
}


/*! Builds the toolbar that goes at the top of the page. */
QToolBar* CarlsimWidget::getToolBar(){
	QToolBar* tmpToolBar = new QToolBar(this);

	//tmpToolBar->setStyleSheet("QToolBar{ background-color: black;  border: 2px solid yellow; border-radius: 5 }");

	playAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/play.png"), tr("action_start_sim"), this);
	connect(playAction, SIGNAL(triggered()), this, SLOT(startSimulation()));
	tmpToolBar->addAction(playAction);

	stopAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/stop.png"), tr("action_stop_sim"), this);
	connect(stopAction, SIGNAL(triggered()), this, SLOT(stopSimulation()));
	stopAction->setEnabled(false);
	tmpToolBar->addAction(stopAction);

	stepAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/step.png"), tr("action_step_sim"), this);
	connect(stepAction, SIGNAL(triggered()), this, SLOT(stepSimulation()));
	tmpToolBar->addAction(stepAction);

	resetAction = new QAction(tr("Reset"), this);
	connect(resetAction, SIGNAL(triggered()), this, SLOT(resetSimtime()));
	tmpToolBar->addAction(resetAction);

	QCheckBox* monitorChkBox = new QCheckBox(tr("Monitor"));
	monitorChkBox->setChecked(false);
	connect(monitorChkBox, SIGNAL(stateChanged(int)), this, SLOT(monitorChanged(int)));
	tmpToolBar->addSeparator();
	tmpToolBar->addWidget(monitorChkBox);

	return tmpToolBar;
}

/*! Loads the current neuron groups into the control widgets */
void CarlsimWidget::loadNeuronGroups(){
	//injectNoiseNeuronGroupCombo->clear();
	injectPatternNeurGrpCombo->clear();

	QList<NeuronGroupInfo> neurGrpInfoList = Globals::getNetwork()->getNeuronGroupsInfo();
	foreach(NeuronGroupInfo info, neurGrpInfoList){
		//injectNoiseNeuronGroupCombo->addItem(info.getName() + "(" + QString::number(info.getID()) + ")");
		injectPatternNeurGrpCombo->addItem(info.getName() + "(" + QString::number(info.getID()) + ")");
	}
}



/*! Sets the injection pattern */
void CarlsimWidget::setInjectionPattern(bool sustain){  
	//Set pattern in the CARLsim wrapper
	try{
		if (patternCurrentCombo->currentIndex() == 0) { //Fire neurons
			auto &pattern = patternMap[getPatternKey(patternCombo->currentText())];
			auto neuronGroupID = getNeuronGroupID(injectPatternNeurGrpCombo->currentText());
			carlsimWrapper->setFiringInjectionPattern(
				pattern,
				neuronGroupID,
				sustain
			);
		}
		else{
			float injectionCurrent = Util::getFloat(patternCurrentCombo->currentText());
			carlsimWrapper->setCurrentInjectionPattern(
					patternMap[getPatternKey(patternCombo->currentText())],
					injectionCurrent,
					getNeuronGroupID(injectPatternNeurGrpCombo->currentText()),
					sustain
			);
		}
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


