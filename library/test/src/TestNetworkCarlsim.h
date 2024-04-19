#ifndef TESTNETWORKCARLSIM_H
#define TESTNETWORKCARLSIM_H

//SpikeStream includes
#include "TestDao.h"

using namespace spikestream;

//Qt includes
#include <QtTest>
#include <QString>

class TestNetworkCarlsim : public TestDao {

// HOTFIX due Automoc !!!
//	Q_OBJECT

	private slots:
		void test2dConMatrix();			//! Test 2d bit connection matrix 
		void testNetwork();				//! Ids, Names,
		void testBoundingBox();			//! 3D Points, SpikeStream ISSUE
		void testNeuronGroups();		//! Neuron Type, Izhikevich Param
		void testConnections();			//! Structure, Topology
		void testConnectionGroups();	//! Synaptic Type

	private:

};

#endif//TESTNETWORKCARLSIM_H
