//SpikeStream includes
#include "Globals.h"
#include "WmExptWidget.h"
#include "WmExptManager.h"
#include "CarlsimOatWidget.h"
#include "MonitorLoaderWidget.h"
#include "Util.h"
#include "DescriptionDialog.h"

using namespace spikestream;

//Qt includes
#include <QLabel>
#include <QLayout>


//Functions for dynamic library loading
extern "C" {
	/*! Creates an QWidget class when library is dynamically loaded. */
	_MSC_DLL_API QWidget* getClass(){
		return new WmExptWidget();
	}

	/*! Returns a descriptive name for this widget */
	_MSC_DLL_API QString getName(){
		return QString("WM Experiment");
	}
}


/*! Constructor */
WmExptWidget::WmExptWidget(QWidget* parent) : AbstractExperimentWidget(parent){

	//Add toolbar to start and stop experiment
	QVBoxLayout* mainVBox = new QVBoxLayout(this);


	QToolBar* toolBar = getToolBar();

	toolBar->addWidget(new QLabel("  Presets: "));
	templateCombo = new QComboBox();
	toolBar->addWidget(templateCombo);

	mainVBox->addWidget(toolBar);

	//Add status display area
	statusTextEdit = new QTextEdit(this);
	statusTextEdit->setReadOnly(true);
	mainVBox->addWidget(statusTextEdit);
	//mainVBox->addStretch(5);


	//Initialize variables
	carlsimWrapper = NULL;
	buildParameters();

	// Template

	// Load configurations
// vte is placed in a subdirectory as it does not depend on the central spikestream config
	QFileInfo configFile(ConfigLoader::getConfigFilePath());
	auto configDir = configFile.absoluteDir().absolutePath() + +"/wm";
	qDebug() << configDir << endl;

	auto defaultTemplate = 0;
	auto items = 0;
	for (QDirIterator confFileIter(configDir, { "*.expt" }, QDir::Files); confFileIter.hasNext(); )
	{
		confFileIter.next();
		auto path = confFileIter.filePath();
		qDebug() << path << endl;
		ConfigLoader* configLoader = new ConfigLoader(path);
		wmConfigLoaders.append(configLoader);
		templateCombo->addItem(configLoader->getParameter("name"));
		if(configLoader->hasParameter("default") && configLoader->getParameter("default").compare("true") == 0)
			defaultTemplate = items;
		items++;
	}

	connect (templateCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTemplate(int)));
	templateCombo->setMinimumSize(50, 20);

	if (items > 0) {
		templateCombo->setCurrentIndex(defaultTemplate);
		//updateTemplate(defaultTemplate); 
		emit updateTemplate(defaultTemplate);
	}
		

	//Create experiment manager to run experiment
	wmExptManager = new WmExptManager();
	connect(wmExptManager, SIGNAL(finished()), this, SLOT(managerFinished()));
	connect(wmExptManager, SIGNAL(statusUpdate(QString)), this, SLOT(updateStatus(QString)));
	connect(wmExptManager, SIGNAL(simulationReloaded()), this, SLOT(updateMonitor()));

	//Reference to hard earned spikeMonitor
	spikeMonitor = nullptr; 

	//placeCells = nullptr;



}


/*! Destructor */
WmExptWidget::~WmExptWidget(){
}



/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Sets the wrapper that will be used in the experiments.
	The void pointer must point to an object of class NemoWrapper */
void WmExptWidget::setWrapper(void *wrapper){
//void WmExptWidget::setWrapper(void *wrapper, CarlsimWrapperWidget* carlsimWrapperWidget){
	this->carlsimWrapper = (CarlsimWrapper*) wrapper;
}


/*----------------------------------------------------------*/
/*------                PUBLIC SLOTS                ------*/
/*----------------------------------------------------------*/
void WmExptWidget::assignOatMonitor() {
	//Signal the CarlsimWidget, that this Experiment Widgets requires the SpikeMonitor
//Hint: reason for "ISSUE could not load symbol" as missing API
//		1 > WmExptWidget.obj : error LNK2019 : unresolved external symbol "public: void __cdecl spikestream::carlsim_monitors::CarlsimOatWidget::findMonitor(class spikestream::carlsim_monitors::OatMonitor * &,class QString,class QString)" (? findMonitor@CarlsimOatWidget@carlsim_monitors@spikestream@@QEAAXAEAPEAVOatMonitor@23@VQString@@1@Z) referenced in function "private: virtual void __cdecl spikestream::WmExptWidget::startExperiment(void)" (? startExperiment@WmExptWidget@spikestream@@EEAAXXZ)
//		1 > C:\cockroach - ut3\build\src\SpikeStream\simulators\carlsim\experiments\epropexperiment\Debug\epropexperiment.dll : fatal error LNK1120 : 1 unresolved externals
	AbstractMonitorWidget* abstractMonitorWidget = nullptr;
	emit requiresMonitorWidget(abstractMonitorWidget, "CarlsimOatWidget");
	carlsim_monitors::CarlsimOatWidget* oatWidget = (carlsim_monitors::CarlsimOatWidget*)abstractMonitorWidget;
	carlsim_monitors::OatMonitor* oatMonitor = nullptr;
	oatWidget->findMonitor(oatMonitor, "OatSpikeMonitor", "dlPFC.states");
	if (oatMonitor == nullptr) {
		qCritical() << "OAT SpikeMonitor not available.";
		return;
	}
	spikeMonitor = (carlsim_monitors::OatSpikeMonitor*) oatMonitor;
}


void WmExptWidget::updateMonitor() {
	wmExptManager->setMonitor(spikeMonitor);
}


void WmExptWidget::updateTemplate(int i) {

	configLoader = wmConfigLoaders[i];

//	configLoader->loadParameter(); // refresh

	QString parameterName;

//	parameterName = "costs";
//	if (configLoader->hasParameter(parameterName)) {
//		costs = configLoader->getParameter(parameterName);
//	}
//	else 
//		costs = "";
//
//	// Boone: Route defines training segments
//	parameterName = "route";
//	if (configLoader->hasParameter(parameterName)) {
//		auto route = configLoader->getParameter(parameterName);
//		segments = route.split(';', Qt::SkipEmptyParts);  // ignore redundant separator
//	}
//	else 
//		segments.clear(); // reset collection in case it was set before (e.g. by a default config)
//
//	// Boone Landmarks define trails
//	parameterName = "landmarks";
//	if (configLoader->hasParameter(parameterName)) {
//		auto landmark = configLoader->getParameter(parameterName);
//		landmarks = landmark.split(';', Qt::SkipEmptyParts);  // ignore redundant separator
//	}
//	else 
//		landmarks.clear();
//	
//
//	// Morris: fixed target with random startpoints define segments
//	// generalized: cartesian product of start x end points and random selection 
//	// random walk by indizes to cartesian product
//	parameterName = "start_points";
//	if (configLoader->hasParameter(parameterName)) {
//		auto startPoints = configLoader->getParameter(parameterName);
//		starts = startPoints.split(';', Qt::SkipEmptyParts);  // ignore redundant separator
//	}
//	else
//		starts.clear();
//
//	parameterName = "end_points";
//	if (configLoader->hasParameter(parameterName)) {
//		auto endPoints = configLoader->getParameter(parameterName);
//		ends = endPoints.split(';', Qt::SkipEmptyParts);  // ignore redundant separator
//	}
//	else
//		ends.clear();

	parameterName = "selection";
	if (configLoader->hasParameter(parameterName)) {
		auto indeces = configLoader->getParameter(parameterName);
		selection = indeces.split(';', Qt::SkipEmptyParts);  // ignore redundant separator
	}
	else
		selection.clear();



	auto setIntParam = [&](QString parameterName) {
		if (configLoader->hasParameter(parameterName)) {
			auto value = Util::getInt(configLoader->getParameter(parameterName));
			parameterMap[parameterName] = value;
			defaultParameterMap[parameterName] = value;
		}
	};

	setIntParam("experiment_number");

	setIntParam("target_neuron");
	setIntParam("start_neuron");
	setIntParam("initial_current");
//	setIntParam("recovery_time_ms");
//	setIntParam("time_grid_ms");
	setIntParam("observation_time_ms");
	setIntParam("start_time_ms");
	setIntParam("end_time_ms");
//	setIntParam("update_delays");
	setIntParam("verbosity");
	setIntParam("training_loops");
//	setIntParam("border");
//	setIntParam("barrier");

	auto setDoubleParam = [&](QString parameterName) {
		if (configLoader->hasParameter(parameterName)) {
			auto value = Util::getDouble(configLoader->getParameter(parameterName));
			parameterMap[parameterName] = value;
			defaultParameterMap[parameterName] = value;
		}
	};

//	setDoubleParam("slow_motion");
//	setDoubleParam("fast_forward");
};


//void WmExptWidget::editCosts() {
//	DescriptionDialog* descDialog = new DescriptionDialog(costs, true, this);
//	if (descDialog->exec() == QDialog::Accepted) {
//		costs = descDialog->getDescription();
//	}
//	delete descDialog;
//}


/*----------------------------------------------------------*/
/*------                 PRIVATE SLOTS                ------*/
/*----------------------------------------------------------*/

/*! Called when the manager exits */
void WmExptWidget::managerFinished(){
	//Check for errors
	//if(wmExptManager->isError()){
	//	qCritical()<<wmExptManager->getErrorMessage();
	//}

	//Signal to other classes that experiment is complete
	playAction->setEnabled(true);
	stopAction->setEnabled(false);
	emit experimentEnded();
}


//Inherited from AbstractExperimentWidget
void WmExptWidget::startExperiment(){
	//Run some checks before experiment can proceed.
	if(carlsimWrapper == NULL){
		qCritical()<<"carlsimWrapper has not been set.";
		return;
	}
	if(!carlsimWrapper->isSimulationLoaded()){
		qCritical()<<"Experiment cannot be started until carlsimWrapper has a loaded simulation.";
		return;
	}
	try{
		checkNetwork();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		return;
	}

	//see assignOatMonitor
	if (spikeMonitor == nullptr) {
		qCritical() << "SpikeMonitor has not been set.";
		return;
	}


	//Start experiment and signal that it has started.
	statusTextEdit->clear();
	playAction->setEnabled(false);
	stopAction->setEnabled(true);
	emit experimentStarted();
	try{
		wmExptManager->startExperiment(carlsimWrapper, spikeMonitor,
			parameterMap, 
			configLoader->getParameter("name"),
			//, // presets (configuration name)
			//costs, 
			//segments, landmarks, // Boone
			//starts, ends, 
			selection
			//placeCells
		);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		managerFinished();
	}
}


//Inherited from AbstractExperimentWidget
void WmExptWidget::stopExperiment(){
	wmExptManager->stop();
}


/*! Adds the specified message to the status pane */
void WmExptWidget::updateStatus(QString statusMsg){
	statusTextEdit->append(statusMsg);
}


/*----------------------------------------------------------*/
/*------              PROTECTED METHODS               ------*/
/*----------------------------------------------------------*/

/*! Builds the current and default parameter map along with the parameter
	info list. */
void WmExptWidget::buildParameters(){
	//Reset maps and list
	parameterInfoList.clear();
	defaultParameterMap.clear();
	parameterMap.clear();

	ParameterInfo tmpInfo("experiment_number", "The experiment that is carried out", ParameterInfo::OPTION);
	QList<QString> exptNameList;
	exptNameList.append("Experiment 0 - Trigger specific event");   
	exptNameList.append("Experiment 1 - Transition Paths");  // from start to target, left, right
	exptNameList.append("Experiment 2 - Random walk");  
	//exptNameList.append("Experiment 4 - Random walk");
	tmpInfo.setOptionNames(exptNameList);
	parameterInfoList.append(tmpInfo);
	defaultParameterMap["experiment_number"] = 0;
	parameterMap["experiment_number"] = defaultParameterMap["experiment_number"];

	parameterInfoList.append(ParameterInfo("random_seed", "Seed for random number generator.", ParameterInfo::INTEGER));
	defaultParameterMap["random_seed"] = 50;
	parameterMap["random_seed"] = defaultParameterMap["random_seed"];

	parameterInfoList.append(ParameterInfo("pause_interval_ms", "Time in ms to pause between sections of the experiment", ParameterInfo::INTEGER));
	defaultParameterMap["pause_interval_ms"] = 1; // 1000
	parameterMap["pause_interval_ms"] = defaultParameterMap["pause_interval_ms"];

	// VTE 
	QString parameterName; 
	
	parameterName = "start_neuron";   // Hint: this could be a pattern
	parameterInfoList.append(ParameterInfo(parameterName, "Neuron ID that indicates start state", ParameterInfo::INTEGER));
	defaultParameterMap[parameterName] = 0; // 0..5
	parameterMap[parameterName] = defaultParameterMap[parameterName];

	parameterName = "target_neuron";   // Hint: this could be a pattern
	parameterInfoList.append(ParameterInfo(parameterName, "Neuron ID of the target state", ParameterInfo::INTEGER));
	defaultParameterMap[parameterName] = 5; 
	parameterMap[parameterName] = defaultParameterMap[parameterName];

	parameterName = "initial_current";
	parameterInfoList.append(ParameterInfo(parameterName, "The initial current to be injected", ParameterInfo::INTEGER));  // alternativ spike
	defaultParameterMap[parameterName] = 20;
	parameterMap[parameterName] = defaultParameterMap[parameterName];

	//parameterName = "recovery_time_ms";
	//parameterInfoList.append(ParameterInfo(parameterName, "The time that is neccessary for the network to receive new input", ParameterInfo::INTEGER));
	//defaultParameterMap[parameterName] = 600;  // sync to 1 sec. grid, skip by switching off the monitoring
	//parameterMap[parameterName] = defaultParameterMap[parameterName];

	//parameterName = "time_grid_ms";
	//parameterInfoList.append(ParameterInfo(parameterName, "ms, sync agains a time grid, e.g. 500ms CARLsim OAT or 1s in SpikeStream", ParameterInfo::INTEGER));
	//defaultParameterMap[parameterName] = 1000;
	//parameterMap[parameterName] = defaultParameterMap[parameterName];

	//parameterName = "slow_motion";
	//parameterInfoList.append(ParameterInfo(parameterName, "speed factor, set in the time pannel 1ms * 0.002 => 0.333s = 3/10s delay between each", ParameterInfo::DOUBLE));
	////defaultParameterMap[parameterName] = 0.002;
	//defaultParameterMap[parameterName] = 0.020;
	//parameterMap[parameterName] = defaultParameterMap[parameterName];

	//parameterName = "fast_forward";
	//parameterInfoList.append(ParameterInfo(parameterName, "speed factor for fast forward, set the time pannel", ParameterInfo::INTEGER));
	//defaultParameterMap[parameterName] = 1.0;
	//parameterMap[parameterName] = defaultParameterMap[parameterName];

	//parameterName = "monitor_off";
	//parameterInfoList.append(ParameterInfo(parameterName, "when fast forward, switch off the monitoring", ParameterInfo::BOOLEAN));
	//defaultParameterMap[parameterName] = true;
	//parameterMap[parameterName] = defaultParameterMap[parameterName];

	parameterName = "observation_time_ms";
	parameterInfoList.append(ParameterInfo(parameterName, "ms how long shall it run in slow motion ", ParameterInfo::INTEGER));
	defaultParameterMap[parameterName] = 200; // 
	parameterMap[parameterName] = defaultParameterMap[parameterName];

	//parameterName = "start_time_ms";
	//parameterInfoList.append(ParameterInfo(parameterName, "ms when to start the experiment, relativ to the time_grid, e.g. 1.1 s", ParameterInfo::INTEGER));
	//defaultParameterMap[parameterName] = 100;
	//parameterMap[parameterName] = defaultParameterMap[parameterName];

	parameterName = "end_time_ms";
	parameterInfoList.append(ParameterInfo(parameterName, "ms when to stop the experiment, e.g. 10ms it stops, when 10ms are passed, (the pannel shows 10), -1 indefinitively", ParameterInfo::INTEGER));
	defaultParameterMap[parameterName] = 200; // 
	parameterMap[parameterName] = defaultParameterMap[parameterName];


	//parameterName = "update_delays";
	//parameterInfoList.append(ParameterInfo(parameterName, "update delays eligibility trace", ParameterInfo::INTEGER));  
	//defaultParameterMap[parameterName] = 1;
	//parameterMap[parameterName] = defaultParameterMap[parameterName];

	parameterName = "verbosity";
	parameterInfoList.append(ParameterInfo(parameterName, "output to experiment log: 0 none, 1 info, 2 details, 3 debug, 4 dev", ParameterInfo::INTEGER));  
	defaultParameterMap[parameterName] = 2;
	parameterMap[parameterName] = defaultParameterMap[parameterName];

	parameterName = "training_loops";
	parameterInfoList.append(ParameterInfo(parameterName, "training loops to be performed", ParameterInfo::INTEGER));
	defaultParameterMap[parameterName] = 1;
	parameterMap[parameterName] = defaultParameterMap[parameterName];

	//parameterName = "border";
	//parameterInfoList.append(ParameterInfo(parameterName, "border", ParameterInfo::INTEGER));
	//defaultParameterMap[parameterName] = 120;
	//parameterMap[parameterName] = defaultParameterMap[parameterName];

	//parameterName = "barrier";
	//parameterInfoList.append(ParameterInfo(parameterName, "barrier NId or -1 if none", ParameterInfo::INTEGER));
	//defaultParameterMap[parameterName] = -1;
	//parameterMap[parameterName] = defaultParameterMap[parameterName];



/*
	slow_motion		0.003	speed factor, set in the time pannel 1ms * 0.002 => 0.333s = 3/10s delay between each 
	fast_forward	0.030	speed factor for fast forward, set the time pannel
	monitor_off		true	when fast forward, switch off the monitoring
	time_grid		 1000	ms, sync agains a time grid, e.g. 500ms CARLsim OAT or 1s in SpikeStream
	observation_time   50	ms how long shall it run in slow motion 
	start_time		  100   ms when to start the experiment, relativ to the time_grid, e.g. 1.1 s
	end_time		   -1	ms when to stop the experiment, e.g. 10ms it stops, when 10ms are passed, (the pannel shows 10), -1 indefinitively 

	Hint: the two modes apply for SpikeStream in general
*/


/* Template
	parameterName = "A_B";
	parameterInfoList.append(ParameterInfo(parameterName , "DESC_OF_A_B", ParameterInfo::INTEGER));
	defaultParameterMap[parameterName ] = ;
	parameterMap[parameterName] = defaultParameterMap[parameterName];
*/

	parametersBuilt = true;
}

      
/*----------------------------------------------------------*/
/*------                PRIVATE METHODS               ------*/
/*----------------------------------------------------------*/

/*! Checks that the network has the right number of neurons for the experiments. */
void WmExptWidget::checkNetwork(){
	Network* tmpNetwork = Globals::getNetwork();
	QList<NeuronGroup*> neurGrpList = tmpNetwork->getNeuronGroups();

	//Check that input layer, temporal coding and feature detection are there
	bool inputLayerFound = false;
	foreach(NeuronGroup* tmpNeurGrp, neurGrpList){
		//if (tmpNeurGrp->getInfo().getName().toUpper() == "INPUT LAYER") {
		//if (tmpNeurGrp->getInfo().getName().toUpper() == "CA1 PLACE CELLS") {
		if (tmpNeurGrp->getInfo().getName() == "dlPFC.in") {
			inputLayerFound = true;
			dlPFC_in = tmpNeurGrp;
			break;
		}
	}

	//Throw exception if we have not found the necessary neuron groups
	if(!inputLayerFound)
		throw SpikeStreamException("Input layer could not be found. Make sure there is a neuron gruop labelled 'Input layer' in your network.");
}



