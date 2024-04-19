#ifndef CARLSIMSPIKEGENERATORCONTAINER_H
#define CARLSIMSPIKEGENERATORCONTAINER_H

//SpikeStream includes


// Spikestream 0.3
#include "api.h" 

class SpikeGenerator;
class CarlsimSpikeGenerator;

namespace spikestream {
	namespace carlsim_injectors {
		class NoiseInjectorModel;
	}
	class CarlsimWrapper; 

	class CarlsimSpikeGeneratorContainer {


	public:

		CarlsimSpikeGeneratorContainer(carlsim_injectors::NoiseInjectorModel* model, int index, CarlsimWrapper* wrapper);
		~CarlsimSpikeGeneratorContainer();

		SpikeGenerator* getGenerator();


	private: 
		CarlsimSpikeGenerator* generator; // a subclass of CARLsim::ConnectionGenerator (implementing the interface)
		CarlsimWrapper* wrapper;
		carlsim_injectors::NoiseInjectorModel* model; 
		int index; 
	};

}

#endif//CARLSIMSPIKEGENERATORCONTAINER_H

