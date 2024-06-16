#ifndef CARLSIMTYPES_H
#define CARLSIMTYPES_H

/*! light weight header c style
	integration of CARLsim
*/

typedef int synapse_id;  // CARLsim definition of synapse_id  

typedef unsigned backend_t;

#include <string>


/*! The call resulted in no errors */
#define CARLSIM_OK 0

// Warning redefinition
///*! The CUDA driver reported an error */
//#define CARLSIM_ERROR 1



// copied all releavant information to ensure problem free integration in spike stream 
namespace carlsim41 {

enum LoggerMode {
	 USER,            //!< User mode, for experiment-oriented simulations.
	 DEVELOPER,       //!< Developer mode, for developing and debugging code.
	 SHOWTIME,        //!< Showtime mode, will only output warnings and errors.
	 SILENT,          //!< Silent mode, no output is generated.
	 CUSTOM,          //!< Custom mode, the user can set the location of all the file pointers.
	 UNKNOWN_LOGGER
};
static const char* loggerMode_string[] = {
	"USER","DEVELOPER","SHOWTIME","SILENT","CUSTOM","Unknown mode"
};


/*!
	* \brief computing backend
	* 
	* CARLsim supports execution on standard x86 CPU Cores or off-the-shelf NVIDIA GPU (CUDA Cores) 
	*/
enum ComputingBackend {
	CPU_CORES, //!< x86/x64 Multi Core Processor 
	GPU_CORES  //!< NVIDIA Many CUDA Core Processor
};

static const char* ComputingBackend_string[] = {
	"x86/x64 Multi Core Processor", 
	"NVIDIA Many CUDA Core Processor",
};

enum SimMode {
	CPU_MODE,     //!< model is run on CPU core(s)
	GPU_MODE,     //!< model is run on GPU card(s)
	HYBRID_MODE   //!< model is run on CPU Core(s), GPU card(s) or both
};
static const char* simMode_string[] = {
	"CPU mode","GPU mode","Hybrid mode"
};


struct carlsim_configuration {
	SimMode preferredSimMode; // for Consturctor 
	ComputingBackend backend;  
	int cudaDev;  //
	int ithGPUs;
	int randSeed;
	LoggerMode loggerMode;
	std::string netName;

} ; 

typedef carlsim_configuration* carlsim_configuration_t; 

carlsim_configuration_t carlsim_new_configuration();

const char* carlsim_version();


/*!
 * \brief CARLsim states
 *
 * A CARLsim simulation goes through the following states:
 * ::CONFIG_STATE   configuration state, where the neural network is configured
 * ::SETUP_STATE    setup state, where the neural network is prepared for execution
 * ::RUN_STATE      run state, where the simulation is executed
 * Certain methods can only be called in certain states. Check their documentation to see which method can be called
 * in which state.
 * 
 * Certain methods perform state transitions. CARLsim::setupNetwork will change the state from ::CONFIG_STATE to
 * ::SETUP_STATE. The first call to CARLsim::runNetwork will change the state from ::SETUP_STATE to ::RUN_STATE.
 */
enum CARLsimState {
	CONFIG_STATE,		//!< configuration state, where the neural network is configured
	SETUP_STATE,		//!< setup state, where the neural network is prepared for execution and monitors are set
	RUN_STATE			//!< run state, where the model is stepped
};
static const char* carlsimState_string[] = {
	"Configuration state", "Setup state", "Run state"
};

/*! Status of API calls which can fail. */
typedef int carlsim_status_t;

void setResult(const char* msg, carlsim_status_t status);

const char* carlsim_strerror();

carlsim_status_t
carlsim_cuda_device_count(unsigned* count); 

carlsim_status_t
carlsim_cuda_device_description(unsigned device, const char** descr);

}


#endif //CARLSIMTYPES
