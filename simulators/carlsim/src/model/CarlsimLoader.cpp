//SpikeStream includes
#include "Globals.h"
#include "CarlsimLoader.h"
#include "CarlsimWrapper.h"
#include "CarlsimGeneratorContainer.h"
#include "CarlsimSpikeGeneratorContainer.h"

#include "NeuronGroup.h"
#include "SpikeStreamSimulationException.h"
#include "SpikeStreamIOException.h"
#include "Util.h"


using namespace spikestream;


//Qt includes
#include <QDebug>

//Other includes
#include <vector>
#include <iostream>
using namespace std;

// Outputs verbose debugging behaviour about the loading of the network.
//#define DEBUG_NEURONS
//#define DEBUG_SYNAPSES

//Neuron type IDs in database. FIXME: WOULD BE BETTER TO USE THE NAME
#define IZHIKEVICH_EXCITATORY_NEURON_ID 1
#define IZHIKEVICH_INHIBITORY_NEURON_ID 2
#define DA_ERGIC_NEURON_ID 4
#define CUSTOM_EXCITATORY_NEURON_ID 5
#define CUSTOM_INHIBITORY_NEURON_ID 6
#define POISSON_EXCITATORY_NEURON_ID 7
#define POISSON_INHIBITORY_NEURON_ID 8

#include "carlsim_definitions.h"

/*! Constructor */
CarlsimLoader::CarlsimLoader(CarlsimWrapper* wrapper_): wrapper(wrapper_) {	
	//Open up log file if logging is enabled
	#if defined(DEBUG_NEURONS) || defined(DEBUG_SYNAPSES)
		logFile = new QFile(Globals::getSpikeStreamRoot() + "/log/CarlsimLoader.log");
		if(logFile->open(QFile::WriteOnly | QFile::Truncate))
			logTextStream = new QTextStream(logFile);
		else{
			throw SpikeStreamIOException("Cannot open log file for CarlsimLoader.");
		}
	#endif//DEBUG_NEURONS || DEBUG_SYNAPSES
	
}


/*! Destructor */
CarlsimLoader::~CarlsimLoader(){
	wrapper = NULL; // only a resource reference, _no_ delete
	//Clean up log file if logging is enabled
	#if defined(DEBUG_NEURONS) || defined(DEBUG_SYNAPSES)
		logFile->close();
		delete logFile;
		delete logTextStream;
	#endif//DEBUG_NEURONS || DEBUG_SYNAPSES
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/


/*! Loads the simulation */
bool CarlsimLoader::buildCarlsimNetwork(Network* network, QHash<unsigned, synapse_id*>& volatileConGrpMap, const bool* stop){

	//Initialize the CARLsim network

	//ensure Carlsim is in Config state 

	// Patch WM dlPFC, check with AxonalPlasticity
	wrapper->carlsim->setIntegrationMethod(RUNGE_KUTTA4, 10);


	//Check that list of volatie connection is empty
	if(!volatileConGrpMap.isEmpty())
		throw SpikeStreamSimulationException("Volatile connection group map should have been cleared when simulation was unloaded.");

	//Create the random number generator
	rng_t rng;
	urng_t ranNumGen( rng, boost::uniform_real<double>(0, 1) );//Constructor of the random number generator

	//Calculate progress
	QList<NeuronGroup*> neurGrpList = network->getNeuronGroups();
	QList<ConnectionGroup*> conGrpList = network->getConnectionGroups();
	int totalSteps = neurGrpList.size() + conGrpList.size();
	int stepsCompleted = 0;

	//Add the neuron groups
	for(int i=0; i<neurGrpList.size() && !*stop; ++i){
		unsigned int neurTypeID = neurGrpList.at(i)->getInfo().getNeuronTypeID();
		if(neurTypeID == IZHIKEVICH_EXCITATORY_NEURON_ID)
			addExcitatoryNeuronGroup(neurGrpList.at(i) /*,ranNumGen*/);
		else if(neurTypeID == IZHIKEVICH_INHIBITORY_NEURON_ID)
			addInhibitoryNeuronGroup(neurGrpList.at(i) /*,ranNumGen*/);
		else if(neurTypeID == DA_ERGIC_NEURON_ID)
			addDaErgicNeuronGroup(neurGrpList.at(i) /*,ranNumGen*/);
		else if(neurTypeID == CUSTOM_EXCITATORY_NEURON_ID)
			addCustomExcitatoryNeuronGroup(neurGrpList.at(i) /*,ranNumGen*/);
		else if(neurTypeID == CUSTOM_INHIBITORY_NEURON_ID)
			addCustomInhibitoryNeuronGroup(neurGrpList.at(i) /*,ranNumGen*/);
		else if(neurTypeID == POISSON_EXCITATORY_NEURON_ID)
			addPoissonExcitatoryNeuronGroup(neurGrpList.at(i) /*,ranNumGen*/);
		else if(neurTypeID == POISSON_INHIBITORY_NEURON_ID)
			addPoissonInhibitoryNeuronGroup(neurGrpList.at(i) /*,ranNumGen*/);
		else
			throw SpikeStreamSimulationException("Neuron group type " + QString::number(neurTypeID) + " is not supported by CARLsim");

		//Update progress
		++stepsCompleted;
		emit progress(stepsCompleted, totalSteps);
	}

	//Add the connection groups that are not disabled */
	for(int i=0; i<conGrpList.size() && !*stop; ++i){
		// Disable == off => 0.0, Disabled == on => 1.0
		double disable = conGrpList.at(i)->getParameter("Disable");  
		if(disable == 0.0)  
			addConnectionGroup(conGrpList.at(i), volatileConGrpMap);
		//Update progress
		++stepsCompleted;
		emit progress(stepsCompleted, totalSteps);
	}

	//Return the status 
	return true;
}

/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/


/*! Adds a connection group to the network. */
void CarlsimLoader::addConnectionGroup(ConnectionGroup* conGroup, QHash<unsigned, synapse_id*>& volatileConGrpMap){
	//Extract parameters
	bool learning = SYN_FIXED;  // CARLsim terminology
	synapse_id* synapseIDArray = NULL;
	if(conGroup->getParameter("Learning") != 0.0){  
		learning = SYN_PLASTIC;
		synapseIDArray = new synapse_id[conGroup->size()];
		volatileConGrpMap[conGroup->getID()] = synapseIDArray;
	} 

		synapseIDArray = new synapse_id[conGroup->size()];
		volatileConGrpMap[conGroup->getID()] = synapseIDArray;



	double weightFactor = conGroup->getParameter("weight_factor");  // SpikeStream weights are just multiplied

	int gIDpre = wrapper->persistentNeurGrpMap[conGroup->getFromNeuronGroupID()]->getVID();
	int gIDpost = wrapper->persistentNeurGrpMap[conGroup->getToNeuronGroupID()]->getVID();

	CarlsimGeneratorContainer* container = new CarlsimGeneratorContainer(conGroup, wrapper, weightFactor); 
	
	auto conGrpID = wrapper->carlsim->connect(gIDpre, gIDpost, container, learning);

	if (learning) {
		wrapper->carlsim->setESTDP(gIDpre, gIDpost, true);  // ExpCurve not visible, use deprecated 
	}

	wrapper->manage(conGrpID, container); 

}

/*! Adds an excitatory neuron group to the simulation network */
void CarlsimLoader::addExcitatoryNeuronGroup(NeuronGroup* neuronGroup /*, urng_t& ranNumGen*/){

	auto grpInfo = neuronGroup->getInfo();
	QString grpName = grpInfo.getName();

	int n = neuronGroup->size();
	
	auto grpId = wrapper->carlsim->createGroup(grpName.toStdString(), n, EXCITATORY_NEURON); 

	//Extract parameters
	float a = neuronGroup->getParameter("a");
	float b = neuronGroup->getParameter("b");
	float c_1 = neuronGroup->getParameter("c_1");
	float d_1 = neuronGroup->getParameter("d_1");
	float d_2 = neuronGroup->getParameter("d_2");
	float v = neuronGroup->getParameter("v");
	float sigma = neuronGroup->getParameter("sigma");


	qDebug("warning: only param a,b,d_1,v are used"); 

	wrapper->carlsim->setNeuronParameters(grpId, a, b, v, d_1);    
	
	//Set Conductances at Group Level if defined
	auto& parameterMap = grpInfo.getParameterMap();
	if (parameterMap.contains("Conductances")) {
		auto conductances = (bool)parameterMap["Conductances"];
		if (conductances) {
			auto tdAMPA = (int)parameterMap["Conductances.tdAMPA"];
			auto tdNMDA = (int)parameterMap["Conductances.tdNMDA"];
			auto tdGABAa = (int)parameterMap["Conductances.tdGABAa"];
			auto tdGABAb = (int)parameterMap["Conductances.tdGABAb"];
			wrapper->carlsim->setConductances(grpId, true, tdAMPA, tdNMDA, tdGABAa, tdGABAb);
		}
		else {
			wrapper->carlsim->setConductances(grpId, false);
		}
	}

	neuronGroup->setVID(grpId);
	wrapper->volatileNeurGrpMap[grpId] = neuronGroup; //! in case CARLsim gerades random ids in the future, see also partition
	if(wrapper->volatileNeurGrpTable.size()<grpId+1) 
		wrapper->volatileNeurGrpTable.resize(grpId+1); 
	wrapper->volatileNeurGrpTable[grpId] = neuronGroup; 

	// store db id for later processing in wrapper
	wrapper->persistentNeurGrpMap[neuronGroup->getID()] = neuronGroup;
}
	


/*! Adds an inhibitory neuron group to the simulation network */
void CarlsimLoader::addInhibitoryNeuronGroup(NeuronGroup* neuronGroup /*, urng_t& ranNumGen*/){
	
	auto grpInfo = neuronGroup->getInfo();
	QString grpName = grpInfo.getName();

	int n = neuronGroup->size();

	auto grpId = wrapper->carlsim->createGroup(grpName.toStdString(), n, INHIBITORY_NEURON);

	//Extract parameters
	float a_1 = neuronGroup->getParameter("a_1");
	float b_1 = neuronGroup->getParameter("b_1");
	float d = neuronGroup->getParameter("d");
	float v = neuronGroup->getParameter("v");
	
	qDebug("warning: only param a_1,b_1,d,v are used");

	wrapper->carlsim->setNeuronParameters(grpId, a_1, b_1, v, d);

	// Set Conductances at Group Level if defined
	auto & parameterMap = grpInfo.getParameterMap();
	if (parameterMap.contains("Conductances")) {
		auto conductances = (bool)parameterMap["Conductances"];
		if (conductances) {
			auto tdAMPA = (int)parameterMap["Conductances.tdAMPA"];
			auto tdNMDA = (int)parameterMap["Conductances.tdNMDA"];
			auto tdGABAa = (int)parameterMap["Conductances.tdGABAa"];
			auto tdGABAb = (int)parameterMap["Conductances.tdGABAb"];
			wrapper->carlsim->setConductances(grpId, true, tdAMPA, tdNMDA, tdGABAa, tdGABAb);
		}
		else {
			wrapper->carlsim->setConductances(grpId, false);
		}
	}


	neuronGroup->setVID(grpId);
	wrapper->volatileNeurGrpMap[grpId] = neuronGroup; //! in case CARLsim gerades random ids in the future, see also partition
	if (wrapper->volatileNeurGrpTable.size() < grpId + 1) 
		wrapper->volatileNeurGrpTable.resize(grpId + 1);
	wrapper->volatileNeurGrpTable[grpId] = neuronGroup; 

	// store db id for later processing in wrapper
	wrapper->persistentNeurGrpMap[neuronGroup->getID()] = neuronGroup; 

}



/*! featInputNeurons  */
void CarlsimLoader::addDaErgicNeuronGroup(NeuronGroup* neuronGroup /*, urng_t& ranNumGen*/){
}




/*! featInputNeurons,  */
void CarlsimLoader::addCustomExcitatoryNeuronGroup(NeuronGroup* neuronGroup /*, urng_t& ranNumGen*/){


	auto grpInfo = neuronGroup->getInfo();
	QString grpName = grpInfo.getName();

	int n = neuronGroup->size();
	
	auto grpId = wrapper->carlsim->createSpikeGeneratorGroup(grpName.toStdString(), n, EXCITATORY_NEURON); 


	CarlsimSpikeGeneratorContainer* container = new CarlsimSpikeGeneratorContainer(neuronGroup, wrapper);

	wrapper->carlsim->setSpikeGenerator(grpId, (SpikeGenerator*) container->getGenerator()); // ugly hard cast

	//Add neurons to the network

	neuronGroup->setVID(grpId);
	wrapper->volatileNeurGrpMap[grpId] = neuronGroup; 
	if(wrapper->volatileNeurGrpTable.size()<grpId+1) 
		wrapper->volatileNeurGrpTable.resize(grpId+1); 
	wrapper->volatileNeurGrpTable[grpId] = neuronGroup; 

	// store db id for later processing in wrapper
	wrapper->persistentNeurGrpMap[neuronGroup->getID()] = neuronGroup; 
}

/*! featInputNeurons  */
void CarlsimLoader::addCustomInhibitoryNeuronGroup(NeuronGroup* neuronGroup /*, urng_t& ranNumGen*/){
}

/*! featInputNeurons  */
void CarlsimLoader::addPoissonExcitatoryNeuronGroup(NeuronGroup* neuronGroup /*, urng_t& ranNumGen*/){
}

/*! featInputNeurons  */
void CarlsimLoader::addPoissonInhibitoryNeuronGroup(NeuronGroup* neuronGroup /*, urng_t& ranNumGen*/){
}


/*! Prints out information about a particular connection */
void CarlsimLoader::printConnection(unsigned source, unsigned targets[], unsigned delays[], float weights[], unsigned char is_plastic[], size_t length){
	for(size_t i=0; i<length; ++i){
		if(is_plastic[i])
			cout<<"Connection from: "<<source<<"; to: "<<targets[i]<<"; delay: "<<delays[i]<<"; weight: "<<weights[i]<<" is plastic: true"<<endl;
		else
			cout<<"Connection from: "<<source<<"; to: "<<targets[i]<<"; delay: "<<delays[i]<<"; weight: "<<weights[i]<<" is plastic: false"<<endl;
	}
}


