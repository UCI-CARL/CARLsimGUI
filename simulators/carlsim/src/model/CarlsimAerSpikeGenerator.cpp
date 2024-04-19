
#include "CarlsimAerSpikeGenerator.h"

#include "CarlsimWrapper.h"
#include "NeuronGroup.h"

#include "carlsim.h"


CarlsimAerSpikeGenerator::CarlsimAerSpikeGenerator(NeuronGroup* group_) {		
	group = group_;
}

CarlsimAerSpikeGenerator::~CarlsimAerSpikeGenerator() {
	wrapper = NULL; 
	group = NULL; 
}


void CarlsimAerSpikeGenerator::setWrapper(spikestream::CarlsimWrapper* wrapper_) {
	wrapper = wrapper_;

}


int CarlsimAerSpikeGenerator::nextSpikeTime(CARLsim* s, int grpId, int nid, 
		int currentTime, int lastScheduledSpikeTime, int endOfTimeSlice) {

	int r = -1; //0xFFFFFFFF  

	if(currentTime>0 && currentTime < 20) {			// loops at currentTime = 0
		if( nid % 2 == 0 && currentTime % 2 == 1)   // neuron id is even, time is odd 
			r = currentTime; 
		else
		if( nid % 2 == 1 && currentTime % 2 == 0)   // neuron id is odd, time is even 
			r = currentTime; 
	}

	//printf("grp:%d n:%d current: %d last: %d eoTS: %d  return: %d \n", grpId, nid, currentTime, lastScheduledSpikeTime, endOfTimeSlice, r);

	return r;

} 

