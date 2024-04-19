//SpikeStream includes
#include "ExampleExptManager.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>


/*! Constructor */
ExampleExptManager::ExampleExptManager() : SpikeStreamThread(){
}


/*! Destructor */
ExampleExptManager::~ExampleExptManager(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

//Inherited from QThread
void ExampleExptManager::run(){
	clearError();
	stopThread = false;
	unsigned origWaitInterval = carlsimWrapper->getWaitInterval_ms();
	carlsimWrapper->setWaitInterval(1);//Minimal wait between steps

	try{
		runExperiment();
	}
	catch(SpikeStreamException& ex){
		setError(ex.getMessage());
	}
	catch(...){
		setError("Pop1ExperimentManager has thrown an unknown exception.");
	}

	carlsimWrapper->setWaitInterval(origWaitInterval);//Restore wrapper to original state
	experimentNumber = NO_EXPERIMENT;
	stopThread = true;
}


/*! Sets up the experiment and starts thread running. */
void ExampleExptManager::startExperiment(CarlsimWrapper* carlsimWrapper, QHash<QString, double>& parameterMap){
	this->carlsimWrapper = carlsimWrapper;
	storeParameters(parameterMap);
	start();
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Runs the experiment. */
void ExampleExptManager::runExperiment(){
	//Seed the random number generator
	Util::seedRandom(randomSeed);

	//Train network on numPatterns patterns
	emit statusUpdate("Starting Experiment " + QString::number(experimentNumber + 1));

	stepCarlsim(1000);

	//Output final result
	emit statusUpdate("Experiment complete.");
}


/*! Advances the simulation by the specified number of time steps */
void ExampleExptManager::stepCarlsim(unsigned numTimeSteps){
	for(unsigned i=0; i<numTimeSteps && !stopThread; ++i){
		carlsimWrapper->stepSimulation();
		while((carlsimWrapper->isWaitForGraphics() || carlsimWrapper->getCurrentTask() == CarlsimWrapper::STEP_SIMULATION_TASK) && !stopThread)
			msleep(pauseInterval_ms);
	}
	msleep(pauseInterval_ms);
}


/*! Stores the parameters for the experiment */
void ExampleExptManager::storeParameters(QHash<QString, double> &parameterMap){
	if(!parameterMap.contains("experiment_number"))
		throw SpikeStreamException("ExampleExptManager: experiment_number parameter missing");
	experimentNumber = (int)parameterMap["experiment_number"];

	if(!parameterMap.contains("random_seed"))
		throw SpikeStreamException("ExampleExptManager: random_seed parameter missing");
	randomSeed = (int)parameterMap["random_seed"];

	if(!parameterMap.contains("pause_interval_ms"))
		throw SpikeStreamException("ExampleExptManager: pause_interval_ms parameter missing");
	pauseInterval_ms = (int)parameterMap["pause_interval_ms"];
}




