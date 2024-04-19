#ifndef CARLSIMLIB_H
#define CARLSIMLIB_H

//SpikeStream includes

//Qt includes
#include <QObject>

// 
#include "api.h" 

#include <vector>

namespace carlsim {
//#include "carlsim.h"
//#include "carlsim_definitions.h"
}

#include "carlsim_conf.h"
#include "carlsim_definitions.h"
#include "carlsim_datastructures.h"
#include "carlsim_types.h"  // new file


class SpikeMonitor; 
class ConnectionMonitor;
class NeuronMonitor;
class SpikeGenerator;
class GroupMonitor;

namespace spikestream {

	class Carlsim;
	class CarlsimGeneratorContainer;	// forward declarations


	/*! Graphical interface for the plugin wrapping the spiking neural simulator */
	class CARLSIMWRAPPER_LIB_EXPORT CarlsimLib {
		//Q_OBJECT

		public:
			CarlsimLib();
			CarlsimLib(const std::string& netName, SimMode preferredSimMode, LoggerMode loggerMode, int ithGPUs, int randSeed);
			~CarlsimLib();

			// INIT
			bool loadLib(); 

			// ALL STATES
			CARLsimState getCARLsimState();
			static int cudaDeviceCount(); //! Fix for integration 
			static void cudaDeviceDescription(unsigned ithGPU, const char **desc); //! Fix for integration

			// CONFIG STATE
			int connect(int grpId1, int grpId2, CarlsimGeneratorContainer* conn, bool synWtType);
			void setESTDP(int preGrpId, int postGrpId, bool isSet); // , STDPType type, ExpCurve curve);

			void setIntegrationMethod(integrationMethod_t method, int numStepsPerMs);


			int createGroup(const std::string &grpName, int n, int neurType);
			int createGroup(const std::string &grpName, Grid3D grid, int neurType, int preferredPartition, ComputingBackend preferredBackend); 
			int createSpikeGeneratorGroup(const std::string& grpName, int nNeur, int neurType, int preferredPartition = ANY, ComputingBackend preferredBackend = CPU_CORES);
			int createSpikeGeneratorGroup(const std::string& grpName, const Grid3D& grid, int neurType, int preferredPartition = ANY, ComputingBackend preferredBackend = CPU_CORES);

			void setNeuronParameters(int grpId, float izh_a, float izh_b, float izh_c, float izh_d);
			void setConductances(bool isSet);
			void setConductances(bool isSet, int tdAMPA, int tdNMDA, int tdGABAa, int tdGABAb);
			void setConductances(bool isSet, int tdAMPA, int trNMDA, int tdNMDA, int tdGABAa, int trGABAb, int tdGABAb);
			void setConductances(int grpId, bool isSet);
			void setConductances(int grpId, bool isSet, int tdAMPA, int tdNMDA, int tdGABAa, int tdGABAb);
			void setConductances(int grpId, bool isSet, int tdAMPA, int trNMDA, int tdNMDA, int tdGABAa, int trGABAb, int tdGABAb);

			void setExternalCurrent(int grpId, float current);
			void setExternalCurrent(int grpId, std::vector<float> currents);
			void setSpikeGenerator(int grpId, SpikeGenerator* spikeGenFunc);
			SpikeMonitor* setSpikeMonitor(int grpId, const std::string& fileName);
			NeuronMonitor* setNeuronMonitor(int grpId, const std::string& fileName);
			GroupMonitor* setGroupMonitor(int grpId, const std::string& fileName);

			void saveSimulation(const std::string& fileName, bool saveSynapseInfo = true);
			void setDefaultSaveOptions(std::string fileName, bool saveSynapseInfo);

			void setupNetwork();

			// SETUP STATE
			int runNetwork(int nSec, int nMsec=0, bool printRunSummary=true);
			ConnectionMonitor* setConnectionMonitor(int grpIdPre, int grpIdPost, const std::string& fileName);

			int getNumNeurons(); 
			int getGroupNumNeurons(int grpId);
			int getGroupStartNeuronId(int grpId);
			int getGroupEndNeuronId(int grpId);



			// RUN_STATE
			SpikeMonitor* getSpikeMonitor(int grpId);
			//void SpikeMonitor_print(SpikeMonitor* monitor, bool printSpikeTimes);
			
			void getFiring(std::vector<bool>& firing, int netId = -1);

#ifdef CARLSIM_AXONPLAST
			uint8_t* getDelays(int gIDpre, int gIDpost, int& Npre, int& Npost);
			void findWavefrontPath(std::vector<int>& path, std::vector<float>& eligibility, int netId, int grpId, int startNId, int goalNId);
			bool updateDelays(int gGrpIdPre, int gGrpIdPost, std::vector<std::tuple<int, int, uint8_t>> connDelays);			
#endif


			//
			void dummy();


		private:
			Carlsim* carlsim;
			//friend CarlsimLoader;
			//friend CarlsimWrapper;
	
	};

}

#endif//CARLSIMWIDGET_H

