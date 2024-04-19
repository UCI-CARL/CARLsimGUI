

//#include "ConnectionGroup.h"

#include "CarlsimSpikeGeneratorContainer.h"

#include "CarlsimSpikeGenerator.h"



//using namespace spikestream

spikestream::CarlsimSpikeGeneratorContainer::CarlsimSpikeGeneratorContainer(carlsim_injectors::NoiseInjectorModel* model, int index, CarlsimWrapper* wrapper) 
	: model(model), index(index), wrapper(wrapper)
{
	generator = new CarlsimSpikeGenerator(model, index);
	generator->setWrapper(wrapper);
}

spikestream::CarlsimSpikeGeneratorContainer::~CarlsimSpikeGeneratorContainer() {
	delete generator; 
}


SpikeGenerator* spikestream::CarlsimSpikeGeneratorContainer::getGenerator() {
	return generator; 
}


