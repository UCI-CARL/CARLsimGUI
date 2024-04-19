//SpikeStream includes
#include "SpikeStreamException.h"
#include "DaSTDPFunction.h"
#include "ConfigLoader.h"
#include "Util.h"
//#include "Globals.h"
using namespace spikestream;

//Other includes
#include <cmath>
#include <iostream>
using namespace std;

//Prints out the arrays when enabled
//#define DEBUG

/*! Constructor */
DaSTDPFunction::DaSTDPFunction() : AbstractSTDPFunction() {

	ConfigLoader* configLoader = new ConfigLoader();
	//ConfigLoader* configLoader = new ConfigLoader(Globals::getConfigFile());

	//Information about parameters
	parameterInfoList.append(ParameterInfo("pre_length", "Length of the pre array", ParameterInfo::INTEGER));
	parameterInfoList.append(ParameterInfo("post_length", "Length of the post array", ParameterInfo::INTEGER));
	parameterInfoList.append(ParameterInfo("A+", "A+ description", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("A-", "A- description", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("T+", "T+ description", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("T-", "T- description", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("min_excitatory_weight", "Minimum weight that excitatory synapse can reach with learning.", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("max_excitatory_weight", "Maximum weight that excitatory synapse can reach with learning.", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("min_inhibitory_weight", "Minimum weight that inhibitory synapse can reach with learning. NOTE: The minimum is defined in ABSOLUTE terms, but the value is negative.", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("max_inhibitory_weight", "Maximum weight that inhibitory synapse can reach with learning. NOTE: The maximum is defined in ABSOLUTE terms, but the value is negative.", ParameterInfo::DOUBLE));

	defaultParameterMap["pre_length"] = Util::getInt(configLoader->getParameter("nemo_dastdp_pre_length", "20"));	
	defaultParameterMap["post_length"] = Util::getInt(configLoader->getParameter("nemo_dastdp_post_length", "20"));
	defaultParameterMap["apply_stdp_interval"] = Util::getInt(configLoader->getParameter("nemo_dastdp_apply_stdp_interval", "10")); 
	defaultParameterMap["reward"] = Util::getDouble(configLoader->getParameter("nemo_dastdp_stdp_reward", "0.01")); 

	parameterInfoList.append(ParameterInfo("da_reward", "DA Reward.", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("syn_decay", "Synaptic Decay Rate.", ParameterInfo::DOUBLE));
	parameterInfoList.append(ParameterInfo("da_decay", "DA rate of decay.", ParameterInfo::DOUBLE));

	defaultParameterMap["da_reward"] = Util::getDouble(configLoader->getParameter("nemo_dastdp_da_reward", "0.1")); 
	defaultParameterMap["syn_decay"] = Util::getDouble(configLoader->getParameter("nemo_dastdp_syn_decay", "0.99967"));  
	defaultParameterMap["da_decay"] = Util::getDouble(configLoader->getParameter("nemo_dastdp_da_decay", "0.97")); 
	
	defaultParameterMap["A+"] = Util::getDouble(configLoader->getParameter("nemo_dastdp_aplus", "0.25"));	
	defaultParameterMap["A-"] = Util::getDouble(configLoader->getParameter("nemo_dastdp_aminus", "0.375"));	
	defaultParameterMap["T+"] = Util::getDouble(configLoader->getParameter("nemo_dastdp_tplus", "13.0"));	
	defaultParameterMap["T-"] = Util::getDouble(configLoader->getParameter("nemo_dastdp_tminus", "15.0"));
	defaultParameterMap["min_excitatory_weight"] = Util::getDouble(configLoader->getParameter("nemo_dastdp_min_excit_weight", "0.001"));
	defaultParameterMap["max_excitatory_weight"] = Util::getDouble(configLoader->getParameter("nemo_dastdp_max_excit_weight", "4.0")); 
	defaultParameterMap["min_inhibitory_weight"] = Util::getDouble(configLoader->getParameter("nemo_dastdp_min_inhib_weight", "-0.001"));
	defaultParameterMap["max_inhibitory_weight"] = Util::getDouble(configLoader->getParameter("nemo_dastdp_max_inhib_weight", "-0.5"));  

	//Initialise current parameter map with default values
	parameterMap = defaultParameterMap;

	//Initialize arrays
	preLength = 0;
	postLength = 0;
	preArray = NULL;
	postArray = NULL;
}


/*! Destructor */
DaSTDPFunction::~DaSTDPFunction(){
	cleanUp();
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Returns the pre array for the specified function.
	Builds the function arrays if this has not been done already. */
float* DaSTDPFunction::getPreArray(){
	checkFunctionUpToDate();
	return preArray;
}


/*! Returns the length of the pre array for the specified function. */
int DaSTDPFunction::getPreLength(){
	checkFunctionUpToDate();
	return preLength;
}


/*! Returns the post array for the specified function.
	Builds the function arrays if this has not been done already. */
float* DaSTDPFunction::getPostArray(){
	checkFunctionUpToDate();
	return postArray;
}


/*! Returns the length of the post array for the specified function. */
int DaSTDPFunction::getPostLength(){
	checkFunctionUpToDate();
	return postLength;
}


/*! Returns the minimum excitatory weight for the specified function. */
float DaSTDPFunction::getMinExcitatoryWeight(){
	return getParameter("min_excitatory_weight");
}


/*! Returns the maximum excitatory weight for the specified function. */
float DaSTDPFunction::getMaxExcitatoryWeight(){
	return getParameter("max_excitatory_weight");
}


/*! Returns the minimum weight for the specified function. */
float DaSTDPFunction::getMinInhibitoryWeight(){
	return getParameter("min_inhibitory_weight");
}


/*! Returns the maximum weight for the specified function. */
float DaSTDPFunction::getMaxInhibitoryWeight(){
	return getParameter("max_inhibitory_weight");
}


/*! Prints out the function */
void DaSTDPFunction::print(){
	checkFunctionUpToDate();

	//Extract parameters
	double aPlus = getParameter("A+");
	double aMinus = getParameter("A-");
	double tPlus = getParameter("T+");
	double tMinus = getParameter("T-");

	cout<<"Standard STDP Function"<<endl;
	cout<<"Parameters. Pre length: "<<preLength<<"; postLength: "<<postLength<<"; A+: "<<aPlus<<"; A-: "<<aMinus<<"; T+: "<<tPlus<<"; T-: "<<tMinus<<endl;
	for(int i=0; i<preLength; ++i)
		cout<<"Pre array ["<<i<<"]: "<<preArray[i]<<endl;
	cout<<endl;
	for(int i=0; i<postLength; ++i)
		cout<<"Post array ["<<i<<"]: "<<postArray[i]<<endl;
	cout<<"Min excitatory weight: "<<getMinExcitatoryWeight()<<"; Max excitatory weight: "<<getMaxExcitatoryWeight()<<"; Min inhibitory weight: "<<getMinInhibitoryWeight()<<"; Max inhibitory weight: "<<getMaxInhibitoryWeight()<<endl;
}


/*!  */
float DaSTDPFunction::getDaReward(){
	return getParameter("da_reward");
}

/*!  */
float DaSTDPFunction::getDaDecay(){
	return getParameter("da_decay");
}

/*!  */
float DaSTDPFunction::getSynDecay(){
	return getParameter("syn_decay");
}



/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Builds the standard STDP function and adds it to the maps. */
void DaSTDPFunction::buildDaSTDPFunction(){
	//Delete previous arrays if allocated
	cleanUp();

	//Extract parameters
	preLength = (int)getParameter("pre_length");
	postLength = (int)getParameter("post_length");
	double aPlus = getParameter("A+");
	double aMinus = getParameter("A-");
	double tPlus = getParameter("T+");
	double tMinus = getParameter("T-");

	//Build the arrays specifying the function
	preArray = new float[preLength];
	for(int i = 0; i < preLength; ++i) {
		float dt = float(i + 1);
		preArray[i] = aPlus * expf( (-1.0 * dt) / tPlus);
	}
	postArray = new float[postLength];
	for(int i = 0; i < postLength; ++i) {
		float dt = float(i + 1);
		postArray[i] = -1.0 * aMinus * expf( (-1.0 * dt) / tMinus);
	}

	#ifdef DEBUG
		print();
	#endif//DEBUG
}


/*! Checks to see if functions have been built and builds them if not. */
void DaSTDPFunction::checkFunctionUpToDate(){
	if(functionUpToDate)
		return;
	buildDaSTDPFunction();
	functionUpToDate = true;
}


/*! Deletes dynamically allocated memory */
void DaSTDPFunction::cleanUp(){
	if(preArray != NULL)
		delete [] preArray;
	preArray = NULL;
	if(postArray != NULL)
		delete [] postArray;
	postArray = NULL;
}






