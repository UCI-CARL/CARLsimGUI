#ifndef CARLSIMNORMALSPIKEGENERATOR_H
#define CARLSIMNORMALSPIKEGENERATOR_H


#include "carlsim.h"

#include "normal_spikegen.h"

#include "CarlsimAbstractSpikeGenerator.h"

#include <vector>

#include <QHash>

namespace spikestream {
	namespace carlsim_injectors {
		class NoiseInjectorModel;
	}
	class CarlsimWrapper;
}


class CarlsimNormalSpikeGenerator: public CarlsimAbstractSpikeGenerator, public NormalSpikeGenerator {

public:

	CarlsimNormalSpikeGenerator(
		spikestream::carlsim_injectors::NoiseInjectorModel* model, int index,
		double mean, double sd, int events, int period);

    ~CarlsimNormalSpikeGenerator();

	void setWrapper(spikestream::CarlsimWrapper* w);


//private: 
//	spikestream::carlsim_injectors::NoiseInjectorModel* model;
//	int index;
//	spikestream::CarlsimWrapper* wrapper;

};



#endif // CARLSIMNORMALSPIKEGENERATOR_H