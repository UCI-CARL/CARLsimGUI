

#include "CarlsimGeneratorContainer.h"

#include "CarlsimGenerator.h"



//using namespace spikestream

spikestream::CarlsimGeneratorContainer::CarlsimGeneratorContainer(ConnectionGroup* group_, CarlsimWrapper* wrapper_, float weightFactor) 
	: group(group_), wrapper(wrapper_)
{
	generator = new CarlsimGenerator(group);
	generator->setWrapper(wrapper);
	generator->setWeightFactor(weightFactor); 
}

spikestream::CarlsimGeneratorContainer::~CarlsimGeneratorContainer() {
	delete generator; 
}


CarlsimGenerator* spikestream::CarlsimGeneratorContainer::getGenerator() {
	return generator; 
}


