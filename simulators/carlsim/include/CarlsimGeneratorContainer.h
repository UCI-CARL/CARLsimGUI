#ifndef CARLSIMGENERATORCONTAINER_H
#define CARLSIMGENERATORCONTAINER_H


// 
#include "api.h" 

class CarlsimGenerator;

namespace spikestream {

	class ConnectionGroup;
	class CarlsimWrapper; 

	class CarlsimGeneratorContainer {


	public:

		CarlsimGeneratorContainer(ConnectionGroup* group, CarlsimWrapper* wrapper, float weightFactor=1.0f);
		~CarlsimGeneratorContainer();

		CarlsimGenerator* getGenerator();


	private: 
		ConnectionGroup* group; 
		CarlsimGenerator* generator; // a subclass of CARLsim::ConnectionGenerator (implementing the interface)
		CarlsimWrapper* wrapper;
	};

}

#endif//CARLSIMGENERATORCONTAINER_H

