#ifndef CARLSIMAERSPIKEGENERATOR_H
#define CARLSIMAERSPIKEGENERATOR_H


#include "carlsim.h"

#include <vector>

#include <QHash>

namespace spikestream {
	class NeuronGroup;
	class CarlsimWrapper;
}


class CarlsimAerSpikeGenerator: public SpikeGenerator   {

public:

	CarlsimAerSpikeGenerator(spikestream::NeuronGroup* group);
	~CarlsimAerSpikeGenerator();


	void setWrapper(spikestream::CarlsimWrapper* w);

	virtual int nextSpikeTime(CARLsim* s, int grpId, int i, 
		int currentTime, int lastScheduledSpikeTime, int endOfTimeSlice);

private: 
	spikestream::NeuronGroup* group;
	spikestream::CarlsimWrapper* wrapper;

};


#endif // CARLSIMAERSPIKEGENERATOR_H