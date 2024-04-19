
//SpikeStream includes
#include "Globals.h"
#include "SpikeStreamException.h"
#include "SpikeStreamIOException.h"
#include "ConfigLoader.h"
#include "Util.h"
using namespace spikestream;

// CARLsimCNS includes
#include "CnsManager.h"


//Cns includes
#include "CARLsimIO/CARLsimIOException.hpp"

//Qt includes
#include <QDebug>

//Other includes
#include <vector>
using namespace std;

////Debug output
//#define DEBUG
//#define DEBUG_NEURON_IDS
//#define DEBUG_CURRENT

#define NC_EXTENSIONS

/*! Constructor */
CnsManager::CnsManager() : AbstractDeviceManager() {
	#ifdef DEBUG_NEURON_IDS
		logFile = new QFile(Globals::getSpikeStreamRoot() + "/log/CnsManager.log");
		if(logFile->open(QFile::WriteOnly | QFile::Truncate))
			logTextStream = new QTextStream(logFile);
		else{
			throw SpikeStreamIOException("Cannot open log file for CnsManager.");
		}
	#endif//DEBUG_NEURON_IDS
}
 

/*! Destructor */
CnsManager::~CnsManager(){
	deleteAllChannels();
	//Clean up log file if logging is enabled
	#ifdef DEBUG_NEURON_IDS
		logFile->close();
		delete logFile;
		delete logTextStream;
	#endif//DEBUG_NEURON_IDS
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Adds an input channel, which will be a source of spikes to pass to the network. */
void CnsManager::addChannel(InboundChannel* inboundChannel, NeuronGroup* neuronGroup){

qDebug() << __FUNCTION__ << __LINE__;
	
//Check channel matches neuron group

	if(neuronGroup == NULL)
		throw SpikeStreamException("Neuron Group is not defined.");
	if(inboundChannel->size() != neuronGroup->size())
		throw SpikeStreamException("Inbound channel size does not match size of neuron group.");

	//Store the channel
	inboundChannels.append(QPair<InboundChannel*, NeuronGroup*>(inboundChannel, neuronGroup));

	//Create an array whose index is the neuron id and whose value is the SpikeStream neuron ID
	neurid_t* tmpNeurIDArray = new neurid_t[neuronGroup->size()];
	int ctr = 0;
	NeuronPositionIterator neurGrpEnd = neuronGroup->positionEnd();
	for(NeuronPositionIterator posIter = neuronGroup->positionBegin(); posIter != neurGrpEnd; ++posIter){

		tmpNeurIDArray[ctr] = posIter.value()->getID();
		//Output debugging information if required
		#ifdef DEBUG_NEURON_IDS
			(*logTextStream)<<"Cns ID: "<<(ctr)<<"; SpikeStream ID: "<<posIter.value()->getID()<<"; SpikeStream Position: ("<<posIter.value()->getXPos()<<", "<<posIter.value()->getYPos()<<", "<<posIter.value()->getZPos()<<")"<<endl;
		#endif//DEBUG_NEURON_IDS

		++ctr;
	}
	inputArrayList.append(tmpNeurIDArray);
}


/*! Adds an outbound channel, which will receive spikes from the network. */
void CnsManager::addChannel(OutboundChannel* outboundChannel, NeuronGroup* neuronGroup){
	//Check that neuron group is not already in use - currently only support one neuron group per channel
	if(neurIDMap.contains(neuronGroup->getStartNeuronID()))
		throw SpikeStreamException("Neuron groups cannot be connected to multiple output channels.");

	//Store output channel
	outboundChannels.append(QPair<OutboundChannel*, NeuronGroup*>(outboundChannel, neuronGroup));

	//Create vector to hold neuron IDs passed to this output channel
	vector<unsigned>* newVector = new vector<unsigned>();
	firingNeurIDVectors.push_back(newVector);

	/* Add entries to neuron ID map linking SpikeStream ID with start neuron ID and input vector.
		This has to be done topographically because neuron ids do not increase smoothly with distance. */
	int startMapSize = neurIDMap.size();
	int finSpikesIDCtr = 0;
	NeuronPositionIterator neurGrpEnd = neuronGroup->positionEnd();
	for(NeuronPositionIterator posIter = neuronGroup->positionBegin(); posIter != neurGrpEnd; ++posIter){
		//Create pair linking ID with a vector holding firing neuron IDs for a particular output channel
		QPair<neurid_t, vector<unsigned>*>* channelDetails = new QPair<neurid_t, vector<unsigned>*>(finSpikesIDCtr, newVector);

		//Store link between SpikeStream ID and pair.
		neurIDMap[posIter.value()->getID()] = channelDetails;

		//Output debugging information if required
		#ifdef DEBUG_NEURON_IDS
			(*logTextStream)<<"Adding output channel "<<outboundChannels.size()<<". SpikeStream ID: "<<posIter.value()->getID()<<"; finSpikes ID: "<<finSpikesIDCtr<<"; Vector address"<<&firingNeurIDVectors.back()<<endl;
			logTextStream->flush();
		#endif//DEBUG_NEURON_IDS

		//Increase ID counter
		++finSpikesIDCtr;
	}

	//Check that IDs are in a continuous range.
	if(startMapSize + neuronGroup->size() != neurIDMap.size())
		throw SpikeStreamException("Neuron ID error in neuron group " + QString::number(neuronGroup->getID()));
}


/*! Deletes all input and output channels */
void CnsManager::deleteAllChannels(){
	while(!inboundChannels.isEmpty()){
		deleteInboundChannel(0);
	}
	while(!outboundChannels.isEmpty()){
		deleteOutboundChannel(0);
	}
}


/*! Deletes an input channel. */
void CnsManager::deleteInboundChannel(int index){
	if(index < 0 || index >= inboundChannels.size())
		throw SpikeStreamException("Failed to delete input channel: index out of range: " + QString::number(index));
	delete inboundChannels.at(index).first;//Delete input channel
	delete [] inputArrayList.at(index);//Delete array linking IDs to SpikeStream IDs
	inputArrayList.removeAt(index);
	inboundChannels.removeAt(index);
	outputNeuronIDs.clear();//Make sure that there are not any old neuron IDs around
}


/*! Deletes an output channel. */
void CnsManager::deleteOutboundChannel(int index){
	//Check index
	if(index < 0 || index >= outboundChannels.size())
		throw SpikeStreamException("Failed to delete output channel: index out of range: " + QString::number(index));

	//Get address of firing neuron IDs vector associated with this output channel.
	//This will be used to identify and remove neuron ID mappings for this channel
	vector<unsigned>* vectAddress = firingNeurIDVectors.at(index);

	//Remove details about neuron group associated with output channel
	QList<neurid_t> tmpKeys = neurIDMap.keys();
	for(QList<neurid_t>::iterator iter = tmpKeys.begin(); iter != tmpKeys.end(); ++iter){
		if(neurIDMap[*iter]->second == vectAddress){
			delete neurIDMap[*iter];//Remove dynamically allocated QPair
			neurIDMap.remove(*iter);//Remove entry from map
		}
	}

	//Remove input vector associated with channel
	delete firingNeurIDVectors.at(index);//Delete dynamically allocated vector
	firingNeurIDVectors.erase(firingNeurIDVectors.begin() + index);

	//Delete channel from memory and remove output channel from list
	delete outboundChannels.at(index).first;//FIXME, CHECK
	outboundChannels.removeAt(index);
}


/*! Returns the parameters for an inbound channel. The index should be a valid
 point in the inbound channels list. */
map<string, Property> CnsManager::getInboundProperties(int index){
	if(index < 0 || index >= inboundChannels.size())
		throw SpikeStreamException("Failed to get input parameters: index out of range: " + QString::number(index));
	return inboundChannels[index].first->getProperties();
}


/*! Returns the parameters for an outbound channel. The index should be a valid
 point in the output channels list. */
map<string, Property> CnsManager::getOutboundProperties(int index){
	if(index < 0 || index >= outboundChannels.size())
		throw SpikeStreamException("Failed to get input parameters: index out of range: " + QString::number(index));
	return outboundChannels[index].first->getProperties();
}


/*! Sets the properties of a particular input channel */
void CnsManager::setInboundProperties(int index, map<string, Property> &propertyMap){
	if(index < 0 || index >= inboundChannels.size())
		throw SpikeStreamException("Failed to set input properties index out of range: " + QString::number(index));
	inboundChannels[index].first->setProperties(propertyMap);
}


/*! Sets the properties of a particular input channel */
void CnsManager::setOutboundProperties(int index, map<string, Property> &propertyMap){
	if(index < 0 || index >= outboundChannels.size())
		throw SpikeStreamException("Failed to set output properties index out of range: " + QString::number(index));
	outboundChannels[index].first->setProperties(propertyMap);
}


//Inherited from AbstractDeviceManager
QList<neurid_t>::iterator CnsManager::outputNeuronsBegin(){
	return outputNeuronIDs.begin();
}


//Inherited from AbstractDeviceManager
QList<neurid_t>::iterator CnsManager::outputNeuronsEnd(){
	return outputNeuronIDs.end();
}



//Inherited from AbstractDeviceManager
QList<neurid_t>::iterator CnsManager::currentNeuronsBegin() {
	return currentNeuronIDs.begin();
}


//Inherited from AbstractDeviceManager
QList<neurid_t>::iterator CnsManager::currentNeuronsEnd() {
	return currentNeuronIDs.end();
}


//Inherited from AbstractDeviceManager
QList<double>::iterator CnsManager::currentValuesBegin() {
	return currentValues.begin();
}


//Inherited from AbstractDeviceManager
QList<double>::iterator CnsManager::currentValuesEnd() {
	return currentValues.end();
}



//Inherited from AbstractDeviceManager
void CnsManager::setInputNeurons(timestep_t, QList<neurid_t>& firingNeuronIDs){
	#ifdef DEBUG
//		qDebug()<<"Setting firing neurons from SpikeStream. num firing neurons: "<<firingNeuronIDs.size()<<"; num output channels: "<<outputChannels.size()<<"; num firing neuron ID vectors: "<<firingNeurIDVectors.size()<<"; num mapped neur IDs: "<<neurIDMap.size();
		qDebug()<<"Firing Neurons: "<<firingNeuronIDs;
		qDebug()<<"NeurIDMap Keys: "<<neurIDMap.keys();
	#endif//DEBUG

	//Clear vectors holding firing neuron ids for each channel
	for(size_t i=0; i< firingNeurIDVectors.size(); ++i)
		firingNeurIDVectors[i]->clear();

	//Work through firing neurons and add them to the appropriate input vector for the channel without the start neuron ID.
	QPair<neurid_t, vector<unsigned>* >* pairPtr = NULL;
	QList<neurid_t>::iterator firingNeuronsEnd = firingNeuronIDs.end();
	for(QList<neurid_t>::iterator fireNeurIter = firingNeuronIDs.begin(); fireNeurIter != firingNeuronsEnd; ++fireNeurIter){
		//See if we have a mapping for this neuron ID
		if(neurIDMap.contains(*fireNeurIter)){//Some neuron groups may not be connected to channels
			// Get mapping for this SpikeStream neuron ID
			pairPtr = neurIDMap[*fireNeurIter];

			//Add matching ID to the vector whose pointer is stored in the pair.
			pairPtr->second->push_back(pairPtr->first);
		}
		#ifdef DEBUG_NEURON_IDS
			else
				qDebug()<<"Neuron ID "<<*fireNeurIter<<" not found in neuron ID map. NeurIDMap Keys: "<<neurIDMap.keys();
		#endif//DEBUG_NEURON_IDS
	}

	try{
		//Pass firing neuron ids to appropriate channels
		for(int chanCtr = 0; chanCtr < outboundChannels.size(); ++chanCtr){
			outboundChannels[chanCtr].first->setFiring(*firingNeurIDVectors[chanCtr]);
		}
	}
	catch(CARLsimIOException& ex){
		throw SpikeStreamException("CnsManager: Error occurred setting input neurons: " + QString(ex.what()));
	}
	catch(exception& ex){
		throw SpikeStreamException("CnsManager: exception occurred setting input neurons." + QString(ex.what()));
	}
	catch(...){
		throw SpikeStreamException("CnsManager: An unknown exception occurred setting input neurons.");
	}
}


//Inherited from AbstractDeviceManager
void CnsManager::step(){
	#ifdef DEBUG
//		qDebug()<<"Stepping FinSpikseManager. Num output channels: "<<outputChannels.size()<<"; num input channels: "<<inputChannels.size();
		qDebug()<<"Stepping FinSpikseManager. Num input channels: "<<inboundChannels.size();
	#endif//DEBUG

	outputNeuronIDs.clear();

	// current backend = 0
	currentNeuronIDs.clear();
	currentValues.clear();

	try{
		//Work through input channels - these pass spikes TO the SpikeStream network
		for (int chanCtr = 0; chanCtr < inboundChannels.size(); ++chanCtr) {
			inboundChannels[chanCtr].first->step();
			const vector<unsigned>& tmpFiringVect = inboundChannels[chanCtr].first->getFiring();

			NeuronGroup *grp = inboundChannels[chanCtr].second; 
			grp->setFiring(tmpFiringVect); 

			//Work through the firing neurons
			for (vector<unsigned>::const_iterator iter = tmpFiringVect.begin(); iter != tmpFiringVect.end(); ++iter) {
#ifdef DEBUG
				qDebug() << "FIRING NEURON: finSpikesID: " << *iter << "; SpikeStream ID: " << inputArrayList[chanCtr][*iter];
#endif//DEBUG

				//Convert ID to SpikeStream ID
				outputNeuronIDs.append(inputArrayList[chanCtr][*iter]);
			}

			// Deactivated for CNS, Corpus Callosum, Backend=0 (Tick returns direct current vector)
			// Reactivated again for WbSensoryCortex			 
			const std::vector<double>& tmpCurrentVect = inboundChannels[chanCtr].first->getCurrentVector();
			for (unsigned id = 0; id < tmpCurrentVect.size(); id++) {
				double current = tmpCurrentVect[id];
				//if (current > 0.1) { // sparse, cut out noise from gaus dist
					unsigned neuronID = inputArrayList[chanCtr][id];
					currentNeuronIDs.append(neuronID);
					currentValues.append(current);
#ifdef DEBUG_CURRENT
					qDebug() << "Cns.ID: " << id << " --> SpikeStream.ID: " << neuronID << " = " << current;
#endif
				//}
			}

		}

		//Step output channels - these receive spikes FROM the SpikeStream network
		for(int chanCtr = 0; chanCtr < outboundChannels.size(); ++chanCtr){
			outboundChannels[chanCtr].first->step();
		}
	}
	catch(CARLsimIOException& ex){
		throw SpikeStreamException("CnsManager: Error occurred stepping channels: " + QString(ex.what()));
	}
	catch(exception& ex){
		throw SpikeStreamException("CnsManager: exception occurred stepping channels." + QString(ex.what()));
	}
	catch(...){
		throw SpikeStreamException("An unknown exception occurred stepping channels.");
	}
}


//Inherited from AbstractDeviceManager
void CnsManager::setSimtime(unsigned long long simtime){

	try{
		//Work through input channels - these pass spikes TO the SpikeStream network
		for(int chanCtr = 0; chanCtr < inboundChannels.size(); ++chanCtr){
			inboundChannels[chanCtr].first->setSimtime(simtime);
		}

		//Step output channels - these receive spikes FROM the SpikeStream network
		for(int chanCtr = 0; chanCtr < outboundChannels.size(); ++chanCtr){
			outboundChannels[chanCtr].first->setSimtime(simtime);
		}
	}
	catch(CARLsimIOException& ex){
		throw SpikeStreamException("CnsManager: Error occurred stepping channels: " + QString(ex.what()));
	}
	catch(exception& ex){
		throw SpikeStreamException("CnsManager: exception occurred stepping channels." + QString(ex.what()));
	}
	catch(...){
		throw SpikeStreamException("An unknown exception occurred stepping channels.");
	}
}

//Inherited from AbstractDeviceManager
bool CnsManager::allChannelBufferEmpty(){

	// Iterate over all inbound channels and return immediately if a non empty buffer was found

	for(int chanCtr = 0; chanCtr < inboundChannels.size(); ++chanCtr){
		if(inboundChannels[chanCtr].first->getBufferQueued()>0)
			return false;
	}
	
	return true;
}

//Inherited from AbstractDeviceManager
void CnsManager::reset(){

	// Iterate over all inbound channels and reset them

	for(int chanCtr = 0; chanCtr < inboundChannels.size(); ++chanCtr)
		inboundChannels[chanCtr].first->reset();

	for(int chanCtr = 0; chanCtr < outboundChannels.size(); ++chanCtr)
		outboundChannels[chanCtr].first->reset();
}


double CnsManager::getDopaminLevel() {
	double da = .0;

	return da;
}

