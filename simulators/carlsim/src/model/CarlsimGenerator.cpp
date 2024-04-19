
#include "CarlsimGenerator.h"

#include "CarlsimWrapper.h"
#include "ConnectionGroup.h"

#include "carlsim.h"

#include <bitset>
#include <vector>
#include <cassert>
#include <tuple>

//#define DEBUG_CONGRP

CarlsimGenerator::CarlsimGenerator(ConnectionGroup* group): 
	group(group), weightFactor(1.0f) {		
}

CarlsimGenerator::~CarlsimGenerator() {
	wrapper = NULL; 
	group = NULL; 
}

void CarlsimGenerator::setWrapper(spikestream::CarlsimWrapper* wrapper_) {
	wrapper = wrapper_;

	NeuronGroup* from = wrapper->persistentNeurGrpMap[group->getFromNeuronGroupID()]; 
	NeuronGroup* to = wrapper->persistentNeurGrpMap[group->getToNeuronGroupID()]; 

#ifdef DEBUG_CONGRP
	printf("Connection Group: %s (%u)\n", 
		group->getInfo().getDescription().toStdString().c_str(), group->getID()); 
	printf("Pre-Neuron-Grp: %s (%u),  Post-Neuron-Grp: %s (%u)\n", 
		from->getInfo().getName().toStdString().c_str(), from->getID(), 
		to->getInfo().getName().toStdString().c_str(), to->getID()); 
#endif
	nfrom = from->size();
	nto = to->size();
	connections.resize(nfrom*nto); 

	auto fromStart = from->getStartNeuronID(); 
	auto toStart = to->getStartNeuronID();

	for(auto iter=group->begin(); iter<group->end(); iter++) {			
		unsigned pre = iter->getFromNeuronID()-fromStart; 
		unsigned post = iter->getToNeuronID()-toStart; 
		float w = iter->getWeight();  
		float d = iter->getDelay(); 
#ifdef DEBUG_CONGRP
		printf("pre[%u](%u) --> post[%u](%u)  w:%f  u:%f \n", 
			pre, pre+fromStart, post, post+toStart, w, d); 
#endif
		connections[idx(pre,post)] = true; 
		synapses[QPair<unsigned,unsigned>(pre,post)] = QPair<float,float>(w,d);

	}
}

int CarlsimGenerator::idx(unsigned pre, unsigned post) {

	assert(pre >= 0 && pre < nfrom);
	assert(post >= 0 && post < nto);
	return pre*nto +post; 
}

bool CarlsimGenerator::isConnected(unsigned pre, unsigned post) {
	return connections[idx(pre,post)]; 
}




void CarlsimGenerator::connect(CARLsim* sim, int srcGrp, int i, int destGrp, 
			   int j, float& weight, float& maxWt,
			   float& delay, bool& connected)
{
	connected = isConnected(i, j);

	if(connected) {
		auto synapse = synapses[QPair<unsigned,unsigned>(i,j)]; 
		weight = synapse.first * weightFactor; 
		delay = synapse.second;
#ifdef DEBUG_CONGRP
		printf("srcGrp[%d].pre[%d] --> destGrp[%d].post[%d] w:%f  d:%f \n", 
			srcGrp, i, destGrp, j, weight, delay); 
#endif
	} else {
		weight = .0f;
		delay = 0.f;
	}

	maxWt = 4.f * weightFactor; // get from Connection Group parameter and store in class
}