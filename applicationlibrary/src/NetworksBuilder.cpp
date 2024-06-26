//SpikeStream includes
#include "Globals.h"
#include "NetworksBuilder.h"
#include "SpikeStreamException.h"
using namespace spikestream;


/*! Constructor */
NetworksBuilder::NetworksBuilder(){
	networkDao = NULL;
	archiveDao = NULL;
}


/*! Destructor */
NetworksBuilder::~NetworksBuilder(){
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Sets pointer to point to a byte array that is filled with the specified training pattern */
void NetworksBuilder::addTraining(unsigned int neuronID, QString trainingStr, bool output){
	//Create arry of appropriate length
	int arrLen;
	if(trainingStr.length() % 8 == 0)
		arrLen = trainingStr.length() / 8;
	else
		arrLen = trainingStr.length() / 8 + 1;
	//unsigned char byteArr[arrLen];
	unsigned char byteArr[100]; // patch


	//Initialize array
	for(int i=0; i<arrLen; ++i)
		byteArr[i] = 0;

	//Set bits corresponding to 1's in byte string
	for(int i=0; i<trainingStr.length(); ++i){
		if(trainingStr[i] == '1')
			byteArr[i/8] |= 1<<(i % 8);
	}

	//Add training to database
	networkDao->addWeightlessNeuronTrainingPattern(neuronID, byteArr, output, arrLen);
}


/*! Adds the connection group to the database */
void NetworksBuilder::addConnectionGroup(unsigned int networkID, ConnectionGroup& connGrp){
	DBInfo netDBInfo = Globals::getNetworkDao()->getDBInfo();
	NetworkDaoThread netDaoThread(netDBInfo);
	netDaoThread.prepareAddConnectionGroup(networkID, &connGrp);
	runThread(netDaoThread);
}


/*! Runs the supplied thread and checks for errors */
void NetworksBuilder::runThread(NetworkDaoThread& thread){
	thread.start();
	thread.wait();
	if(thread.isError()){
		throw SpikeStreamException(thread.getErrorMessage());
	}
}


/*! Clears the error state of the class. */
void NetworksBuilder::clearError(){
	error = false;
	errorMessage = "";
}


/*! Sets the class in an error state. Used when running as a thread. */
void NetworksBuilder::setError(const QString &errMsg){
	errorMessage = errMsg;
	error = true;
}

