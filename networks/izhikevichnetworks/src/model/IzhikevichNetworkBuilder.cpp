//SpikeStream includes
#include "Globals.h"
#include "IzhikevichNetworkBuilder.h"
#include "NetworkInfo.h"
#include "NeuronGroupInfo.h"
#include "NeuronGroup.h"
#include "SpikeStreamException.h"
#include "SpikeStreamIOException.h"
#include "Util.h"
using namespace spikestream;

//Other includes
#include <algorithm>
using namespace std;

/*! Random integer between 0 and max-1 */
#define getrandom(max1) ((rand()%(int)((max1))))

/*! Enables debuggging output */
//#define DEBUG


/*! Constructor */
IzhikevichNetworkBuilder::IzhikevichNetworkBuilder(){
	//Initialize variables
	currentTask = 0;
	newNetwork = NULL;
}


/*! Destructor */
IzhikevichNetworkBuilder::~IzhikevichNetworkBuilder(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

//Inherited from QThread
void IzhikevichNetworkBuilder::run(){
	//Initialize thread variables
	clearError();
	stopThread = false;

	try{
		//Check network has been set
		if(newNetwork == NULL)
			throw SpikeStreamException("Network has not been set.");

		//Execute appropriate method
		switch (currentTask){
			case ADD_POLYCHRONIZATION_NETWORK_TASK:
				addPolychronizationNetwork();
			break;
			default:
				throw SpikeStreamException ("Task not defined or not recognized");
		}
	}
	catch(SpikeStreamException& ex){
		setError(ex.getMessage());
	}
	catch(...){
		setError("Unknown exception thrown by IzhikevichNetworkBuilder::run()");
	}
	//Finish off
	stopThread = true;
	currentTask = NO_TASK_DEFINED;
}


/*! Launches thread that adds polychronization network based on Izhikevich E.M. (2006). Polychronization: Computation With Spikes. Neural Computation,18:245-282
	Code based on Izhikevich's code: http://www.izhikevich.org/publications/spnet.htm */
void IzhikevichNetworkBuilder::startAddPolychronizationNetwork(Network* newNetwork){
	this->newNetwork = newNetwork;
	currentTask = ADD_POLYCHRONIZATION_NETWORK_TASK;
	start();
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds parity gate training to the neuron.
    A parity gate fires if it receives an odd number of spikes. */
void IzhikevichNetworkBuilder::addPolychronizationNetwork(){
	//Variables
	const	int		Ne = 800;		// excitatory neurons
	const	int		Ni = 200;		// inhibitory neurons
	const	int		N  = Ne+Ni;		// total number of neurons
	const	int		M  = 100;		// the number of synapses per neuron
	const	int		D  = 20;		// maximal axonal conduction delay
	int		post[N][M];				// indeces of postsynaptic neurons
//	float	s[N][M], sd[N][M];		// matrix of synaptic weights and their derivatives
//	short	delays_length[N][D];	// distribution of delays
//	short	delays[N][D][M];		// arrangement of delays

	#ifdef DEBUG
		QTextStream* logTextStream;
		QFile* logFile = new QFile(Globals::getSpikeStreamRoot() + "/log/IzhikevichNetworkBuilder.log");
		if(logFile->open(QFile::WriteOnly | QFile::Truncate))
			logTextStream = new QTextStream(logFile);
		else{
			throw SpikeStreamIOException("Cannot open log file for IzhikevichNetworkBuilder.");
		}
	#endif//DEBUG

	//Get neuron types
	NetworkDao networkDao(Globals::getNetworkDao()->getDBInfo());
	NeuronType exNeurType = networkDao.getNeuronType("Izhikevich Excitatory Neuron");
	NeuronType inhibNeurType = networkDao.getNeuronType("Izhikevich Inhibitory Neuron");

	//Get synapse type and default parameter maps for connections
	SynapseType synapseType = networkDao.getSynapseType("Izhikevich Synapse");
	QHash<unsigned, QHash<QString, double> > defaultParameterMaps;//The key is the synapse type ID. The value is the map of default parameters.
	QList<SynapseType> synTypeList = networkDao.getSynapseTypes();
	for(int i=0; i<synTypeList.size(); ++i)
		defaultParameterMaps[synTypeList.at(i).getID()] = networkDao.getDefaultSynapseParameters(synTypeList.at(i).getID());

	//Initialize seed
	int randomSeed = 45;
	srand(randomSeed);

	//Create connection pattern
	int exists, r;
	for (int i=0;i<N;i++){//Work through neurons
		for (int j=0;j<M;j++) {//Work through all connections per neuron
			do {
				exists = 0;		// avoid multiple synapses

				//Excitatory neurons connect to any neurons
				if (i<Ne)
					r = getrandom(N);

				//Inhibitory neurons only connect to excitatory neurons
				else
					r = getrandom(Ne);// inh -> exc only

				//Check for self connections
				if (r==i)
					exists=1;									// no self-synapses

				//Check to see if synapse already exists
				for (int k=0;k<j;k++)
					if (post[i][k]==r)
						exists = 1;	// synapse already exists
			}
			while (exists == 1);
			post[i][j]=r;//Neuron number i, connecion number j connects to neuron number r
		}
	}

	//Create excitatory and inhibitory neuron groups
	QHash<QString, double> parameterMap;
	NeuronGroup* exNeurGrp = new NeuronGroup(NeuronGroupInfo(0, "Excitatory neuron group", "Excitatory neuron group", parameterMap, exNeurType));
	setExcitatoryParameters(parameterMap);
	exNeurGrp->setParameters(parameterMap);

	parameterMap.clear();
	NeuronGroup* inhibNeurGrp = new NeuronGroup(NeuronGroupInfo(0, "Inhibitory neuron group",  "Inhibitory neuron group", parameterMap, inhibNeurType));
	setInhibitoryParameters(parameterMap);
	inhibNeurGrp->setParameters(parameterMap);

	//Create a group of excitatory neurons
	//float xStart = 0.0f, yStart = 0.0f, zStart = 0.0f;
	float xStart = 5.0f, yStart = 0.0f, zStart = 0.0f;
	float xSpacing = 1.0f, ySpacing = 1.0f, zSpacing = 1.0f;
	float xPos, yPos, zPos;
	unsigned numXNeur = 10, numYNeur = 10, numZNeur = 8, neurCtr = 0;
	Neuron* neuronArray[N];
	for(unsigned xCntr = 0; xCntr < numXNeur; ++xCntr){
		for(unsigned yCntr = 0; yCntr < numYNeur; ++yCntr){
			for(unsigned zCntr = 0; zCntr < numZNeur; ++zCntr){
				xPos = xStart + xCntr*xSpacing;
				yPos = yStart + yCntr*ySpacing;
				zPos = zStart + zCntr*zSpacing;
				neuronArray[neurCtr] = exNeurGrp->addNeuron(xPos, yPos, zPos);
				#ifdef DEBUG
					(*logTextStream)<<"nemo_add_neuron(nemoNetwork, "<<neurCtr<<", 0.02, 0.2, -65, 8, -13, -65, 0);"<<endl;
				#endif//DEBUG
				++neurCtr;
			}
		}
	}

	//Create a group of inhibitory neurons
	//xStart = 0.0f;
	xStart = 7.0f;
	yStart = 0.0f;
	zStart = 12.0f;
	numXNeur = 8;
	numYNeur = 5;
	numZNeur = 5;
	for(unsigned xCntr = 0; xCntr < numXNeur; ++xCntr){
		for(unsigned yCntr = 0; yCntr < numYNeur; ++yCntr){
			for(unsigned zCntr = 0; zCntr < numZNeur; ++zCntr){
				xPos = xStart + xCntr*xSpacing;
				yPos = yStart + yCntr*ySpacing;
				zPos = zStart + zCntr*zSpacing;
				neuronArray[neurCtr] = inhibNeurGrp->addNeuron(xPos, yPos, zPos);
				#ifdef DEBUG
					(*logTextStream)<<"nemo_add_neuron(nemoNetwork, "<<neurCtr<<", 0.1, 0.2, -65, 2, -13, -65, 0);"<<endl;
				#endif//DEBUG
				++neurCtr;
			}
		}
	}

	//Add neuron groups to network
	QList<NeuronGroup*> neuronGroupList;
	neuronGroupList.append(exNeurGrp);
	neuronGroupList.append(inhibNeurGrp);
	newNetwork->addNeuronGroups(neuronGroupList);

	//Create connection groups
	parameterMap.clear();//Just need an empty parameters for the NeuronGroupInfo
	ConnectionGroup* excitExcitConGrp = new ConnectionGroup(
			ConnectionGroupInfo(0, getConGrpDescription(exNeurGrp, exNeurGrp), exNeurGrp->getID(), exNeurGrp->getID(), parameterMap, synapseType) );
	QHash<QString, double> conParamMap = defaultParameterMaps[synapseType.getID()];
	conParamMap["Learning"] = 1.0;
	conParamMap["weight_factor"] = 10.0;
	excitExcitConGrp->setParameters(conParamMap);

	ConnectionGroup* excitInhibConGrp = new ConnectionGroup(
			ConnectionGroupInfo(0, getConGrpDescription(exNeurGrp, inhibNeurGrp), exNeurGrp->getID(), inhibNeurGrp->getID(), parameterMap, synapseType) );
	excitInhibConGrp->setParameters(conParamMap);

	ConnectionGroup* inhibExcitConGrp = new ConnectionGroup(
			ConnectionGroupInfo(0, getConGrpDescription(inhibNeurGrp, exNeurGrp), inhibNeurGrp->getID(), exNeurGrp->getID(), parameterMap, synapseType) );
	conParamMap["Learning"] = 0.0;
	inhibExcitConGrp->setParameters(conParamMap);

	//Add connections
	int mOverD = M/D, toNeurIdx;
	for(int fromNeurIdx=0; fromNeurIdx<N; ++fromNeurIdx){
		for (int conIdx=0; conIdx<M;++conIdx){//Work through all the connections
			//Find index of to connection
			toNeurIdx = post[fromNeurIdx][conIdx];

			//Couple of checks
			if(toNeurIdx >= N)
				throw SpikeStreamException("To neuron index is out of range: " + QString::number(toNeurIdx));

			if(fromNeurIdx < Ne && toNeurIdx < Ne){
				excitExcitConGrp->addConnection(neuronArray[fromNeurIdx]->getID(), neuronArray[toNeurIdx]->getID(), 1 + conIdx / mOverD, 0.6);
				#ifdef DEBUG
					(*logTextStream)<<"nemo_add_synapse(nemoNetwork, "<<fromNeurIdx<<", "<<toNeurIdx<<", "<<(1 + conIdx / mOverD)<<", 6, 1, &newNemoSynapseID);"<<endl;
				#endif//DEBUG
			}
			else if(fromNeurIdx < Ne && toNeurIdx >= Ne){
				excitInhibConGrp->addConnection(neuronArray[fromNeurIdx]->getID(), neuronArray[toNeurIdx]->getID(), 1+ conIdx / mOverD, 0.6);
				#ifdef DEBUG
					(*logTextStream)<<"nemo_add_synapse(nemoNetwork, "<<fromNeurIdx<<", "<<toNeurIdx<<", "<<(1 + conIdx / mOverD)<<", 6, 1, &newNemoSynapseID);"<<endl;
				#endif//DEBUG
			}
			else if(fromNeurIdx >= Ne && toNeurIdx < Ne){
				inhibExcitConGrp->addConnection(neuronArray[fromNeurIdx]->getID(), neuronArray[toNeurIdx]->getID(), 1, -0.5);
				#ifdef DEBUG
					(*logTextStream)<<"nemo_add_synapse(nemoNetwork, "<<fromNeurIdx<<", "<<toNeurIdx<<", 1, -5, 0, &newNemoSynapseID);"<<endl;
				#endif//DEBUG
			}
			else
				throw SpikeStreamException("Condition not recognized.");
		}
	}

	//Add connection groups to network
	QList<ConnectionGroup*> conGroupList;
	conGroupList.append(excitExcitConGrp);
	conGroupList.append(excitInhibConGrp);
	conGroupList.append(inhibExcitConGrp);
	newNetwork->addConnectionGroups(conGroupList);

	//Clean up logging
	#ifdef DEBUG
		logFile->close();
		delete logFile;
		delete logTextStream;
	#endif//DEBUG
}


/*! Returns an automatically generated description of a connection between two neuron groups */
QString IzhikevichNetworkBuilder::getConGrpDescription(NeuronGroup* fromNeuronGroup, NeuronGroup* toNeuronGroup){
	QString fromGrpStr = fromNeuronGroup->getInfo().getName();
	if(fromNeuronGroup->getInfo().getNeuronType().getDescription().contains("Excitatory", Qt::CaseInsensitive))
		fromGrpStr += " (Excit)";
	else if(fromNeuronGroup->getInfo().getNeuronType().getDescription().contains("Inhibitory", Qt::CaseInsensitive))
		fromGrpStr += " (Inhib)";
	else
		throw SpikeStreamException("Neuron type not recognized.");
	QString toGrpStr = toNeuronGroup->getInfo().getName();
	if(toNeuronGroup->getInfo().getNeuronType().getDescription().contains("Excitatory", Qt::CaseInsensitive))
		toGrpStr += " (Excit)";
	else if(toNeuronGroup->getInfo().getNeuronType().getDescription().contains("Inhibitory", Qt::CaseInsensitive))
		toGrpStr += " (Inhib)";
	else
		throw SpikeStreamException("Neuron type not recognized.");
	return QString(fromGrpStr + "->" + toGrpStr);
}


/*! Sets excitatory parameters to match Izhikevich paper */
void IzhikevichNetworkBuilder::setExcitatoryParameters(QHash<QString, double>& parameterMap){
	parameterMap.clear();
	parameterMap["a"] = 0.02;
	parameterMap["b"] = 0.2;
	parameterMap["c_1"] = 0.0;
	parameterMap["d_1"] = 8.0;
	parameterMap["d_2"] = 0.0;
	parameterMap["v"] = -65.0;
	parameterMap["sigma"] = 0.0;
	parameterMap["seed"] = 45;
}


/*! Sets inhibitory parameters to match Izhikevich paper */
void IzhikevichNetworkBuilder::setInhibitoryParameters(QHash<QString, double>& parameterMap){
	parameterMap.clear();
	parameterMap["a_1"] = 0.1;
	parameterMap["a_2"] = 0.0;
	parameterMap["b_1"] = 0.2;
	parameterMap["b_2"] = 0.0;
	parameterMap["d"] = 2;
	parameterMap["v"] = -65.0;
	parameterMap["sigma"] = 0.0;
	parameterMap["seed"] = 45;
}



