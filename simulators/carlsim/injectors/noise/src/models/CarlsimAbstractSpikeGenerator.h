#ifndef CARLSIMABSTRACTSPIKEGENERATOR_H
#define CARLSIMABSTRACTSPIKEGENERATOR_H



#include <vector>

#include <QHash>

namespace spikestream {
	namespace carlsim_injectors {
		class NoiseInjectorModel;
	}
	class CarlsimWrapper;
}


class CarlsimAbstractSpikeGenerator   {


public:

	CarlsimAbstractSpikeGenerator(
		spikestream::carlsim_injectors::NoiseInjectorModel* model, int index):
			model(model), index(index) {}

    ~CarlsimAbstractSpikeGenerator() {};

	void virtual setWrapper(spikestream::CarlsimWrapper* w) = 0;


protected: 
	spikestream::carlsim_injectors::NoiseInjectorModel* model;
	int index;
	spikestream::CarlsimWrapper* wrapper;

};



#endif // CARLSIMABSTRACTSPIKEGENERATOR_H