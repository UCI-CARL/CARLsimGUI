//SpikeStream includes
#include "CARLsimCNSWbBuilderThread.h"
#include "Globals.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;

#include <QDebug>


/*! Constructor */
CARLsimCNSWbBuilderThread::CARLsimCNSWbBuilderThread(){
}


/*! Destructor */
CARLsimCNSWbBuilderThread::~CARLsimCNSWbBuilderThread(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/


/*! Prepares class before it runs as a separate thread to add one or more neuron groups */
void CARLsimCNSWbBuilderThread::prepareAddNeuronGroups(
				const unsigned securities, 
				const unsigned states,
				const QString& prefix,

				const unsigned space,
				const int pos_x, 
				const int pos_y, 
				const int pos_z,
				const NeuronParam_t &ps,
				const NeuronParam_t &ls, 
				const NeuronParam_t &tof,
				const NeuronParam_t &vel

	){
	//Run some checks
	if(!Globals::networkLoaded())
		throw SpikeStreamException("Cannot add neuron group - no network loaded.");
	if(Globals::getNetwork()->hasArchives())
		throw SpikeStreamException("Cannot add neuron group to a locked network.\nDelete archives linked with this network and try again");


	this->securities = securities;
	this->states = states;


	this->space = space;

	this->xStart = pos_x;
	this->yStart = pos_y;
	this->zStart = pos_z;

	this->spacing = 1;
	this->density = 1;

	this->ps = ps;
	this->ls = ls;
	this->tof = tof;
	this->vel = vel;

	//Create the neuron groups to be added. A separate neuron group is added for each neuron type
	createNeuronGroups();

}




/*! Thread run method */
void CARLsimCNSWbBuilderThread::run(){
	clearError();
	stopThread = false;
	try{
		//Seed the random number generator
		srand(12345678);

		//Create network and archive dao for this thread
		threadNetworkDao = new NetworkDao(Globals::getNetworkDao()->getDBInfo());
		threadArchiveDao = new ArchiveDao(Globals::getArchiveDao()->getDBInfo());

		//Need to set a new network and archive dao in the network because we are running as a separate thread
		Network* currentNetwork = Globals::getNetwork();

		//Add the neuron groups to the database
		addNeuronGroupsToDatabase();


		if(stopThread)
			return;

		//Wait for network to finish
		while(currentNetwork->isBusy()){
			emit progress(threadNetworkDao->getNeuronCount(newNeuronGroupList), totalNumberOfNeurons, "Adding neurons to database...");
			if(stopThread)
				currentNetwork->cancel();
			msleep(250);
		}

		//Check for errors
		if(currentNetwork->isError())
			setError(currentNetwork->getErrorMessage());

		//Clean up network and archive dao
		delete threadNetworkDao;
		delete threadArchiveDao;
	}
	catch (SpikeStreamException& ex){
		setError(ex.getMessage());
	}
	catch(...){
		setError("An unknown error occurred.");
	}
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds a neuron group with the specified parameters to the database */
void CARLsimCNSWbBuilderThread::addNeuronGroupsToDatabase(){
	//Add the neurons to the neuron groups
	//addNeurons();

	//Add the neuron groups to the network
	Globals::getNetwork()->addNeuronGroups(newNeuronGroupList);
}




/*! Creates a neuron group for each type of neuron.
	NOTE: Only call this within the prepare method. */
void CARLsimCNSWbBuilderThread::createNeuronGroups(){
	//Reset maps
	neuronTypePercentThreshMap.clear();

	//Get all the available neuron types
	//NeuronType excitatoryNeuron = Globals::getNetworkDao()->getNeuronTypes()[0];
	NeuronType excitatoryNeuron = Globals::getNetworkDao()->getNeuronType("Izhikevich Excitatory Neuron");

	NeuronType inhibitoryNeuron = Globals::getNetworkDao()->getNeuronType("Izhikevich Inhibitory Neuron");
	Q_ASSERT(inhibitoryNeuron.getDescription() == "Izhikevich Inhibitory Neuron");

	unsigned group_i = 0;
	totalNumberOfNeurons = 0;

	// Note: This is reused for all groups
	QHash<QString, double> paramMap;
	paramMap["spacing"] = spacing; // 1.0 (default)
	paramMap["density"] = density; // 1.0 = 100% neurons 
	paramMap["percent_neuron_type_id_1"] = 100;


	double securities_space = 2.0; // 2 columns between each security cluster 
	double maxWidth = 0.0;

	QString prefix = "ctx";   


	// Maybe check if neuron group exists, and than set the check in the widget?

	int neurons = 4*2;   // 
	int rows = 2; // x
	int columns = 2;  // y
	int planes = 2; // z

	float xPos, yPos, zPos; 

	float xSpace = 1.0f;
	float ySpace = 1.0f;
	float zSpace = 1.0f;
	

	// ps Neurons  --> Threat assesment
	{
		QString name = QString("%1.%2").arg(prefix).arg("ps"); // 
		QString description = QString("%1 %2 (%3)").arg("Proximity").arg("SensoryCortex").arg("ps");
		paramMap.clear();
		paramMap["Conductances"] = 0.; // false
		//paramMap["NM"] = ps.nm; // nm index to be translated by CARLsim wrapper, 
		NeuronGroup* group = new NeuronGroup(NeuronGroupInfo(0, name, description, paramMap, excitatoryNeuron));   

		// set neuron param for the group itself
		paramMap.clear();
		ps.setExcitatoryParameters(paramMap);
		group->setParameters(paramMap);

		// only one security is supported
		paramMap["x"] = xStart;
		paramMap["y"] = yStart;
		paramMap["z"] = zStart;


		// orintation transfored to movements with sensor x/y    x-axis --> left to right 
		vector<tuple<double, double>> ps = { {0.0, 5.25}, {1.25,7.25}, {4.0,8.0}, {7.25,6.0},  {7.25, 2.0}, {4.0, 0.0}, {1.25, 0.75}, {0.0, 2.75} };

		for (auto iter = ps.begin(); iter < ps.end(); iter++) {
			auto x = xStart + get<0>(*iter);
			auto y = yStart + get<1>(*iter);
			auto z = zStart; 
			group->addNeuron(x, y, z);
			totalNumberOfNeurons++;
		}
		
		newNeuronGroupList.append(group);
	}


	auto _zStart = zStart;
	zStart += 2;
	// ls Neurons  --> Threat assesment
	{
		QString name = QString("%1.%2").arg(prefix).arg("ls"); // 
		QString description = QString("%1 %2 (%3)").arg("Light").arg("Sensory Cortex").arg("ls");
		paramMap.clear();
		paramMap["Conductances"] = 0.; // false
		//paramMap["NM"] = ls.nm; // nm index to be translated by CARLsim wrapper, 
		NeuronGroup* group = new NeuronGroup(NeuronGroupInfo(0, name, description, paramMap, excitatoryNeuron));   // izhi excit -> param bistable

		// set neuron param for the group itself
		paramMap.clear();
		ls.setExcitatoryParameters(paramMap);
		group->setParameters(paramMap);

		// only one security is supported
		paramMap["x"] = xStart;
		paramMap["y"] = yStart;
		paramMap["z"] = zStart;


		// orintation transfored to movements with sensor x/y    x-axis --> left to right 
		vector<tuple<double, double>> ls = { {0.0, 4.0}, {1.25, 7.0}, {4.0, 8.0}, {7.0, 6.5},  {8.0, 4.0}, {7.0, 1.5}, {4.0 , 0.0}, {1.25, 1.0} };

		for (auto iter = ls.begin(); iter < ls.end(); iter++) {
			auto x = xStart + get<0>(*iter);
			auto y = yStart + get<1>(*iter);
			auto z = zStart;
			group->addNeuron(x, y, z);
			totalNumberOfNeurons++;
		}

		newNeuronGroupList.append(group);
	}


	zStart = _zStart;
	zStart -= 3;

	// tof Neurons  
	{
		QString name = QString("%1.%2").arg(prefix).arg("tof"); // 
		QString description = QString("%1 %2 (%3)").arg("ToF").arg("Sensory Cortex").arg("tof");
		paramMap.clear();
		paramMap["Conductances"] = 0.; // false
		//paramMap["NM"] = ps.nm; // nm index to be translated by CARLsim wrapper, 
		NeuronGroup* group = new NeuronGroup(NeuronGroupInfo(0, name, description, paramMap, excitatoryNeuron));   // izhi excit -> param bistable

		// set neuron param for the group itself
		paramMap.clear();
		tof.setExcitatoryParameters(paramMap);
		group->setParameters(paramMap);

		// only one security is supported
		paramMap["x"] = xStart;
		paramMap["y"] = yStart;
		paramMap["z"] = zStart;


		// orientation transformed to movements with sensor x/y    x-axis --> left to right 
		vector<tuple<double, double>> tof = { {-1.0, 4.0} };

		for (auto iter = tof.begin(); iter < tof.end(); iter++) {
			auto x = xStart + get<0>(*iter);
			auto y = yStart + get<1>(*iter);
			for (auto z = zStart; z < zStart + 2; z++) {
				group->addNeuron(x, y, z);
				totalNumberOfNeurons++;
			}
		}

		newNeuronGroupList.append(group);
	}


	zStart = _zStart;
	zStart -= 5;

	// vel Neurons  
	{
		QString name = QString("%1.%2").arg(prefix).arg("vel"); // 
		QString description = QString("%1 %2 (%3)").arg("Velocity").arg("Motor Cortex").arg("vel");
		paramMap.clear();
		paramMap["Conductances"] = 0.; // false
		//paramMap["NM"] = vel.nm; // nm index to be translated by CARLsim wrapper, 
		NeuronGroup* group = new NeuronGroup(NeuronGroupInfo(0, name, description, paramMap, excitatoryNeuron)); 

		// set neuron param for the group itself
		paramMap.clear();
		vel.setExcitatoryParameters(paramMap);
		group->setParameters(paramMap);

		// only one security is supported
		paramMap["x"] = xStart;
		paramMap["y"] = yStart;
		paramMap["z"] = zStart - 5;


		// orintation transfored to movements with sensor x/y    x-axis --> left to right 
		vector<tuple<double, double>> vel = { {4.0, 8.0}, {4.0, 0.0} };

		for (auto iter = vel.begin(); iter < vel.end(); iter++) {
			auto x = xStart + get<0>(*iter);
			auto y = yStart + get<1>(*iter);
			for (auto z = zStart; z < zStart + 2; z++) {
				group->addNeuron(x, y, z);
				totalNumberOfNeurons++;
			}
		}

		newNeuronGroupList.append(group);
	}


}





/*! Returns a parameter from the neuron group info parameter map checking that it actually exists */
double CARLsimCNSWbBuilderThread::getParameter(const QString& paramName, const QHash<QString, double>& paramMap){
	if(!paramMap.contains(paramName))
		throw SpikeStreamException("Parameter with name '" + paramName + "' does not exist in parameter map.");
	return paramMap[paramName];
}


/*! Prints a summary of the neuron group(s) that have been created. */
void CARLsimCNSWbBuilderThread::printSummary(){
	cout<<newNeuronGroupMap.size()<<" neuron groups added. Width: "<<width<< " length: "<<length<<" height: "<<height<<endl;
	for(QHash<unsigned int, NeuronGroup*>::iterator iter = newNeuronGroupMap.begin(); iter != newNeuronGroupMap.end(); ++iter){
		cout<<"Neuron type "<<iter.key()<<" added "<<iter.value()->size()<<" neurons."<<endl;
	}
}


/*! Sets excitatory parameters to match Izhikevich paper */
void CARLsimCNSWbBuilderThread::NeuronParam_t::setExcitatoryParameters(QHash<QString, double>& parameterMap) {
	parameterMap.clear();
	parameterMap["a"] = a; 
	parameterMap["b"] = b; 
	parameterMap["c_1"] = 0.0;
	parameterMap["d_1"] = d; 
	parameterMap["d_2"] = 0.0;
	parameterMap["v"] = c;
	parameterMap["sigma"] = 0.0;
	parameterMap["seed"] = 45;
}


/*! Sets inhibitory parameters to match Izhikevich paper */
void CARLsimCNSWbBuilderThread::NeuronParam_t::setInhibitoryParameters(QHash<QString, double>& parameterMap) {
	parameterMap.clear();
	parameterMap["a_1"] = a; // 0.02;
	parameterMap["a_2"] = 0.0;
	parameterMap["b_1"] = b; // 0.2;
	parameterMap["b_2"] = 0.0;
	parameterMap["d"] = d; // 2.75;
	parameterMap["v"] = c; // -50.0;
	parameterMap["sigma"] = 0.0;
	parameterMap["seed"] = 45;
}