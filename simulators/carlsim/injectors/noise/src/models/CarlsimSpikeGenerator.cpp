
#include "CarlsimSpikeGenerator.h"

#include "CarlsimWrapper.h"
#include "NeuronGroup.h"
#include "NoiseInjectorModel.h"

#include "carlsim.h"


CarlsimSpikeGenerator::CarlsimSpikeGenerator(spikestream::carlsim_injectors::NoiseInjectorModel* model, int index)
	: model(model), index(index) {		
}

CarlsimSpikeGenerator::~CarlsimSpikeGenerator() {
	wrapper = NULL; 
	model = NULL; 
}


void CarlsimSpikeGenerator::setWrapper(spikestream::CarlsimWrapper* wrapper_) {
	wrapper = wrapper_;

	NeuronGroup* group = model->neurGrpList[index];
	int vid = group->getVID();
	wrapper->carlsim->setSpikeGenerator(vid, this); 
}


int CarlsimSpikeGenerator::nextSpikeTime(CARLsim* s, int grpId, int nid, 
		int currentTime, int lastScheduledSpikeTime, int endOfTimeSlice) {

	auto &spikeVector = model->spikeVectorList[index];
	// assert(grpId, model->neurGrpList[index]->getVID()); 
    
	int r = -1; //0xFFFFFFFF   break 

	if(currentTime>0) {
		if(spikeVector[nid]) {
			r = currentTime; 
			spikeVector[nid] = false; // consumed
			// assert(!model->spikeVectorList[index]); // ensure referce
		}
	}

	//if #DEBUG_CALLBACK 
	//printf("grp:%d n:%d current: %d last: %d eoTS: %d  return: %d \n", grpId, nid, currentTime, lastScheduledSpikeTime, endOfTimeSlice, r);

	return r;

} 

