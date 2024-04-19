#include "TestNeuronGroup.h"
#include "NeuronGroup.h"
#include "Neuron.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;


#include "Connection.h"
#include "ConnectionGroup.h"

/// First check elementrary functions

void TestNeuronGroup::testLNBuildGroups() {

	qDebug() << __FUNCTION__;

	NeuronType neurType(2, "neur type description", "neur type paramTableName", "");

	NeuronGroup orders(NeuronGroupInfo(1, "orders", "Orders Outbound Layer", QHash<QString, double>(), neurType));
	NeuronGroup cortex(NeuronGroupInfo(2, "cortex", "Cortex", QHash<QString, double>(), neurType));
	NeuronGroup samples(NeuronGroupInfo(3, "samples", "Samples Inbound Layer", QHash<QString, double>(), neurType));

	//order.addLayer(6, 10, 30, 5, 0);   //! \note CAUTION: height translate to y-coordinates! see implementation for details
	orders.addLayer2(6, 10, 30, 5, 0);   //! uses the fixed addLayer, which translates to "height" to z-axis
	QCOMPARE(orders.size(), 60); ; 
	QCOMPARE(orders.getBoundingBox().toString(), QString("Box x1=30; y1=5; z1=0; x2=35; y2=5; z2=9"));

	cortex.addLayer2(80, 50, 40, 3, 20);  // this would be a 3d

	// clone samples from order
	Box b = orders.getBoundingBox();
	float w = b.getWidth()+1;  // CAUTION returns 4 (x2-x1), see implementation of getWidgth
	float h = b.getHeight()+1;  // CAUTION returns 9 (z2-z1)
	float x = b.getX1();
	float y = b.getY1();		
	float z = b.getZ1();
	QCOMPARE(w, 6.f); 
	QCOMPARE(h, 10.f); 
	QCOMPARE(x, 30.f);
	QCOMPARE(y, 5.f); 
	QCOMPARE(z, 0.f); 
	
	samples.addLayer2(w, h, x+20, y, z); // 20 to the right

	Box b2(samples.getBoundingBox()); // copy bounding box 
	QCOMPARE(b2.toString(), QString("Box x1=50; y1=5; z1=0; x2=55; y2=5; z2=9")); 
	b2.translate(-20, 0, 0);
	QCOMPARE(b2.toString(), QString("Box x1=30; y1=5; z1=0; x2=35; y2=5; z2=9"));
 
	// plain iteration
	/*
	for (NeuronMap::iterator iter = samples.begin(), mapEnd = samples.end();
		iter != mapEnd; ++iter)
	{
		Neuron* n = iter.value();
		uint64_t key = iter.key(); // = id 
		qDebug() << "ID=" << n->getID() << " key=" << key << " Location=" << n->getLocation().toString();
	}
	*/

	// positional iteration
	/*
	for (NeuronPositionIterator pos_iter = samples.positionBegin(), pos_end = samples.positionEnd();
		pos_iter != pos_end; pos_iter++) 
	{
		Neuron* n = pos_iter.value();
		uint64_t key = pos_iter.key(); // = pos key
		qDebug() << "ID="<< n->getID() << " key="<< key << " Location=" << n->getLocation().toString();  // pos_iter.key() << 
	}

	key = 219902336040960  Location = "(50, 5, 0)"
	key = 219902336040969  Location = "(50, 5, 9)"
	key = 233096475574277  Location = "(53, 5, 5)"
	key = 241892568596489  Location = "(55, 5, 9)"

	*/

	QCOMPARE((uint64_t) samples.getPositionKey(50, 5, 0), (uint64_t)219902336040960);


	ConnectionGroup outbound(ConnectionGroupInfo(21, "cortex -> orders (outbound)", 2, 1, QHash<QString, double>(), SynapseType()));

	ConnectionGroup inbound(ConnectionGroupInfo(32, "samples -> cortex (inbound)", 3, 2, QHash<QString, double>(), SynapseType()));


	// add some random connections
	// cortex.addLayer2(80, 50, 40, 3, 20);  // this would be a 3d
	//int from = cortex.getNeuronIDAtLocation(Point3D(40, 3, 20));
	//int to = orders.getNeuronIDAtLocation(Point3D(30, 5, 0));

	outbound.addConnection(cortex.getNeuronIDAtLocation(Point3D(40, 3, 20)), orders.getNeuronIDAtLocation(Point3D(30, 5, 0)), 1.f, 0.100f);
	outbound.addConnection(cortex.getNeuronIDAtLocation(Point3D(119, 3, 69)), orders.getNeuronIDAtLocation(Point3D(30, 5, 0)), 1.f, 0.200f);
	outbound.addConnection(cortex.getNeuronIDAtLocation(Point3D(40, 3, 20)), orders.getNeuronIDAtLocation(Point3D(35, 5, 9)), 1.f, 0.300f);
	outbound.addConnection(cortex.getNeuronIDAtLocation(Point3D(119, 3, 69)), orders.getNeuronIDAtLocation(Point3D(35, 5, 9)), 1.f, 0.400f);
	outbound.addConnection(cortex.getNeuronIDAtLocation(Point3D(61, 3, 45)), orders.getNeuronIDAtLocation(Point3D(33, 5, 5)), 1.f, 0.500f);

	for (ConnectionIterator iter = outbound.begin(), mapEnd = outbound.end(); iter != mapEnd; ++iter) {
		qDebug()
			<< "cortex" << cortex.getNeuronLocation(iter->getFromNeuronID()).toString()
			<< " --> "
			<< "orders" << orders.getNeuronLocation(iter->getToNeuronID()).toString()
			<< " w=" << iter->getWeight();
	}

	// this is the actual code
	for (ConnectionIterator iter = outbound.begin(), mapEnd = outbound.end(); iter != mapEnd; ++iter)
	{	
		Point3D location(orders.getNeuronLocation(iter->getToNeuronID()));  // outbound =>  cortex  --> order
		location.translate(20, 0, 0);										// inbound  =>  samples --> cortex
		Neuron* sampleNeuron = samples.addNeuron(location.getXPos(), location.getYPos(), location.getZPos()); 
		inbound.addConnection(sampleNeuron->getID(), iter->getFromNeuronID(), iter->getDelay(), iter->getWeight());
	}
	

	for (ConnectionIterator iter = inbound.begin(), mapEnd = inbound.end(); iter != mapEnd; ++iter) {
		qDebug()
			<< "samples" << samples.getNeuronLocation(iter->getFromNeuronID()).toString()
			<< " --> "
			<< "cortex" << cortex.getNeuronLocation(iter->getToNeuronID()).toString()
			<< " w=" << iter->getWeight();
	}


	QCOMPARE(inbound.size(), outbound.size()); // inbound must be of the same size

	// connections are stored innternally in a deque, so the order is maintained
	// that means for each connection must be:  
	for (int i = 0; i < inbound.size(); i++) {
		Connection &outb_conn = outbound[i];
		Connection &inb_conn = inbound[i];
		QCOMPARE(inb_conn.getWeight(), outb_conn.getWeight());
		QCOMPARE(inb_conn.getToNeuronID(), outb_conn.getFromNeuronID()); 
		Point3D inb_p = samples.getNeuronLocation(inb_conn.getFromNeuronID());
		Point3D outb_p = orders.getNeuronLocation(outb_conn.getToNeuronID());
		QCOMPARE(inb_p.getYPos(), outb_p.getYPos());
		QCOMPARE(inb_p.getZPos(), outb_p.getZPos());
		QCOMPARE(inb_p.getXPos() - 20, outb_p.getXPos()); 
	}

	//! Put this in proper files etc
	qDebug() << "Put this in proper files etc";


}

void TestNeuronGroup::testLNConnectGroups() {

	
}


/*----------------------------------------------------------*/
/*-----                     TESTS                      -----*/
/*----------------------------------------------------------*/

void TestNeuronGroup::testAddLayer(){
	NeuronType neurType(2, "neur type description", "neur type paramTableName", "");
	NeuronGroup neurGrp( NeuronGroupInfo(0, "no name", "no description", QHash<QString, double>(), neurType) );

    //Add a couple of neurons to check the offset
    neurGrp.addNeuron(-1, -1, -1);
    neurGrp.addNeuron(-2, -2, -2);

    neurGrp.addLayer(13, 15, 2, 3, 4);

    NeuronMap* neuronMap = neurGrp.getNeuronMap();
    QCOMPARE(neuronMap->size(), (int)197);

	//Group should have a neuron at location (2, 3, 4);
	QVERIFY(neurGrp.getNeuronIDAtLocation(Point3D(2.0f, 3.0f, 4.0f)) != 0);

	//Group should have a neuron at location (2, 13, 4)
	QVERIFY(neurGrp.getNeuronIDAtLocation(Point3D(2.0f, 13.0f, 4.0f)) != 0);
}


void TestNeuronGroup::testAddNeuron(){
	NeuronType neurType(2, "neur type description", "neur type paramTableName", "");
	NeuronGroup neurGrp( NeuronGroupInfo(0, "no name", "no description", QHash<QString, double>(), neurType) );
    neurGrp.addNeuron(23, 27, 12);
    neurGrp.addNeuron(230, 270, 120);
    neurGrp.addNeuron(2334, 2745, 1245);

    //Check that map contains three matching neurons
    NeuronMap* neuronMap = neurGrp.getNeuronMap();
    QCOMPARE(neuronMap->size(), (int)3);

	//Check neurons exist at correct locations
	QVERIFY(neurGrp.getNeuronIDAtLocation(Point3D(23.0f, 27.0f, 12.0f)) != 0);
	QVERIFY(neurGrp.getNeuronIDAtLocation(Point3D(230.0f, 270.0f, 120.0f)) != 0);
	QVERIFY(neurGrp.getNeuronIDAtLocation(Point3D(2334.0f, 2745.0f, 1245.0f)) != 0);
}


void TestNeuronGroup::testGetPointFromPositionKey(){
	//uint64_t key = 0b000000000000000000010000000000000000000111000000000000000001011;
	//Fix for VS2012 that does not support litaral of base 2
	uint64_t key = 8796107702283;
	
	// key is a 3x21 bit compact format, so use shift in order to express it from points
	//uint64_t key = 2<<21<<21 | 7<<21 | 11;  // does not work, since first component exceeds word boundaries and becomes 0
	// working, but this is the implemention under test, so the decimal constant is the best
	//uint64_t key = 2; key <<= 21;  // x
	//key |= 7; key <<=21; // y
	//key |= 11; // z

	Point3D point = NeuronGroup::getPointFromPositionKey(key);
	Point3D expectedPoint(2,7,11);
	QCOMPARE(point, expectedPoint);
}


void TestNeuronGroup::testGetPositionKey(){
	try{
		uint64_t key = NeuronGroup::getPositionKey(2,7,11);
		//uint64_t expectedResult = 0b000000000000000000010000000000000000000111000000000000000001011;
		uint64_t expectedResult = 8796107702283; //Fix for VS2012 that does not support base 2 litarals
		//uint64_t expectedResult = 2<<21<<21 | 7<<21 | 11;;
		QCOMPARE(key, expectedResult);
	}
	catch(SpikeStreamException& ex){
		QFAIL(ex.getMessage().toLatin1());
	}
	catch(...){
		QFAIL("NeuronGroup test failure. An unknown exception occurred.");
	}
}


void TestNeuronGroup::testPositionIterator(){
	NeuronType neurType(2, "neur type description", "neur type paramTableName", "");
	NeuronGroup neurGrp( NeuronGroupInfo(0, "no name", "no description", QHash<QString, double>(), neurType) );
	Neuron* neur5 = neurGrp.addNeuron(23, 27, 14);//Should be 5th
	Neuron* neur3 = neurGrp.addNeuron(23, 27, 12);//Should be 3rd
	Neuron* neur2 = neurGrp.addNeuron(23, 26, 13);//Should be 2nd
	Neuron* neur4 = neurGrp.addNeuron(23, 27, 13);//Should be 4th
	Neuron* neur1 = neurGrp.addNeuron(22, 26, 14);//Should be 1st

	int counter = 1;
	for(NeuronPositionIterator iter = neurGrp.positionBegin(); iter != neurGrp.positionEnd(); ++iter){
		switch(counter){
			case 1:
				QCOMPARE(iter.value()->getID(), neur1->getID());
			break;
			case 2:
				QCOMPARE(iter.value()->getID(), neur2->getID());
			break;
			case 3:
				QCOMPARE(iter.value()->getID(), neur3->getID());
			break;
			case 4:
				QCOMPARE(iter.value()->getID(), neur4->getID());
			break;
			case 5:
				QCOMPARE(iter.value()->getID(), neur5->getID());
			break;
			default: QFAIL("Iterator out of range");
		}
		++counter;
	}
}


