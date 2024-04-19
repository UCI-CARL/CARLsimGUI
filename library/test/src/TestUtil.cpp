//SpikeStream includes
#include "TestUtil.h"
#include "Util.h"
#include "NumberConversionException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

#include <cmath>

/*!
Round to integral value
Rounds x to an integral value, using the rounding direction specified by fegetround.
http://www.cplusplus.com/reference/cmath/rint/
http://www.cplusplus.com/reference/cmath/round/
http://www.cplusplus.com/reference/cfenv/fegetround/

http://en.cppreference.com/w/cpp/numeric/math/rint
*/
void TestUtil::test_rint(){

    QCOMPARE(rint(2.3), 2.0);
    QCOMPARE(rint(3.8), 4.0);
    QCOMPARE(rint(5.5), 6.0);
    QCOMPARE(rint(-2.3), -2.0);
    QCOMPARE(rint(-3.8), -4.0);
    QCOMPARE(rint(-5.5), -6.0);

    QCOMPARE(rint(0.318318), 0.0);
    QCOMPARE(rint(0.499999999), 0.0);
    QCOMPARE(rint(1.499999), 1.0);
    QCOMPARE(rint(0.50000000), 1.0);	// fails for impl. "Stack overflow"
    QCOMPARE(rint(0.50000001), 1.0);	
    QCOMPARE(rint(1.50000000), 2.0);	
    QCOMPARE(rint(-0.499999), 0.0);
    QCOMPARE(rint(-0.5000000), -1.0);
    QCOMPARE(rint(-0.5000001), -1.0);
    QCOMPARE(rint(2.5), 3.0);
    QCOMPARE(rint(3.4999999999), 3.0);
    QCOMPARE(rint(-9.49494949), -9.0);

	
	//QCOMPARE(rint(0.49999999999999994), 0.0);	// fails for impl. "Postgress", "Boost",
	QCOMPARE(rint(0.4999999999999994), 0.0);	// but this works
	QCOMPARE(rint(0.4999999999999999), 0.0);    
	//QCOMPARE(rint(0.49999999999999993), 0.0);  // this fails aswell
	//QCOMPARE(rint(0.49999999999999995), 0.0);  //  
	
}


void TestUtil::testFillSelectionArray(){
    bool selArr [7];
    Util::fillSelectionArray(selArr, 7, 4);
    QCOMPARE(selArr[0], false);
    QCOMPARE(selArr[1], false);
    QCOMPARE(selArr[2], false);
    QCOMPARE(selArr[3], true);
    QCOMPARE(selArr[4], true);
    QCOMPARE(selArr[5], true);
    QCOMPARE(selArr[6], true);

    Util::fillSelectionArray(selArr, 7, 7);
    QCOMPARE(selArr[0], true);
    QCOMPARE(selArr[1], true);
    QCOMPARE(selArr[2], true);
    QCOMPARE(selArr[3], true);
    QCOMPARE(selArr[4], true);
    QCOMPARE(selArr[5], true);
    QCOMPARE(selArr[6], true);

    Util::fillSelectionArray(selArr, 7, 0);
    QCOMPARE(selArr[0], false);
    QCOMPARE(selArr[1], false);
    QCOMPARE(selArr[2], false);
    QCOMPARE(selArr[3], false);
    QCOMPARE(selArr[4], false);
    QCOMPARE(selArr[5], false);
    QCOMPARE(selArr[6], false);
}


void TestUtil::testGetUIntList(){
    //Check an empty list
    QList<unsigned int> tstList = Util::getUIntList("");
    QCOMPARE(tstList.size(), 0);

    //Check a good list
    tstList = Util::getUIntList("33, 56");
    QCOMPARE(tstList.size(), (int)2);
    QCOMPARE(tstList.at(0), (unsigned int)33);
    QCOMPARE(tstList.at(1), (unsigned int)56);

    //Check it fails on a dodgy list
    bool exceptionThrown = false;
    try{
	tstList = Util::getUIntList("33, -56");
    }
    catch(NumberConversionException& ex){
	exceptionThrown = true;
    }
    QVERIFY(exceptionThrown);

    //Check it fails on another dodgy list
    exceptionThrown = false;
    try{
	tstList = Util::getUIntList("678, 533.2, 56");
    }
    catch(NumberConversionException& ex){
	exceptionThrown = true;
    }
    QVERIFY(exceptionThrown);
}



void TestUtil::testRDouble(){
    QCOMPARE(Util::rDouble(0.318318, 3), 0.318);
    QCOMPARE(Util::rDouble(0.318518, 3), 0.319);
    QCOMPARE(Util::rDouble(4.2020, 2), 4.20);
    QCOMPARE(Util::rDouble(23.3, 1), 23.3);
    QCOMPARE(Util::rDouble(23.3, 2), 23.3);
}


void TestUtil::testRandom(){

	// Random generator initialized with current time
	Util::seedRandom(0); 
    for(int i=0; i< 10; ++i){
	int ranNum = Util::getRandom(-50, 50);
		QVERIFY(ranNum <= 50 && ranNum >= -50);
    }
    for(int i=0; i<10; ++i){
	int ranNum = Util::getRandom(0, 10000);
		QVERIFY(ranNum <= 10000 && ranNum >= 0);
    }

	// Deterministic Random 
    Util::seedRandom(100);
#ifdef Q_OS_WIN
	// Under MS VisualC  Qt4.8.5 utilized the MS random number generator -> rand.c
	QCOMPARE(Util::getRandom(25, 250), (int)27);
	QCOMPARE(Util::getRandom(25, 250), (int)33);
	QCOMPARE(Util::getRandom(25, 250), (int)62);
	QCOMPARE(Util::getRandom(25, 250), (int)139);
	QCOMPARE(Util::getRandom(25, 250), (int)193); 
#else
	QCOMPARE(Util::getRandom(25, 250), (int)165);
	QCOMPARE(Util::getRandom(25, 250), (int)116);
	QCOMPARE(Util::getRandom(25, 250), (int)40);
	QCOMPARE(Util::getRandom(25, 250), (int)79);
	QCOMPARE(Util::getRandom(25, 250), (int)229);
#endif

}



