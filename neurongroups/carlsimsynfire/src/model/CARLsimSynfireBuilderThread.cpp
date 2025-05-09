//SpikeStream includes
#include "CARLsimSynfireBuilderThread.h"
#include "Globals.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;

#include <QDebug>


/*! Constructor */
CARLsimSynfireBuilderThread::CARLsimSynfireBuilderThread(){
}


/*! Destructor */
CARLsimSynfireBuilderThread::~CARLsimSynfireBuilderThread(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/


/*! Prepares class before it runs as a separate thread to add one or more neuron groups */
void CARLsimSynfireBuilderThread::prepareAddNeuronGroups(

				//const QString& name, 
				//const QString& description,

				const QString& prefix,

				const unsigned segments,

				const int exc_columns,
				const int exc_rows,

				const int inh_columns,
				const int inh_rows,

				const NeuronParam_t &exc,
				const NeuronParam_t &inh

	){
	//Run some checks
	if(!Globals::networkLoaded())
		throw SpikeStreamException("Cannot add neuron group - no network loaded.");
	if(Globals::getNetwork()->hasArchives())
		throw SpikeStreamException("Cannot add neuron group to a locked network.\nDelete archives linked with this network and try again");

	//this->name = name;
	//this->description = description;

	this->prefix = prefix;

	this->segments = segments;
	//this->states = states;
	this->exc_columns = exc_columns;
	this->exc_rows = exc_rows;

	this->inh_columns = inh_columns;
	this->inh_rows = inh_rows;

	// defauls from gui ?

	this->space = 1;

	this->xStart = 1;
	this->yStart = 1;
	this->zStart = 2;

	this->spacing = 1;
	this->density = 1;

	this->exc = exc;
	this->inh = inh;

	//Create the neuron groups to be added. A separate neuron group is added for each neuron type
	createNeuronGroups();

}




/*! Thread run method */
void CARLsimSynfireBuilderThread::run(){
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
void CARLsimSynfireBuilderThread::addNeuronGroupsToDatabase(){
	//Add the neurons to the neuron groups
	//addNeurons();

	//Add the neuron groups to the network
	Globals::getNetwork()->addNeuronGroups(newNeuronGroupList);
}




/*! Creates a neuron group for each type of neuron.
	NOTE: Only call this within the prepare method. */
void CARLsimSynfireBuilderThread::createNeuronGroups() {
	//Reset maps
	neuronTypePercentThreshMap.clear();

	//Get all the available neuron types
	//NeuronType excitatoryNeuron = Globals::getNetworkDao()->getNeuronTypes()[0];
	NeuronType excitatoryNeuron = Globals::getNetworkDao()->getNeuronType("Izhikevich Excitatory Neuron");

	NeuronType inhibitoryNeuron = Globals::getNetworkDao()->getNeuronType("Izhikevich Inhibitory Neuron");
	Q_ASSERT(inhibitoryNeuron.getDescription() == "Izhikevich Inhibitory Neuron");

	NeuronType customNeuron = Globals::getNetworkDao()->getNeuronType("Custom Excitatory Neuron");
	// --> CUSTOM_EXCITATORY_NEURON_ID
	//  <Injector type="Fire"> 
	// --> SpikeMon
	// Not Jet Implemented !!!


	unsigned group_i = 0;
	totalNumberOfNeurons = 0;

	// Note: This is reused for all groups
	QHash<QString, double> paramMap;
	paramMap["spacing"] = spacing; // 1.0 (default)
	paramMap["density"] = density; // 1.0 = 100% neurons 
	paramMap["percent_neuron_type_id_1"] = 100;


	double securities_space = 2.0; // 2 columns between each security cluster 
	double maxWidth = 0.0;

	//QString prefix = "G";     // get this from the parameters


	// Maybe check if neuron group exists, and than set the check in the widget?

	//int neurons = 4*2;   // 
	//int rows = 2; // x
	//int columns = 2;  // y
	//int planes = 2; // z

	float xPos, yPos, zPos;

	float xSpace = 1.0f;
	float ySpace = 1.0f;
	float zSpace = 1.0f;

	float zSpace_exc_inh = 3.0;

	//// Debug former:  unittest18d_epuck_gui.config !!!
	//// 
	//// start with hard wired parameter for 2 segments 
	//// Loop over Segments  -> param  
	//// columns = 5
	//// rows = 20 for exc   total -> GUI readonly
	//// rows = 5 for inh    total -> GUI readonly 
	//// 
	//int segments = 10;

	//int exc_columns = 10;
	//int exc_rows = 10;

	//int inh_columns = 5;
	//int inh_rows = 5;

	// ISSUE: index by 0 or 1 ???  -> CS: 0  NS/Math: 1  ???  paper diverge => primary paparer is Hepnre / Mayr => 0 


	// stim neurons - Spike Gen Group 
	{
		auto _zStart = zStart; // save
		zStart += zSpace_exc_inh;

		QString name = QString("%1%2").arg(prefix).arg("stim"); // i_segment
		QString description = QString("%1 %2").arg("Synfire").arg("stim");
		paramMap.clear();
		//paramMap["Custom"] = 1.;   --> shoud be defined by neuron type custom

		NeuronGroup* group = new NeuronGroup(NeuronGroupInfo(0, name, description, paramMap, customNeuron));  // TODO: NJI
		//NeuronGroup* group = new NeuronGroup(NeuronGroupInfo(0, name, description, paramMap, excitatoryNeuron));
		
		// set neuron param for the group itself
		paramMap.clear();

		//exc.setExcitatoryParameters(paramMap);
		/// ????
		//stim.setExcitatoryParameters(paramMap);
		//paramMap["mean"] = 10.0;    // stim_u
		//paramMap["sd"] = 2.8;		  // stim_sd
		//paramMap["spikes"] = 400;	  // stim_n

		group->setParameters(paramMap);

		// only one security is supported
		paramMap["x"] = xStart;
		paramMap["y"] = yStart;
		paramMap["z"] = zStart;


		// Keep on the ground due Paper 
		// align to the back
		auto x = xStart + inh_rows + 1;  // shift right    
		auto z = zStart;
		for (int row = 1; row <= exc_rows; row++) {
			auto y = yStart;
			for (int col = 1; col <= exc_columns; col++) {
				group->addNeuron(x, y, z);
				totalNumberOfNeurons++;
				y++; // back
			}
			z++;  // up
		}

		newNeuronGroupList.append(group);

		zStart = _zStart; // restore

		xStart += inh_rows / 2;   // Add extra distance from stim to present chain more clearly.
	}


	for (int i_segment = 0; i_segment < segments; i_segment++) {

		xStart += inh_rows + 1 + inh_rows;  

		// exc Neurons  --> 
		auto _zStart = zStart; // save
		zStart += zSpace_exc_inh;
		{
			// TODO 
			QString name = QString("%1%2%3").arg(prefix).arg("exc").arg(i_segment); // i_segment
			QString description = QString("%1 %2 (%3)").arg("Synfire").arg("Exc").arg(i_segment);  
			paramMap.clear();
			paramMap["Conductances"] = 0.; // TODO 

			//// TODO  provide param for ... nS  see papaer
			//paramMap["Conductances"] = 1.; // 
			// [USER ERROR setConductances(1,1,0,0,0,0)] tdAMPA must be positive.
			//auto conductances = (bool)parameterMap["Conductances"];
			//if (conductances) {
			//	auto tdAMPA = (int)parameterMap["Conductances.tdAMPA"];
			//	auto tdNMDA = (int)parameterMap["Conductances.tdNMDA"];
			//	auto tdGABAa = (int)parameterMap["Conductances.tdGABAa"];
			//	auto tdGABAb = (int)parameterMap["Conductances.tdGABAb"];
				

			NeuronGroup* group = new NeuronGroup(NeuronGroupInfo(0, name, description, paramMap, excitatoryNeuron));

			// set neuron param for the group itself
			paramMap.clear();
			exc.setExcitatoryParameters(paramMap);
			group->setParameters(paramMap);

			// only one security is supported
			paramMap["x"] = xStart;
			paramMap["y"] = yStart;
			paramMap["z"] = zStart;


			// Keep on the ground due Paper 
			// align to the back
			auto x = xStart + inh_rows + 1;  // shift right
			auto z = zStart;
			for (int row = 1; row <= exc_rows; row++) {
				auto y = yStart;
				for (int col = 1; col <= exc_columns; col++) {
					group->addNeuron(x, y, z);
					totalNumberOfNeurons++;
					y++; // back
				}
				z++;  // up
			}

			newNeuronGroupList.append(group);
		}
		zStart = _zStart; // restore


		// inh neurons 
		{
			QString name = QString("%1%2%3").arg(prefix).arg("inh").arg(i_segment); // i_segment
			QString description = QString("%1 %2 (%3)").arg("Synfire").arg("Inh").arg(i_segment);   // i_segment
			paramMap.clear();
			paramMap["Conductances"] = 0.; // see above 
		
			NeuronGroup* group = new NeuronGroup(NeuronGroupInfo(0, name, description, paramMap, inhibitoryNeuron));   // izhi excit -> param bistable

			// set neuron param for the group itself
			paramMap.clear();
			inh.setInhibitoryParameters(paramMap);
			group->setParameters(paramMap);

			// only one security is supported
			paramMap["x"] = xStart;
			paramMap["y"] = yStart;
			paramMap["z"] = zStart;


			auto x = xStart;
			auto z = zStart;
			for (int row = 1; row <= inh_rows; row++) {
				auto y = yStart + (exc_columns - inh_columns) / 2.0; 
				for (int col = 1; col <= inh_columns; col++) {
					group->addNeuron(x, y, z);
					totalNumberOfNeurons++;
					y++; // back
				}
				x++;  // up
			}

			newNeuronGroupList.append(group);
		}

	}

}





/*! Returns a parameter from the neuron group info parameter map checking that it actually exists */
double CARLsimSynfireBuilderThread::getParameter(const QString& paramName, const QHash<QString, double>& paramMap){
	if(!paramMap.contains(paramName))
		throw SpikeStreamException("Parameter with name '" + paramName + "' does not exist in parameter map.");
	return paramMap[paramName];
}


/*! Prints a summary of the neuron group(s) that have been created. */
void CARLsimSynfireBuilderThread::printSummary(){
	cout<<newNeuronGroupMap.size()<<" neuron groups added. Width: "<<width<< " length: "<<length<<" height: "<<height<<endl;
	for(QHash<unsigned int, NeuronGroup*>::iterator iter = newNeuronGroupMap.begin(); iter != newNeuronGroupMap.end(); ++iter){
		cout<<"Neuron type "<<iter.key()<<" added "<<iter.value()->size()<<" neurons."<<endl;
	}
}


/*! Sets excitatory parameters to match Izhikevich paper */
void CARLsimSynfireBuilderThread::NeuronParam_t::setExcitatoryParameters(QHash<QString, double>& parameterMap) {
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
void CARLsimSynfireBuilderThread::NeuronParam_t::setInhibitoryParameters(QHash<QString, double>& parameterMap) {
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



/*! Sets inhibitory parameters to match Izhikevich paper */
void CARLsimSynfireBuilderThread::CustomNeuronParam_t::setCustomParameters(QHash<QString, double>& parameterMap) {
	parameterMap.clear();

	/* CAUTION this is configured outside   overload with experiment param
	auto grpId = wrapper->carlsim->createSpikeGeneratorGroup(grpName.toStdString(), n, EXCITATORY_NEURON);
	CarlsimSpikeGeneratorContainer* container = new CarlsimSpikeGeneratorContainer(neuronGroup, wrapper);
	wrapper->carlsim->setSpikeGenerator(grpId, (SpikeGenerator*)container->getGenerator()); // ugly hard cast
	*/


	// translate to SpikeGen CARLsim loader 
	parameterMap["mean"] = mean;
	parameterMap["sd"] = sd;
	parameterMap["spikes"] = spikes;

}