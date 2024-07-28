#include "Globals.h"
#include "NeuronGroup.h"
#include "CARLsimCNSYoloBuilderThread.h"
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
CARLsimCNSYoloBuilderThread::CARLsimCNSYoloBuilderThread() : AbstractConnectionBuilder() {
}


/*! Destructor */
CARLsimCNSYoloBuilderThread::~CARLsimCNSYoloBuilderThread(){
}


/*! Thread run method */
void CARLsimCNSYoloBuilderThread::run() {
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

/*! Returns a neuron group whose neurons are constructed according to the
	parameters in the neuron group info. */
void CARLsimCNSYoloBuilderThread::buildConnectionGroup(){

	

	SynapseType synapseType = threadNetworkDao->getSynapseType("Izhikevich Synapse");
	QHash<QString, double> defaultParameterMaps = threadNetworkDao->getDefaultSynapseParameters(synapseType.getID());
	defaultParameterMaps["Learning"] = 0;

	// Connect dist -> vel 
	{
		connectionGroupInfo.setDescription("dist -> vel");   //  pre,pos name 
		connectionGroupInfo.setFromNeuronGroupID(yoloGroup->getID());
		connectionGroupInfo.setToNeuronGroupID(velGroup->getID());		
		newConnectionGroup = new ConnectionGroup(connectionGroupInfo);
		QHash<QString, double> conParamMap = defaultParameterMaps;
		conParamMap["weight_factor"] = dist2vel.w_factor;
		newConnectionGroup->setParameters(conParamMap);

		// Forward at max speed
		const int yolo_dist_id_0 = yolo_ids[4];
		const int yolo_dist_id_1 = yolo_ids[5];
		//newConnectionGroup->addConnection(yolo_dist_id_1, vel_ids[3], dist2vel.delays, dist2vel.weights);
		//newConnectionGroup->addConnection(yolo_dist_id_1, vel_ids[1], dist2vel.delays, dist2vel.weights);

		//// Stop (Reverse)
		newConnectionGroup->addConnection(yolo_dist_id_0, vel_ids[0], dist2vel.delays, dist2vel.weights);
		newConnectionGroup->addConnection(yolo_dist_id_0, vel_ids[1], dist2vel.delays, dist2vel.weights);
		newConnectionGroup->addConnection(yolo_dist_id_0, vel_ids[2], dist2vel.delays, dist2vel.weights);
		newConnectionGroup->addConnection(yolo_dist_id_0, vel_ids[3], dist2vel.delays, dist2vel.weights);

		conGrpList.append(newConnectionGroup);

	}

	// Connect dir -> vel 
	{
		connectionGroupInfo.setDescription("dir -> vel");   
		connectionGroupInfo.setFromNeuronGroupID(yoloGroup->getID());
		connectionGroupInfo.setToNeuronGroupID(velGroup->getID());
		newConnectionGroup = new ConnectionGroup(connectionGroupInfo);
		QHash<QString, double> conParamMap = defaultParameterMaps;
		conParamMap["weight_factor"] = dir2vel.w_factor;
		newConnectionGroup->setParameters(conParamMap);

		const int yolo_dir_id_0 = yolo_ids[2];
		const int yolo_dir_id_1 = yolo_ids[3];


		// < .5 => head left   got easily stuck at a wall, as it is only forward
		newConnectionGroup->addConnection(yolo_dir_id_0, vel_ids[3], dir2vel.delays, dir2vel.weights);
		// > .5 => head right
		newConnectionGroup->addConnection(yolo_dir_id_1, vel_ids[1], dir2vel.delays, dir2vel.weights);


		//// < .5 => 0.3 turn 45° - x left  
		//newConnectionGroup->addConnection(yolo_dir_id_0, vel_ids[0], dir2vel.delays, dir2vel.weights*0.9);
		//// update
		//newConnectionGroup->addConnection(yolo_dir_id_0, vel_ids[3], dir2vel.delays, dir2vel.weights);

		//// > .5 => 0.7 turn 45° - x right
		//newConnectionGroup->addConnection(yolo_dir_id_1, vel_ids[2], dir2vel.delays, dir2vel.weights*0.9);
		//// update
		//newConnectionGroup->addConnection(yolo_dir_id_1, vel_ids[1], dir2vel.delays, dir2vel.weights);



		//// right front 45% only opposite inverse
		//newConnectionGroup->addConnection(yolo_ids[1], vel_ids[0], dir2vel.delays, dir2vel.weights);
		//// update
		//newConnectionGroup->addConnection(yolo_ids[1], vel_ids[3], dir2vel.delays, dir2vel.weights);

		//// left
		//newConnectionGroup->addConnection(yolo_ids[6], vel_ids[2], dir2vel.delays, dir2vel.weights);
		//// update
		//newConnectionGroup->addConnection(yolo_ids[6], vel_ids[1], dir2vel.delays, dir2vel.weights);


		//// right  - 90° away from wall
		//newConnectionGroup->addConnection(yolo_dir_id_0, vel_ids[3], dir2vel.delays, dir2vel.weights);
		//// left
		//newConnectionGroup->addConnection(yolo_dir_id_1, vel_ids[1], dir2vel.delays, dir2vel.weights);


		//// right back 120° - curve back to wall
		//newConnectionGroup->addConnection(yolo_ids[3], vel_ids[1], dir2vel.delays, dir2vel.weights);
		//// left back
		//newConnectionGroup->addConnection(yolo_ids[4], vel_ids[3], dir2vel.delays, dir2vel.weights);


		conGrpList.append(newConnectionGroup);
	}




	//Connect prob -> obj -> vel 
	const int yolo_prob_id_1 = yolo_ids[1];
	const int obj_id_0 = obj_ids[0];

	//Connect prob -> obj 
	{
		//excites the inhibitory neuron
		connectionGroupInfo.setDescription("prob -> obj");
		connectionGroupInfo.setFromNeuronGroupID(yoloGroup->getID());
		connectionGroupInfo.setToNeuronGroupID(objGroup->getID());
		newConnectionGroup = new ConnectionGroup(connectionGroupInfo);
		QHash<QString, double> conParamMap = defaultParameterMaps;
		conParamMap["weight_factor"] = prob2obj.w_factor;
		newConnectionGroup->setParameters(conParamMap);

		newConnectionGroup->addConnection(yolo_prob_id_1, obj_id_0, prob2obj.delays, prob2obj.weights);

		conGrpList.append(newConnectionGroup);
	}

	//Connect obj -> tof 
	{
		//inhibitory neuron inhibits tof
		connectionGroupInfo.setDescription("obj -> tof");
		connectionGroupInfo.setFromNeuronGroupID(objGroup->getID());
		connectionGroupInfo.setToNeuronGroupID(tofGroup->getID());
		newConnectionGroup = new ConnectionGroup(connectionGroupInfo);
		QHash<QString, double> conParamMap = defaultParameterMaps;
		conParamMap["weight_factor"] = obj2tof.w_factor;
		newConnectionGroup->setParameters(conParamMap);

		newConnectionGroup->addConnection(obj_id_0, tof_ids[1], obj2tof.delays, obj2tof.weights);

		conGrpList.append(newConnectionGroup);
	}



}




/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Extracts parameters from neuron group info and checks that they are in range. */
void CARLsimCNSYoloBuilderThread::checkParameters() {


	// references to neuron groups
	QHash<QString, NeuronGroup*>& groupsMap = Globals::getNetwork()->getNeuronGroupsMap(); // build only once


	yoloGroup = groupsMap["yolo"];

	objGroup = groupsMap["obj"];

	velGroup = groupsMap["ctx.vel"];

	tofGroup = groupsMap["ctx.tof"];

	assert(yoloGroup);

	assert(velGroup);

	assert(objGroup);

	assert(tofGroup);

	//// ps, ls  --> Data 
	////vector<int> perm_dst = { 4, 6, 7, 5, 3, 1, 0, 2 };

	// yolo
	vector<int> perm_yolo = { 0, 1, 2, 3, 4, 5, 6, 7};	
	//vector<int> perm_yolo = { 4, 6, 7, 5, 3, 1, 0, 2 };
	//vector<int> perm_yolo = { 0, 2, 4, 6, 1, 3, 5, 7};

	// vel
	vector<int> perm_vel = { 0, 2, 1, 3 };

	// obj
	vector<int> perm_obj = { 0 };

	// tof 
	vector<int> perm_tof = { 0, 1 };


	// get ids from dlPFC
	auto getIds = [&](NeuronGroup* neurGrp, QVector<unsigned int>& ids) {
		ids.resize(neurGrp->size());
		unsigned int start_id = neurGrp->getStartNeuronID();
		auto xStart = neurGrp->getBoundingBox().getX1(); 
		auto yStart = neurGrp->getBoundingBox().getY1();
		auto zStart = neurGrp->getBoundingBox().getZ1();
		for (NeuronMap::iterator iter = neurGrp->begin(); iter != neurGrp->end(); iter++) {   // && !stopThread
			unsigned int id = iter.key();
			Neuron* neur = iter.value(); 
			auto i  = id - start_id;  // make relative id 
			//printf("%i  %d \n", i, id);

			//if (psGroup == neurGrp)  // || lsGroup 
			//	i = perm_dst[i]; // map index to ring topology
			//else
			if (neurGrp == yoloGroup)
				i = perm_yolo[i]; // map index to z 
			else
			if (neurGrp == velGroup)
				i = perm_vel[i]; // map index to z 
			else
			if (neurGrp == objGroup)
				i = perm_obj[i]; // map index to z 
			else
			if (neurGrp == tofGroup)
				i = perm_tof[i]; // map index to z 

			ids[i] = iter.key(); 
 
		}
	};


	getIds(yoloGroup, yolo_ids);
	
	getIds(velGroup, vel_ids);

	getIds(objGroup, obj_ids);

	getIds(tofGroup, tof_ids);

}
