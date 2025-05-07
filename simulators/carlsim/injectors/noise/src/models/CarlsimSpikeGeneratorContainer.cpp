

//#include "ConnectionGroup.h"

#include "CarlsimSpikeGeneratorContainer.h"

#include "CarlsimSpikeGenerator.h"
#include "CarlsimNormalSpikeGenerator.h"

#include "NoiseInjectorModel.h"

//using namespace spikestream

spikestream::CarlsimSpikeGeneratorContainer::CarlsimSpikeGeneratorContainer(carlsim_injectors::NoiseInjectorModel* model, int index, CarlsimWrapper* wrapper) 
	: model(model), index(index), wrapper(wrapper)
{
	auto type = model->typeList[index];
	if(type == carlsim_injectors::NoiseInjectorModel::FIRE) {
		generator = new CarlsimSpikeGenerator(model, index);
	} else 
	if (type == carlsim_injectors::NoiseInjectorModel::NORMAL) {
		double mean = model->meanList.at(index);  // repetitive
		double sd = model->percentageList.at(index);
		int events = (int)model->currentList.at(index);
		generator = new CarlsimNormalSpikeGenerator(model, index, mean, sd, events);
	}
	else {
		// throw
		generator = nullptr;
	}
	
	generator->setWrapper(wrapper);
}

spikestream::CarlsimSpikeGeneratorContainer::~CarlsimSpikeGeneratorContainer() {
	delete generator; 
}


SpikeGenerator* spikestream::CarlsimSpikeGeneratorContainer::getGenerator() {
	return (SpikeGenerator * ) generator;
}


