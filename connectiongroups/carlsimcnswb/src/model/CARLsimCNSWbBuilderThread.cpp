#include "Globals.h"
#include "NeuronGroup.h"
#include "CARLsimCNSWbBuilderThread.h"
#include "SpikeStreamException.h"
#include "Util.h"


#include <cmath>
#include <complex>
#include <iomanip>
#include <iostream>
#include <numbers>
using namespace std::complex_literals;


using namespace spikestream;

/*! Constructor */
CARLsimCNSWbBuilderThread::CARLsimCNSWbBuilderThread() : AbstractConnectionBuilder() {
}


/*! Destructor */
CARLsimCNSWbBuilderThread::~CARLsimCNSWbBuilderThread(){
}


/*! Thread run method */
void CARLsimCNSWbBuilderThread::run() {
	clearError();
	stopThread = false;
	newConnectionGroup = NULL;

	try {
		//Seed the random number generator
		srand(12345678);

		//Create network and archive dao for this thread
		Network* currentNetwork = Globals::getNetwork();
		threadNetworkDao = new NetworkDao(Globals::getNetworkDao()->getDBInfo());

		//Build connection group and add it to network
		buildConnectionGroup();
		if (stopThread)
			return;

		//QList<ConnectionGroup*> conGrpList;  --> moved 
		//conGrpList.append(newConnectionGroup);  --> moved to buildConnectionGroup(s)
		Globals::getNetwork()->addConnectionGroups(conGrpList);

		//Wait for network to finish adding connection groups
		while (currentNetwork->isBusy()) {
			emit progress(threadNetworkDao->getConnectionCount(newConnectionGroup), (int)newConnectionGroup->size(), "Adding connections to database..."); // 0.3
			if (stopThread)
				currentNetwork->cancel();
			msleep(250);
		}

		//Check for errors
		if (currentNetwork->isError())
			setError(currentNetwork->getErrorMessage());

		//Clean up network dao
		delete threadNetworkDao;
	}
	catch (SpikeStreamException& ex) {
		setError(ex.getMessage());
	}
	catch (...) {
		setError("Abstract Connection Builder: An unknown error occurred.");
	}
}

/*----------------------------------------------------------*/
/*-----                PROTECTED METHODS               -----*/
/*----------------------------------------------------------*/


#define FEAT_FULL_CONNECT

/*! Returns a neuron group whose neurons are constructed according to the
	parameters in the neuron group info. */
void CARLsimCNSWbBuilderThread::buildConnectionGroup(){

	

	SynapseType synapseType = threadNetworkDao->getSynapseType("Izhikevich Synapse");
	QHash<QString, double> defaultParameterMaps = threadNetworkDao->getDefaultSynapseParameters(synapseType.getID());
	defaultParameterMaps["Learning"] = 0;

	// Connect tof -> vel 
	{
		connectionGroupInfo.setDescription("tof -> vel");   //  pre,pos name 
		connectionGroupInfo.setFromNeuronGroupID(tofGroup->getID());
		connectionGroupInfo.setToNeuronGroupID(velGroup->getID());		
		newConnectionGroup = new ConnectionGroup(connectionGroupInfo);
		QHash<QString, double> conParamMap = defaultParameterMaps;
		conParamMap["weight_factor"] = tof2vel.w_factor;
		newConnectionGroup->setParameters(conParamMap);

#ifdef FEAT_FULL_CONNECT
		for(int pre=0; pre<2; pre++)
			for (int post = 0; post < 4; post++) {
				newConnectionGroup->addConnection(tof_ids[pre], vel_ids[post], tof2vel.delays, tof2vel.weights / 10.0);
			}
		conParamMap["Learning"] = 1;  //  STDP parameter 
#else
		// Forward at max speed
		newConnectionGroup->addConnection(tof_ids[1], vel_ids[3], tof2vel.delays, tof2vel.weights);
		newConnectionGroup->addConnection(tof_ids[1], vel_ids[1], tof2vel.delays, tof2vel.weights);

		newConnectionGroup->addConnection(tof_ids[0], vel_ids[0], tof2vel.delays, tof2vel.weights);
		newConnectionGroup->addConnection(tof_ids[0], vel_ids[1], tof2vel.delays, tof2vel.weights);
		newConnectionGroup->addConnection(tof_ids[0], vel_ids[2], tof2vel.delays, tof2vel.weights);
		newConnectionGroup->addConnection(tof_ids[0], vel_ids[3], tof2vel.delays, tof2vel.weights);
#endif
		conGrpList.append(newConnectionGroup);
	}

	// Connect ps -> vel 
	{
		connectionGroupInfo.setDescription("ps -> vel");   
		connectionGroupInfo.setFromNeuronGroupID(psGroup->getID());
		connectionGroupInfo.setToNeuronGroupID(velGroup->getID());
		newConnectionGroup = new ConnectionGroup(connectionGroupInfo);
		QHash<QString, double> conParamMap = defaultParameterMaps;
		conParamMap["weight_factor"] = ps2vel.w_factor;
		newConnectionGroup->setParameters(conParamMap);

#ifdef FEAT_FULL_CONNECT
		for (int pre = 0; pre < 8; pre++)
			for (int post = 0; post < 4; post++) {
				newConnectionGroup->addConnection(ps_ids[pre], vel_ids[post], ps2vel.delays, ps2vel.weights / 10.0);
			}
		conParamMap["Learning"] = 1;  //  STDP parameter 
#else

		// ps0 right front 15°  => back  update: neutral
		newConnectionGroup->addConnection(ps_ids[0], vel_ids[0], ps2vel.delays, ps2vel.weights);
		// update
		newConnectionGroup->addConnection(ps_ids[0], vel_ids[3], ps2vel.delays, ps2vel.weights);

		// left front front
		newConnectionGroup->addConnection(ps_ids[7], vel_ids[2], ps2vel.delays, ps2vel.weights);
		// update
		newConnectionGroup->addConnection(ps_ids[7], vel_ids[1], ps2vel.delays, ps2vel.weights);


		// right front 45% only opposite inverse
		newConnectionGroup->addConnection(ps_ids[1], vel_ids[0], ps2vel.delays, ps2vel.weights);
		// update
		newConnectionGroup->addConnection(ps_ids[1], vel_ids[3], ps2vel.delays, ps2vel.weights);

		// left
		newConnectionGroup->addConnection(ps_ids[6], vel_ids[2], ps2vel.delays, ps2vel.weights);
		// update
		newConnectionGroup->addConnection(ps_ids[6], vel_ids[1], ps2vel.delays, ps2vel.weights);



		// right  - 90° away from wall
		newConnectionGroup->addConnection(ps_ids[2], vel_ids[3], ps2vel.delays, ps2vel.weights);
		// left
		newConnectionGroup->addConnection(ps_ids[5], vel_ids[1], ps2vel.delays, ps2vel.weights);


		// right back 120° - curve back to wall
		newConnectionGroup->addConnection(ps_ids[3], vel_ids[1], ps2vel.delays, ps2vel.weights);
		// left back
		newConnectionGroup->addConnection(ps_ids[4], vel_ids[3], ps2vel.delays, ps2vel.weights);
#endif

		conGrpList.append(newConnectionGroup);
	}


	// Connect tof -> vel 
	{
		connectionGroupInfo.setDescription("mot -> vel");   //  pre,pos name 
		connectionGroupInfo.setFromNeuronGroupID(motGroup->getID());
		connectionGroupInfo.setToNeuronGroupID(velGroup->getID());
		newConnectionGroup = new ConnectionGroup(connectionGroupInfo);
		QHash<QString, double> conParamMap = defaultParameterMaps;
		conParamMap["weight_factor"] = mot2vel.w_factor;
		newConnectionGroup->setParameters(conParamMap);

		//// 1:1 variant with 4 neurons like VEL
		//for(int i=0; i<4; i++)
		//	newConnectionGroup->addConnection(mot_ids[i], vel_ids[i], mot2vel.delays, mot2vel.weights);

		//// 6 neuron variant with 8 connections
		//// left
		newConnectionGroup->addConnection(mot_ids[4], vel_ids[1], mot2vel.delays, mot2vel.weights);  // forward
		newConnectionGroup->addConnection(mot_ids[2], vel_ids[1], mot2vel.delays, mot2vel.weights);
		newConnectionGroup->addConnection(mot_ids[2], vel_ids[0], mot2vel.delays, mot2vel.weights);
		newConnectionGroup->addConnection(mot_ids[0], vel_ids[0], mot2vel.delays, mot2vel.weights);  // backward
		//right
		newConnectionGroup->addConnection(mot_ids[5], vel_ids[3], mot2vel.delays, mot2vel.weights);  // forward
		newConnectionGroup->addConnection(mot_ids[3], vel_ids[3], mot2vel.delays, mot2vel.weights);
		newConnectionGroup->addConnection(mot_ids[3], vel_ids[2], mot2vel.delays, mot2vel.weights);
		newConnectionGroup->addConnection(mot_ids[1], vel_ids[2], mot2vel.delays, mot2vel.weights);  // backwward


		conGrpList.append(newConnectionGroup);
	}

}

/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Extracts parameters from neuron group info and checks that they are in range. */
void CARLsimCNSWbBuilderThread::checkParameters() {


	// references to neuron groups
	QHash<QString, NeuronGroup*>& groupsMap = Globals::getNetwork()->getNeuronGroupsMap(); // build only once


	tofGroup = groupsMap["ctx.tof"];
	psGroup = groupsMap["ctx.ps"];

	velGroup = groupsMap["ctx.vel"];

	motGroup = groupsMap["ctx.mot"];

	assert(tofGroup);
	assert(psGroup);

	assert(velGroup);

	assert(motGroup);

	// ps, ls  --> Data 
	//vector<int> perm_dst = { 4, 6, 7, 5, 3, 1, 0, 2 };

	// tof 
	vector<int> perm_tof = { 0, 1 };

	// vel
	vector<int> perm_vel = { 0, 2, 1, 3 };

	// mot
	//vector<int> perm_mot = { 0, 2, 4, 1, 3, 5};
	vector<int> perm_mot = { 0, 1, 2, 3, 4, 5 };

	// get ids from dlPFC
	auto getIds = [&](NeuronGroup* neurGrp, QVector<unsigned int>& ids) {
		ids.resize(neurGrp->size());
		unsigned int start_id = neurGrp->getStartNeuronID();
		//auto xStart = neurGrp->getBoundingBox().getX1(); 
		//auto yStart = neurGrp->getBoundingBox().getY1();
		//auto zStart = neurGrp->getBoundingBox().getZ1();
		for (NeuronMap::iterator iter = neurGrp->begin(); iter != neurGrp->end(); iter++) {   // && !stopThread
			unsigned int id = iter.key();
			Neuron* neur = iter.value(); 
			auto i  = id - start_id;  // make relative id 
			//printf("%i  %d \n", i, id);

			//if (psGroup == neurGrp)  // || lsGroup 
			//	i = perm_dst[i]; // map index to ring topology
			//else
			if (neurGrp == tofGroup)
				i = perm_tof[i]; // map index to z 
			else
			if(neurGrp == velGroup)
				i = perm_vel[i]; // map index to z 
			else
			if (neurGrp == motGroup)
				i = perm_mot[i]; // map index to z 

			ids[i] = iter.key(); 
 
		}
	};

	// get ids from dlPFC
	auto getIdsPos = [&](NeuronGroup* neurGrp, QVector<unsigned int>& ids) {
		ids.resize(neurGrp->size());
		unsigned int start_id = neurGrp->getStartNeuronID();
		auto xStart = neurGrp->getBoundingBox().getX1(); 
		auto yStart = neurGrp->getBoundingBox().getY1();
		auto zStart = neurGrp->getBoundingBox().getZ1();

		vector<tuple<float, float>> ps = { {0.0, 5.25}, {1.25,7.25}, {4.0,8.0}, {7.25,6.0},  {7.25, 2.0}, {4.0, 0.0}, {1.25, 0.75}, {0.0, 2.75} };

		int i = 0; 
		for (auto iter = ps.begin(); iter < ps.end(); iter++) {
			Point3D location(xStart + get<0>(*iter), yStart + get<1>(*iter), zStart);
			unsigned int id = neurGrp->getNeuronIDAtLocation(location); 
			ids[i++] = id;
		}
	};
	

	getIds(tofGroup, tof_ids);

	getIdsPos(psGroup, ps_ids);

	getIds(velGroup, vel_ids);

	getIds(motGroup, mot_ids);

}
