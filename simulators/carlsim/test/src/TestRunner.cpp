
//SpikeStream includes
#include "TestRunner.h"
#include "TestCarlsimLibrary.h"
#include "TestCarlsimWrapper.h"
#include "TestCarlsimAxonPlast.h"

/*! Runs all of the tests */
void TestRunner::runTests(){

	//Create core application object so that drivers load in Windows
	int argsSize = 0;
//	char* argsChar[0];
	char** argsChar = NULL;
	QCoreApplication coreApplication(argsSize, argsChar);

	TestCarlsimLibrary testCarlsimLibrary;
	QTest::qExec(&testCarlsimLibrary);

	TestCarlsimWrapper testCarlsimWrapper;
	QTest::qExec(&testCarlsimWrapper);

	//TestCarlsimAxonPlast testCarlsimAxonPlast;
	//QTest::qExec(&testCarlsimAxonPlast);



}


