#ifndef CARLSIMSPIKEGENERATOR_H
#define CARLSIMSPIKEGENERATOR_H


#include "carlsim.h"

#include <vector>

#include <QHash>

namespace spikestream {
	class NeuronGroup;
	class CarlsimWrapper;
}


class CarlsimSpikeGenerator: public SpikeGenerator   {

public:

	CarlsimSpikeGenerator(spikestream::NeuronGroup* group);
    ~CarlsimSpikeGenerator();

	void setWrapper(spikestream::CarlsimWrapper* w);


	virtual int nextSpikeTime(CARLsim* s, int grpId, int i, 
		int currentTime, int lastScheduledSpikeTime, int endOfTimeSlice);

private: 
	spikestream::NeuronGroup* group;
	spikestream::CarlsimWrapper* wrapper;

};



#endif // CARLSIMSPIKEGENERATOR_H