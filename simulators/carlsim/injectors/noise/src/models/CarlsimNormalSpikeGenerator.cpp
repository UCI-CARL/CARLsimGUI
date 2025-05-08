
#include "CarlsimNormalSpikeGenerator.h"

#include "CarlsimWrapper.h"
#include "NeuronGroup.h"
#include "NoiseInjectorModel.h"

#include "carlsim.h"  // CAUTION: from install dir 


CarlsimNormalSpikeGenerator::CarlsimNormalSpikeGenerator(spikestream::carlsim_injectors::NoiseInjectorModel* model, int index, 
	double mean, double sd, int events, int period)
	: CarlsimAbstractSpikeGenerator(model, index), NormalSpikeGenerator(mean, sd, events, period) {
//	: NormalSpikeGenerator(50, 0.8, 200), model(model), index(index) {  // travel
//	: NormalSpikeGenerator(50, 1.6, 200), model(model), index(index) {  // travel 
//	: NormalSpikeGenerator(50, 2.8, 400), model(model), index(index) {  // stops
//	: NormalSpikeGenerator(50, 3.4, 600), model(model), index(index) {
//	: NormalSpikeGenerator(10, 2.8, 100), model(model), index(index) {
//	: NormalSpikeGenerator(10, 2.8, 20), model(model), index(index) {

	// model at Index -> super parameter for mean, sd, spikes
	//auto param = model->at(index);


}

CarlsimNormalSpikeGenerator::~CarlsimNormalSpikeGenerator() {
	wrapper = NULL; 
	model = NULL; 
}


void CarlsimNormalSpikeGenerator::setWrapper(spikestream::CarlsimWrapper* wrapper_) {
	wrapper = wrapper_;

	NeuronGroup* group = model->neurGrpList[index];
	int vid = group->getVID();
	wrapper->carlsim->setSpikeGenerator(vid, this);
}

