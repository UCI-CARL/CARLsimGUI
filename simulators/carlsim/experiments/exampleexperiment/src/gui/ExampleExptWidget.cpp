//SpikeStream includes
#include "Globals.h"
#include "ExampleExptWidget.h"
using namespace spikestream;

//Qt includes
#include <QLabel>
#include <QLayout>


//Functions for dynamic library loading
extern "C" {
	/*! Creates an QWidget class when library is dynamically loaded. */
	_MSC_DLL_API QWidget* getClass(){
		return new ExampleExptWidget();
	}

	/*! Returns a descriptive name for this widget */
	_MSC_DLL_API QString getName(){
		return QString("Example Experiment");
	}
}


/*! Constructor */
ExampleExptWidget::ExampleExptWidget(QWidget* parent) : AbstractExperimentWidget(parent){
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
	exampleExptManager = new ExampleExptManager();
	connect(exampleExptManager, SIGNAL(finished()), this, SLOT(managerFinished()));
	connect(exampleExptManager, SIGNAL(statusUpdate(QString)), this, SLOT(updateStatus(QString)));
}


/*! Destructor */
ExampleExptWidget::~ExampleExptWidget(){
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Sets the wrapper that will be used in the experiments.
	The void pointer must point to an object of class NemoWrapper */
void ExampleExptWidget::setWrapper(void *wrapper){
	this->carlsimWrapper = (CarlsimWrapper*)wrapper;
}


/*----------------------------------------------------------*/
/*------                 PRIVATE SLOTS                ------*/
/*----------------------------------------------------------*/

/*! Called when the manager exits */
void ExampleExptWidget::managerFinished(){
	//Check for errors
	if(exampleExptManager->isError()){
		qCritical()<<exampleExptManager->getErrorMessage();
	}

	//Signal to other classes that experiment is complete
	playAction->setEnabled(true);
	stopAction->setEnabled(false);
	emit experimentEnded();
}


//Inherited from AbstractExperimentWidget
void ExampleExptWidget::startExperiment(){
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
		exampleExptManager->startExperiment(carlsimWrapper, parameterMap);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		managerFinished();
	}
}


//Inherited from AbstractExperimentWidget
void ExampleExptWidget::stopExperiment(){
	exampleExptManager->stop();
}


/*! Adds the specified message to the status pane */
void ExampleExptWidget::updateStatus(QString statusMsg){
	statusTextEdit->append(statusMsg);
}


/*----------------------------------------------------------*/
/*------              PROTECTED METHODS               ------*/
/*----------------------------------------------------------*/

/*! Builds the current and default parameter map along with the parameter
	info list. */
void ExampleExptWidget::buildParameters(){
	//Reset maps and list
	parameterInfoList.clear();
	defaultParameterMap.clear();
	parameterMap.clear();

	ParameterInfo tmpInfo("experiment_number", "The experiment that is carried out", ParameterInfo::OPTION);
	QList<QString> exptNameList;
	exptNameList.append("Experiment 1 - Sequence Learning");
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
	
}

      
/*----------------------------------------------------------*/
/*------                PRIVATE METHODS               ------*/
/*----------------------------------------------------------*/

/*! Checks that the network has the right number of neurons for the experiments. */
void ExampleExptWidget::checkNetwork(){
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



