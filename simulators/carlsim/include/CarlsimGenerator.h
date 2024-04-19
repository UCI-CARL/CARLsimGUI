#ifndef CARLSIMGENERATOR_H
#define CARLSIMGENERATOR_H


#include "carlsim.h"

#include <vector>
//#include <tuple>
//#include <map>

#include <QHash>

namespace spikestream {
	class ConnectionGroup;
	class CarlsimWrapper;
}


class CarlsimGenerator: public ConnectionGenerator   {


public:

	CarlsimGenerator(spikestream::ConnectionGroup* group);
    ~CarlsimGenerator();


	void setWrapper(spikestream::CarlsimWrapper* w);
	void setWeightFactor(float factor = 1.0f) { weightFactor = factor; }

	// the pure virtual function inherited from base class
	// note that weight, maxWt, delay, and connected are passed by reference
	virtual void connect(CARLsim* sim, int srcGrp, int i, int destGrp, 
			int j, float& weight, float& maxWt,
			float& delay, bool& connected);


	bool isConnected(unsigned pre, unsigned post);



private: 
	spikestream::ConnectionGroup* group;
	spikestream::CarlsimWrapper* wrapper;

	std::vector<bool> connections;   //! (pre,post)->bool

	int nfrom, nto; 
	int idx(unsigned pre, unsigned post);

	QHash< QPair<unsigned, unsigned>, QPair<float, float>> synapses; //! (pre,post)->(w,d)

	float weightFactor; // adapt CUBA, COBA

};



#endif