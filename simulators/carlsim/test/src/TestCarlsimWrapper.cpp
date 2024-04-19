#include "TestCarlsimWrapper.h"
#include "CarlsimWrapper.h"
using namespace spikestream;


void TestCarlsimWrapper::testConstructor(){
	CarlsimWrapper* carlsimWrapper = new CarlsimWrapper();
	delete carlsimWrapper;
}


