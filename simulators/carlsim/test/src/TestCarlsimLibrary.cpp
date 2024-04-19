

#include "TestCarlsimLibrary.h"

//SpikeStream includes
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <vector>
#include <cstdlib>
#include <iostream>
#include <cassert>

#define DEBUG true

#include "carlsim.h"
#include "carlsim_datastructures.h"
#include "stopwatch.h"

#ifdef DO_testCarlsimDLL2

/* Test callback 
1. An 10x10 array is initialized with arbitrary valiues
.. represending the connections

g1 is presynamptic neuron group, g2 is 

g1 -> g2 if c[i,k]==1

g1 has 5 neurons

g2 has 10 neorons

int c[5][10] = 0;

c[0,0] = 1; 
c[m,n] = 1; 
c[0,n] = 1;
c[m,0] = 1;
c[2,2] = 1;
c[4,6] = 1; 


 */


// custom ConnectionGenerator
class MyConGen : public ConnectionGenerator {
public:
    MyConGen(int pre, int post):n_pre(pre), n_post(post) {
		n_con = 0;
	}
    ~MyConGen() {}
 
	int n_pre, n_post, n_con; 

    // the pure virtual function inherited from base class
    // note that weight, maxWt, delay, and connected are passed by reference
    void connect(CARLsim* sim, int srcGrp, int i, int destGrp, 
		   int j, float& weight, float& maxWt,
            float& delay, bool& connected) {

		// assert srcgrp = 0, destgrp = 1
        // connect dedicated points
        connected = (i == 0 && j == 0)
				|| (i == n_pre-1 && j == 0)
				|| (i == 0 && j == n_post-1)
				|| (i == n_pre-1 && j == n_post-1);

        weight = 0.5f;
        maxWt = 1.5f;
        delay = 3;

		if(connected)
			n_con++;
			
    }
};

void TestCarlsimLibrary::testCarlsimDLL2(){

	std::cerr << "Creating network with user defined callback \n";

	// keep track of execution time
	Stopwatch watch;
	
	// create a network on GPU
	int numGPUs = 1;
	int randSeed = 42;
	CARLsim sim("callback", CPU_MODE, USER, numGPUs, randSeed);

	int n_pre = 5; 
	int n_post = 10;
	int g_pre=sim.createSpikeGeneratorGroup("pre", n_pre, EXCITATORY_NEURON);
	int g_post=sim.createGroup("post", n_post, EXCITATORY_NEURON);
	sim.setNeuronParameters(g_post, 0.02f, 0.2f, -65.0f, 8.0f);

	// http://uci-carl.github.io/CARLsim4/classConnectionGenerator.html
	MyConGen congen(n_pre, n_post);
	sim.connect(g_pre, g_post, &congen, SYN_FIXED);
	
		sim.setConductances(true);
	// sim.setIntegrationMethod(FORWARD_EULER, 2);

	// ---------------- SETUP STATE -------------------
	// build the network
	watch.lap("setupNetwork");

	// da hättich ohne unit test mir einen Wolf gesucht !!!
	QCOMPARE(congen.n_con, 0); // no connection are established by callback before setup 
	sim.setupNetwork();
	QCOMPARE(congen.n_con, 4); // after setup, all connection have been established over callback


	// set some monitors
	sim.setSpikeMonitor(g_pre,"DEFAULT");
	sim.setSpikeMonitor(g_post,"DEFAULT");
	sim.setConnectionMonitor(g_pre,g_post,"DEFAULT");

	//setup some baseline input
	PoissonRate pre(n_pre);
	pre.setRates(30.0f);
	sim.setSpikeRate(g_pre,&pre);

	// ---------------- RUN STATE -------------------
	watch.lap("runNetwork");

	// run for a total of 10 seconds
	// at the end of each runNetwork call, SpikeMonitor stats will be printed
	for (int i=0; i<10; i++) {
		sim.runNetwork(1,0);
	}

	// print stopwatch summary
	watch.stop();
	

}
#endif

#ifdef DO_testCarlsimDLL1
void TestCarlsimLibrary::testCarlsimDLL1(){
	unsigned ncount = 1000;
	unsigned scount = 1000;

	std::cerr << "Creating network (C API)\n";

		// keep track of execution time
	Stopwatch watch;
	

	// ---------------- CONFIG STATE -------------------
	
	// create a network on GPU
	int numGPUs = 1;
	int randSeed = 42;
	CARLsim sim("hello world", CPU_MODE, USER, numGPUs, randSeed);

	// configure the network
	// set up a COBA two-layer network with gaussian connectivity
	Grid3D gridIn(13,9,1); // pre is on a 13x9 grid
	Grid3D gridOut(3,3,1); // post is on a 3x3 grid
	int gin=sim.createSpikeGeneratorGroup("input", gridIn, EXCITATORY_NEURON);
	int gout=sim.createGroup("output", gridOut, EXCITATORY_NEURON);
	sim.setNeuronParameters(gout, 0.02f, 0.2f, -65.0f, 8.0f);
	sim.connect(gin, gout, "gaussian", RangeWeight(0.05), 1.0f, RangeDelay(1), RadiusRF(3,3,1));
	sim.setConductances(true);
	// sim.setIntegrationMethod(FORWARD_EULER, 2);

	// ---------------- SETUP STATE -------------------
	// build the network
	watch.lap("setupNetwork");
	sim.setupNetwork();

	// set some monitors
	sim.setSpikeMonitor(gin,"DEFAULT");
	sim.setSpikeMonitor(gout,"DEFAULT");
	sim.setConnectionMonitor(gin,gout,"DEFAULT");

	//setup some baseline input
	PoissonRate in(gridIn.N);
	in.setRates(30.0f);
	sim.setSpikeRate(gin,&in);


	// ---------------- RUN STATE -------------------
	watch.lap("runNetwork");

	// run for a total of 10 seconds
	// at the end of each runNetwork call, SpikeMonitor stats will be printed
	for (int i=0; i<10; i++) {
		sim.runNetwork(1,0);
	}

	// print stopwatch summary
	watch.stop();
	
}

#endif
