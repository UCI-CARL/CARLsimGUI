//SpikeStream includes
#include "GlobalVariables.h"
#include "SpikeStreamException.h"
#include "TestNetworkCarlsim.h"
#include "NetworkDao.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QHash>

//Other includes
#include <iostream>
#include <cassert>
using namespace std;


/*----------------------------------------------------------*/
/*-----                     TESTS                      -----*/
/*----------------------------------------------------------*/


/*! efficient access and storage as compact bit, 
	vector<bool> \sa https://en.cppreference.com/w/cpp/container/vector_bool
	lambda \sa https://docs.microsoft.com/en-us/cpp/cpp/lambda-expressions-in-cpp?view=vs-2019
	LN20201015 */
void TestNetworkCarlsim::test2dConMatrix() {

	int nfrom = 5;
	int nto = 10;
	
	vector<bool> c(nfrom*nto);
	auto idx = [&nto](int pre, int post) {return pre*nto +post; };

	//size_t bytes = size_of(c); 

	auto i_1_3 = idx(1,3); //1*nto+3
	auto i_0_nto = idx(0,nto-1); // nto

	QCOMPARE(i_1_3, 10+3);
	QCOMPARE(i_0_nto, 9); 

	c[idx(0,0)] = true; 
	c[idx(nfrom-1,nto-1)] = true; 
	c[idx(nfrom-1, 0)] = true; 
	c[idx(0,nto-1)] = true; 
	c[idx(2,3)] = true; 

	auto x = c[3]; 

	QCOMPARE((bool) c[idx(2,3)], true); 
	QCOMPARE((bool) c[idx(0,0)], true); 
	QCOMPARE((bool) c[idx(nfrom-1,nto-1)], true); 
	QCOMPARE((bool) c[idx(nfrom-1, 0)], true); 
	QCOMPARE((bool) c[idx(0,nto-1)], true); 

	auto connected = [&,nfrom,nto](int pre, int post) -> bool {
		assert(pre>0 && pre<nfrom);
		assert(post>0 && post<nto);
		return c[pre*nto +post]; 
	};

	QCOMPARE(connected(nfrom-1, nto-1), true); 
	QCOMPARE(connected(nfrom-1, nto-2), false); 
	QCOMPARE(connected(nfrom-2, nto-1), false); 
	QCOMPARE(connected(1, 1), false); 



}



//! Validate Neurons of both Groups
void TestNetworkCarlsim::testNetwork(){
	//Add a couple of networks
	addTestNetwork3_CarlsimBasic();

	//Check if the network id corresponds with the name 
	NetworkDao netDao(networkDBInfo);
	NetworkInfo info; 
	QCOMPARE(netDao.getNetworkInfo("testNetwork3", info), true); 
	QCOMPARE(info.getID(), testNet3ID); 

	//Check counts of neuons in all groups 
	QCOMPARE(netDao.getNeuronCount(testNet3ID), (unsigned int)5+10); 

	//Check amount of neurons in specific groups
	NeuronGroupInfo info31, info32;
	info31.setID(neurGrp31ID);
	info32.setID(neurGrp32ID); 
	QCOMPARE(netDao.getNeuronCount(info31), (unsigned int)5); 
	QCOMPARE(netDao.getNeuronCount(info32), (unsigned int)10); 

}


//! Validate Neurons of both Groups
void TestNetworkCarlsim::testBoundingBox(){
	addTestNetwork3_CarlsimBasic();
	NetworkDao netDao(networkDBInfo);
	
	//Check Bounding Box which must 
	Box box31 = netDao.getNeuronGroupBoundingBox(neurGrp31ID);
	Box box32 = netDao.getNeuronGroupBoundingBox(neurGrp32ID);
	QCOMPARE(box31.getWidth(), 5.f - 1.f);  // this is a bug in SpikeStream !
	QCOMPARE(box32.getWidth(), 10.f - 1.f); 
	//QCOMPARE(box31.getHeight()*box31.getLength()*box32.getHeight()*box32.getLength(), 1.f); // all other dimensions must be 1, 
	QCOMPARE(box31.getHeight()+box31.getLength()+box32.getHeight()+box32.getLength(), 0.f); // see above, (SpikeStream bug) 
}


//! Neuron Type, Izhikevich Param
void TestNetworkCarlsim::testNeuronGroups(){

	//Add a couple of networks
	addTestNetwork3_CarlsimBasic();

	//Check if the network id corresponds with the name 
	NetworkDao netDao(networkDBInfo);
	
	//Retrieve all neuron groups and validate its content
	QList<NeuronGroupInfo> neurGrpInfoList;
	netDao.getNeuronGroupsInfo(testNet3ID, neurGrpInfoList);
	QCOMPARE(neurGrpInfoList.size(), 2); 

	// check content of specific group
	NeuronGroupInfo const & grpInfo = neurGrpInfoList.at(1); 
	QCOMPARE(grpInfo.getName(), QString("name3-2")); 
	QCOMPARE(neurGrpInfoList.at(0).getName(), QString("name3-1")); 

	// Check Izhikevich Type
	NeuronType grpNeuronType = grpInfo.getNeuronType(); 
	QCOMPARE(grpInfo.getNeuronTypeID(), grpNeuronType.getID());
	QCOMPARE(grpNeuronType.getID(), (unsigned int) 1); // internal DB Id, corresponding with NeMo but not mandatory with others
	QCOMPARE(grpNeuronType.getDescription(), QString("Izhikevich Excitatory Neuron"));
	QCOMPARE(grpNeuronType.getParameterTableName(), QString("IzhikevichExcitatoryNeuronParameters"));

	// Check Izhikevich Excitatory Params
	QHash<QString, double> params = netDao.getNeuronParameters(grpInfo);
	//for(auto iter = params.begin(); iter!=params.end(); iter++) 
	//	cerr << iter.key().toStdString() << ":" << iter.value() << " "; 
	//cerr << "\n";
	QCOMPARE(params["a"], .022);
	QCOMPARE(params["b"], .22);
	QCOMPARE(params["seed"], 42.);
}



//! Group structure, Synaptic Type
void TestNetworkCarlsim::testConnectionGroups(){

	//Add the basic carlsim networks
	addTestNetwork3_CarlsimBasic();
	NetworkDao netDao(networkDBInfo);

	//neurGrp31ID: 5 Neurons, neurGrp32ID: 10 Neurons 
	//31 -> 32: 4 Ecken + 1 sonderpunkt
	QCOMPARE(netDao.getConnectionCount(testNet3ID), (unsigned int) 4+1);

	QList<ConnectionGroupInfo> conGrpInfoList; 
	netDao.getConnectionGroupsInfo(testNet3ID, conGrpInfoList);
	QCOMPARE(conGrpInfoList.size(), 1);
	ConnectionGroupInfo &connGrpInfo = conGrpInfoList[0];
	QCOMPARE(connGrpInfo.getID(), connGrp3132ID); 
	QCOMPARE(connGrpInfo.getDescription(), QString("c_31_32")); 
	QCOMPARE(netDao.getConnectionCount(connGrpInfo), (unsigned int) 4+1);
	QCOMPARE(connGrpInfo.getFromNeuronGroupID(), neurGrp31ID); 
	QCOMPARE(connGrpInfo.getToNeuronGroupID(), neurGrp32ID); 

	SynapseType const &synapseT = connGrpInfo.getSynapseType();
	QCOMPARE(synapseT.getDescription(), QString("Izhikevich Synapse")); 
	//QHash<QString, double> params = connGrpInfo.getParameterMap(); 
	//for(auto p=params.begin(); p!=params.end(); p++) {
	//	cerr << p.key().toStdString() << " " << p.value() << "\n"; 
	//}
	QCOMPARE(connGrpInfo.getParameter("connparam1"), 0.63); 
	QCOMPARE(connGrpInfo.getParameter("connparam2"), 1.31); 

		// TODO: Warum fehlen die Einträge (wg. Lernen .. ) 
	// Annahme: -> muss direkt in die DB inserted werden
	// 	6	1	0	10
	//SELECT * FROM spikestreamnetworktest.izhikevichsynapseparameters;
	QHash<QString, double> const &paramMap = netDao.getSynapseParameters(connGrpInfo);
	QCOMPARE(paramMap.size(), 3); 
	QCOMPARE((bool) paramMap["Learning"], true); 
	QCOMPARE((bool) paramMap["Disable"], false); 
	QCOMPARE(paramMap["weight_factor"], 10.0); 


	// Carlsim vs SpikeStream NeMo: 
	// a Carlsim synaptic connections are always contained in a connection group, 
	// a connection group projects from one pre-synaptic group to one post-synaptic group (which can be the same)
	// same is true in spike stream, see table def of connection group
}


//! Validate connections between presynaptic and postsynaptic neurons
void TestNetworkCarlsim::testConnections(){

	/*
		int n_pre = n31, n_post = n32;
		for(int i=0;i<n31;i++)
			for(int j=0;j<n32;j++) {

				bool connected = (i == 0 && j == 0)
				|| (i == n_pre-1 && j == 0)
				|| (i == 0 && j == n_post-1)
				|| (i == n_pre-1 && j == n_post-1)
				|| (i == 2 && j == 6);
			if(connected)
				testConnIDList3.append(addTestConnection(connGrp3132ID, testNeurIDList3[i], testNeurIDList3[j], (i*100.f+j)/100.f, i));  // weight(2,5) = 2.5 delay = 1..pre ms

	# ConnectionID, FromNeuronID, ToNeuronID, ConnectionGroupID, Delay, Weight
	8483, 8829, 8834, 1599, 0, 0
	8484, 8829, 8843, 1599, 0, 0.09
	8485, 8831, 8840, 1599, 2, 2.06
	8486, 8833, 8834, 1599, 4, 4
	8487, 8833, 8843, 1599, 4, 4.09

	# NeuronID, NeuronGroupID, X, Y, Z
	8829, 3206, 0, 1, 1
	8830, 3206, 1, 1, 1
	8831, 3206, 2, 1, 1
	8832, 3206, 3, 1, 1
	8833, 3206, 4, 1, 1
	8834, 3207, 0, 1, 1
	8835, 3207, 1, 1, 1
	8836, 3207, 2, 1, 1
	8837, 3207, 3, 1, 1
	8838, 3207, 4, 1, 1
	8839, 3207, 5, 1, 1
	8840, 3207, 6, 1, 1
	8841, 3207, 7, 1, 1
	8842, 3207, 8, 1, 1
	8843, 3207, 9, 1, 1

	*/

	//Add the basic carlsim networks
	addTestNetwork3_CarlsimBasic();
	NetworkDao netDao(networkDBInfo);

	// major design flaw in SpikeStream.DAO: not concurrent save (IDs are not guaranteed and 
	// can have gaps when other networks are created in parallel)
	// solution: store relative ID, like CARLsim,   + user key  (grpid, nrnid),  grpid is relative to network
	// also denormalize network id into all .. => fasst access and indizes possible, datasize is no problem, in a db
	// also it is a retieval db 

	unsigned g31n0ID = netDao.getStartNeuronID(neurGrp31ID);
	unsigned g32n0ID = netDao.getStartNeuronID(neurGrp32ID);

	//cerr << g31n0ID << " " << g32n0ID << "\n";

	QList<QPair<unsigned, Connection>> &con_0_0 = netDao.getConnections(g31n0ID+0,g32n0ID+0);
	QList<QPair<unsigned, Connection>> &con_0_9 = netDao.getConnections(g31n0ID+0,g32n0ID+9);
	QList<QPair<unsigned, Connection>> &con_4_0 = netDao.getConnections(g31n0ID+4,g32n0ID+0);
	QList<QPair<unsigned, Connection>> &con_4_9 = netDao.getConnections(g31n0ID+4,g32n0ID+9);
	QList<QPair<unsigned, Connection>> &con_2_6 = netDao.getConnections(g31n0ID+2,g32n0ID+6);
	
	QCOMPARE(con_0_0.size(), 1);
	QCOMPARE(con_0_9.size(), 1);
	QCOMPARE(con_4_0.size(), 1);
	QCOMPARE(con_4_9.size(), 1);
	QCOMPARE(con_2_6.size(), 1);

	QCOMPARE(con_0_0[0].second.getWeight(), .000f); 
	QCOMPARE(con_0_9[0].second.getWeight(), .009f); 
	QCOMPARE(con_4_0[0].second.getWeight(), .400f); 
	QCOMPARE(con_4_9[0].second.getWeight(), .409f); 
	QCOMPARE(con_2_6[0].second.getWeight(), .206f); 

	QCOMPARE(con_0_0[0].second.getDelay(),  0.f); 
	QCOMPARE(con_0_9[0].second.getDelay(),  0.f); 
	QCOMPARE(con_4_0[0].second.getDelay(),  4.f); 
	QCOMPARE(con_4_9[0].second.getDelay(),  4.f); 
	QCOMPARE(con_2_6[0].second.getDelay(),  2.f); 

}

