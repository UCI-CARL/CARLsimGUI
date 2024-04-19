//SpikeStream includes
#include "VteExptManager.h"
#include "Util.h"
#include "Globals.h"

using namespace spikestream;

//Qt includes
#include <QDebug>


/*! Constructor */
VteExptManager::VteExptManager() : SpikeStreamThread(){
}


/*! Destructor */
VteExptManager::~VteExptManager(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

//Inherited from QThread
void VteExptManager::run(){
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
void VteExptManager::startExperiment(CarlsimWrapper* carlsimWrapper, QHash<QString, double>& parameterMap){
	this->carlsimWrapper = carlsimWrapper;
	storeParameters(parameterMap);
	start();  // QThread
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Runs the experiment. */
void VteExptManager::runExperiment() {
	//Seed the random number generator
	Util::seedRandom(randomSeed);

	//Train network on numPatterns patterns
	emit statusUpdate("Starting Experiment " + QString::number(experimentNumber + 1));

	//Reset the time panel
	if (carlsimWrapper->getTimeStep() == 0)
		carlsimWrapper->resetTime();

	//for(auto t = carlsimWrapper->getSnnTimeMs(); 
	//	!stopThread && (end_time == -1 || t < end_time); 
	//	t = carlsimWrapper->getSnnTimeMs())

	//Use internal clock
	for(auto t = carlsimWrapper->getTimeStep(); 
		!stopThread && (endTime_ms == -1 || t < endTime_ms); 
		t = carlsimWrapper->getTimeStep())
	{
		//Calculatate relative time inside a grid cell, e.g. 13530 = 530; 
		int rt = t % timeGrid_ms;
		int skip = startTime_ms - rt;
		if (skip < 0)
			skip = timeGrid_ms - rt + startTime_ms;

		//Set fast forward speed to reach the start
		//carlsimWrapper->setModelSpeed(fast_forward);
		pauseInterval_ms = 1.0 / fastForward; //fixme

		//DO NOT INTERFER WITH USER CHOICE TO MONITOR
		//Switch off monitors
		//bool monitoring = carlsimWrapper->getMonitor();  // FIXME implement getter
		//bool monitoring = true;
		//if (monitoring)
		//	carlsimWrapper->setMonitor(false);   

		//Run to the start of the experiment
		stepCarlsim(skip);

		//Prepare the experiment
		//if (monitoring)
		//	carlsimWrapper->setMonitor(true);
		//carlsimWrapper->setModelSpeed(slow_motion);
		pauseInterval_ms = 1.0 / slowMotion; //fixme

		//Branch to specific experiment
		//nueronId
		// create a current vector of 16 with initial current of zero
		vector<float> currents(16, 0.f);
		unsigned gNId = startNeuron;
		float current = initialCurrent;
		currents[gNId] = current;
		auto neuronGroups = Globals::getNetwork()->getNeuronGroupsMap();
		auto neuronGroup = neuronGroups["CA1 Place Cells"];
		unsigned gGrpId = neuronGroup->getVID();
		carlsimWrapper->carlsim->setExternalCurrent(gGrpId, currents);

		//Start the experiment
		int duration = observationTime_ms;
		emit statusUpdate(QString::asprintf("Excite neuron %d in group %d with %0.1f at %d ms", gGrpId, startNeuron, initialCurrent, carlsimWrapper->getTimeStep()));
		stepCarlsim(1);
		carlsimWrapper->carlsim->setExternalCurrent(gGrpId, 0);  // reset input current
		stepCarlsim(duration - 1);

		//Finalize experiment
		//Run to end of the grid
		//t = carlsimWrapper->getSnnTimeMs();
		t = carlsimWrapper->getTimeStep();		//Use internal clock
		rt = t % timeGrid_ms;

		//carlsimWrapper->setModelSpeed(fast_forward);
		pauseInterval_ms = 1.0 / fastForward; //fixme
		stepCarlsim(timeGrid_ms - rt - 1); // run to the end of the grid (with monitoring on)

		auto pauseInterval_ms = 10000; 
		//emit statusUpdate("wait before grid " + QString::number(pauseInterval_ms/1000.0));
		//Wait a little, before the Raster vanish  --> para wait grid switch
		if(!stopThread)
			msleep(pauseInterval_ms);
		stepCarlsim(1); // run to the end of the grid (with monitoring on)
	}

	//Output final result
	emit statusUpdate("Experiment complete.");
}


/*! Advances the simulation by the specified number of time steps */
void VteExptManager::stepCarlsim(unsigned numTimeSteps){
	for(unsigned i=0; i<numTimeSteps && !stopThread; ++i){
		carlsimWrapper->stepSimulation();
		msleep(pauseInterval_ms);
		while((carlsimWrapper->isWaitForGraphics() || carlsimWrapper->getCurrentTask() == CarlsimWrapper::STEP_SIMULATION_TASK) && !stopThread)
			msleep(pauseInterval_ms);
	}
	msleep(pauseInterval_ms);  // deactivated as it makes no sense
}


/*! Stores the parameters for the experiment */
void VteExptManager::storeParameters(QHash<QString, double>& parameterMap) {

	//Old imperial code but still checks out
	if (!parameterMap.contains("experiment_number"))
		throw SpikeStreamException("VteExptManager: experiment_number parameter missing");
	experimentNumber = (int)parameterMap["experiment_number"];

	if (!parameterMap.contains("random_seed"))
		throw SpikeStreamException("VteExptManager: random_seed parameter missing");
	randomSeed = (int)parameterMap["random_seed"];

	if (!parameterMap.contains("pause_interval_ms"))
		throw SpikeStreamException("VteExptManager: pause_interval_ms parameter missing");
	pauseInterval_ms = (int)parameterMap["pause_interval_ms"];


	// VTE
	auto validate = [&](const char* param) {
		if (!parameterMap.contains(param))
			throw SpikeStreamException(QString("VteExptManager: ") + param + " parameter missing");
		return param;
	};
	startNeuron = (int)parameterMap[validate("start_neuron")];
	initialCurrent = parameterMap[validate("initial_current")];
	recoveryTime_ms = (int)parameterMap[validate("recovery_time_ms")];
	timeGrid_ms = (int)parameterMap[validate("time_grid_ms")];
	slowMotion = (double)parameterMap[validate("slow_motion")];
	fastForward = (double)parameterMap[validate("fast_forward")];
	observationTime_ms = (int)parameterMap[validate("observation_time_ms")];
	startTime_ms = (int)parameterMap[validate("start_time_ms")];
	endTime_ms = (int)parameterMap[validate("end_time_ms")];

	
}




