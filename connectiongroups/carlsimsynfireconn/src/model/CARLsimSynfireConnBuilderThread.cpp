#include "Globals.h"
#include "NeuronGroup.h"
#include "CARLsimSynfireConnBuilderThread.h"
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
CARLsimSynfireConnBuilderThread::CARLsimSynfireConnBuilderThread() : AbstractConnectionBuilder() {
}


/*! Destructor */
CARLsimSynfireConnBuilderThread::~CARLsimSynfireConnBuilderThread(){
}


/*! Thread run method */
void CARLsimSynfireConnBuilderThread::run() {
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
void CARLsimSynfireConnBuilderThread::buildConnectionGroup(){

	

	SynapseType synapseType = threadNetworkDao->getSynapseType("Izhikevich Synapse");
	QHash<QString, double> defaultParameterMaps = threadNetworkDao->getDefaultSynapseParameters(synapseType.getID());
	defaultParameterMaps["Learning"] = 0;

	// TODO iterate over segements
	// TODO special case stim

	auto prefix = "G"; // TODO from param file
	auto segments = 10; // TODO from param file
	auto exc_syn_per_neuron = 60; 
	//auto exc_syn_per_neuron = 5;
	auto n = 100; 


	for (int i_segment = 0; i_segment < segments; i_segment++) {

		// Connect exc[i-1] -> exc[i]
		{
			auto pre = i_segment == 0 ? stimGroup : excGroups[i_segment - 1];
			auto post = excGroups[i_segment];
			QString desc = QString("exc[%1] -> exc[%2] excitation").arg(i_segment - 1).arg(i_segment);
			connectionGroupInfo.setDescription(desc);   //  pre,pos name 
			connectionGroupInfo.setFromNeuronGroupID(pre->getID());
			connectionGroupInfo.setToNeuronGroupID(post->getID());
			newConnectionGroup = new ConnectionGroup(connectionGroupInfo);
			QHash<QString, double> conParamMap = defaultParameterMaps;
			conParamMap["weight_factor"] = exc2exc.w_factor;
			newConnectionGroup->setParameters(conParamMap);

			// random fan-in connections 
			QVector<unsigned int> & preIds = i_segment == 0 ? stimNeuronIds : excNeuronIds[i_segment - 1];
			QVector<unsigned int>& postIds = excNeuronIds[i_segment];
			for (int i = 0; i < post->size(); i++) {
				unsigned int post_id = postIds[i]; 
				for (int j = 0; j < exc_syn_per_neuron; j++) {
					auto k = Util::getRandomUInt(0, preIds.size()-1);
					unsigned int pre_id = preIds[k];
					newConnectionGroup->addConnection(pre_id, post_id, exc2exc.delays, exc2exc.weights);
				}
			}

			conGrpList.append(newConnectionGroup);
		}


		// Connect exc[i-1] -> inh[i]
		{
			auto pre = i_segment == 0 ? stimGroup : excGroups[i_segment - 1];
			auto post = inhGroups[i_segment];
			QString desc = QString("exc[%1] -> inh[%2] excitation").arg(i_segment - 1).arg(i_segment);
			connectionGroupInfo.setDescription(desc); 
			connectionGroupInfo.setFromNeuronGroupID(pre->getID());
			connectionGroupInfo.setToNeuronGroupID(post->getID());
			newConnectionGroup = new ConnectionGroup(connectionGroupInfo);
			QHash<QString, double> conParamMap = defaultParameterMaps;
			conParamMap["weight_factor"] = exc2inh.w_factor;
			newConnectionGroup->setParameters(conParamMap);

			// random fan-in connections 
			QVector<unsigned int>& preIds = i_segment == 0 ? stimNeuronIds : excNeuronIds[i_segment - 1];
			QVector<unsigned int>& postIds =inhNeuronIds[i_segment];
			for (int i = 0; i < post->size(); i++) {
				unsigned int post_id = postIds[i];
				for (int j = 0; j < exc_syn_per_neuron; j++) {
					auto k = Util::getRandomUInt(0, preIds.size() - 1);
					unsigned int pre_id = preIds[k];
					newConnectionGroup->addConnection(pre_id, post_id, exc2inh.delays, exc2inh.weights);
				}
			}

			conGrpList.append(newConnectionGroup);
		}



		// Connect inh[i] -> exc[i]
		{	
			auto pre = inhGroups[i_segment];
			auto post = excGroups[i_segment];
			QString desc = QString("inh[%1] -> exc[%2] inhibition").arg(i_segment).arg(i_segment);
			connectionGroupInfo.setDescription(desc);
			connectionGroupInfo.setFromNeuronGroupID(pre->getID());
			connectionGroupInfo.setToNeuronGroupID(post->getID());
			newConnectionGroup = new ConnectionGroup(connectionGroupInfo);
			QHash<QString, double> conParamMap = defaultParameterMaps;
			conParamMap["weight_factor"] = inh2exc.w_factor;
			newConnectionGroup->setParameters(conParamMap);

			// random fan-in connections 
			QVector<unsigned int>& preIds = inhNeuronIds[i_segment];
			QVector<unsigned int>& postIds = excNeuronIds[i_segment];
			for (int i = 0; i < post->size(); i++) {
				unsigned int post_id = postIds[i];
				for (int j = 0; j < exc_syn_per_neuron; j++) {
					auto k = Util::getRandomUInt(0, preIds.size() - 1);
					unsigned int pre_id = preIds[k];
					newConnectionGroup->addConnection(pre_id, post_id, inh2exc.delays, inh2exc.weights);
				}
			}

			conGrpList.append(newConnectionGroup);
		}

	}

}




/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Extracts parameters from neuron group info and checks that they are in range. */
void CARLsimSynfireConnBuilderThread::checkParameters() {


	// references to neuron groups
	QHash<QString, NeuronGroup*>& groupsMap = Globals::getNetwork()->getNeuronGroupsMap(); // build only once



	auto prefix = "G"; // TODO from param file
	auto segments = 10; // TODO from param file

	{
		QString name = QString("%1%2").arg(prefix).arg("stim");
		stimGroup = groupsMap[name];
		assert(stimGroup);
		stimNeuronIds.reserve(stimGroup->size());
		for (NeuronMap::iterator iter = stimGroup->begin(); iter != stimGroup->end(); iter++) { 
			unsigned int id = iter.key();
			stimNeuronIds.append(id);
		}
	}

	for (int i_segment = 0; i_segment < segments; i_segment++) {
		QString name = QString("%1%2%3").arg(prefix).arg("exc").arg(i_segment);
		auto group = groupsMap[name]; 
		assert(group);
		excGroups.append(group);
		QVector<unsigned int> ids; // reserve capa
		ids.reserve(group->size());
		for (NeuronMap::iterator iter = group->begin(); iter != group->end(); iter++) {  
			unsigned int id = iter.key();
			ids.append(id);
		}
		excNeuronIds.append(ids); 
	}

	for (int i_segment = 0; i_segment < segments; i_segment++) {
		QString name = QString("%1%2%3").arg(prefix).arg("inh").arg(i_segment);
		auto group = groupsMap[name];
		assert(group);
		inhGroups.append(group);
		QVector<unsigned int> ids; // reserve capa
		ids.reserve(group->size());
		for (NeuronMap::iterator iter = group->begin(); iter != group->end(); iter++) {
			unsigned int id = iter.key();
			ids.append(id);
		}
		inhNeuronIds.append(ids);
	}

	

}
