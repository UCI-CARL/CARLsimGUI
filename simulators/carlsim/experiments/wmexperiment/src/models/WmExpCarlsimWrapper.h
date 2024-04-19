#ifndef WMEXPCARLSIMWRAPPER_H
#define WMEXPCARLSIMWRAPPER_H


//#include "Maze.h"
//#include "ResultsCursor.h"

#include "CarlsimWrapper.h"

#include "CarlsimOatSpikeMonitor.h"
//class OatSpikeMonitor;

//
//#include <stdint.h>
//#include <vector>	// std::vector
//#include <string>	// std::string


/*
* modify spacial map
*/
typedef std::vector<std::vector<int>> spikes_t;

enum loglevel { ERR, INF, DEB, DEV };

static loglevel ll = DEV;

//#include "maze.h"

//class spikestream::WmExptManager;

class WmExpCarlsimWrapper 
{

private:
	//Maze* maze;
	CarlsimWrapper* wrapper; // reference, container for the network, config, etc., see CarlsimLoader as sample
	carlsim_monitors::OatSpikeMonitor* monitor; //
	//int iteration; 
	//int loss;
	//int activity;

	//int grpId; 
	//const int netId = 8; //CPU_MODE is supported
	////const int grpSize = 16;  // fixme
	//int grpSize;  // fixme

	//std::vector<int> path;
	//std::vector<float> eligibility;

 

public:
	//Allow Eperiment direct access to private section
	//friend class spikestream::EpropExptManager;

	//Maze::ConnDelays_t connDelays;  // pre, post, delay  (map as list, could be map iterator )
	////Obsolete 
	//Maze::ConnDelays_t  traversalCosts;  // pre, post, delay  (map as list, could be map iterator )
	//Maze::ConnDelays_t connDelays2;  // pre, post, delay  for update


	//WmExpCarlsimWrapper(CarlsimWrapper* wrapper, carlsim_monitors::OatSpikeMonitor* monitor, Maze* maze, int grpId);
	WmExpCarlsimWrapper(CarlsimWrapper* wrapper, carlsim_monitors::OatSpikeMonitor* monitor);

	virtual ~WmExpCarlsimWrapper();

	// might be a slot
	void setMonitor(carlsim_monitors::OatSpikeMonitor* monitor);

	//int getLoss() { return loss; }
	//int getIteration() { return iteration; }
	//int getActivity() { return activity; }
	//int getLength() { return path.size(); }


	void printSpikes(int offset = 0);

	//neuron identified by coordinates
	//bool getSpike(int row, int column, int &time); 
	bool checkTargetFired (int targetNId, unsigned int currentTime);
	bool checkNeuronFiring (int nId, double f_expected, double eps, int ms, int spikes_min);

	//bool findWaveFront(int startNId, int endNId);

	//void writePathString(std::string &pathString);

	//void writePath(std::string filename);
	//void writeEligibility(std::string filename);
	//void writeCosts(std::string filename);

	//// write full qualified and directly to results
	//void writePath(std::unique_ptr<ResultsCursor> & fileResults);
	//void writeEligibility(std::unique_ptr<ResultsCursor> & fileResults);
	//void writeCosts(std::unique_ptr<ResultsCursor> &fileResults);


	////Read delays into connDelays 
	//void getDelays();

	//////Tolman 
	////void setBarrier(int barrierNId, int cost); 


	////Apply EProp algo
	//void eprop(); 

	////Write Delays stored in connDelays
	//void updateDelays();

//			map.emplace(std::tuple<int, int>(pre, post), delay);
//			connDelays.push_back(std::tuple<int, int, uint8_t>(pre, post, delay));



	//std::vector<Point3D> neurons3D;

	////float weight = 85.f;  // no reliable firing of RS neuron, sometimes with 1ms delay
	//float weight = 100.f;   // reliable firing of RS neuron 

	//int n;

	///* converts the maze into delays and Point3Ds
	//*/
	//void initialize();


};

#endif //WMEXPCARLSIMWRAPPER_H