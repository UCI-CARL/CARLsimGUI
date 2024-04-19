
//SpikeStream includes
#include "TestAnalysisDao.h"
#include "TestArchiveDao.h"
#include "TestConnection.h"
#include "TestDatabaseDao.h"
#include "TestMemory.h"
#include "TestRunner.h"
#include "TestNetwork.h"
#include "TestNeuronGroup.h"
#include "TestNetworkDao.h"
#include "TestNetworkDaoThread.h"
#include "TestNetworkCarlsim.h"
#include "TestXMLParameterParser.h"
#include "TestUtil.h"
#include "TestWeightlessNeuron.h"

//Other includes
#include <iostream>
using namespace std;

// does not work
//#include <QtTest/QtTest>
//QTEST_MAIN(TestRunner)
//#include "TestRunner.moc";


/*! Runs all of the tests */
void TestRunner::runTests(){

	//Create core application object so that drivers load in Windows
	int argsSize = 0;
	//char* argsChar[0];
	char** argsChar = NULL;


	QCoreApplication coreApplication(argsSize, argsChar);

	int failed = 0;



	TestDatabaseDao testDatabaseDao;
	failed += QTest::qExec(&testDatabaseDao);



	if(failed)
		qDebug()  << "CAUTION:" << failed /*QString::number(failed)*/ << "TestCases failed !";
	else
		qDebug() << "OK, all TestCases have passed.";

	
}


