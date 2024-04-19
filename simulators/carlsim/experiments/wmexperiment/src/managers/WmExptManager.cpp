//SpikeStream includes
#include "WmExptManager.h"
#include "Util.h"
#include "Globals.h"
//#include "ResultsCursor.h"

using namespace spikestream;

//Qt includes
#include <QDebug>


/*! Constructor */
WmExptManager::WmExptManager(): SpikeStreamThread(){
	carlsimWrapper = nullptr;
	expWrapper = nullptr;
	//maze = nullptr;
	spikeMonitor = nullptr;
	//placeCells = nullptr;
}


/*! Destructor */
WmExptManager::~WmExptManager(){

	//// free memory ressource
	//if (expWrapper != nullptr)
	//	delete expWrapper; 
	//if(maze != nullptr)
	//	delete maze;

}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

//Inherited from QThread
void WmExptManager::run(){
	clearError();
	stopThread = false;
	unsigned origWaitInterval = carlsimWrapper->getWaitInterval_ms();
	carlsimWrapper->setWaitInterval(1);//Minimal wait between steps

	try{
		//Train network on numPatterns patterns
		emit statusUpdate("Starting Experiment " + QString::number(experimentNumber + 1));

		//Define array of pointer to function member
		void (WmExptManager:: * experiments[])() = { 
			&WmExptManager::runExperiment1,
			&WmExptManager::runExperiment2,	 // sequencially	
			&WmExptManager::runExperiment3   // random walk
		//	&WmExptManager::runExperiment4    // Morris Water Maze 10 x 8 x 4 Trials 
		};

		//Dispatch function
		(this->*experiments[experimentNumber])();
		
	}
	catch(SpikeStreamException& ex){
		setError(ex.getMessage());
	}
	catch(...){
		setError("EpropExperimentManager has thrown an unknown exception.");
	}

	carlsimWrapper->setWaitInterval(origWaitInterval); //Restore wrapper to original state
	experimentNumber = NO_EXPERIMENT;
	stopThread = true;
}


/*! Sets up the experiment and starts thread running. */
void WmExptManager::startExperiment(CarlsimWrapper* carlsimWrapper, carlsim_monitors::OatSpikeMonitor* spikeMonitor,
	QHash<QString, 
	double>& parameterMap, 
	QString presetsName,
// ,
//	QString costs, 
//	QStringList segmentStrings, QStringList landmarkStrings, 
//	QStringList startStrings, QStringList endStrings, 
	QStringList selectionStrings
//	NeuronGroup* placeCells
 ) {
	this->carlsimWrapper = carlsimWrapper;

	// FIXME this needs to be feed, what the experiment stops --> see diffenrenc pause 
	// investigate ressource model in C++11
	//maze = new Maze(4);  // FIXME load from template + parameters 

	auto neuronGroups = Globals::getNetwork()->getNeuronGroupsMap();
	auto neuronGroup = neuronGroups["dlPFC.states"];
	if (!neuronGroup)
		return;
	auto grpSize = neuronGroup->size();

	//load parameter so that they are available for the functions below
	storeParameters(parameterMap);

	presets = presetsName;  // presets name


	expWrapper = new WmExpCarlsimWrapper(carlsimWrapper, spikeMonitor);

	this->spikeMonitor = spikeMonitor;
	//this->placeCells = placeCells; 


	if (!selectionStrings.isEmpty()) {
		for (int i = 0; i < selectionStrings.size(); i++) {
			int id = selectionStrings[i].toInt();
			startIds.append(id); 
		}
	}
	
	// synchronously connected at Widget level to CarlsimOatWidget findMonitor
	// maybe over global router to avoid to much peer to peer ?
	//emit requiresOatMonitor("OatSpikeMonitor", "dlPFC.states");

	// validate monitor was set

	start();  // QThread
}



void WmExptManager::setMonitor(carlsim_monitors::OatSpikeMonitor* spikeMonitor) {
	this->spikeMonitor = spikeMonitor;
}
	


// FIXME
// 
// stop experiment 
// free ressources



/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/



/*! Runs the experiment. */
void WmExptManager::runExperiment1() {
	//Seed the random number generator
	Util::seedRandom(randomSeed);

	//Reset the time panel
	if (carlsimWrapper->getTimeStep() == 0)
		carlsimWrapper->resetTime();

	// Override interval for update of weights/delays from Experiment Parameter
	// FIXME read parameter 
	// FIXME extend wrapper with the instead, .. intervall from STDP
	//carlsimWrapper->setWeightsUpdateIntervall(WeightsUpdateInterval_ms);  

	//for(auto t = carlsimWrapper->getSnnTimeMs(); 
	//	!stopThread && (end_time == -1 || t < end_time); 
	//	t = carlsimWrapper->getSnnTimeMs())




	////Avoid random spontanous self excit at 3ms	
	//// inject 20 to interneurons (if existing)
	//auto neuronGroups = Globals::getNetwork()->getNeuronGroupsMap();
	//auto neuronGroup = neuronGroups["dlPFC.in"];
	//if (neuronGroup) {
	//	unsigned gGrpId = neuronGroup->getVID();
	//	carlsimWrapper->carlsim->setExternalCurrent(gGrpId, initialCurrent);

	//	// fire once
	//	stepCarlsim(1);

	//	// Reset current
	//	carlsimWrapper->carlsim->setExternalCurrent(gGrpId, 0);
	//}


	//Use internal clock
	auto t = carlsimWrapper->getTimeStep(); 
	for(auto end = t + endTime_ms; //Treat endTime_ms as duration instead of fixed time 
		!stopThread && (endTime_ms == -1 || t < end); 
		t = carlsimWrapper->getTimeStep())
	{

//		//Calculatate relative time inside a grid cell, e.g. 13530 = 530; 
//		int rt = t % timeGrid_ms;
//		int skip = startTime_ms - rt;
//		if (skip < 0)
//			skip = timeGrid_ms - rt + startTime_ms;
//
//		//Set fast forward speed to reach the start
//		//carlsimWrapper->setModelSpeed(fast_forward);
//		pauseInterval_ms = 1.0 / fastForward; //fixme
//
//		//DO NOT INTERFER WITH USER CHOICE TO MONITOR
//		//Switch off monitors
//		//bool monitoring = carlsimWrapper->getMonitor();  // FIXME implement getter
//		//bool monitoring = true;
//		//if (monitoring)
//		//	carlsimWrapper->setMonitor(false);   
//
//		//Run to the start of the experiment
//		stepCarlsim(skip);
//
//		//Prepare the experiment
//		//if (monitoring)
//		//	carlsimWrapper->setMonitor(true);
//		//carlsimWrapper->setModelSpeed(slow_motion);
//		pauseInterval_ms = 1.0 / slowMotion; //fixme

		//Branch to specific experiment
		//nueronId
		// create a current vector of 16 with initial current of zero
		auto neuronGroups = Globals::getNetwork()->getNeuronGroupsMap();
		auto neuronGroup = neuronGroups["dlPFC.in"];
		unsigned gGrpId = neuronGroup->getVID();
		auto grpSize = neuronGroup->size();
		vector<float> currents(grpSize, 0.f);
		unsigned gNId = startNeuron; // relative ID
		float current = initialCurrent;
		currents[gNId] = current;
		carlsimWrapper->carlsim->setExternalCurrent(gGrpId, currents);

		//Start the experiment
		int duration = observationTime_ms;
		emit statusUpdate(QString::asprintf("Excite neuron %d in group %d with %0.1f at %d ms", startNeuron, gGrpId, initialCurrent, carlsimWrapper->getTimeStep()));
		stepCarlsim(1);
		carlsimWrapper->carlsim->setExternalCurrent(gGrpId, 0);  // reset input current

		spikeMonitor->startRecording();
		for (auto steps = 0; steps < duration - 1 && !stopThread; steps++) {
			stepCarlsim(1);
			t = carlsimWrapper->getTimeStep();
			//
			//bool targetFired = (expWrapper->checkTargetFired(targetNeuron, t));
			//spikeMonitor->startRecording();
			//if (targetFired) {
			//	emit statusUpdate(QString::asprintf("Spike Wave has reached target neuron %d at %d", targetNeuron, t));
			//	break;
			
		}	
		spikeMonitor->stopRecording();


		// check
		expWrapper->printSpikes(0);

		if (startNeuron < targetNeuron) {
			bool check1 = expWrapper->checkNeuronFiring(startNeuron, 150., 0.1, duration, 10);
			emit statusUpdate(QString::asprintf("State neuron %d has expected firing rate: %s", startNeuron, check1 ? "Yes" : "No"));
		} else {
			bool check1 = expWrapper->checkNeuronFiring(startNeuron, 150., 0.1, duration/2., 5);
			bool check2 = expWrapper->checkNeuronFiring(targetNeuron, 150.0, 10.0, duration/2., 5);
			emit statusUpdate(QString::asprintf("State neuron %d has expected firing rate: %s", startNeuron, check1 ? "Yes" : "No"));
			emit statusUpdate(QString::asprintf("State switch from %d to %d successful: %s", startNeuron, targetNeuron, check2 ? "Yes" : "No"));
		}
		


//		//Print spike. The recording must be stopped in order to access the spikes with getSpikeVector2D(). 
//		//Elsewise the following USER ERROR is thrown: Recording cannot be on at this point.
//		spikeMonitor->stopRecording();  
//		expWrapper->printSpikes(0);
//		spikeMonitor->startRecording();
//
//		expWrapper->getDelays();
//
//			// Hint: Another potential design improvement against SpikeStream: exp. might want to define the LogLevel
//			// Workaround: parameter of CARLsim simulator -> (Dialog/Config-File)
//		int startNId = startNeuron;
//		int endNId = targetNeuron;
//		//Hint: the internals like path, are enapsulated
//		if (expWrapper->findWaveFront(startNId, endNId)) {
//
//			std::string pathString; 
//			expWrapper->writePathString(pathString);
//			emit statusUpdate(QString::asprintf("Path: %s", pathString.c_str())); 
//			expWrapper->writePath("results\\path.txt");
//			expWrapper->writeEligibility("results\\eligibility.txt");
//
//
//			//Refine delays utilizing EProp / eligibility trace 
//			expWrapper->eprop();
//
//			//Update new found, calculated delays
//			expWrapper->updateDelays();
//
//			emit statusUpdate(QString::asprintf("Iteration #%d of E-Prop yields a total loss of %d.",
//				expWrapper->getIteration(), expWrapper->getLoss()));
//
//
//			//if (expWrapper->getLoss() < 1) {
//			//	emit statusUpdate(QString::asprintf("Optimal path found, terminating E-Prop."));
//			//	stopThread = true;
//			//}
//				
//		}
//
//		//Finalize experiment
//		//Run to end of the grid
//		//t = carlsimWrapper->getSnnTimeMs();
//		t = carlsimWrapper->getTimeStep();		//Use internal clock
//		rt = t % timeGrid_ms;
//
//		//carlsimWrapper->setModelSpeed(fast_forward);
//		pauseInterval_ms = 1.0 / fastForward; //fixme
//		stepCarlsim(timeGrid_ms - rt - 1); // run to the end of the grid (with monitoring on)
//
//		auto pauseInterval_ms = 10000; 
//		//emit statusUpdate("wait before grid " + QString::number(pauseInterval_ms/1000.0));
//		//Wait a little, before the Raster vanish  --> para wait grid switch
//		if(!stopThread)
//			msleep(pauseInterval_ms);
//		stepCarlsim(1); // run to the end of the grid (with monitoring on)
	}

	//Output final result
	emit statusUpdate("Experiment complete.");
}



/*! Runs sequencial from start to target state over both possible paths */
void WmExptManager::runExperiment2() {


	//Seed the random number generator
	Util::seedRandom(randomSeed);

	//Reset the time panel
	if (carlsimWrapper->getTimeStep() == 0)
		carlsimWrapper->resetTime();

	int startId_i = 0;
	int loop = 1;

//	auto statusUpdateLoop = [&]() {if(verbosity>=2) emit statusUpdate(QString::asprintf("Training Loop #%d", loop)); };
//	statusUpdateLoop();

	int errors_state = 0;
	int errors_switch = 0;

	//Use internal clock
	auto t = carlsimWrapper->getTimeStep();
	if (verbosity >= 1) emit statusUpdate(QString::asprintf("Iteration #%d", loop));

	for (auto end = t + endTime_ms; //Treat endTime_ms as duration instead of fixed time 
		!stopThread && (endTime_ms == -1 || t < end);
		t = carlsimWrapper->getTimeStep())
	{
		auto startNId = startIds.at(startId_i);

		if (verbosity >= 2) emit statusUpdate(QString::asprintf("StartId[%d] = %d",
			startId_i, startNId));

		auto neuronGroups = Globals::getNetwork()->getNeuronGroupsMap();
		auto neuronGroup = neuronGroups["dlPFC.in"];
		unsigned gGrpId = neuronGroup->getVID();
		auto grpSize = neuronGroup->size();
		vector<float> currents(grpSize, 0.f);
		float current = initialCurrent;
		currents[startNId] = current;
		carlsimWrapper->carlsim->setExternalCurrent(gGrpId, currents);

		//Start the experiment
		int duration = observationTime_ms;
		if (verbosity >= 3) emit statusUpdate(QString::asprintf("Excite neuron %d in group %d with %0.1f at %d ms", startNId, gGrpId, initialCurrent, carlsimWrapper->getTimeStep()));
		stepCarlsim(1);
		carlsimWrapper->carlsim->setExternalCurrent(gGrpId, 0);  // reset input current

		spikeMonitor->startRecording();
		for (auto steps = 0; !stopThread && steps < duration - 1; steps++) {
			stepCarlsim(1);
			t = carlsimWrapper->getTimeStep();
		}
		spikeMonitor->stopRecording();

		if (verbosity >= 3)
			expWrapper->printSpikes(0);

		// check
		if (startNId < targetNeuron) {
			bool check1 = expWrapper->checkNeuronFiring(startNId, 150., 0.1, duration, 10);
			if (verbosity >= 2) emit statusUpdate(QString::asprintf("State neuron %d has expected firing rate: %s", startNId, check1 ? "Yes" : "No"));
			if (!check1)
				++errors_state;
		}
		else {
			bool check1 = expWrapper->checkNeuronFiring(startNId, 150., 0.1, duration / 2., 5);
			bool check2 = expWrapper->checkNeuronFiring(0, 150.0, 10.0, duration / 2., 5);
			if (verbosity >= 2) emit statusUpdate(QString::asprintf("State neuron %d has expected firing rate: %s", startNId, check1 ? "Yes" : "No"));
			if (verbosity >= 2) emit statusUpdate(QString::asprintf("State switch from %d to %d successful: %s", startNId, 0, check2 ? "Yes" : "No"));
			if (!check1)
				++errors_state;
			if (!check2)
				++errors_switch;
		}

		if (++startId_i >= startIds.size()) {
			// round robin
			startId_i = 0; 
			if (++loop > training_loops)
				break;
			else
				if (verbosity >= 1) emit statusUpdate(QString::asprintf("Iteration #%d", loop));
		}
	}

	//Output final result	
	if(errors_state > 0 || errors_switch > 0) 
		emit statusUpdate(QString::asprintf("Experiment Test Landmarks faild: state errors = %d switching errors0 %d",
			errors_state, errors_switch));
	else
		emit if (verbosity >= 1)statusUpdate(QString::asprintf("Experiment complete"));

}




/*! Random walk */
void WmExptManager::runExperiment3() {


	//Seed the random number generator
	Util::seedRandom(randomSeed);

	//Reset the time panel
	if (carlsimWrapper->getTimeStep() == 0)
		carlsimWrapper->resetTime();

	int startId_i = -1;
	int loop = 1;

	int errors_state = 0;
	int errors_switch = 0;

	//Use internal clock
	auto t = carlsimWrapper->getTimeStep();
	int startNId = -1;
	int prev_startNId = startNId;
	for (auto end = t + endTime_ms; //Treat endTime_ms as duration instead of fixed time 
		!stopThread && (endTime_ms == -1 || t < end);
		t = carlsimWrapper->getTimeStep())
	{
		auto neuronGroups = Globals::getNetwork()->getNeuronGroupsMap();
		auto neuronGroup = neuronGroups["dlPFC.in"];
		unsigned gGrpId = neuronGroup->getVID();
		auto grpSize = neuronGroup->size();

		startNId = Util::getRandomUInt(0, neuronGroup->size());		
		if (startNId == prev_startNId || (prev_startNId == targetNeuron && startNId ==0) )   // switch off (bi-stab)
			continue; // patch 10% state errors

		vector<float> currents(grpSize, 0.f);
		float current = initialCurrent;
		currents[startNId] = current;
		carlsimWrapper->carlsim->setExternalCurrent(gGrpId, currents);

		//Start the experiment
		//int duration = observationTime_ms;
		int duration = Util::getRandomUInt(50, 250);    // 20,220  produces 1% state errors

		int fuse_latency = 91;  // ms

		// patch switching errors caused by test design
		//if (duration < fuse_latency && startNId == targetNeuron)
		//	duration += fuse_latency;   
		if (startNId == targetNeuron)
			duration = std::max(200, duration);   // at least 200ms 

		if (verbosity >= 2)
			emit statusUpdate(QString::asprintf("Loop #%d State Neuron[%d], Duration = %d", loop, startNId, duration));
		else
			if (verbosity >= 1)
				emit statusUpdate(QString::asprintf("Loop #%d", loop));

		if (verbosity >= 3) emit statusUpdate(QString::asprintf("Excite neuron %d in group %d with %0.1f at %d ms", startNId, gGrpId, initialCurrent, carlsimWrapper->getTimeStep()));
		stepCarlsim(1);
		carlsimWrapper->carlsim->setExternalCurrent(gGrpId, 0);  // reset input current

		spikeMonitor->clear();
		spikeMonitor->startRecording();
		for (auto steps = 0; !stopThread && steps < duration - 1; steps++) {
			stepCarlsim(1);
			t = carlsimWrapper->getTimeStep();
		}
		spikeMonitor->stopRecording();


		if (stopThread)
			break;

		if (verbosity >= 3)
			expWrapper->printSpikes(0);

		// check
		// targetNeuron = 5, final state
		if (startNId < targetNeuron || duration < fuse_latency) {
			bool check1 = expWrapper->checkNeuronFiring(startNId, 150., 0.5, duration, 10.0 * duration / fuse_latency);
			if (!check1 || verbosity >= 3) emit statusUpdate(QString::asprintf("State neuron %d has expected firing rate: %s", startNId, check1 ? "Yes" : "No"));
			if (!check1) {
				++errors_state;
				if (verbosity >= 2) expWrapper->printSpikes(0);
			}
		}
		else {
			bool check1 = expWrapper->checkNeuronFiring(startNId, 150., 0.5, fuse_latency, 5.0 * duration / fuse_latency);
			bool check2 = expWrapper->checkNeuronFiring(0, 150.0, 0.5, duration - fuse_latency, 5.0 * (duration - fuse_latency) / fuse_latency);
			if (!check1 || verbosity >= 3) emit statusUpdate(QString::asprintf("State neuron %d has expected firing rate: %s", startNId, check1 ? "Yes" : "No"));
			if (!check2 || verbosity >= 3) emit statusUpdate(QString::asprintf("State switch from %d to %d successful: %s", startNId, 0, check2 ? "Yes" : "No"));
			if (!check1)
				++errors_state;
			if (!check2)
				++errors_switch;
			if(!check1 || !check2)
				if (verbosity >= 2) expWrapper->printSpikes(0);
		}

		if (++loop > training_loops)
			break;

		prev_startNId = startNId;
	}

	//Output final result	
	if (errors_state > 0 || errors_switch > 0)
		emit statusUpdate(QString::asprintf("Experiment Test Landmarks faild: state errors = %d switching errors %d",
			errors_state, errors_switch));
	else
		emit if (verbosity >= 1)statusUpdate(QString::asprintf("Experiment complete"));

}


//
/////*! Test Boone Landmarks */
//void WmExptManager::runExperiment3() {
//
//	assert(landmarks.size() > 0);
//
//	//Seed the random number generator
//	Util::seedRandom(randomSeed);
//
//	//Reset the time panel
//	if (carlsimWrapper->getTimeStep() == 0)
//		carlsimWrapper->resetTime();
//
//
//	//Avoid random spontanous self excit at 3ms	
//	// inject 20 to interneurons (if existing)
//	auto neuronGroups = Globals::getNetwork()->getNeuronGroupsMap();
//	auto neuronGroup = neuronGroups["CA1 Inter"];
//	if (neuronGroup) {
//		unsigned gGrpId = neuronGroup->getVID();
//		carlsimWrapper->carlsim->setExternalCurrent(gGrpId, 20);
//
//		// fire once
//		stepCarlsim(1);
//
//		// Reset current
//		carlsimWrapper->carlsim->setExternalCurrent(gGrpId, 0);
//	}
//
//	int landmark_i = 0;
//	auto statusUpdateLoop = [&]() {emit statusUpdate(QString::asprintf("Test %d Landmarks", landmarks.size())); };
//	statusUpdateLoop();
//	int total_loss = 0;
//
//	auto results = ResultsCursor();
//	// std::unique_ptr<ResultsCursor>
//	auto experimentResults = results.spanExperiment(presets, "E-Prop");
//	expWrapper->writeCosts(experimentResults->spawnFile("maze.txt"));
//	auto phaseResults = experimentResults->spawnDir(QString::asprintf("%d_trial", 3));
//	phaseResults->spawnDir("png"); // target for exec
//
//	//Use internal clock
//	auto t = carlsimWrapper->getTimeStep();
//	for (auto end = t + endTime_ms; //Treat endTime_ms as duration instead of fixed time 
//		!stopThread && (endTime_ms == -1 || t < end);
//		t = carlsimWrapper->getTimeStep())
//	{
//		auto landmarkResults = phaseResults->spawnLandmark(landmark_i);
//		auto landmark = landmarks[landmark_i];
//		auto startNId = landmark.first;
//		auto endNId = landmark.second;
//		emit statusUpdate(QString::asprintf("Trail #%d: Route S=(%d,%d) E=(%d,%d)  [%d]-->[%d]",
//			landmark_i+1,
//			maze->x(startNId), maze->y(startNId),
//			maze->x(endNId), maze->y(endNId),
//			startNId, endNId));
//
//		//patch
//		startNeuron = landmark.first;
//		targetNeuron = landmark.second;
//
//		//Calculatate relative time inside a grid cell, e.g. 13530 = 530; 
//		int rt = t % timeGrid_ms;
//		int skip = startTime_ms - rt;
//		if (skip < 0)
//			skip = timeGrid_ms - rt + startTime_ms;
//
//		//Set fast forward speed to reach the start
//		//carlsimWrapper->setModelSpeed(fast_forward);
//		pauseInterval_ms = 1.0 / fastForward; //fixme
//
//		//Run to the start of the experiment
//		stepCarlsim(skip);
//
//		//Prepare the experiment
//		//carlsimWrapper->setModelSpeed(slow_motion);
//		pauseInterval_ms = 1.0 / slowMotion; //fixme
//
//		//Branch to specific experiment
//		//nueronId
//		// create a current vector of 16 with initial current of zero
//		auto neuronGroups = Globals::getNetwork()->getNeuronGroupsMap();
//		auto neuronGroup = neuronGroups["CA1 Place Cells"];
//		unsigned gGrpId = neuronGroup->getVID();
//		auto grpSize = neuronGroup->size();
//		vector<float> currents(grpSize, 0.f);
//		float current = initialCurrent;
//		currents[startNId] = current;
//		carlsimWrapper->carlsim->setExternalCurrent(gGrpId, currents);
//
//		//Start the experiment
//		int duration = observationTime_ms;
//		//emit statusUpdate(QString::asprintf("Excite neuron %d in group %d with %0.1f at %d ms", startNId, gGrpId, initialCurrent, carlsimWrapper->getTimeStep()));
//		stepCarlsim(1);
//		carlsimWrapper->carlsim->setExternalCurrent(gGrpId, 0);  // reset input current
//
//		//FIXME run single steps and stop immediately if target has fired
//		//stepCarlsim(duration - 1);
//		for (auto steps = 0; steps < duration - 1; steps++) {
//			stepCarlsim(1);
//			t = carlsimWrapper->getTimeStep();
//			spikeMonitor->stopRecording();
//			bool targetFired = (expWrapper->checkTargetFired(endNId, t));
//			spikeMonitor->startRecording();
//			if (targetFired) {
//				//emit statusUpdate(QString::asprintf("Spike Wave has reached target neuron %d at %d", endNId, t));
//				break;
//			}
//		}
//
//		//Print spike. The recording must be stopped in order to access the spikes with getSpikeVector2D(). 
//		//Elsewise the following USER ERROR is thrown: Recording cannot be on at this point.
//		spikeMonitor->stopRecording();
//		expWrapper->printSpikes(0);
//		spikeMonitor->startRecording();
//
//		expWrapper->getDelays();
//
//		// Hint: Another potential design improvement against SpikeStream: exp. might want to define the LogLevel
//		// Workaround: parameter of CARLsim simulator -> (Dialog/Config-File)
//		//Hint: the internals like path, are enapsulated
//		if (expWrapper->findWaveFront(startNId, endNId)) {
//
//			std::string pathString;
//			expWrapper->writePathString(pathString);
//			emit statusUpdate(QString::asprintf("Path: %s", pathString.c_str()));
//
//			//expWrapper->writePath("results\\path.txt");
//			expWrapper->writePath(results.spawnFile("path.txt"));  // current and cummulative experiment
//			expWrapper->writePath(landmarkResults->spawnFile("path.txt"));  // current and cummulative experiment
//
//			//expWrapper->writeEligibility("results\\eligibility.txt");
//			expWrapper->writeEligibility(results.spawnFile("eligibility.txt"));
//			expWrapper->writeEligibility(landmarkResults->spawnFile("eligibility.txt"));
//
//			landmarkResults->exec("octave --eval eprop_boone_trial_landmark");
//
//			//Refine delays utilizing EProp / eligibility trace 
//			expWrapper->eprop();
//
//			//Dont update when running test
//			////Update new found, calculated delays
//			if(update_delays)
//				expWrapper->updateDelays();
//			total_loss += expWrapper->getLoss();
//			emit statusUpdate(QString::asprintf("E-Prop loss: %d.", expWrapper->getLoss()));
//
//		}
//
//		//Finalize experiment
//		//Run to end of the grid
//		//t = carlsimWrapper->getSnnTimeMs();
//		t = carlsimWrapper->getTimeStep();		//Use internal clock
//		rt = t % timeGrid_ms;
//
//		//carlsimWrapper->setModelSpeed(fast_forward);
//		pauseInterval_ms = 1.0 / fastForward; //fixme
//		stepCarlsim(timeGrid_ms - rt - 1); // run to the end of the grid (with monitoring on)
//
//		//auto pauseInterval_ms = 10000;
//		////emit statusUpdate("wait before grid " + QString::number(pauseInterval_ms/1000.0));
//		////Wait a little, before the Raster vanish  --> para wait grid switch
//		//if (!stopThread)
//		//	msleep(pauseInterval_ms);
//		stepCarlsim(1); // run to the end of the grid (with monitoring on)
//
//		// replace unique pointer
//		landmarkResults = phaseResults->spawnLandmark(landmark_i);
//
//		if (++landmark_i == landmarks.size()) {
//			break;  // exp2 testing done
//		}
//
//	}
//
//	//Output final result
//	emit statusUpdate(QString::asprintf("Experiment Test Landmarks complete.\nTrails: %d Total Loss: %d", 
//		expWrapper->getIteration(), total_loss));
//}



///*! Test Morris Random Walk */
//void WmExptManager::runExperiment4() {
//
//	assert(segments.size() > 0);
//
//	//Seed the random number generator
//	Util::seedRandom(randomSeed);
//
//	//Reset the time panel
//	if (carlsimWrapper->getTimeStep() == 0)
//		carlsimWrapper->resetTime();
//
//
//	//Avoid random spontanous self excit at 3ms	
//	// inject 20 to interneurons (if existing)
//	auto neuronGroups = Globals::getNetwork()->getNeuronGroupsMap();
//	auto neuronGroup = neuronGroups["CA1 Inter"];
//	if (neuronGroup) {
//		unsigned gGrpId = neuronGroup->getVID();
//		carlsimWrapper->carlsim->setExternalCurrent(gGrpId, 20);
//
//		// fire once
//		stepCarlsim(1);
//
//		// Reset current
//		carlsimWrapper->carlsim->setExternalCurrent(gGrpId, 0);
//	}
//
//
//
//	Maze::ConnDelays_t connDelaysInitial;  // pre, post, delay  for update
//	expWrapper->getDelays();
//	connDelaysInitial = expWrapper->connDelays;
//	assert(connDelaysInitial.size() == 1200);  // Morris 13x13 with 8 cardinals
//
//
//
////	// Reload CARLsim Simulation 
////// {
////			//Reset Delays
////	carlsimWrapper->resetDelays();
////	//Re-Load CARLsim : Unload / Load
////	emit carlsimWrapper->monitorStopRecording(0);
////	carlsimWrapper->stopSimulation();
////	carlsimWrapper->unloadSimulation();
////
////	// sync wait max 3s until unloaded
////	for (int timeout = 10; timeout > 0; timeout--) {
////		if (!carlsimWrapper->isSimulationLoaded())
////			break;
////		sleep(1);
////	}
////	carlsimWrapper->loadSimulation();
////	// sync wait max 3s until simulation is loaded
////	for (int timeout = 10; timeout > 0; timeout--) {
////		if (carlsimWrapper->isSimulationLoaded())
////			break;
////		sleep(1);
////	}
////	//emit carlsimWrapper->carlsimConfigStateNetworkBuilt();
////
////
////	//Reset (Simulation)
////	//carlsimWrapper->resetTime();
////
////	//ISSUE might be another thread now
////
////	auto monitor_before = spikeMonitor;
////	//Request Widget to set the new Monitor in the model, as done before in start		
////	emit simulationReloaded();
////	//ISSUE: emit does not call update, ... something wrong with the SIGNAL SLOT 
////	sleep(1);
////
////	auto monitor_after = spikeMonitor;
////
////	assert(monitor_before != monitor_after);
////
////	// CARLsim still has a NULL pointer at spike_monitor.cpp#192
////	//	return spikeMonitorCorePtr_->isRecording();
////
//
//
//
//
//	int segment_i = 0;
//	int simulation = 1;
//	int session = 1;
//	auto statusUpdateSession = [&]() {if (verbosity >= 2) emit statusUpdate(QString::asprintf("Simulation #%d Session #%d", simulation, session)); };
//	statusUpdateSession();
//	int total_loss = 0;
//	int total_activity = 0;
//	int total_length = 0;
//
//
//	auto results = ResultsCursor();
//	// std::unique_ptr<ResultsCursor>
//	auto experimentResults = results.spanExperiment(presets, "E-Prop");
//	expWrapper->writeCosts(experimentResults->spawnFile("maze.txt"));
//	auto phaseResults = experimentResults->spawnDir(QString::asprintf("%d_trails", 4));
//	phaseResults->spawnDir("png"); // target for exec
//	auto simulationResults = phaseResults->spawnDir(QString::asprintf("simulation_%d", simulation));
//	auto sessionResults = simulationResults->spawnDir(QString::asprintf("session_%d", session));
//
//	auto trailsResults = phaseResults->spawnFile("trails.txt");
//	trailsResults->openFile(); // w
//	QTextStream trainingLog(trailsResults->file);
//
//	//Use internal clock
//	auto t = carlsimWrapper->getTimeStep();
//	for (auto end = t + endTime_ms; //Treat endTime_ms as duration instead of fixed time 
//		!stopThread && (endTime_ms == -1 || t < end);
//		t = carlsimWrapper->getTimeStep())
//	{
//		// N, S, E, W, see offset in segment FIXME 
//		auto trialResults = sessionResults->spawnDir(QString::asprintf("trial_%d", (segment_i % 4) + 1));  
//		auto segment = segments[segment_i];
//		auto startNId = segment.first;
//		auto endNId = segment.second;
//		if (verbosity >= 2) emit statusUpdate(QString::asprintf("Segment[%d] S=(%d,%d) E=(%d,%d)  [%d]-->[%d]",
//			segment_i,
//			maze->x(startNId), maze->y(startNId),
//			maze->x(endNId), maze->y(endNId),
//			startNId, endNId));
//
//		//patch
//		startNeuron = segment.first;
//		targetNeuron = segment.second;
//
//		//Calculatate relative time inside a grid cell, e.g. 13530 = 530; 
//		int rt = t % timeGrid_ms;
//		int skip = startTime_ms - rt;
//		if (skip < 0)
//			skip = timeGrid_ms - rt + startTime_ms;
//
//		//Set fast forward speed to reach the start
//		//carlsimWrapper->setModelSpeed(fast_forward);
//		pauseInterval_ms = 1.0 / fastForward; //fixme
//
//		//Run to the start of the experiment
//		stepCarlsim(skip);
//
//		//Prepare the experiment
//		//carlsimWrapper->setModelSpeed(slow_motion);
//		pauseInterval_ms = 1.0 / slowMotion; //fixme
//
//		//Branch to specific experiment
//		//nueronId
//		// create a current vector of 16 with initial current of zero
//		auto neuronGroups = Globals::getNetwork()->getNeuronGroupsMap();
//		auto neuronGroup = neuronGroups["CA1 Place Cells"];
//		unsigned gGrpId = neuronGroup->getVID();
//		auto grpSize = neuronGroup->size();
//		vector<float> currents(grpSize, 0.f);
//		float current = initialCurrent;
//		currents[startNId] = current;
//		carlsimWrapper->carlsim->setExternalCurrent(gGrpId, currents);
//
//		//Start the experiment
//		int duration = observationTime_ms;
//		if (verbosity >= 3) emit statusUpdate(QString::asprintf("Excite neuron %d in group %d with %0.1f at %d ms", startNId, gGrpId, initialCurrent, carlsimWrapper->getTimeStep()));
//		stepCarlsim(1);
//		carlsimWrapper->carlsim->setExternalCurrent(gGrpId, 0);  // reset input current
//
//		//FIXME run single steps and stop immediately if target has fired
//		//stepCarlsim(duration - 1);
//		for (auto steps = 0; steps < duration - 1; steps++) {
//			stepCarlsim(1);
//			t = carlsimWrapper->getTimeStep();
//			spikeMonitor->stopRecording();
//			bool targetFired = (expWrapper->checkTargetFired(endNId, t));
//			spikeMonitor->startRecording();
//			if (targetFired) {
//				if (verbosity >= 3) emit statusUpdate(QString::asprintf("Spike Wave has reached target neuron %d at %d", endNId, t));
//				break;
//			}
//		}
//
//		//Print spike. The recording must be stopped in order to access the spikes with getSpikeVector2D(). 
//		//Elsewise the following USER ERROR is thrown: Recording cannot be on at this point.
//		spikeMonitor->stopRecording();
//		expWrapper->printSpikes(0);
//		spikeMonitor->startRecording();
//
//		expWrapper->getDelays();
//
//		// Hint: Another potential design improvement against SpikeStream: exp. might want to define the LogLevel
//		// Workaround: parameter of CARLsim simulator -> (Dialog/Config-File)
//		//Hint: the internals like path, are enapsulated
//		if (expWrapper->findWaveFront(startNId, endNId)) {
//
//			std::string pathString;
//			expWrapper->writePathString(pathString);
//			if (verbosity >= 3) emit statusUpdate(QString::asprintf("Path: %s", pathString.c_str()));
//
//			//expWrapper->writePath("results\\path.txt");  // current and cummulative experiment
//			expWrapper->writePath(results.spawnFile("path.txt"));  // current and cummulative experiment
//			expWrapper->writePath(trialResults->spawnFile("path.txt"));  // current and cummulative experiment
//
//			//expWrapper->writeEligibility("results\\eligibility.txt");
//			expWrapper->writeEligibility(results.spawnFile("eligibility.txt"));
//			expWrapper->writeEligibility(trialResults->spawnFile("eligibility.txt"));
//
//			trialResults->exec("octave --eval eprop_morris_simulation_session_trial");
//
//			//Refine delays utilizing EProp / eligibility trace 
//			expWrapper->eprop();
//
//			//Update new found, calculated delays
//			if (update_delays)
//				expWrapper->updateDelays();
//
//			total_loss += expWrapper->getLoss();
//			total_activity += expWrapper->getActivity();
//			total_length += expWrapper->getLength();
//
//			if (verbosity >= 3) emit statusUpdate(QString::asprintf("E-Prop yields a loss of %d.", expWrapper->getLoss()));
//		}
//
//		//Finalize experiment
//		//Run to end of the grid
//		//t = carlsimWrapper->getSnnTimeMs();
//		t = carlsimWrapper->getTimeStep();		//Use internal clock
//		rt = t % timeGrid_ms;
//
//		//carlsimWrapper->setModelSpeed(fast_forward);
//		pauseInterval_ms = 1.0 / fastForward; //fixme
//		stepCarlsim(timeGrid_ms - rt - 1); // run to the end of the grid (with monitoring on)
//
//		//auto pauseInterval_ms = 10000;
//		////emit statusUpdate("wait before grid " + QString::number(pauseInterval_ms/1000.0));
//		////Wait a little, before the Raster vanish  --> para wait grid switch
//		//if (!stopThread)
//		//	msleep(pauseInterval_ms);
//		stepCarlsim(1); // run to the end of the grid (with monitoring on)
//
//
//
//		// log results for session 
//
//
//		// next segment = trial
//		segment_i++; 
//
//		// summarize the results for one trial
//		// each session pertains 4 tials
//		if (segment_i % 4 == 0) {
//		//if (segment_i % 2 == 0) {
//			// report results of current session
//			trainingLog << simulation << " " << session << " "  << total_loss << " " << total_activity << " " << total_length << endl;
//			statusUpdateSession();
//
//			//prepare next session 
//			session++;
//			total_loss = 0;
//			total_activity = 0;
//			total_length = 0;
//
//			// replace unique pointer
//			sessionResults = simulationResults->spawnDir(QString::asprintf("session_%d", session));
//		}
//
//		//Run another simulation with the next 8 sessions 
//		if (session % (8+1) == 0) {
//		//if (session % 2 == 0) {
//			simulation++;
//			//Terminate after 10 simulations or 
//			// if less segments were defined than loops
//			if (simulation > training_loops || segment_i == segments.size()) {
//				emit if (verbosity >= 2) statusUpdate(QString::asprintf("Experiment complete after %d trails", segment_i));
//				break;
//			}
//
//
//			//// Plan B
//			//carlsimWrapper->resetDelays();
//			////set connDelays <<-  from tempWeights   
//			//expWrapper->updateDelays();  // actually, this is part of a the function
//			// elsewise the display in GUI is inconsistent with CARLsim
//
//			// Planc D
//			// Store initial Delays
//			// copy to 2
//			// call Update Delays
//			if (verbosity >= 2) statusUpdate(QString::asprintf("Reset Delays for simulation %d", simulation));
//			carlsimWrapper->resetDelays();
//			expWrapper->connDelays2 = connDelaysInitial;
//			expWrapper->updateDelays();  
//
//			// Plan c
//			// Remote Control Experiment: Auto Load Network, Sim, Expl, Template, Start .. Timeout
//			// Restart for only 1 sim run, redefine seed, and define start stop
//			// 
//			// Collect results afterwards by script and call Matlab
//			// 
//			
////			// Reload CARLsim Simulation -- does not work !!!
////// {
////			//Reset Delays
////			carlsimWrapper->resetDelays();
////			//Re-Load CARLsim : Unload / Load
////			emit carlsimWrapper->monitorStopRecording(0);
////			carlsimWrapper->stopSimulation();
////			carlsimWrapper->unloadSimulation();
////
////			// sync wait max 3s until unloaded
////			for(int timeout = 10; timeout > 0; timeout--) {
////				if (!carlsimWrapper->isSimulationLoaded())
////					break;
////				sleep(1);
////			}
////			carlsimWrapper->loadSimulation();
////			// sync wait max 3s until simulation is loaded
////			for (int timeout = 10; timeout > 0; timeout--) {
////				if (carlsimWrapper->isSimulationLoaded())
////					break;
////				sleep(1);
////			}
////			//emit carlsimWrapper->carlsimConfigStateNetworkBuilt();
////
////
////			//Reset (Simulation)
////			//carlsimWrapper->resetTime();
////
////			//ISSUE might be another thread now
////
////			auto monitor_before = spikeMonitor; 
////			//Request Widget to set the new Monitor in the model, as done before in start		
////			emit simulationReloaded(); 
////
////			//ISSUE: emit does not call update, ... something wrong with the SIGNAL SLOT 
////			sleep(1);
////
////			auto monitor_after = spikeMonitor;
////
////			assert(monitor_before != monitor_after);
////
////			// crash at about 100 ms 
////			// CARLsim still has a NULL pointer at spike_monitor.cpp#192
////			//	return spikeMonitorCorePtr_->isRecording();
////
////// }
//
//
//			// replace unique pointer for simulation
//			simulationResults = phaseResults->spawnDir(QString::asprintf("simulation_%d", simulation));
//
//			session = 1; // round robin
//			total_loss = 0;
//			total_activity = 0;
//
//			// replace unique pointer
//			sessionResults = simulationResults->spawnDir(QString::asprintf("session_%d", session));
//		}
//
//	}
//
//	//Output final result
//	emit if (verbosity >= 1)statusUpdate(QString::asprintf("Experiment complete"));
//	phaseResults->exec("octave --eval eprop_boone_train_convergence");
//}
//
//
//
//


/*! Advances the simulation by the specified number of time steps */
void WmExptManager::stepCarlsim(unsigned numTimeSteps){
	for(unsigned i=0; i<numTimeSteps && !stopThread; ++i){
		carlsimWrapper->stepSimulation();
		msleep(pauseInterval_ms);
		while((carlsimWrapper->isWaitForGraphics() || carlsimWrapper->getCurrentTask() == CarlsimWrapper::STEP_SIMULATION_TASK) && !stopThread)
			msleep(pauseInterval_ms);
	}
	msleep(pauseInterval_ms);  // deactivated as it makes no sense
}


/*! Stores the parameters for the experiment */
void WmExptManager::storeParameters(QHash<QString, double>& parameterMap) {

	//Old imperial code but still checks out
	if (!parameterMap.contains("experiment_number"))
		throw SpikeStreamException("WmExptManager: experiment_number parameter missing");
	experimentNumber = (int)parameterMap["experiment_number"];

	if (!parameterMap.contains("random_seed"))
		throw SpikeStreamException("WmExptManager: random_seed parameter missing");
	randomSeed = (int)parameterMap["random_seed"];

	if (!parameterMap.contains("pause_interval_ms"))
		throw SpikeStreamException("WmExptManager: pause_interval_ms parameter missing");
	pauseInterval_ms = (int)parameterMap["pause_interval_ms"];


	// VTE
	auto validate = [&](const char* param) {
		if (!parameterMap.contains(param))
			throw SpikeStreamException(QString("WmExptManager: ") + param + " parameter missing");
		return param;
	};
	startNeuron = (int)parameterMap[validate("start_neuron")];

	targetNeuron = (int)parameterMap[validate("target_neuron")];
	initialCurrent = parameterMap[validate("initial_current")];
	//recoveryTime_ms = (int)parameterMap[validate("recovery_time_ms")];
	//timeGrid_ms = (int)parameterMap[validate("time_grid_ms")];
	//slowMotion = (double)parameterMap[validate("slow_motion")];
	//fastForward = (double)parameterMap[validate("fast_forward")];
	observationTime_ms = (int)parameterMap[validate("observation_time_ms")];
	//startTime_ms = (int)parameterMap[validate("start_time_ms")];
	endTime_ms = (int)parameterMap[validate("end_time_ms")];
	//update_delays = (bool)parameterMap[validate("update_delays")];
	verbosity = (int)parameterMap[validate("verbosity")];
	training_loops = (int)parameterMap[validate("training_loops")];
	//border = (int)parameterMap[validate("border")];
	//barrier = (int)parameterMap[validate("barrier")];

}




