#ifndef CARLSIMNORMALSPIKEGENERATOR_H
#define CARLSIMNORMALSPIKEGENERATOR_H


#include "carlsim.h"

#include "normal_spikegen.h"

#include <vector>

#include <QHash>

namespace spikestream {
	namespace carlsim_injectors {
		class NoiseInjectorModel;
	}
	class CarlsimWrapper;
}


class CarlsimNormalSpikeGenerator: public NormalSpikeGenerator   {


public:

	CarlsimNormalSpikeGenerator(
		spikestream::carlsim_injectors::NoiseInjectorModel* model, 
		int index);

    ~CarlsimNormalSpikeGenerator();

	void setWrapper(spikestream::CarlsimWrapper* w);


private: 
	spikestream::carlsim_injectors::NoiseInjectorModel* model;
	int index;
	spikestream::CarlsimWrapper* wrapper;

};



#endif // CARLSIMNORMALSPIKEGENERATOR_H