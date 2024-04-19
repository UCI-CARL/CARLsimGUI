
#include "CarlsimSpikeGenerator.h"

#include "CarlsimWrapper.h"
#include "NeuronGroup.h"

#include "carlsim.h"


CarlsimSpikeGenerator::CarlsimSpikeGenerator(NeuronGroup* group_) {		
	group = group_;

	group->initializeFiring(); 
}

CarlsimSpikeGenerator::~CarlsimSpikeGenerator() {
	wrapper = NULL; 
	group = NULL; 
}


void CarlsimSpikeGenerator::setWrapper(spikestream::CarlsimWrapper* wrapper_) {
	wrapper = wrapper_;

}


int CarlsimSpikeGenerator::nextSpikeTime(CARLsim* s, int grpId, int nid, 
		int currentTime, int lastScheduledSpikeTime, int endOfTimeSlice) {

	int r = -1; //0xFFFFFFFF   break 


	auto &firing = group->getFiring(); 

	assert(nid < firing.size());  // exception 

	if (firing[nid]) {
		r = currentTime;
		firing[nid] = false;
	}

	return r;

} 

