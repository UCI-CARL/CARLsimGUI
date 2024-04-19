#include "carlsim_types.h"

#include "CarlsimLib.h"



using namespace carlsim41;


carlsim_configuration_t carlsim41::carlsim_new_configuration() {
	carlsim_configuration_t conf = new carlsim_configuration();
	conf->cudaDev = 0; 
	conf->backend = carlsim41::CPU_CORES;
	conf->preferredSimMode = CPU_MODE; 
	conf->ithGPUs = 0;
	conf->loggerMode = USER;
	conf->netName = "SpikeStream";
	conf->randSeed = 42;
	return conf;
}

const char*
carlsim41::carlsim_version()
{
	return "4.1/6.0"; 
}

/* We cannot propagate exceptions via the C API, so we catch all and convert to
 * error codes instead. Error descriptions are stored on a per-process basis. */

static std::string g_lastError;
static carlsim41::carlsim_status_t g_lastCallStatus = CARLSIM_OK;  


const char*
carlsim41::carlsim_strerror() {
	return g_lastError.c_str();   
}


void
carlsim41::setResult(const char* msg, carlsim_status_t status) {
	g_lastError = msg;
	g_lastCallStatus = status;
}


carlsim_status_t
carlsim41::carlsim_cuda_device_count(unsigned* count) {
	*count=2; return CARLSIM_OK;
}; 

carlsim_status_t
carlsim41::carlsim_cuda_device_description(unsigned device, const char** descr) {
	*descr = "Dummy"; 
	return CARLSIM_OK;
};



//}