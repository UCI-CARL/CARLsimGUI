#ifndef CARLSIMWRAPPER_H
#define CARLSIMWRAPPER_H

//SpikeStream includes
#include "AbstractDeviceManager.h"
#include "AbstractSimulation.h"
#include "ArchiveDao.h"
#include "ArchiveInfo.h"
#include "NetworkDao.h"
#include "ParameterInfo.h"
#include "Pattern.h"
#include "SpikeStreamTypes.h"
#include "ConfigLoader.h"
#include "CarlsimLib.h"
#include "carlsim_types.h"

//Qt includes
#include <QHash>
#include <QMutex>
#include <QThread>
#include <QDateTime>


//Other includes
#include <vector>
using namespace std;

// 
#include "api.h" 

class ConnectionMonitor;
class GroupMonitor; 


namespace spikestream {


	class CarlsimGeneratorContainer; 
	class CarlsimMonitor; // prepared


	/*! Wraps the CARLsim CUDA simulation library launching separate threads for heavy functions
		such as play and load */
	class CARLSIMWRAPPER_LIB_EXPORT CarlsimWrapper : public QThread, public AbstractSimulation {
		Q_OBJECT
		Q_PROPERTY(ModelAutosync modelAutosync READ getModelAutosync WRITE setModelAutosync)  
		Q_PROPERTY(ModelStart modelStart READ getModelStart WRITE setModelStart)
		Q_ENUMS(ModelAutosync)  
		Q_ENUMS(ModelStart)  



		public:

			/*! Defines how modelTime0 is set when the simulations starts.
				REALTIME:	When the first step in the snn begins
				FIXED:		Defined starting time be parameter config file
				CHANNEL:	Use the timestamp of the first message received by any channel */
			enum ModelStart {REALTIME=0, FIXED, CHANNEL}; 

			/*! OFF		No Autosync
				FIRST	Wait until first message is received by any channel. 
						See also ModelTime0
				LAST	Wait if all channels have no more messages in their buffer
						This requires dynamic speed up to catch up (not yet implemented).
			*/
			enum ModelAutosync { OFF=0, FIRST, LAST}; 


			CarlsimWrapper();
			CarlsimWrapper(ConfigLoader* configLoader); 
			~CarlsimWrapper();


			void addDeviceManager(AbstractDeviceManager* deviceManager);
			void cancelLoading();
			void cancelResetWeights();
			void cancelSaveWeights();
			void clearWaitForGraphics() { waitForGraphics = false; }
			unsigned getArchiveID() { return archiveInfo.getID(); }
			int getCurrentTask() { return currentTaskID; }
			QString getErrorMessage() { return errorMessage; }
			QList<neurid_t> getFiringNeuronIDs() { return firingNeuronList; }
			carlsim41::carlsim_configuration_t getCarlsimConfig(){ return carlsimConfig; }
			unsigned getSTDPFunctionID() { return stdpFunctionID; } 

#ifdef NC_EXTENSIONS_CARLSIM
			double getModelSpeed() { return modelSpeed; }
			unsigned getModelStepMs() {return modelStepMs; }
			ModelAutosync getModelAutosync() { return modelAutosync; }
			ModelStart getModelStart() { return modelStart; }
			int getModelLagMs() { return modelLagMs; }

			void setModelAutosync(ModelAutosync autosync) { modelAutosync = autosync; }
			void setModelStart(ModelStart start) { modelStart = start; }
			void setModelLagMs(int lag) { modelLagMs= lag; }

			void emit_timeChanged(); 

			void setDa(float level);

			float getDa() { return da; }

			/*! Increment the DA level by the configured value. This method is public and thread safe and utilizes mutex */
			void rewardDa(); 

			/*! Increments the DA level by a given amount. This method is public and thread safe and designed to 
				be used by the devices */
			void rewardDa(float increment); 		
			
#endif

			timestep_t getTimeStep() { return timeStepCounter; }

			unsigned getUpdateInterval_ms() { return this->updateInterval_ms; }
			unsigned getWaitInterval_ms() { return waitInterval_ms; }
			bool isError() { return error; }
			bool isMonitorFiringNeurons() { return monitorFiringNeurons; }
			bool isMonitorWeights() { return monitorWeights; }
			bool isWaitForGraphics()  { return waitForGraphics; }
			bool isWeightsReset() { return weightsReset; }
			bool isWeightsSaved() { return weightsSaved; }
#ifdef CARLSIM_AXONPLAST
			void cancelResetDelays();
			void cancelSaveDelays();
			bool isMonitorDelays() { return monitorDelays; }
			bool isDelaysReset() { return delaysReset; }
			bool isDelaysSaved() { return delaysSaved; }
			void resetDelays();
			void saveDelays();
			void setMonitorDelays(bool enable);
#endif
			bool isSimulationLoaded() { return simulationLoaded; }
			bool isSimulationRunning();
			void resetWeights();
			void run();
			void saveWeights();
			void setArchiveMode(bool mode, const QString& archiveDescription = "");
			void setFrameRate(unsigned int frameRate);
			void setFiringNeuronIDs(QList<neurid_t>& neurIDList);
			void setInjectCurrentNeuronIDs(QList<neurid_t>& neurIDList, double current);
#ifdef NC_EXTENSIONS_CARLSIM
			// each neuron id has its own current
			void setInjectCurrents(QList<QPair<neurid_t, double>>& neuronCurrentsList, bool validate=true);
			typedef QVector<synapse_id> synapsesIds_t;
			typedef tuple<float, float, float> synapseProperties_t;
			typedef QVector<synapseProperties_t> synapsesProperties_t;
			// public interface
			void querySynapsesPrepare(QVector<unsigned> ids);
			void querySynapses();
			synapsesProperties_t& querySynapsesResult() { return m_synapsesProperties; }
#endif
			void setInjectNoise(unsigned neuronGroupID, double percentage, bool sustain);
			void setFiringInjectionPattern(const Pattern& pattern, unsigned neuronGroupID, bool sustain);
			void setCurrentInjectionPattern(const Pattern& pattern, float current, unsigned neuronGroupID, bool sustain);
			void setMonitor(bool mode);
			void setMonitorNeurons(bool firing, bool membranePotential);
			void setCarlsimConfig(carlsim41::carlsim_configuration_t carlsimConfig) { this->carlsimConfig = carlsimConfig; }
			void setMonitorWeights(bool enable);
			void setNeuronParameters(unsigned neuronGroupID, QHash<QString, double> parameterMap);
			void setSynapseParameters(unsigned connectionGroupID, QHash<QString, double> parameterMap);
			void setSTDPFunctionID(unsigned stdpFunctionID) { this->stdpFunctionID = stdpFunctionID; }
			void setSustainNoise(bool sustain) { this->sustainNoise = sustain;  }
			void setSustainPattern(bool sustainPattern) { this->sustainPattern = sustainPattern; }
			void setUpdateFiringNeurons(bool updateFiringNeurons){ this->updateFiringNeurons = updateFiringNeurons; }
			void setWaitInterval(unsigned waitInterval_ms) { this->waitInterval_ms = waitInterval_ms; }
			void playSimulation();
			void stepSimulation();
			void stopSimulation();
			void unloadSimulation();
			void loadSimulation();


			///====================  VARIABLES  ==========================
			/*! No task defined */
			static const int NO_TASK_DEFINED = 0;

			/*! Task of resetting temporary weight values to stored values. */
			static const int RESET_WEIGHTS_TASK = 1;

			/*! Task of playing simulation */
			static const int RUN_SIMULATION_TASK = 2;

			/*! Task of saving weights from simulation into database. */
			static const int SAVE_WEIGHTS_TASK = 3;

			/*! Task of setting neuron parameters */
			static const int SET_NEURON_PARAMETERS_TASK = 4;

			/*! Task of advancing one time step of the simulation. */
			static const int STEP_SIMULATION_TASK = 5;

#ifdef NC_EXTENSIONS_CARLSIM
			/*! Task of quering certain properties of the synapse and its neurons (target/source) v,u, */
			static const int QUERY_SYNAPSES_TASK = 6; 

			/*! Task of quering certain properties of the synapse and its neurons (target/source) v,u, */
			static const int QUERY_WEIGHTS_TASK = 7; 
#endif

#ifdef CARLSIM_AXONPLAST
			/*! Task of resetting temporary weight values to stored values. */
			static const int RESET_DELAYS_TASK = 8;

			/*! Task of saving weights from simulation into database. */
			static const int SAVE_DELAYS_TASK = 9;
#endif




		signals:
			void carlsimConfigState(); //<! carlsim has entered the ::CONFIG_STATE
			void carlsimConfigStateNetworkBuilt(); //<! carlsim is ::CONFIG_STATE and the network was built and now is ready for asigning callbacks 
			void carlsimSetupState(); //<! carlsim has entered the ::SETUP_STATE
			void carlsimRunState(); //<! carlsim has entered the ::RUN_STATE
			void carlsimUnloaded(); //<! simulation was unloaded and carlsim was deleted
			void progress(int stepsComplete, int totalSteps);
			void simulationStopped();
			void timeStepChanged(unsigned int timeStep);
			void timeStepChanged(unsigned int timeStep, const QList<unsigned>& neuronIDList);
			void timeStepChanged(unsigned int timeStep, const QHash<unsigned, float>& membranePotentialMap);
			void timeChanged(	unsigned int timeStep, unsigned long long snnTimeMs, 
								QDateTime modelTime0, QDateTime modelTime, 
								QDateTime realTime0, QDateTime realTime, 
								unsigned long long realTimeUs );
			void modelSpeedChanged(double speed);
			void modelStepMsChanged(unsigned step);

			void stepCurrentInjection(); 
			void monitorStartRecording(unsigned int counter); 
			void monitorStopRecording(unsigned int counter);
			void monitorsLoaded();
			void loadSimulationRequired();

		public slots: 
			void setModelSpeed(double speed); 
			void setModelStepMs(unsigned step); 

		private slots:
			void updateProgress(int stepsComplete, int totalSteps);
			void handleMonitorsLoaded();


		private:
			//======================  VARIABLES  ========================
			/*! Thread specific version of the network dao */
			NetworkDao* networkDao;

			/*! Thread specific version of the archive dao */
			ArchiveDao* archiveDao;

			/*! Information about the archive */
			ArchiveInfo archiveInfo;

			/*! ID of the current task that is running */
			int currentTaskID;

			/*! Records that simulation is currently loaded and ready to play */
			bool simulationLoaded;

			/*! In archive mode the current firing neurons are written to disk */
			bool archiveMode;

			/*! In monitor firing neurons mode a signal is emitted containing a list of the firing neurons */
			bool monitorFiringNeurons;

			/*! In monitor membrane potential mode a signal is emitted containing a map
				linking neuron ids with their membrane potential. */
			bool monitorMembranePotential;

			/*! In monitor weights mode the volatile weights are updated at each time step*/
			bool monitorWeights;

#ifdef CARLSIM_AXONPLAST
			/*! In monitor delays mode the volatile weights are updated at each time step*/
			bool monitorDelays;
#endif

			/*! Global control to switch monitoring on or off */
			bool monitor;

			/*! Extracts firing neurons at each time step regardless of the monitor state. */
			bool updateFiringNeurons;

			/*! The time step of the simulation */
			unsigned int timeStepCounter;

			/*! Records if an error has occurred */
			bool error;

			/*! Message associated with an error */
			QString errorMessage;

			/*! Controls whether the thread is running or not */
			bool stopThread;

			/*! Configuration */
			carlsim41::carlsim_configuration_t carlsimConfig;

public:
			/*! Pointer to the SNN simulation that has been constructed. */
			CarlsimLib* carlsim;	// in CARLsim the configuration is represented by a state 
			friend class CarlsimLoader;

private: 
		
			/*! Interval between each time step in milliseconds. */
			unsigned updateInterval_ms;

			/*! Amount of time that wrapper waits before checking for next task to execute. */
			unsigned waitInterval_ms;

			/*! In monitor mode we need to wait for the graphics to update before
				moving on to the next time step */
			bool waitForGraphics;

			/*! Mutex controlling access to variables */
			QMutex mutex;

			/*! Mutex that is locked during run method loop and prevents changes whilst it runs. */
			QMutex runMutex;

			/*! ID of the STDP function */
			unsigned stdpFunctionID;

			/*! List of neurons that are firing at the current time step */
			QList<neurid_t> firingNeuronList;

			/*! Map of neuron groups to inject firing noise into at the next time step.
				The key is the neuron group ID, the value is the number of neurons to fire. */
			QHash<unsigned, unsigned> injectNoiseMap;

			/*! Controls whether noise is sustained for more than one time step */
			bool sustainNoise;


			/*! List of neuron IDs to fire in the next time step */
			QList<neurid_t> neuronIDsToFire;


			/*! Map linking neurons with membrane potential */
			QHash<unsigned, float> membranePotentialMap;

			/*! Map of the volatile connection groups.
				The key in the outer map is the volatile connection group ID.
				The key in the inner map is the CARLsim ID of the connection.
				The value in the inner map is the SpikeStream ID of the connection. */
			QHash<unsigned, synapse_id*> volatileConGrpMap;  


			/*! Implement both version, a key and an index based lookup */
			QHash<neurgrpid_t, NeuronGroup*> volatileNeurGrpMap; //! key based lookup NeurGrpID
			QHash<unsigned, NeuronGroup*> persistentNeurGrpMap; //! given conGroup->getFromNeuronGroupID() contains only DB ids 
			QVector<NeuronGroup*> volatileNeurGrpTable;  //! index based lookup 

			friend class CarlsimGenerator;
			QVector<CarlsimGeneratorContainer*> connectionGenerators;  //! index based lookup on ConnectionGroups
			void manage(int conGrpId, CarlsimGeneratorContainer*); // move ressource 
			void releaseConnectionGenerators();

			/*! List of presynaptic neuron ids used for saving weights. */
			QList<unsigned> preSynapticNeuronIDs;

			/*! Flag set to true when weights have been saved. */
			bool weightsSaved;

			/*! Flag set to true when weights are reset */
			bool weightsReset;

			/*! Flag used to cancel weight save */
			bool weightSaveCancelled;

			/*! Flag used to cancel weight reset */
			bool weightResetCancelled;

#ifdef CARLSIM_AXONPLAST
			/*! Flag set to true when delays have been saved. */
			bool delaysSaved;

			/*! Flag set to true when delays are reset */
			bool delaysReset;

			/*! Flag used to cancel delay save */
			bool delaySaveCancelled;

			/*! Flag used to cancel delay reset */
			bool delayResetCancelled;
#endif

			/*! Reward used for STDP learning */
			float stdpReward;

			/*! Interval between applications of the learning function. */
//			timestep_t applySTDPInterval;

#ifdef NC_EXTENSIONS_CARLSIM
			/*! Explicit flag if DA modulated STDP is active or not */
			bool daStdp;  

			/*! Current dopamine level */
			float da;

			/*! Rate of decay at each step of the simulaton */
			float daDecay; 

			/*! The amount of da which is added to the da level for each reward */
			float daReward; 
#endif

			/*! ID of neuron group within which parameters will be set. */
			unsigned neuronGroupID;

			/*! Map containing neuron parameters. */
			QHash<QString, double> neuronParameterMap;

			/*! Vector of neuron IDs to be fired at next time step */
			vector<unsigned> injectionPatternVector;

			/*! Vector of neuron IDs to have current injected at next time step. */
			vector<unsigned> injectionCurrentNeurIDVector;

			//port to Plugin
			/*! Current associated with the injection IDs. */
			vector<float> injectionCurrentVector;

			/*! Controls whether pattern is injected on every time step. */
			bool sustainPattern;

			/*! Neuron group in which pattern is to be injected. */
			unsigned patternNeuronGroupID;

			/*! List of device managers that interact with devices */
			QList<AbstractDeviceManager*> deviceManagerList;


			/* ! Firing Bit-Vector with Index = Global Neuron IDs of CARLsim hybrid network
			*/
			std::vector<bool> firing;		


			//======================  METHODS  ========================
			unsigned addInjectCurrentNeuronIDs();
			unsigned addInjectFiringNeuronIDs();
			void clearError();
			void fillInjectNoiseArray(unsigned*& array, int* arraySize);
			void getMembranePotential();
			void loadCarlsim();
			void resetCarlsimWeights();
			void runCarlsim();
			void saveCarlsimWeights();
#ifdef NC_EXTENSIONS_CARLSIM
			void saveSynapsesProperties();   
#endif 
#ifdef CARLSIM_AXONPLAST
			void resetCarlsimDelays();
			void saveCarlsimDelays();
#endif
			void setError(const QString& errorMessage);
			void setExcitatoryNeuronParameters(NeuronGroup* neuronGroup);
			void setInhibitoryNeuronParameters(NeuronGroup* neuronGroup);
			void setCustomExcitatoryNeuronParameters(NeuronGroup* neuronGroup);
			void setCustomInhibitoryNeuronParameters(NeuronGroup* neuronGroup);
			void setNeuronParametersInCarlsim();
			void stepCarlsim();
			void unloadCarlsim();

	public: 
			void updateNetworkWeights();
#ifdef CARLSIM_AXONPLAST
			void updateNetworkDelays();
#endif

#ifdef NC_EXTENSIONS_CARLSIM

	public:		
			const QDateTime getModelTime0() { return modelTime0; }
			void setModelTime0(const QDateTime& time) { modelTime0 = time; }

			unsigned long long getModelTimeMs() { return modelTimeMs; }  
			QDateTime getModelTime() { return QDateTime::fromMSecsSinceEpoch(modelTimeMs); }  

			unsigned long long getRealTimeUs() { return realTimeUs; }
			const QDateTime getRealTime0() { return realTime0; }
			void setRealTime0(QDateTime time) { realTime0 = time; } // used by reset

			unsigned long long getSnnTimeMs() { return snnTimeMs; }


			void resetTime();  // resets stepCounter and Modeltime

	private: // methods
			bool waitForChannels(int timeout_ms, int channel_wait_ms); 

	private: // data members
			QDateTime modelTime0;   
			unsigned long long modelTimeMs;	
			double modelSpeed; 
			unsigned modelStepMs;	
			ModelAutosync modelAutosync;
			ModelStart modelStart;
			int modelLagMs; 

			QDateTime realTime0; 
			quint64 realTimeUs;

			unsigned long long snnTimeMs;  

		private:
			/*! Current associated with the injection IDs. */
			synapsesIds_t m_synapsesIds;
			synapsesProperties_t m_synapsesProperties;

#ifdef DEBUG_WEIGHTS_LEARNING__CONN_MON
			ConnectionMonitor* __exp__monitor;
#endif

#endif

	};

}

#endif//CARLSIMWRAPPER_H
