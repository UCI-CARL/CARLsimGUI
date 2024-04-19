#ifndef CARLSIMSPIKEGENERATOR_H
#define CARLSIMSPIKEGENERATOR_H


#include "carlsim.h"

#include <vector>

#include <QHash>

namespace spikestream {
	namespace carlsim_injectors {
		class NoiseInjectorModel;
	}
	class CarlsimWrapper;
}


class CarlsimSpikeGenerator: public SpikeGenerator   {


public:

	CarlsimSpikeGenerator(spikestream::carlsim_injectors::NoiseInjectorModel* model, int index);
    ~CarlsimSpikeGenerator();


	void setWrapper(spikestream::CarlsimWrapper* w);

	virtual int nextSpikeTime(CARLsim* s, int grpId, int i, 
		int currentTime, int lastScheduledSpikeTime, int endOfTimeSlice);

private: 
	spikestream::carlsim_injectors::NoiseInjectorModel* model;
	int index;
	spikestream::CarlsimWrapper* wrapper;


};



#endif // CARLSIMSPIKEGENERATOR_H