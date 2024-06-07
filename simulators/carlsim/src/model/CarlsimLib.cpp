#include "CarlsimLib.h"

#include "CarlsimGeneratorContainer.h"

#undef _SNN_DEFINITIONS_H_
#include "carlsim_conf.h"  
#include "carlsim.h"
#include "carlsim_datastructures.h"

//#include <vector>


spikestream::CarlsimLib::CarlsimLib() {
	carlsim = (Carlsim *) new CARLsim();
}


spikestream::CarlsimLib::CarlsimLib(const std::string& netName, SimMode preferredSimMode, LoggerMode loggerMode, int ithGPUs, int randSeed) {
	carlsim = (Carlsim *) new CARLsim(netName, preferredSimMode, loggerMode, ithGPUs, randSeed);
}

spikestream::CarlsimLib::~CarlsimLib() {
	delete ((CARLsim*)carlsim);		// the cast delegates to the desctructor, 
	carlsim = NULL;
}

bool spikestream::CarlsimLib::loadLib()
{
	return false;
}

// ALL STATES
CARLsimState spikestream::CarlsimLib::getCARLsimState() {
	return ((CARLsim*)carlsim)->getCARLsimState();
}

#ifndef __NO_CUDA__	
int spikestream::CarlsimLib::cudaDeviceCount() {
	//return ((CARLsim*)carlsim)->cudaDeviceCount();
	return CARLsim::cudaDeviceCount();
}

void spikestream::CarlsimLib::cudaDeviceDescription(unsigned ithGPU, const char **desc) {
	//((CARLsim*)carlsim)->cudaDeviceDescription(ithGPU, desc);
	CARLsim::cudaDeviceDescription(ithGPU, desc);
}
#endif

// CONFIG STATE

int spikestream::CarlsimLib::connect(int grpId1, int grpId2, CarlsimGeneratorContainer* container, bool synWtType) 
{
	return ((CARLsim*)carlsim)->connect(grpId1, grpId2, (ConnectionGenerator*) container->getGenerator(), synWtType);
}

void spikestream::CarlsimLib::setESTDP(int preGrpId, int postGrpId, bool isSet) {
	((CARLsim*)carlsim)->setESTDP(preGrpId, postGrpId, isSet);
}

//void spikestream::CarlsimLib::setESTDP(int preGrpId, int postGrpId, bool isSet, STDPType type, ExpCurve curve) {
//	((CARLsim*)carlsim)->setESTDP(preGrpId, postGrpId, isSet, type, curve);
//}


// setIntegrationMethod(RUNGE_KUTTA4, 10);	// OK 15-16 Spikes / 100ms
void spikestream::CarlsimLib::setIntegrationMethod(integrationMethod_t method, int numStepsPerMs) {
	((CARLsim*)carlsim)->setIntegrationMethod(method, numStepsPerMs);
}


int spikestream::CarlsimLib::createGroup(const std::string &grpName, int n , int neurType){
	return ((CARLsim*)carlsim)->createGroup(grpName, n, neurType); 
}

int spikestream::CarlsimLib::createGroup(const std::string &grpName, Grid3D grid, int neurType, int preferredPartition, ComputingBackend preferredBackend){
	return ((CARLsim*)carlsim)->createGroup(grpName, grid, neurType, preferredPartition, preferredBackend); 
}

int spikestream::CarlsimLib::createSpikeGeneratorGroup(const std::string& grpName, int nNeur, int neurType, int preferredPartition, ComputingBackend preferredBackend){
	return ((CARLsim*)carlsim)->createSpikeGeneratorGroup(grpName, nNeur, neurType, preferredPartition, preferredBackend);
}


int spikestream::CarlsimLib::createSpikeGeneratorGroup(const std::string& grpName, const Grid3D& grid, int neurType, int preferredPartition, ComputingBackend preferredBackend){
	return ((CARLsim*)carlsim)->createSpikeGeneratorGroup(grpName, grid, neurType, preferredPartition, preferredBackend);
}



void spikestream::CarlsimLib::setNeuronParameters(int grpId, float izh_a, float izh_b, float izh_c, float izh_d) {
	((CARLsim*)carlsim)->setNeuronParameters(grpId, izh_a, izh_b, izh_c, izh_d);
}

void spikestream::CarlsimLib::setConductances(bool isSet){
	((CARLsim*)carlsim)->setConductances(isSet); 
}

void spikestream::CarlsimLib::setConductances(bool isSet, int tdAMPA, int tdNMDA, int tdGABAa, int tdGABAb) {
	((CARLsim*)carlsim)->setConductances(isSet, tdAMPA, tdNMDA, tdGABAa, tdGABAb);
}

void spikestream::CarlsimLib::setConductances(bool isSet, int tdAMPA, int trNMDA, int tdNMDA, int tdGABAa, int trGABAb, int tdGABAb) {
	((CARLsim*)carlsim)->setConductances(isSet, tdAMPA, trNMDA, tdNMDA, tdGABAa, trGABAb, tdGABAb);
}

void spikestream::CarlsimLib::setConductances(int grpId, bool isSet) {
	((CARLsim*)carlsim)->setConductances(grpId, isSet);
}

void spikestream::CarlsimLib::setConductances(int grpId, bool isSet, int tdAMPA, int tdNMDA, int tdGABAa, int tdGABAb) {
	((CARLsim*)carlsim)->setConductances(grpId, isSet, tdAMPA, tdNMDA, tdGABAa, tdGABAb);
}

void spikestream::CarlsimLib::setConductances(int grpId, bool isSet, int tdAMPA, int trNMDA, int tdNMDA, int tdGABAa, int trGABAb, int tdGABAb) {
	((CARLsim*)carlsim)->setConductances(grpId, isSet, tdAMPA, trNMDA, tdNMDA, tdGABAa, trGABAb, tdGABAb);
}





void spikestream::CarlsimLib::saveSimulation(const std::string& fileName, bool saveSynapseInfo) {
	((CARLsim*)carlsim)->saveSimulation(fileName, saveSynapseInfo);
}

void spikestream::CarlsimLib::setDefaultSaveOptions(std::string fileName, bool saveSynapseInfo) {
	((CARLsim*)carlsim)->setDefaultSaveOptions(fileName, saveSynapseInfo);
}




void spikestream::CarlsimLib::setupNetwork() {
	((CARLsim*)carlsim)->setupNetwork();
}


void spikestream::CarlsimLib::setSpikeGenerator(int grpId, SpikeGenerator* spikeGenFunc) {
	((CARLsim*)carlsim)->setSpikeGenerator(grpId, spikeGenFunc);
}

SpikeMonitor* spikestream::CarlsimLib::setSpikeMonitor(int grpId, const std::string& fileName) {
	return ((CARLsim*)carlsim)->setSpikeMonitor(grpId, fileName);
}

NeuronMonitor* spikestream::CarlsimLib::setNeuronMonitor(int grpId, const std::string& fileName) {
	return ((CARLsim*)carlsim)->setNeuronMonitor(grpId, fileName);
}

GroupMonitor* spikestream::CarlsimLib::setGroupMonitor(int grpId, const std::string& fileName) {
	return ((CARLsim*)carlsim)->setGroupMonitor(grpId, fileName);
}


ConnectionMonitor* spikestream::CarlsimLib::setConnectionMonitor(int grpIdPre, int grpIdPost, const std::string& fileName) {
	return ((CARLsim*)carlsim)->setConnectionMonitor(grpIdPre, grpIdPost, fileName);
}

void spikestream::CarlsimLib::setExternalCurrent(int grpId, float current) {
	((CARLsim*)carlsim)->setExternalCurrent(grpId, current);
}

void spikestream::CarlsimLib::setExternalCurrent(int grpId, std::vector<float> currents) {
	((CARLsim*)carlsim)->setExternalCurrent(grpId, currents);
}



void spikestream::CarlsimLib::dummy() {
	((CARLsim*)carlsim)->getCARLsimState();
}



// SETUP_STATE
int spikestream::CarlsimLib::runNetwork(int nSec, int nMsec, bool printRunSummary) {
	return ((CARLsim*)carlsim)->runNetwork(nSec, nMsec, printRunSummary);
}

int spikestream::CarlsimLib::getNumNeurons() {
	return ((CARLsim*)carlsim)->getNumNeurons();
}

int spikestream::CarlsimLib::getGroupNumNeurons(int grpId) {
	return ((CARLsim*)carlsim)->getGroupNumNeurons(grpId);
}

int spikestream::CarlsimLib::getGroupStartNeuronId(int grpId) {
	return ((CARLsim*)carlsim)->getGroupStartNeuronId(grpId);
}

int spikestream::CarlsimLib::getGroupEndNeuronId(int grpId) {
	return ((CARLsim*)carlsim)->getGroupEndNeuronId(grpId);
}


// RUN_STATE

SpikeMonitor* spikestream::CarlsimLib::getSpikeMonitor(int grpId) {
	return ((CARLsim*)carlsim)->getSpikeMonitor(grpId);
}

void spikestream::CarlsimLib::getFiring(std::vector<bool>& firing, int netId) {
	((CARLsim*)carlsim)->getFiring(firing, netId);
}


//void spikestream::CarlsimLib::SpikeMonitor_print(SpikeMonitor* monitor, bool printSpikeTimes) {
//	monitor->print(printSpikeTimes);
//}

#ifdef CARLSIM_AXONPLAST
uint8_t* spikestream::CarlsimLib::getDelays(int gIDpre, int gIDpost, int& Npre, int& Npost) {
	return ((CARLsim*)carlsim)->getDelays(gIDpre, gIDpost, Npre, Npost);
}

void spikestream::CarlsimLib::findWavefrontPath(std::vector<int>& path, std::vector<float>& eligibility, int netId, int grpId, int startNId, int goalNId) {
	((CARLsim*)carlsim)->findWavefrontPath(path, eligibility, netId, grpId, startNId, goalNId);
}


bool spikestream::CarlsimLib::updateDelays(int gGrpIdPre, int gGrpIdPost, std::vector<std::tuple<int, int, uint8_t>> connDelays) {
	return ((CARLsim*)carlsim)->updateDelays(gGrpIdPre, gGrpIdPost, connDelays);
}

#endif
