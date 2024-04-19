

//#include "ConnectionGroup.h"

#include "CarlsimSpikeGeneratorContainer.h"

#include "CarlsimSpikeGenerator.h"



//using namespace spikestream

spikestream::CarlsimSpikeGeneratorContainer::CarlsimSpikeGeneratorContainer(NeuronGroup* group_, CarlsimWrapper* wrapper_) 
	: group(group_), wrapper(wrapper_)
{
	generator = new CarlsimSpikeGenerator(group);
	generator->setWrapper(wrapper);
}

spikestream::CarlsimSpikeGeneratorContainer::~CarlsimSpikeGeneratorContainer() {
	delete generator; 
}


SpikeGenerator* spikestream::CarlsimSpikeGeneratorContainer::getGenerator() {
	return generator; 
}


