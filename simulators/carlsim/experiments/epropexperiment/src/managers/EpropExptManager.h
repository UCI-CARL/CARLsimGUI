#ifndef EPROPEXPTMANAGER_H
#define EPROPEXPTMANAGER_H




//SpikeStream includes
#include "CarlsimWrapper.h"
#include "SpikeStreamThread.h"
#include "SpikeStreamTypes.h"

#include "Maze.h"
#include "ExpCarlsimWrapper.h"

//Qt includes
#include <QString>


namespace spikestream {
	class carlsim_monitors::OatSpikeMonitor;

	/*! Runs selected experiments on network exploring how temporal code can be used
		as part of pattern learning in a network. */
	class EpropExptManager : public SpikeStreamThread {
		Q_OBJECT

		public:
			EpropExptManager();
			~EpropExptManager();
			void run();
			void startExperiment(CarlsimWrapper* carlsimWrapper,
				carlsim_monitors::OatSpikeMonitor* spikeMonitor,
				QHash<QString, double>& parameterMap, 
				QString name,
				QString costs, 
				QStringList segments, QStringList landmarks,
				QStringList starts, QStringList ends, QStringList selection,
				NeuronGroup* placeCells);
			void setMonitor(carlsim_monitors::OatSpikeMonitor* spikeMonitor);

		signals:
			void statusUpdate(QString statusMsg);
			void requiresOatMonitor(QString type, QString name);
			void simulationReloaded();

		private:
			//=====================  VARIABLES  =====================
			/*! Wrapper of simulation */
			CarlsimWrapper* carlsimWrapper;
			Maze* maze; 
			ExpCarlsimWrapper* expWrapper; // specialization 

			//Keep hard earned SpikeMonitor as reference
			carlsim_monitors::OatSpikeMonitor* spikeMonitor;

			NeuronGroup* placeCells;

			std::vector<std::pair<int,int>> segments; // pre,post NId
			std::vector<std::pair<int, int>> landmarks; // pre,post NId

			QString presets;  // config-name, used for resultsCursor

			/*! Seed for random number generator */
			int randomSeed;

			/*! ID of experiment that is carried out. */
			int experimentNumber;

			static const int NO_EXPERIMENT = -1;
			static const int EXPERIMENT1 = 0;

			/*! Time to pause between sections of the experiment */
			int pauseInterval_ms;

			/*! Neuron ID that indicates the current location */
			int startNeuron;

			/*! Neuron ID that indicates the target location */
			int targetNeuron;

			/*! The initial current to be injected */
			double initialCurrent;

			/*! Time after the observation neccessary for recovery to run the next iteration */
			int recoveryTime_ms;

			/*! Time grid raster to sync the experiment against */
			int timeGrid_ms;

			/*! Speed factor for slow motion during observation time (as in time pannel, best effort) */
			double slowMotion;

			/*! Speed factor for fast forward (as in time pannel, best effort) */
			double fastForward;

			/*! ms how long shall it run in slow motion */
			int observationTime_ms;

			/*! ms when to start the experiment, relativ to the time_grid */
			int startTime_ms;

			/*! ms when to stop the experiment relative to the grid, -1 forever */
			int endTime_ms;

			/*! flag if set, update delays is called*/
			bool update_delays;

			/*! level ov verbosity of output to experiment log: 0 none, 1 info, 2 details, 3 debug, 4 dev */
			int verbosity;

			/*! training loops to be performed */
			int training_loops;

			/*! costs for Border and Barriers  */
			int border; 

			/*! barrier defined by nId, -1 none */
			int barrier;


			//======================  METHODS  ======================
			void runExperiment1(); // Single point
			void runExperiment2(); // Boone training
			void runExperiment3(); // Boone trails
			void runExperiment4(); // Morris trails (similar to Boone: random start, fixed target produces segments) 
			//void runExperiment4(); // Tolman trails
			void stepCarlsim(unsigned numTimeSteps);
			void storeParameters(QHash<QString, double>& parameterMap);

	};

}

#endif//EPROPEXPTMANAGER_H
