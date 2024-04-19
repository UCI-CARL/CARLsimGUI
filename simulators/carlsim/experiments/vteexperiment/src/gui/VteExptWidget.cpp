//SpikeStream includes
#include "Globals.h"
#include "VteExptWidget.h"
using namespace spikestream;

//Qt includes
#include <QLabel>
#include <QLayout>


//Functions for dynamic library loading
extern "C" {
	/*! Creates an QWidget class when library is dynamically loaded. */
	_MSC_DLL_API QWidget* getClass(){
		return new VteExptWidget();
	}

	/*! Returns a descriptive name for this widget */
	_MSC_DLL_API QString getName(){
		return QString("VTE Experiment");
	}
}


/*! Constructor */
VteExptWidget::VteExptWidget(QWidget* parent) : AbstractExperimentWidget(parent){
	//Add toolbar to start and stop experiment
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	QToolBar* toolBar = getToolBar();
	mainVBox->addWidget(toolBar);

	//Add status display area
	statusTextEdit = new QTextEdit(this);
	statusTextEdit->setReadOnly(true);
	mainVBox->addWidget(statusTextEdit);
	mainVBox->addStretch(5);

	//Initialize variables
	carlsimWrapper = NULL;
	buildParameters();

	//Create experiment manager to run experiment
	vteExptManager = new VteExptManager();
	connect(vteExptManager, SIGNAL(finished()), this, SLOT(managerFinished()));
	connect(vteExptManager, SIGNAL(statusUpdate(QString)), this, SLOT(updateStatus(QString)));
}


/*! Destructor */
VteExptWidget::~VteExptWidget(){
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Sets the wrapper that will be used in the experiments.
	The void pointer must point to an object of class NemoWrapper */
void VteExptWidget::setWrapper(void *wrapper){
	this->carlsimWrapper = (CarlsimWrapper*)wrapper;
}


/*----------------------------------------------------------*/
/*------                 PRIVATE SLOTS                ------*/
/*----------------------------------------------------------*/

/*! Called when the manager exits */
void VteExptWidget::managerFinished(){
	//Check for errors
	if(vteExptManager->isError()){
		qCritical()<<vteExptManager->getErrorMessage();
	}

	//Signal to other classes that experiment is complete
	playAction->setEnabled(true);
	stopAction->setEnabled(false);
	emit experimentEnded();
}


//Inherited from AbstractExperimentWidget
void VteExptWidget::startExperiment(){
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

	//Start experiment and signal that it has started.
	statusTextEdit->clear();
	playAction->setEnabled(false);
	stopAction->setEnabled(true);
	emit experimentStarted();
	try{
		vteExptManager->startExperiment(carlsimWrapper, parameterMap);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		managerFinished();
	}
}


//Inherited from AbstractExperimentWidget
void VteExptWidget::stopExperiment(){
	vteExptManager->stop();
}


/*! Adds the specified message to the status pane */
void VteExptWidget::updateStatus(QString statusMsg){
	statusTextEdit->append(statusMsg);
}


/*----------------------------------------------------------*/
/*------              PROTECTED METHODS               ------*/
/*----------------------------------------------------------*/

/*! Builds the current and default parameter map along with the parameter
	info list. */
void VteExptWidget::buildParameters(){
	//Reset maps and list
	parameterInfoList.clear();
	defaultParameterMap.clear();
	parameterMap.clear();

	ParameterInfo tmpInfo("experiment_number", "The experiment that is carried out", ParameterInfo::OPTION);
	QList<QString> exptNameList;
	exptNameList.append("Experiment 1 - Start from fixed location");   
	exptNameList.append("Experiment 2 - Perform recovery"); // skip to next grid, e.g. 1s respecting min recovery time 
	exptNameList.append("Experiment 3 - Run pattern sequence");  // Random: apply Injector at 1%,  Alternative: Pattern to identify the location be a starting point (in xlattice coordinates), list of patterns as option, or points 
	exptNameList.append("Experiment 4 - Random walk");
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
	parameterInfoList.append(ParameterInfo(parameterName, "Neuron ID that indicates the current location", ParameterInfo::INTEGER));
	defaultParameterMap[parameterName] = 0;
	parameterMap[parameterName] = defaultParameterMap[parameterName];

	parameterName = "initial_current";
	parameterInfoList.append(ParameterInfo(parameterName, "The initial current to be injected", ParameterInfo::INTEGER));  // alternativ spike
	defaultParameterMap[parameterName] = 30;
	parameterMap[parameterName] = defaultParameterMap[parameterName];

	parameterName = "recovery_time_ms";
	parameterInfoList.append(ParameterInfo(parameterName, "The time that is neccessary for the network to receive new input", ParameterInfo::INTEGER));
	defaultParameterMap[parameterName] = 600;  // sync to 1 sec. grid, skip by switching off the monitoring
	parameterMap[parameterName] = defaultParameterMap[parameterName];

	parameterName = "time_grid_ms";
	parameterInfoList.append(ParameterInfo(parameterName, "ms, sync agains a time grid, e.g. 500ms CARLsim OAT or 1s in SpikeStream", ParameterInfo::INTEGER));
	defaultParameterMap[parameterName] = 1000;
	parameterMap[parameterName] = defaultParameterMap[parameterName];

	parameterName = "slow_motion";
	parameterInfoList.append(ParameterInfo(parameterName, "speed factor, set in the time pannel 1ms * 0.002 => 0.333s = 3/10s delay between each", ParameterInfo::DOUBLE));
	defaultParameterMap[parameterName] = 0.002;
	parameterMap[parameterName] = defaultParameterMap[parameterName];

	parameterName = "fast_forward";
	parameterInfoList.append(ParameterInfo(parameterName, "speed factor for fast forward, set the time pannel", ParameterInfo::INTEGER));
	defaultParameterMap[parameterName] = 1.0;
	parameterMap[parameterName] = defaultParameterMap[parameterName];

	//parameterName = "monitor_off";
	//parameterInfoList.append(ParameterInfo(parameterName, "when fast forward, switch off the monitoring", ParameterInfo::BOOLEAN));
	//defaultParameterMap[parameterName] = true;
	//parameterMap[parameterName] = defaultParameterMap[parameterName];

	parameterName = "observation_time_ms";
	parameterInfoList.append(ParameterInfo(parameterName, "ms how long shall it run in slow motion ", ParameterInfo::INTEGER));
	defaultParameterMap[parameterName] = 25;
	parameterMap[parameterName] = defaultParameterMap[parameterName];

	parameterName = "start_time_ms";
	parameterInfoList.append(ParameterInfo(parameterName, "ms when to start the experiment, relativ to the time_grid, e.g. 1.1 s", ParameterInfo::INTEGER));
	defaultParameterMap[parameterName] = 100;
	parameterMap[parameterName] = defaultParameterMap[parameterName];

	parameterName = "end_time_ms";
	parameterInfoList.append(ParameterInfo(parameterName, "ms when to stop the experiment, e.g. 10ms it stops, when 10ms are passed, (the pannel shows 10), -1 indefinitively", ParameterInfo::INTEGER));
	defaultParameterMap[parameterName] = 1000 * 2.5; // -1;
	parameterMap[parameterName] = defaultParameterMap[parameterName];

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


}

      
/*----------------------------------------------------------*/
/*------                PRIVATE METHODS               ------*/
/*----------------------------------------------------------*/

/*! Checks that the network has the right number of neurons for the experiments. */
void VteExptWidget::checkNetwork(){
	Network* tmpNetwork = Globals::getNetwork();
	QList<NeuronGroup*> neurGrpList = tmpNetwork->getNeuronGroups();

	//Check that input layer, temporal coding and feature detection are there
	bool inputLayerFound = false;
	foreach(NeuronGroup* tmpNeurGrp, neurGrpList){
		//if (tmpNeurGrp->getInfo().getName().toUpper() == "INPUT LAYER") {
		if (tmpNeurGrp->getInfo().getName().toUpper() == "CA1 PLACE CELLS") {
				inputLayerFound = true;
			break;
		}
	}

	//Throw exception if we have not found the necessary neuron groups
	if(!inputLayerFound)
		throw SpikeStreamException("Input layer could not be found. Make sure there is a neuron gruop labelled 'Input layer' in your network.");
}



