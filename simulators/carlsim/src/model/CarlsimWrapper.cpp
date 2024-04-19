
#ifdef NC_EXTENSIONS_CARLSIM
	#define NC_EXTENSIONS
#endif

//SpikeStream includes
#include "Globals.h"
#include "CarlsimLoader.h"
#include <CarlsimWrapper.h>
#include "CarlsimSpikeGeneratorContainer.h"
#include "PerformanceTimer.h"
#include "SpikeStreamException.h"
#include "SpikeStreamSimulationException.h"
#include "Util.h"

#include "spike_monitor.h"
#include "callback.h"

#include "carlsim_datastructures.h"
   

//Other includes
#include "boost/random.hpp"
 
// STL
//#include <tuple>


//Outputs debugging information for CARLsim calls
#define DEBUG_INJECT_CURRENT
#define DEBUG_INJECT_NOISE
//#define DEBUG_LEARNING
#define DEBUG_LOAD
#define DEBUG_CARLSIM
//#define DEBUG_PARAMETERS
//#define DEBUG_PERFORMANCE
//#define DEBUG_STEP
//#define DEBUG_WEIGHTS
//#define TIME_PERFORMANCE

//investigate basic behavior of firings
//#define DEBUG_FIRING_WITH_SPIKE_MONITORS
#define PATCH_CURRENT_NEURONS


#ifdef DEBUG_WEIGHTS_LEARNING__CONN_MON
#include "connection_monitor.h"
#endif


#ifdef TIME_PERFORMANCE
	unsigned numberOfFiringNeurons = 0;
#endif//TIME_PERFORMANCE

/*! Constructor */
CarlsimWrapper::CarlsimWrapper() : AbstractSimulation() {
	//Initialise variables
	currentTaskID = NO_TASK_DEFINED;
	simulationLoaded = false;
	stopThread = true;
	archiveMode = false;
	monitorFiringNeurons = false;
	updateFiringNeurons = false;
	monitorMembranePotential = false;
	monitor = false;   
	monitorWeights = false;
#ifdef CARLSIM_AXONPLAST
	monitorDelays = false;
#endif
	updateInterval_ms = 500;
	patternNeuronGroupID = 0;
	sustainPattern = false;
	waitInterval_ms = 200;
}


CarlsimWrapper::CarlsimWrapper(ConfigLoader* configLoader) : AbstractSimulation() {
	//Initialize variables
	currentTaskID = NO_TASK_DEFINED;
	simulationLoaded = false;
	stopThread = true;
	archiveMode = false;
	monitorFiringNeurons = false;
	updateFiringNeurons = false;
	monitorMembranePotential = false;
	monitor = false;
	monitorWeights = false;
#ifdef CARLSIM_AXONPLAST
	monitorDelays = false; 
#endif
	updateInterval_ms = 500;
	patternNeuronGroupID = 0;
	sustainPattern = false;
	//sustainCurrent = false;  //Plugin Port
	waitInterval_ms = 200;

	//Zero is the default STDP function
	stdpFunctionID = Util::getUInt(configLoader->getParameter("carlsim_stdp", "0")); 

	//Get a configuration object initialized with the default values
	carlsimConfig = carlsim41::carlsim_new_configuration();

	/*! default backend for CARLsim */
	QString carlsim_backend = configLoader->getParameter("carlsim_backend", "gpu");  
	if(carlsim_backend=="gpu") {
		carlsimConfig->preferredSimMode = carlsim41::GPU_MODE;
		carlsimConfig->ithGPUs = Util::getInt(configLoader->getParameter("carlsim_device", "-1")); 
	} else
	if(carlsim_backend=="cpu") {
		carlsimConfig->preferredSimMode = carlsim41::CPU_MODE;
	} else
		throw SpikeStreamException("Invalid parameter for CARLsim");


#ifdef NC_EXTENSIONS_CARLSIM 
	/*! set in ::loadSimulation */
	daStdp = false;
	da = .0f;
	daReward = .0f;
	daDecay = .0f;
#endif


#ifdef NC_EXTENSIONS_CARLSIM
	/*! ModelTime */
	QString param = configLoader->getParameter("carlsim_model_simtime", "");
	if(param=="")
		modelTime0 = QDateTime::currentDateTime();
	else
		modelTime0 = QDateTime::fromString(param, "yyyy-MM-ddThh:mm:ss.zzz");
	modelTimeMs = modelTime0.toMSecsSinceEpoch(); 
	modelSpeed = Util::getFloat(configLoader->getParameter("carlsim_model_speed", "1.0")); // realtime vs. simulation 
	modelStepMs = Util::getUInt(configLoader->getParameter("carlsim_model_step", "1"));	// ms of model time per each step (alpha)
	setProperty("modelStart", configLoader->getParameter("carlsim_model_start", "REALTIME")); 
	setProperty("modelAutosync", configLoader->getParameter("carlsim_model_autosync", "OFF")); 
	modelLagMs = Util::getInt(configLoader->getParameter("carlsim_model_lag", "0")); // ms behind real time
	modelTime0.addMSecs( - modelLagMs); 
#endif

}

/*! Destructor */
CarlsimWrapper::~CarlsimWrapper(){
	if(simulationLoaded)
		unloadSimulation();
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds a manager to interface with a device */
void CarlsimWrapper::addDeviceManager(AbstractDeviceManager* deviceManager){
	deviceManagerList.append(deviceManager);
}


/*! Cancels the loading of a simulation */
void CarlsimWrapper::cancelLoading(){
	stopThread = true;
}


/*! Cancels the resetting of weights */
void CarlsimWrapper::cancelResetWeights(){
	weightResetCancelled = true;
}


/*! Cancels the saving of weights */
void CarlsimWrapper::cancelSaveWeights(){
	weightSaveCancelled = true;
}


/*! Returns true if simulation is currently being played */
bool CarlsimWrapper::isSimulationRunning(){
	if(currentTaskID == RUN_SIMULATION_TASK || currentTaskID == STEP_SIMULATION_TASK)
		return true;
	return false;
}


/*! Loads the simulation into the CUDA hardware.
	This method should only be invoked in the thread within which Carlsim is played.
	It has been made public for testing purposes. */
void CarlsimWrapper::loadCarlsim(){
	simulationLoaded = false;
	timeStepCounter = 0;
	waitForGraphics = false;
	archiveMode = false;

	// Set the plugin path
	QString pluginPath = Globals::getSpikeStreamRoot()+"/bin/carlsim-plugins";
	qDebug()<<"CARLsim plugin path: "<<pluginPath;

qDebug() << "carlsim_add_plugin_path skipped" << __FUNCTION__ << __LINE__;

	//Get the network
	if(!Globals::networkLoaded())
		throw SpikeStreamSimulationException("Cannot load simulation: no network loaded.");
	Network* currentNetwork = Globals::getNetwork();
	carlsimConfig->netName = currentNetwork->getName().toStdString();  

	// instanciate CARLsim 
	carlsim = new CarlsimLib(carlsimConfig->netName, 
		(SimMode) carlsimConfig->preferredSimMode, (LoggerMode) carlsimConfig->loggerMode, 
		carlsimConfig->ithGPUs, carlsimConfig->randSeed);
	emit carlsimConfigState();

	//Set up the archive info
	archiveInfo.reset();
	archiveInfo.setNetworkID(currentNetwork->getID());

	//Build the CARLsim network
	CarlsimLoader* carlsimLoader = new CarlsimLoader(this);
	connect(carlsimLoader, SIGNAL(progress(int, int)), this, SLOT(updateProgress(int, int)));
	#ifdef DEBUG_LOAD
		qDebug()<<"About to build carlsim network.";
	#endif//DEBUG_LOAD
	if(!carlsimLoader->buildCarlsimNetwork(currentNetwork, volatileConGrpMap, &stopThread))
		throw SpikeStreamSimulationException("Cannot load simulation: build network failed.");

	emit carlsimConfigStateNetworkBuilt(); // CAUTION: connect sync, 

	// After all config is done, load the simulation, meaning setupNetwork for CARLsim
	carlsim->setupNetwork();
	CARLsimState state = carlsim->getCARLsimState();
	if(state!=::SETUP_STATE)
		throw SpikeStreamException("CARLsim could not setup the network.");; 
	#ifdef DEBUG_LOAD
		qDebug()<<"Carlsim network successfully built.";
	#endif//DEBUG_LOAD

	emit carlsimSetupState(); // inform about the state change 

#ifdef DEBUG_WEIGHTS_LEARNING__CONN_MON
	// experimental
	__exp__monitor = carlsim->setConnectionMonitor(0, 1, "DEFAULT");
	__exp__monitor->takeSnapshot();
#endif


	//Clean up loader
	delete carlsimLoader;

	//Set the STDP functionn in the configuration
	#ifdef DEBUG_LEARNING
		STDPFunctions::print(stdpFunctionID);
	#endif

#ifdef NC_EXTENSIONS_DA		
	/*! */
	if(stdpFunctionID == STDPFunctions::DA_STDP) {
		da = .0f;
		daReward = STDPFunctions::getDaReward(stdpFunctionID);
		daDecay = STDPFunctions::getDaDecay(stdpFunctionID);
	}
#endif

	//Load the network into the simulator
	#ifdef DEBUG_LOAD
		qDebug()<<"About to load carlsim network into simulator.";
	#endif//DEBUG_LOAD
	#ifdef DEBUG_LOAD
		qDebug()<<"CARLsim network successfully loaded into simulator.";
	#endif//DEBUG_LOAD

	if(!stopThread)
		simulationLoaded = true;
}


/*! Plays the simulation */
void CarlsimWrapper::playSimulation(){
	if(!simulationLoaded)
		throw SpikeStreamException("Cannot run simulation - no simulation loaded.");

	//Do nothing if we are already in play mode
	if(currentTaskID == RUN_SIMULATION_TASK)
		return;

	runMutex.lock();
	currentTaskID = RUN_SIMULATION_TASK;
	runMutex.unlock();

}


/*! Resets the temporary weights to the stored values. */
void CarlsimWrapper::resetWeights(){
	if(!simulationLoaded)
		throw SpikeStreamException("Cannot reset weights - no simulation loaded.");
	weightsReset = false;
	weightResetCancelled = false;
	currentTaskID = RESET_WEIGHTS_TASK;
}


/*! Saves the volatile weights to the database */
void CarlsimWrapper::saveWeights(){
	if(!simulationLoaded)
		throw SpikeStreamException("Cannot save weights - no simulation loaded.");
	weightsSaved = false;
	weightSaveCancelled = false;
	currentTaskID = SAVE_WEIGHTS_TASK;
}

#ifdef CARLSIM_AXONPLAST

/*! Cancels the resetting of delays */
void CarlsimWrapper::cancelResetDelays() {
	delayResetCancelled = true;
}


/*! Cancels the saving of delays */
void CarlsimWrapper::cancelSaveDelays() {
	delaySaveCancelled = true;
}

/*! Resets the temporary delays to the stored values. */
void CarlsimWrapper::resetDelays() {
	if (!simulationLoaded)
		throw SpikeStreamException("Cannot reset delays - no simulation loaded.");
	delaysReset = false;
	delayResetCancelled = false;
	currentTaskID = RESET_DELAYS_TASK;
}


/*! Saves the volatile weights to the database */
void CarlsimWrapper::saveDelays() {
	if (!simulationLoaded)
		throw SpikeStreamException("Cannot save delays - no simulation loaded.");
	delaysSaved = false;
	delaySaveCancelled = false;
	currentTaskID = SAVE_DELAYS_TASK;
}

#endif


#ifdef NC_EXTENSIONS_CARLSIM


void CarlsimWrapper::querySynapsesPrepare(QVector<unsigned> ids){

	if(!simulationLoaded)
		throw SpikeStreamException("Cannot query weights - no simulation loaded.");

	// prepare transaction
	// thread save access to data members
	// this this only locks the resouce access 
	QMutexLocker locker(&mutex); 

	// set control variables
	weightsSaved = false;
	weightSaveCancelled = false;
	
	// transfer payload 
	if(m_synapsesIds.size() != ids.size()) {
		m_synapsesIds.resize(ids.size());
		m_synapsesProperties.resize(ids.size());
	}

	QVector<unsigned>::iterator readIter = ids.begin();
	CarlsimWrapper::synapsesIds_t::iterator writeIter = m_synapsesIds.begin();
	bool found;
	for(; readIter!=ids.end(); readIter++, writeIter++) {
		found = false;
		for(QHash<unsigned, synapse_id*>::iterator conGrpIter = volatileConGrpMap.begin(); 
			!found && conGrpIter != volatileConGrpMap.end(); ++conGrpIter) {
			//Get the volatile connection group and matching array of CARLsim synapse IDs
			ConnectionGroup* tmpConGrp = Globals::getNetwork()->getConnectionGroup(conGrpIter.key());
			synapse_id* synapseIDArray = conGrpIter.value();
			unsigned conCntr = 0;
			ConnectionIterator conGrpEnd = tmpConGrp->end();
			for(ConnectionIterator conIter = tmpConGrp->begin(); conIter != conGrpEnd; ++conIter, ++conCntr){
				if( conIter->getID() == *readIter) {
					found = true;

					*writeIter = synapseIDArray[conCntr];
					unsigned neuronNid = *writeIter>>32;
					unsigned synapseIdx = *writeIter & 0xFFFFFFFF;

					break; // works only for one level => found
				}
			}
		}
	}


}

/*! Saves the synapse properties weights to the database */
void CarlsimWrapper::querySynapses(){
	if(!simulationLoaded)
		throw SpikeStreamException("Cannot query weights - no simulation loaded.");

	// prepare transaction
	// this holds the execution while another task is still running
	// this shall make polling obsolete  this means the method can be called directly 
	// after a step and does start immediately. 
	// \ note locking only mutex is not sufficient this this only locks the resouce access 
	QMutexLocker locker(&runMutex); 

	// set control variables
	weightsSaved = false;
	weightSaveCancelled = false;

	// start transation
	currentTaskID = QUERY_SYNAPSES_TASK;

}
#endif


/*! Forces neurons with the specified IDs to fire in the next time step. */
void CarlsimWrapper::setFiringNeuronIDs(QList<neurid_t>& neurIDList){
	//Run checks
	if(!simulationLoaded)
		throw SpikeStreamException("Neurons cannot be fired when a simulation is not loaded.");

	//Pointer to current network
	Network* nwPtr = Globals::getNetwork();

	QList<neurid_t>::iterator neurIDListEnd = neurIDList.end();
	for(QList<neurid_t>::iterator iter = neurIDList.begin(); iter != neurIDListEnd; ++iter){
		if(!nwPtr->containsNeuron(*iter))
			throw SpikeStreamException("Neuron ID " + QString::number(*iter) + " cannot be found in the network.");
		neuronIDsToFire.append(*iter);
	}
}


/*! Forces the specified percentage of neurons in the specified neuron group to fire at the
	next time step. Throws an exception if the neuron group ID does not exist in the current
	network or if the percentage is < 0 or > 100. */
void  CarlsimWrapper::setInjectNoise(unsigned neuronGroupID, double percentage, bool sustain){
	sustainNoise = sustain;

	//Run checks
	if(!simulationLoaded)
		throw SpikeStreamException("Noise cannot be injected when a simulation is not loaded.");
	if(percentage < 0.0 || percentage > 100.0)
		throw SpikeStreamException("Injecting noise. Percentage is out of range");
	if(!Globals::getNetwork()->containsNeuronGroup(neuronGroupID))
		throw SpikeStreamException("Injecting noise. Neuron group ID cannot be found in current network: " + QString::number(neuronGroupID));

	//Calculate number of neurons to fire and store it in the map
	injectNoiseMap[neuronGroupID] = Util::rUInt( (percentage / 100.0) * (double)Globals::getNetwork()->getNeuronGroup(neuronGroupID)->size());
	if(injectNoiseMap[neuronGroupID] > (unsigned)Globals::getNetwork()->getNeuronGroup(neuronGroupID)->size())
		throw SpikeStreamException("Number of neurons to fire is greater than neuron group size: " + QString::number(injectNoiseMap[neuronGroupID]));
}


/*! Steps through a single time step */
void CarlsimWrapper::stepSimulation(){
	if(!simulationLoaded)
		throw SpikeStreamException("Cannot step simulation - no simulation loaded.");

	//Do nothing if we are already in step mode
	if(currentTaskID == STEP_SIMULATION_TASK)
		return;

	//Lock mutex to prevent interference with current task
	runMutex.lock();

	//Set to step mode for when run loop is next ready
	currentTaskID = STEP_SIMULATION_TASK;

	//Release mutex
	runMutex.unlock();
}


// Run method inherited from QThread
void CarlsimWrapper::run(){
	stopThread = false;
	clearError();

	try{
		//Create thread specific network and archive daos
		networkDao = new NetworkDao(Globals::getNetworkDao()->getDBInfo());
		archiveDao = new ArchiveDao(Globals::getArchiveDao()->getDBInfo());

		//Load up the simulation and reset the task ID
		loadCarlsim();
		currentTaskID = NO_TASK_DEFINED;

		//Wait for run or step command
		while(!stopThread){
			runMutex.lock();

			//Run simulation
			if(currentTaskID == RUN_SIMULATION_TASK){
				runCarlsim();
			}
			//Step simulation
			else if(currentTaskID == STEP_SIMULATION_TASK){
				stepCarlsim();
			}
			//Reset weights
			else if(currentTaskID == RESET_WEIGHTS_TASK){
				resetCarlsimWeights();
			}
			//Save weights
			else if(currentTaskID == SAVE_WEIGHTS_TASK){
				saveCarlsimWeights();
			}
#ifdef CARLSIM_AXONPLAST
			//Reset delays
			else if (currentTaskID == RESET_DELAYS_TASK) {
				resetCarlsimDelays();
			}
			//Save delays
			else if (currentTaskID == SAVE_DELAYS_TASK) {
				saveCarlsimDelays();
			}
#endif

#ifdef NC_EXTENSIONS_CARLSIM
			//Save synapses properties 
			else if(currentTaskID == QUERY_SYNAPSES_TASK){
				saveSynapsesProperties();
			}
#endif 
			//Set neuron parameters
			else if(currentTaskID == SET_NEURON_PARAMETERS_TASK){
			}
			//Do nothing
			else if(currentTaskID == NO_TASK_DEFINED){
				;//Do nothing
			}
			//Task ID not recognized
			else{
				throw SpikeStreamException("Task ID not recognizedL: " + QString::number(currentTaskID));
			}

			//Reset task ID
			currentTaskID = NO_TASK_DEFINED;//Don't want it to continue stepping.

			runMutex.unlock();

			//Short sleep waiting for the next command
			msleep(waitInterval_ms);
		}

		//Clean up the thread specific network and archive daos
		delete networkDao;
		delete archiveDao;
	}
	catch(SpikeStreamException& ex){
		setError(ex.getMessage());
	}
	catch(...){
		setError("An unknown error occurred while CARLsimWrapper thread was running.");
	}

	unloadCarlsim();

	stopThread = true;
}


/*! Sets the number of frames per second.
	This is converted into the update interval.
	If the frame rate is 0 the simulation runs at maximum speed. */
void CarlsimWrapper::setFrameRate(unsigned int frameRate){
	QMutexLocker locker(&mutex);
	if(frameRate == 0)
		this->updateInterval_ms = 0;
	else
		this->updateInterval_ms = 1000 / frameRate;
}

#ifdef NC_EXTENSIONS_CARLSIM

/*!  Sets the the speed factor how fast the model time runs compared to the realtime. */
void CarlsimWrapper::setModelSpeed(double speed) { 
	if(abs(speed - modelSpeed) < 0.001) 
		return; // prevent recursion
	QMutexLocker locker(&mutex);
	modelSpeed = speed;
	emit modelSpeedChanged(modelSpeed);
}
	

/*!	Defines how many ms on step of the SSN correspond in model time.*/
void CarlsimWrapper::setModelStepMs(unsigned step) { 
	if(step == modelStepMs) 
		return; // prevent recursion, Note: Rounding neccessary due Double
	QMutexLocker locker(&mutex);
	modelStepMs = step; 
	emit modelStepMsChanged(modelStepMs);
}



void CarlsimWrapper::setDa(float level) {
	QMutexLocker locker(&mutex);
	da = level; 
}

void CarlsimWrapper::rewardDa() {
	QMutexLocker locker(&mutex);
	da += daReward; 
}

void CarlsimWrapper::rewardDa(float increment) {
	QMutexLocker locker(&mutex);
	da += increment; 
}

#endif


/*! Sets the archive mode.
	An archive is created the first time this method is called after the simulation has loaded. */
void CarlsimWrapper::setArchiveMode(bool newArchiveMode, const QString& description){
	if(newArchiveMode && !simulationLoaded)
		throw SpikeStreamSimulationException("Cannot switch archive mode on unless simulation is loaded.");

	/* Create archive if this is the first time the mode has been set
		Use globals archive dao because this method is called from a separate thread */
	if(archiveInfo.getID() == 0){
		archiveInfo.setDescription(description);
		Globals::getArchiveDao()->addArchive(archiveInfo);
		Globals::getEventRouter()->archiveListChangedSlot();
	}
	//Rename archive if one is already loaded
	else{
		archiveInfo.setDescription(description);
		Globals::getArchiveDao()->setArchiveProperties(archiveInfo.getID(), description);
		Globals::getEventRouter()->archiveListChangedSlot();
	}

	this->archiveMode = newArchiveMode;
}


/*! Sets a firing pattern along with the neuron group.
	The pattern can be injected for one time step or continuously. */
void CarlsimWrapper::setFiringInjectionPattern(const Pattern& pattern, unsigned neuronGroupID, bool sustain){
	//Lock mutex to prevent multiple threads accessing injection vector
	mutex.lock();

	sustainPattern = sustain;

	//Get copy of the pattern that is aligned on the centre of the neuron group
	NeuronGroup* neurGrp = Globals::getNetwork()->getNeuronGroup(neuronGroupID);
	Pattern alignedPattern( pattern.getAlignedPattern(neurGrp->getBoundingBox()) );

	//Add neurons that are contained within the pattern
	NeuronMap::iterator neuronMapEnd = neurGrp->end();
	qDebug() << "CarlsimWrapper::setFiringInjectionPattern injectionPatternVector not implemented" << endl;
	for(NeuronMap::iterator iter = neurGrp->begin(); iter != neuronMapEnd; ++iter){
		auto &point = (*iter)->getLocation();
		qDebug() << point.toString() << endl; 
		if(alignedPattern.contains(point) ){
			//injectionPatternVector.push_back( (*iter)->getID());
		}
	}

	//Release mutex
	mutex.unlock();

}


/*! Sets a current injection pattern along with the neuron group.
	The pattern can be injected for one time step or continuously. */
void CarlsimWrapper::setCurrentInjectionPattern(const Pattern& pattern, float current, unsigned neuronGroupID, bool sustain){

	//Lock mutex to prevent multiple threads accessing current vectors
	mutex.lock();

	sustainPattern = sustain;

	//Get copy of the pattern that is aligned on the centre of the neuron group
	NeuronGroup* neurGrp = Globals::getNetwork()->getNeuronGroup(neuronGroupID);
	Pattern alignedPattern( pattern.getAlignedPattern(neurGrp->getBoundingBox()) );

	auto neurGrpSize = neurGrp->size();  
	std::vector<float> injectionCurrentNeurIDVector(neurGrp->size(), 0.0f);

	if (sustain) {
		//Add neurons that are contained within the pattern
		NeuronMap::iterator neuronMapEnd = neurGrp->end();
		for (NeuronMap::iterator iter = neurGrp->begin(); iter != neuronMapEnd; ++iter) {
			if (alignedPattern.contains((*iter)->getLocation())) {
				auto neuronID = (*iter)->getID();
				auto gNId = neuronID - neurGrp->getStartNeuronID();
				injectionCurrentNeurIDVector[gNId] = current;
				qDebug() << "CarlsimWrapper::setCurrentInjectionPattern " << neuronID << " injectionCurrentNeurIDVector[" << gNId << "] = " << current << endl;
			}
		}
	}

	carlsim->setExternalCurrent( // delegate to CARLsim
		neurGrp->getVID(),				 // CARLsim's ID is stored in the group 
		injectionCurrentNeurIDVector);	 // convert to std vector

	mutex.unlock();
	
}




/*! Sets the monitor mode, which controls whether firing neuron data is extracted
	from the simulation at each time step */
void CarlsimWrapper::setMonitorNeurons(bool firing, bool membranePotential){
	if( (firing || membranePotential) && !simulationLoaded)
		throw SpikeStreamSimulationException("Cannot switch neuron monitor mode on unless simulation is loaded.");
	if(firing && membranePotential)
		throw SpikeStreamSimulationException("Cannot monitor firing neurons and membrane potential at the same time.");

	//Store new monitoring mode
	this->monitorFiringNeurons = firing;
	this->monitorMembranePotential = membranePotential;
}


/*! Controls whether the time step is updated at each time step. */
void CarlsimWrapper::setMonitor(bool mode){
	this->monitor = mode;
}


/*! Sets wrapper to retrieve the weights from CARLsim and save them to the current weight
	field in the database. Does this operation every time step until it is turned off. */
void  CarlsimWrapper::setMonitorWeights(bool enable){
	monitorWeights = enable;
}

#ifdef CARLSIM_AXONPLAST
/*! Sets wrapper to retrieve the weights from CARLsim and save them to the current weight
	field in the database. Does this operation every time step until it is turned off. */
void  CarlsimWrapper::setMonitorDelays(bool enable) {
	monitorDelays = enable;
}
#endif

/*! Sets the parameters of the neurons within CARLsim */
void CarlsimWrapper::setNeuronParameters(unsigned neuronGroupID, QHash<QString, double> parameterMap){
	if(!simulationLoaded)
		throw SpikeStreamException("Cannot set neuron parameters - no simulation loaded.");
	this->neuronGroupID = neuronGroupID;
	neuronParameterMap = parameterMap;
	currentTaskID = SET_NEURON_PARAMETERS_TASK;
}


/*! Sets the parameters of the synapses within CARLsim */
void CarlsimWrapper::setSynapseParameters(unsigned, QHash<QString, double>){
	if(!simulationLoaded)
		throw SpikeStreamException("Cannot set synapse parameters - no simulation loaded.");
	throw SpikeStreamException("This method should not be called because it is not implemented.");
}


/*! Stops the simulation playing without exiting the thread. */
void CarlsimWrapper::stopSimulation(){
	currentTaskID = NO_TASK_DEFINED;
}


/*! Unloads the current simulation and exits run method. */
void CarlsimWrapper::unloadSimulation(){
	stopThread = true;
}

/*! Loads the simulation of the current network */
void CarlsimWrapper::loadSimulation() {
	emit loadSimulationRequired();
}



/*----------------------------------------------------------*/
/*-----                  PRIVATE SLOTS                 -----*/
/*----------------------------------------------------------*/

/*! Called by other classes being executed by this class to inform this
	class about progress with an operation */
void CarlsimWrapper::updateProgress(int stepsComplete, int totalSteps){
	emit progress(stepsComplete, totalSteps);
}

 /*! Delegate information, that the monitors have been loaded. */
void CarlsimWrapper::handleMonitorsLoaded() {
	emit monitorsLoaded();
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Adds neurons due to noise at the end of the injectNeuronID vector.
	These neurons will be forced to fire at the next time step.
	Returns the number of neurons added. */
unsigned CarlsimWrapper::addInjectFiringNeuronIDs(){
	//Calculate total number of firing neurons
	unsigned arraySize = 0, arrayCounter = 0;
	for(QHash<unsigned, unsigned>::iterator iter = injectNoiseMap.begin(); iter != injectNoiseMap.end(); ++iter)
		arraySize += iter.value();

	//Add a random selection of neuron ids from each group
	unsigned randomIndex, neurGrpSize, numSelectedNeurons;
	QHash<unsigned, bool> addedNeurIndxMap;//Prevent duplicates
	for(QHash<unsigned, unsigned>::iterator iter = injectNoiseMap.begin(); iter != injectNoiseMap.end(); ++iter){
		//Get list of neuron ids
		QList<unsigned> neuronIDList = Globals::getNetwork()->getNeuronGroup(iter.key())->getNeuronIDs();
		neurGrpSize = neuronIDList.size();
		addedNeurIndxMap.clear();
		numSelectedNeurons = iter.value();
		#ifdef DEBUG_INJECT_NOISE
			qDebug()<<"Selecting random neurons for injecting noise. Num neurons: "<<neurGrpSize<<"; num neurons to select: "<<numSelectedNeurons;
		#endif//DEBUG_INJECT_NOISE

		//Select indexes from the list of neuron ids
		while((unsigned)addedNeurIndxMap.size() < numSelectedNeurons){
			randomIndex = Util::getRandom(0, neurGrpSize);//Get random position in list of neuron ids
			if(!addedNeurIndxMap.contains(randomIndex)){//New index
				if(arrayCounter >= arraySize)//Sanity check
					throw SpikeStreamException("Error adding noise injection neuron ids - array counter out of range.");
				injectionPatternVector.push_back(neuronIDList.at(randomIndex));//Add neuron id to pattern vector
				#ifdef DEBUG_INJECT_NOISE
					qDebug()<<"Random index: "<<randomIndex<<"; Inject noise neuron ID: "<<neuronIDList.at(randomIndex);
				#endif//DEBUG_INJECT_NOISE
				addedNeurIndxMap[randomIndex] = true;//Record the fact that we have selected this ID
				++arrayCounter;
			}
		}
	}

	//Sanity check, then return number of neurons added
	if(arrayCounter != arraySize)
		throw SpikeStreamException("Error adding inject noise neuron IDs. Array counter: " + QString::number(arrayCounter) + "; Array size: " + QString::number(arraySize));

	//Add neurons that are specified to fire during this time step
	QList<neurid_t>::iterator neurIDListFireEnd = neuronIDsToFire.end();
	for(QList<neurid_t>::iterator iter = neuronIDsToFire.begin(); iter != neurIDListFireEnd; ++iter){
		injectionPatternVector.push_back(*iter);
		++arraySize;
	}
	neuronIDsToFire.clear();

	//Add firing neuron IDs from plugins
	for(int i=0; i<deviceManagerList.size(); ++i){
		if(deviceManagerList[i]->isFireNeuronMode()){//Only add firing neuron IDs if we are in firing neuron mode
			QList<neurid_t>::iterator outputNeuronsEnd = deviceManagerList[i]->outputNeuronsEnd();
			for(QList<neurid_t>::iterator iter =  deviceManagerList[i]->outputNeuronsBegin(); iter != outputNeuronsEnd; ++iter){
				injectionPatternVector.push_back(*iter);
				++arraySize;
			}
		}
	}


#ifdef PATCH_CURRENT_NEURONS  
	//Add current for group from plugins
	for (int i = 0; i < deviceManagerList.size(); ++i) {
		//if (!deviceManagerList[i]->isFireNeuronMode()) {   // XML 
		{
			QList<neurid_t>::iterator currentNeuronsEnd = deviceManagerList[i]->currentNeuronsEnd();
			QList<double>::iterator currentValuesEnd = deviceManagerList[i]->currentValuesEnd();

			QList<neurid_t>::iterator iter = deviceManagerList[i]->currentNeuronsBegin();
			QList<double>::iterator iter2 = deviceManagerList[i]->currentValuesBegin();

			if (iter == currentNeuronsEnd)
				continue;  // empty, skip this

			//std::map<NeuronGroup*, std::vector<float>> groups;
			QHash<NeuronGroup*, std::vector<float>> groups;

			for (; iter != currentNeuronsEnd && iter2 != currentValuesEnd; ++iter, ++iter2) {
				auto neuronId = *iter;
				NeuronGroup* neurGrp = Globals::getNetwork()->getNeuronGroupFromNeuronID(neuronId);
				if (!groups.contains(neurGrp)) {
					auto neurGrpSize = neurGrp->size();
					groups[neurGrp] = std::vector<float> (neurGrp->size(), 0.0f);
					//std::vector<float> injectionCurrentNeurIDVector(neurGrp->size(), 0.0f);
				}
				std::vector<float>& injectionCurrentNeurIDVector = groups[neurGrp];

				auto gNId = *iter - neurGrp->getStartNeuronID();
				injectionCurrentNeurIDVector[gNId] = *iter2;				
			}

			for (auto grpIter = groups.constKeyValueBegin();
				grpIter != groups.constKeyValueEnd(); grpIter++) 
			{
				carlsim->setExternalCurrent(		// delegate to CARLsim
					grpIter->first->getVID(),		// CARLsim's ID is stored in the group 
					grpIter->second);				// convert to std vector
			}		

		}
	}
#else 
	//qDebug() << "PATCH_CURRENT_NEURONS deactivated";
#endif


	//Return the number of neurons that have been added
	return arraySize;
}

/*! Clears the error state */
void CarlsimWrapper::clearError(){
	error = false;
	errorMessage = "";
}


/*! Fills the supplied array with a selection of neurons to force to fire at the next time step. */
void CarlsimWrapper::fillInjectNoiseArray(unsigned*& array, int* arraySize){
	//Calculate total number of firing neurons
	*arraySize = 0;
	for(QHash<unsigned, unsigned>::iterator iter = injectNoiseMap.begin(); iter != injectNoiseMap.end(); ++iter)
		*arraySize = *arraySize + iter.value();

	//Create array
	int arrayCounter = 0;
	array = new unsigned[*arraySize];

	//Fill array with a random selection of neuron ids from each group
	unsigned randomIndex, neurGrpSize, numSelectedNeurons;
	QHash<unsigned, bool> addedNeurIndxMap;//Prevent duplicates
	for(QHash<unsigned, unsigned>::iterator iter = injectNoiseMap.begin(); iter != injectNoiseMap.end(); ++iter){
		//Get list of neuron ids
		QList<unsigned> neuronIDList = Globals::getNetwork()->getNeuronGroup(iter.key())->getNeuronIDs();
		neurGrpSize = neuronIDList.size();
		addedNeurIndxMap.clear();
		numSelectedNeurons = iter.value();

		//Select indexes from the list of neuron ids
		while((unsigned)addedNeurIndxMap.size() < numSelectedNeurons){
			randomIndex = Util::getRandom(0, neurGrpSize);//Get random position in list of neuron ids
			if(!addedNeurIndxMap.contains(randomIndex)){//New index
				if(arrayCounter >= *arraySize)//Sanity check
					throw SpikeStreamException("Error adding noise injection neuron ids - array counter out of range.");
				array[arrayCounter] = neuronIDList.at(randomIndex);//Add neuron id to array
				addedNeurIndxMap[randomIndex] = true;//Record the fact that we have selected this ID
				++arrayCounter;
			}
		}
	}
}


/*! Extracts the membrane potential for all the neurons from the simulation. */
void CarlsimWrapper::getMembranePotential(){
	membranePotentialMap.clear();

	float tmpMemPot, maxMemPot = 0.0f, minMemPot = 0.0f;
	QList<NeuronGroup*> neurGrpList = Globals::getNetwork()->getNeuronGroups();
	for(int i=0; i<neurGrpList.size(); ++i){
		NeuronMap::iterator neurGrpListEnd = neurGrpList.at(i)->end();
		for(NeuronMap::iterator iter = neurGrpList.at(i)->begin(); iter != neurGrpListEnd; ++iter){
			membranePotentialMap[iter.key()] = tmpMemPot;
			if(tmpMemPot > maxMemPot)
				maxMemPot = tmpMemPot;
			if(tmpMemPot < minMemPot)
				minMemPot = tmpMemPot;
		}
	}
}

// Carlsim Channels
/*! Wait for the channels to have messages
	returns false if wait was in vain and the timeout was triggered */
bool CarlsimWrapper::waitForChannels(int timeout_ms, int channel_wait_ms) {

	bool waitForChannels = true;

	//qDebug() << "Timeout waiting for Channels, timeout: %1" << timeout_ms/1000;
	while(!stopThread && waitForChannels && timeout_ms > 0 ) {  
		for(int i=0; waitForChannels && i<deviceManagerList.size(); ++i)
			waitForChannels = waitForChannels && deviceManagerList[i]->allChannelBufferEmpty();
		if(waitForChannels) {
			msleep(channel_wait_ms);
			timeout_ms -= channel_wait_ms;  // Note: Parameter reused and modified
		}
	}

	if(waitForChannels && timeout_ms<=0) {
		currentTaskID = NO_TASK_DEFINED;  
		qDebug() << "Timeout waiting for Channels";  
		// Inform other classes that simulation has stopped playing
		emit simulationStopped();
		return false;
	}
	
	return true;
}



// High Precision Timer (ns) for RealTime clocks
/*! Plays the current simulation */
void CarlsimWrapper::runCarlsim(){
	// Check simulation is loaded
	if(!simulationLoaded)
		throw SpikeStreamSimulationException("Cannot run simulation - no simulation loaded.");

	// Carlsim Devices
	// Wait for the channels to have messages
	if(modelAutosync==FIRST || modelAutosync==LAST) 
		if(!waitForChannels(300*1000, 1000)) // 5min, 1000ms
			return; // timeout

	// High Precision Clock:
	// This clock uses the Windows functions QueryPerformanceCounter and QueryPerformanceFrequency to access the 
	QElapsedTimer timer;

	qint64 elapsedNs;
	qint64 sleepUs = 0;
	
	// Start the high precision clock
	timer.start();
	qint64 rtNs = 0;

	qint64 prevNs; 
	elapsedNs = timer.nsecsElapsed();

	while(currentTaskID == RUN_SIMULATION_TASK && !stopThread){
		// Lock mutex so that update time interval cannot change during this calculation
		mutex.lock();

		double realStepUs = modelStepMs*1000.0 / modelSpeed;
		realTimeUs += realStepUs;
	
		rtNs += realStepUs*1000;

		// Advance simulation one step
		stepCarlsim();

		prevNs = elapsedNs;
		elapsedNs = timer.nsecsElapsed(); 

		// Sleep for remaining time
		sleepUs = (rtNs - elapsedNs)/1000;
		if(sleepUs > 0) 
			usleep(sleepUs);

		// Unlock mutex
		mutex.unlock();

		// Wait for graphics to update if we are monitoring the simulation
		while(!stopThread && waitForGraphics)
			usleep(100);

		#ifdef TIME_PERFORMANCE
			timeTotal += startTime.msecsTo(QTime::currentTime());
			++timeCounter;
			if(timeCounter > 1000)
				currentTaskID = NO_TASK_DEFINED;
		#endif//TIME_PERFORMANCE

		// Carlsim Device Manager
		// wait if all devices processed last message (buffer empty)
		if(modelAutosync==LAST) 
			if(!waitForChannels(90*1000, 100)) // 90s, 100ms
				return; // timeout

	}

	//Output performance measure
	#ifdef TIME_PERFORMANCE
		cout<<"Average time per timestep:"<<( (double)timeTotal / (double)timeCounter )<<" ms; Average number of firing neurons per ms: "<<( (double)numberOfFiringNeurons / (double)timeCounter )<<endl;
	#endif//TIME_PERFORMANCE

	//Inform other classes that simulation has stopped playing
	emit simulationStopped();
}


/*! Resets the temporary weights to the saved weights */
void CarlsimWrapper::resetCarlsimWeights(){
	//Return immediately if there is nothing to save
	if(volatileConGrpMap.size() == 0){
		weightsReset = true;
		weightsSaved = true;
		return;
	}

	//Work through all connection groups
	Network* currentNetwork = Globals::getNetwork();
	for(QHash<unsigned, synapse_id*>::iterator conGrpIter = volatileConGrpMap.begin(); conGrpIter != volatileConGrpMap.end(); ++conGrpIter){
		ConnectionGroup* tmpConGrp = currentNetwork->getConnectionGroup(conGrpIter.key());
		ConnectionIterator endConGrp = tmpConGrp->end();
		for(ConnectionIterator conIter = tmpConGrp->begin(); conIter != endConGrp; ++conIter){
			//Copy current weights to temporary weights
			conIter->setTempWeight(conIter->getWeight());

			//Check for cancellation
			if(weightResetCancelled){
				Globals::getEventRouter()->weightsChangedSlot();
				return;
			}
		}
	}

	//Inform other classes about weight change
	Globals::getEventRouter()->weightsChangedSlot();

	//Weight are saved and reset
	weightsReset = true;
	weightsSaved = true;
}


/*! Saves the weights from the network into the database */
void CarlsimWrapper::saveCarlsimWeights(){
	//Return immediately if there is nothing to save
	if(volatileConGrpMap.size() == 0){
		weightsSaved = true;
		return;
	}

	//Update temporary weights in network to match weights in CARLsim
	updateNetworkWeights();

	//Copy temporary weight to weight field in network
	Network* currentNetwork = Globals::getNetwork();
	unsigned cntr = 0;
	for(QHash<unsigned, synapse_id*>::iterator conGrpIter = volatileConGrpMap.begin(); !weightSaveCancelled && conGrpIter != volatileConGrpMap.end(); ++conGrpIter){
		currentNetwork->copyTempWeightsToWeights(conGrpIter.key());
		++cntr;
		emit progress(cntr, volatileConGrpMap.size());
	}

	//Inform other classes about weight change and saved state of network
	Globals::getEventRouter()->weightsChangedSlot();
	Globals::getEventRouter()->networkListChangedSlot();

	//Weight saving is complete
	weightsSaved = true;
}

#ifdef CARLSIM_AXONPLAST

/*! Resets the temporary delays to the saved delays*/
void CarlsimWrapper::resetCarlsimDelays() {
	//Return immediately if there is nothing to save
	if (volatileConGrpMap.size() == 0) {
		delaysReset = true;
		delaysSaved = true;
		return;
	}

	//Work through all connection groups
	Network* currentNetwork = Globals::getNetwork();
	for (QHash<unsigned, synapse_id*>::iterator conGrpIter = volatileConGrpMap.begin(); conGrpIter != volatileConGrpMap.end(); ++conGrpIter) {
		ConnectionGroup* tmpConGrp = currentNetwork->getConnectionGroup(conGrpIter.key());
		ConnectionIterator endConGrp = tmpConGrp->end();
		for (ConnectionIterator conIter = tmpConGrp->begin(); conIter != endConGrp; ++conIter) {

			//Copy current weights to temporary weights
			conIter->setTempDelay(conIter->getDelay());

			//Check for cancellation
			if (delayResetCancelled) {
				Globals::getEventRouter()->delaysChangedSlot();
				return;
			}
		}
	}

	//Inform other classes about weight change
	Globals::getEventRouter()->delaysChangedSlot();

	////Weight are saved and reset
	delaysReset = true;
	delaysSaved = true;
}


/*! Saves the weights from the network into the database */
void CarlsimWrapper::saveCarlsimDelays() {
	//Return immediately if there is nothing to save
	if (volatileConGrpMap.size() == 0) {
		delaysSaved = true;
		return;
	}

	//Update temporary weights in network to match weights in CARLsim
	updateNetworkDelays();

	//Copy temporary weight to weight field in network
	Network* currentNetwork = Globals::getNetwork();
	unsigned cntr = 0;
	for (QHash<unsigned, synapse_id*>::iterator conGrpIter = volatileConGrpMap.begin(); !weightSaveCancelled && conGrpIter != volatileConGrpMap.end(); ++conGrpIter) {
assert(false); // not yet implemented
//		currentNetwork->copyTempDelaysToDelay(conGrpIter.key());
		++cntr;
		emit progress(cntr, volatileConGrpMap.size());
	}

	//Inform other classes about weight change and saved state of network
	Globals::getEventRouter()->delaysChangedSlot();
	Globals::getEventRouter()->networkListChangedSlot();

	//Weight saving is complete
	delaysSaved = true;
}

#endif


/*! Puts class into error state */
void CarlsimWrapper::setError(const QString& errorMessage){
	currentTaskID = NO_TASK_DEFINED;
	error = true;
	this->errorMessage = errorMessage;
	stopThread = true;
}


/*! Sets the neuron parameters in the simulation. */
void CarlsimWrapper::setNeuronParametersInCarlsim(){
	if(neuronGroupID == 0){
		throw SpikeStreamException("Failed to set neuron parameters. NeuronGroupID has not been set.");
	}

	//Get the neuron group
	NeuronGroup* tmpNeurGrp = Globals::getNetwork()->getNeuronGroup(neuronGroupID);

	//Set the parameters depending on the type of neuron.
	if(tmpNeurGrp->getInfo().getNeuronType().getDescription() == "Izhikevich Excitatory Neuron"){
		setExcitatoryNeuronParameters(tmpNeurGrp);
	}
	else
	if(tmpNeurGrp->getInfo().getNeuronType().getDescription() == "Izhikevich Inhibitory Neuron"){
		setInhibitoryNeuronParameters(tmpNeurGrp);
	} 
	else
	if (tmpNeurGrp->getInfo().getNeuronType().getDescription() == "Custom Excitatory Neuron") {
		setCustomExcitatoryNeuronParameters(tmpNeurGrp);
	}
	else
	if (tmpNeurGrp->getInfo().getNeuronType().getDescription() == "Custom Inhibitory Neuron") {
		setCustomInhibitoryNeuronParameters(tmpNeurGrp);
	}
}


/*! Sets the parameters in an excitatory neuron group */
void CarlsimWrapper::setExcitatoryNeuronParameters(NeuronGroup* neuronGroup){
	//Get the parameters
	float a = neuronGroup->getParameter("a");
	float b = neuronGroup->getParameter("b");
	float c_1 = neuronGroup->getParameter("c_1");
	float d_1 = neuronGroup->getParameter("d_1");
	float d_2 = neuronGroup->getParameter("d_2");
	float v = neuronGroup->getParameter("v");
	float sigma = neuronGroup->getParameter("sigma");

	//Create the random number generator
	rng_t rng;
	urng_t ranNumGen( rng, boost::uniform_real<double>(0, 1) );

	//Set parameters in the neurons
	float c, d, u, rand1, rand2;
	NeuronMap::iterator neurGrpEnd = neuronGroup->end();
	for(NeuronMap::iterator iter = neuronGroup->begin(); iter != neurGrpEnd; ++iter){
		//Get random numbers
		rand1 = ranNumGen();
		rand2 = ranNumGen();

		//Calculate excitatory neuron parameters
		c = v + c_1 * rand1 * rand2;
		d = d_1 - d_2 * rand1 * rand2;
		u = b * v;

		#ifdef DEBUG_PARAMETERS
			qDebug()<<"Setting excitatory neuron parameters for neuron "<<iter.key()<<". a="<<a<<"; b="<<b<<"; c="<<c<<"; d="<<d<<"; u="<<u<<"; v="<<v<<"; sigma="<<sigma;
		#endif//DEBUG_PARAMETERS
	}
}


/*! Sets the parameters in an inhibitory neuron group */
void CarlsimWrapper::setInhibitoryNeuronParameters(NeuronGroup* neuronGroup){
	//Extract inhibitory neuron parameters
	float a_1 = neuronGroup->getParameter("a_1");
	float a_2 = neuronGroup->getParameter("a_2");
	float b_1 = neuronGroup->getParameter("b_1");
	float b_2 = neuronGroup->getParameter("b_2");
	float d = neuronGroup->getParameter("d");
	float v = neuronGroup->getParameter("v");
	float sigma = neuronGroup->getParameter("sigma");

	//Create the random number generator 
	rng_t rng;
	urng_t ranNumGen( rng, boost::uniform_real<double>(0, 1) );

	//Set parameters in the neurons
	float a, b, u, rand1, rand2;
	NeuronMap::iterator neurGrpEnd = neuronGroup->end();
	for(NeuronMap::iterator iter = neuronGroup->begin(); iter != neurGrpEnd; ++iter){
		//Get random numbers
		rand1 = ranNumGen();
		rand2 = ranNumGen();

		//Calculate inhibitory neuron parameters
		a = a_1 + a_2 * rand1;
		b = b_1 - b_2 * rand2;
		u = b * v;
	}
}


/*! Sets the parameters in an excitatory neuron group */
void CarlsimWrapper::setCustomExcitatoryNeuronParameters(NeuronGroup* neuronGroup) {

}

/*!Sets the parameters in an inhibitory neuron group*/
void CarlsimWrapper::setCustomInhibitoryNeuronParameters(NeuronGroup * neuronGroup) {
	
}


/*! Advances the simulation by one step */
void CarlsimWrapper::stepCarlsim(){

	// clocks: snn, model, and real
	if(timeStepCounter==0) {
		realTime0 = QDateTime::currentDateTime(); // base for eff. used realtime (snn stopped/paused)
		realTimeUs = 0;

		if(modelStart==REALTIME) {
			// var1
			modelTime0 = realTime0.addMSecs( - modelLagMs );
			modelTimeMs = modelTime0.toMSecsSinceEpoch(); // ??
		}

		snnTimeMs = 0;
	}

	unsigned *firedArray, numFiredNeurons = 0; //, numCurrentNeurons = 0;  
	size_t firedCount = 0;
	firingNeuronList.clear();

	//---------------------------------------
	//     Step simulation
	//---------------------------------------
	//Add inject noise neurons to end of injection vector
	if(!injectNoiseMap.isEmpty() || !neuronIDsToFire.isEmpty() || !deviceManagerList.isEmpty())
		numFiredNeurons = addInjectFiringNeuronIDs(); 

	#ifdef DEBUG_STEP
		qDebug()<<"About to step carlsim.";
	#endif//DEBUG_STEP

	//current state 
	CARLsimState state = carlsim->getCARLsimState();

	emit monitorStartRecording(getSnnTimeMs());	// 0,50,100,..

#ifdef DEBUG_FIRING_WITH_SPIKE_MONITORS
	bool isRecording = false; 
	// capture firing by utilizing CARLsim's SpikeMonitors without files 
	// iterate over all groups (wg. Blinken)
	foreach(auto grp, Globals::getNetwork()->getNeuronGroups()) {

		SpikeMonitor* monitor0 = NULL; 
		if(state == SETUP_STATE) {
			monitor0 = carlsim->setSpikeMonitor(grp->getVID(), "NULL");  	
		} else 
		if(state == RUN_STATE) {
			monitor0 = carlsim->getSpikeMonitor(grp->getVID());
		} else {
			throw " user error ";
		}

		if(monitor) {
			monitor0->startRecording();
			isRecording = true; 
		}
	}
#endif
	
	if(numFiredNeurons) {
	}

	emit stepCurrentInjection();   // infom all injectors to do their thing



	bool printRunSummary=false; 
	int ms = 1; // steps
	int s = 0; //
	auto result = carlsim->runNetwork(s, ms, printRunSummary);  
	// during runNetwork carlsim is in EXE_STATE
	// after the synchronous call returns, carlsim is back in the RUN_STATE .

	CARLsimState new_state = carlsim->getCARLsimState();
	if(state == SETUP_STATE && new_state != RUN_STATE) {
		throw SpikeStreamException("CARLsim was not started");
	} else 
	if(state == RUN_STATE && new_state != RUN_STATE) {
		throw SpikeStreamException("CARLsim is no longer running");
	} 
	emit carlsimRunState(); // notify

	emit monitorStopRecording(getSnnTimeMs());

#ifdef DEBUG_FIRING_WITH_SPIKE_MONITORS
	if(isRecording) {  // recording needs to be stoped even if monitor has beed switched off meanwhile 
		SpikeMonitor* monitor0 = NULL; 
		foreach(auto grp, Globals::getNetwork()->getNeuronGroups()) {
			monitor0 = carlsim->getSpikeMonitor(grp->getVID());
			monitor0->stopRecording();
		}
	}
#endif

		if (firing.empty()) {  
			auto numNeurons = carlsim->getNumNeurons();
			firing.resize(numNeurons);
		}
#ifndef DEBUG_PERFORMANCE2  
		carlsim->getFiring(firing);   
#endif
#ifndef DEBUG_PERFORMANCE1 
		foreach(auto grp, Globals::getNetwork()->getNeuronGroups()) {
			int gGrpId = grp->getVID();
			int gStartN = carlsim->getGroupStartNeuronId(gGrpId);
			int gEndN = carlsim->getGroupEndNeuronId(gGrpId);
			int start  = grp->getStartNeuronID(); 
			int offset = 0;			
			// Neuron IDs are _NOT_ ordered by the underlying container
			// => the index mapping must be done by 
			for (auto iter = grp->begin(); iter != grp->end(); iter++) {
				offset = (*iter)->getID() - start;
				if (firing[gStartN + offset]) {
					firingNeuronList.append((*iter)->getID());
					firedCount++; // see below
				}
			}
		}
#endif
//	}



#ifdef DEBUG_FIRING_WITH_SPIKE_MONITORS
	if (monitor) {
		SpikeMonitor* monitor0 = NULL;
		foreach(auto grp, Globals::getNetwork()->getNeuronGroups()) {
			monitor0 = carlsim->getSpikeMonitor(grp->getVID());
			std::vector<std::vector<int> >& spikes0 = monitor0->getSpikeVector2D();  // 1x20
																					 //firings
			int nSpikes0 = monitor0->getPopNumSpikes();
			int nSilent0 = monitor0->getNumSilentNeurons();
			firedCount = nSpikes0; // see below
			qDebug() << "SpikeMonitor[" << grp->getVID() << "] spikes:" << nSpikes0 << " silent:" << nSilent0 << "\n";
			for (int id = 0; id<spikes0.size(); id++) {
				int n = spikes0[id].size();
				//printf("neuron[%d](%d): ", id, n); 
				for (int i = 0; i<n; i++) {
					int t = spikes0[id][i];
					//printf("%d ", t); 
					//
				}
				//printf("\n");
			}
			monitor0->print(); // debug
		}
	}
#endif

	
	#ifdef DEBUG_STEP
		qDebug()<<"Carlsim successfully stepped.";
	#endif//DEBUG_STEP

	//Empty noise injection map if we are not sustaining it
	if(!sustainNoise)
		injectNoiseMap.clear();

	//Delete pattern if it is not sustained
	if(!sustainPattern){
		injectionPatternVector.clear();
		injectionCurrentNeurIDVector.clear();
		//injectionCurrentVector.clear();  
	}
	//Delete neurons added to end of vector from noise or current
	else {
		if(numFiredNeurons > 0){
			injectionPatternVector.erase(injectionPatternVector.end() - numFiredNeurons, injectionPatternVector.end());
		}
	}


	//---------------------------------------------------------
	//        Pass list of firing neurons to other classes
	//---------------------------------------------------------
	if(archiveMode || (monitorFiringNeurons && monitor) || !deviceManagerList.isEmpty() || updateFiringNeurons){
		//Add firing neuron ids to list
		for(unsigned i=0; i<firedCount; ++i)
			; //firingNeuronList.append(firedArray[i]);   
		#ifdef DEBUG_STEP
			if(firedCount > 0)
				qDebug()<<"Number of firing neurons: "<<firedCount;
		#endif//DEBUG_STEP
		#ifdef TIME_PERFORMANCE
			numberOfFiringNeurons += firedCount;
		#endif//TIME_PERFORMANCE

		//Store firing neurons in database
		if(archiveMode){
			archiveDao->addArchiveData(archiveInfo.getID(), timeStepCounter, firingNeuronList);
		}

#ifdef NC_EXTENSIONS_DA
		if(daStdp && daDecay == 1.0f) 
			da = 0.f; 
#endif
		for(int i=0; i<deviceManagerList.size(); ++i){
			AbstractDeviceManager* deviceManager = deviceManagerList[i];
#ifdef NC_EXTENSIONS
			deviceManager->setSimtime(modelTimeMs + 1*3600*1000 );
#endif
#ifdef NC_EXTENSIONS_DA
			da += deviceManager->getDopaminLevel();	
#endif
			deviceManager->setInputNeurons(timeStepCounter, firingNeuronList);
			deviceManager->step();
		}
	}


	//-----------------------------------------------
	//         Extract membrane potential
	//-----------------------------------------------
	if(monitor && monitorMembranePotential){
		#ifdef DEBUG_STEP
			qDebug()<<"About to read membrane potential.";
		#endif//DEBUG_STEP
		getMembranePotential();
		#ifdef DEBUG_STEP
			qDebug()<<"Successfully read membrane potential.";
		#endif//DEBUG_STEP
	}


	//--------------------------------------------
	//               Apply STDP
	//--------------------------------------------
	if(!volatileConGrpMap.isEmpty()){

		float reward = stdpReward; //!< original SpikeStream 
#ifdef NC_EXTENSIONS_DA
		if(daStdp){
			//! decay DA level   
			da *= daDecay;

			/*! Redefinition of reward. 
				For DA modulated STDP the DA level is the major component
				\note This value is acutally used when the apply interval is due */
			reward = stdpReward + da;  
		}
		if(timeStepCounter % applySTDPInterval == 0){
			#ifdef DEBUG_LEARNING
				qDebug()<<"Applying STDP. TimeStepCounter="<<timeStepCounter<<"; applySTDPInterval="<<applySTDPInterval;
			#endif
		}
#endif

	}


	//--------------------------------------------
	//             Retrieve weights
	//--------------------------------------------
	if(monitorWeights && monitor && (timeStepCounter % 100 == 0)){//Same condition as applying STDP  
		updateNetworkWeights();

		//Inform other classes that weights have changed
		Globals::getEventRouter()->weightsChangedSlot();
	}

#ifdef CARLSIM_AXONPLAST
	int delaysUpdateInterval = 500; // Configure Update interval
	if (monitorDelays && monitor && (timeStepCounter % delaysUpdateInterval == 0)) {
		updateNetworkDelays();  

		//Inform other classes that weights have changed
		Globals::getEventRouter()->delaysChangedSlot();
	}
#endif

	// snn time
	++snnTimeMs;	// each steps consumes 1 ms

	// model time 
	modelTimeMs += modelStepMs;   // after the snn steped the model time was consumed

	if(monitor){
		/* Set flag to cause thread to wait for graphics to update.
			This is needed even if we are just running a time step counter */
		waitForGraphics = true;

		// new clock signal
		emit timeChanged(	timeStepCounter, getSnnTimeMs(), 
							getModelTime0(), getModelTime(),
							getRealTime0(), QDateTime::currentDateTime(),
							getRealTimeUs() );

		//Inform listening classes that this time step has been processed
		if(monitorFiringNeurons)
			emit timeStepChanged(timeStepCounter, firingNeuronList);
		else if (monitorMembranePotential)
			emit timeStepChanged(timeStepCounter, membranePotentialMap);
		else
			emit(timeStepChanged(timeStepCounter));

	}


	//Update time step counter
	++timeStepCounter;


}


/*! Unloads CARLsim and sets the simulation loaded state to false. */
void CarlsimWrapper::unloadCarlsim(){
	/* Unlock mutex if it is still locked.
		need to call try lock in case  mutex is unlocked, in which case calling
		unlock again may cause a crash */
	mutex.tryLock();
	mutex.unlock();

	runMutex.tryLock();
	runMutex.unlock();

	delete carlsim;  
	carlsim = NULL;


	//Clean up dynamic arrays in volatile connection group map
	for(QHash<unsigned, synapse_id*>::iterator conGrpIter = volatileConGrpMap.begin(); conGrpIter != volatileConGrpMap.end(); ++conGrpIter){
		delete conGrpIter.value();
	}
	volatileConGrpMap.clear();

	//Clean up sustain noise etc.
	sustainNoise = false;
	injectNoiseMap.clear();
	sustainPattern = false;
	injectionPatternVector.clear();
	injectionCurrentNeurIDVector.clear();

	simulationLoaded = false;
	archiveInfo.reset();

	emit carlsimUnloaded();
}

/*! Query properties of synapses identified by a (small) collection of ids */
void CarlsimWrapper::saveSynapsesProperties(){

	// ensure exclusive access to data members
	QMutexLocker locker(&mutex); 

	// WP: data member had been properly initialized (in public interface method)
	synapsesIds_t::const_iterator idIter=m_synapsesIds.begin();
	synapsesProperties_t::iterator propIter=m_synapsesProperties.begin(); 

	float weight; //Variable passed to CARLsim to get weight
	float accumulator;
	for(;idIter!=m_synapsesIds.end(); idIter++, propIter++) {
		synapseProperties_t props = make_tuple(weight, accumulator, 0.f); // local variable

		(*propIter).swap(props);
	}

	weightsSaved = true;

}



/*! Updates the weights in the network with weights from CARLsim.
	NOTE: Must be called from within the CARLsim thread. */
void CarlsimWrapper::updateNetworkWeights(){
	if(!simulationLoaded){
		throw SpikeStreamException("Failed to update network weights. Simulation not loaded.");
	}

#ifdef DEBUG_WEIGHTS_LEARNING__CONN_MON
	__exp__monitor->takeSnapshot();
	auto weights = __exp__monitor->getWeights();
#endif

	//Work through all of the volatile connection groups
	for(QHash<unsigned, synapse_id*>::iterator conGrpIter = volatileConGrpMap.begin(); conGrpIter != volatileConGrpMap.end(); ++conGrpIter){
		//Get the volatile connection group and matching array of synapse IDs
		ConnectionGroup* tmpConGrp = Globals::getNetwork()->getConnectionGroup(conGrpIter.key());
		synapse_id* synapseIDArray = conGrpIter.value();
		double weightFactor = tmpConGrp->getParameter("weight_factor");//Amount by which weight was multiplied when connection was added to CARLsim

		//Work through connection group and query volatile connections
		float tmpWeight;//Variable passed to CARLsim to get weight
		unsigned conCntr = 0;
		ConnectionIterator conGrpEnd = tmpConGrp->end();
		for(ConnectionIterator conIter = tmpConGrp->begin(); conIter != conGrpEnd; ++conIter){
			#ifdef DEBUG_WEIGHTS
				qDebug()<<"About to query weights: carlsim synapseID="<<synapseIDArray[conCntr]<<" spikestream synapse id="<<conIter->getID();
			#endif//DEBUG_WEIGHTS

			auto fromID = conIter->getFromNeuronID();
			auto toID = conIter->getToNeuronID();

			NeuronGroup* fromNG = Globals::getNetwork()->getNeuronGroup(tmpConGrp->getFromNeuronGroupID ());
			NeuronGroup* toNG = Globals::getNetwork()->getNeuronGroup(tmpConGrp->getToNeuronGroupID ());

			int fromStart = fromNG->getStartNeuronID();

			int toStart = toNG->getStartNeuronID();

			int gFromID = fromID - fromStart;
			int gToID = toID - toStart;

#ifdef DEBUG_WEIGHTS_LEARNING__CONN_MON
			tmpWeight = weights[gFromID][gToID]; 
			
			//Update weight in connection
			#ifdef DEBUG_WEIGHTS
				qDebug()<<"Old weight: "<<conIter->getTempWeight()<<"; New weight: "<<(tmpWeight / weightFactor);
			#endif//DEBUG_WEIGHTS
			conIter->setTempWeight(tmpWeight / weightFactor);

			#ifdef DEBUG_WEIGHTS
				qDebug()<<"TimeStep: "<<timeStepCounter<<"; Weight query complete: weight="<<tmpWeight<<" carlsim synapse id="<<synapseIDArray[conCntr];
			#endif//DEBUG_WEIGHTS

#endif

			//Increase counter for accessing synapse id array
			++conCntr;
		}
	}

}


#ifdef CARLSIM_AXONPLAST
/*! Updates the delays in the network with delays from CARLsim.
	NOTE: Must be called from within the CARLsim thread. */
void CarlsimWrapper::updateNetworkDelays() {
	if (!simulationLoaded) {
		throw SpikeStreamException("Failed to update network weights. Simulation not loaded.");
	}

	//Get neuron types
	NetworkDao networkDao(Globals::getNetworkDao()->getDBInfo());
	NeuronType exNeurType = networkDao.getNeuronType("Izhikevich Excitatory Neuron");
	foreach(auto cGrp, Globals::getNetwork()->getConnectionGroups()) {  

		// Place Cells 
		auto fromGroup = Globals::getNetwork()->getNeuronGroup(cGrp->getFromNeuronGroupID());
		auto toGroup = Globals::getNetwork()->getNeuronGroup(cGrp->getToNeuronGroupID());
		if (fromGroup == toGroup && fromGroup->getNeuronTypeID() == exNeurType.getID()) {

#ifdef DEBUG_AXONPLAST
			printf("Read delays from CARLsim for %s at %0.3f ms\n",
				cGrp->getInfo().getDescription().toStdString().c_str(), float(timeStepCounter) / 1000.0f);
#endif

			auto grp = fromGroup;
			int gGrpId = grp->getVID();
			int gStartN = carlsim->getGroupStartNeuronId(gGrpId);
			int gEndN = carlsim->getGroupEndNeuronId(gGrpId);
			int start = grp->getStartNeuronID();
			int offset = 0;

			int numPreN, numPostN;
			uint8_t* delays = carlsim->getDelays(gGrpId, gGrpId, numPreN, numPostN);

			//Validate Return of the getDelays function
			assert(numPreN == fromGroup->size());
			assert(numPostN == toGroup->size());
			int m = 0;
			uint8_t* p = delays;
			for (int n = 0; n < numPreN * numPostN; n++)
				if (*p++ > 0)
					m++;
			int regularPlaceCellConnections = 2 * (sqrt(numPreN) * (sqrt(numPostN) - 1) + (sqrt(numPreN) - 1) * (sqrt(numPostN)));
			if (m != regularPlaceCellConnections)
				printf("WARNING inconsistent delays returned %d instead of %d\n", m, regularPlaceCellConnections);
			 
			//Print diagonal matrix for small networks
			if (numPostN < 50) {
				printf("  \\ post\n");
				printf("pre");
				for (int j = 0; j < numPostN; j++)
					printf(" %02d", j);
				printf("\n");

				// 2 x 12 of 16x(16-1)
				for (int i = 0; i < numPreN; i++) {
					printf(" %02d", i);
					for (int j = 0; j < numPostN; j++) {
						int d = delays[j * numPostN + i];
						//int d = delays[j + i * numPreN];
						if (d > 0)
							printf(" % 2d", d);
						else
							printf("   ");
					}
					printf("\n");
				}
			}

			for (ConnectionIterator iter = cGrp->begin(); iter != cGrp->end(); iter++) {

				unsigned preSynN = iter->getFromNeuronID() - start;
				unsigned postSynN = iter->getToNeuronID() - start;

				float d_old = iter->getTempDelay();
				int d_index = preSynN + numPostN * postSynN;
				float d_new = delays[d_index];

				if (numPostN < 50) {
					printf(" %02d -> %02d = %f (%f)\n", preSynN, postSynN, d_new, d_old);
				}

				if (d_new != 0) {
					iter->setTempDelay(d_new); 
				}
				else {
					printf("Warning: CARLsim returned a zero delay for pre %d and post %d\n", preSynN, postSynN);
				}


			}

			printf("\n");
			delete delays;

		}
	}
}
#endif

void CarlsimWrapper::resetTime() { 	

	QMutexLocker locker(&mutex);  

	timeStepCounter = 0; 
	snnTimeMs = 0;

	// Note: initialized in step0
	modelTimeMs = modelTime0.toMSecsSinceEpoch();  
	realTimeUs = 0;
	
	// CarlsimDeviceManager delegate reset to all devices
	for(int i=0; i<deviceManagerList.size(); ++i)
		deviceManagerList[i]->reset(); 

	emit timeChanged(	timeStepCounter, getSnnTimeMs(), 
					getModelTime0(), getModelTime(),
					getRealTime0(), QDateTime::currentDateTime(),
					getRealTimeUs() );
}


void CarlsimWrapper::emit_timeChanged() {

#ifndef DEBUG_PERFORMANCE3
	emit timeChanged(	timeStepCounter, getSnnTimeMs(), 
					getModelTime0(), getModelTime(),
					getRealTime0(), QDateTime::currentDateTime(),
					getRealTimeUs() );
#endif

}


void CarlsimWrapper::manage(int conGrpId, CarlsimGeneratorContainer* congen) {
	if(connectionGenerators.size() < conGrpId+1) 
		connectionGenerators.resize(conGrpId+1);
	connectionGenerators[conGrpId] = congen;
} 

void CarlsimWrapper::releaseConnectionGenerators() {
	for(auto iter = connectionGenerators.begin(); iter<connectionGenerators.end(); iter++)
		delete *iter;  // the stored ConnectionGenerator
}