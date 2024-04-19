#ifndef CARLSIMSPIKEGENERATORCONTAINER_H
#define CARLSIMSPIKEGENERATORCONTAINER_H

//SpikeStream includes


// 
#include "api.h" 

class SpikeGenerator;
class CarlsimSpikeGenerator;

namespace spikestream {

	class NeuronGroup;
	class CarlsimWrapper; 

	class CarlsimSpikeGeneratorContainer {


	public:

		CarlsimSpikeGeneratorContainer(NeuronGroup* group, CarlsimWrapper* wrapper);
		~CarlsimSpikeGeneratorContainer();

		SpikeGenerator* getGenerator();


	private: 
		NeuronGroup* group; 
		CarlsimSpikeGenerator* generator; // a subclass of CARLsim::ConnectionGenerator (implementing the interface)
		CarlsimWrapper* wrapper;
	};

}

#endif//CARLSIMSPIKEGENERATORCONTAINER_H

