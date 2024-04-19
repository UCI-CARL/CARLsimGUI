#ifndef TEMPORALCODINGEXPTMANAGER_H
#define TEMPORALCODINGEXPTMANAGER_H

//SpikeStream includes
#include "CarlsimWrapper.h"
#include "SpikeStreamThread.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QString>


namespace spikestream {

	/*! Runs selected experiments on network exploring how temporal code can be used
		as part of pattern learning in a network. */
	class VteExptManager : public SpikeStreamThread {
		Q_OBJECT

		public:
			VteExptManager();
			~VteExptManager();
			void run();
			void startExperiment(CarlsimWrapper* carlsimWrapper, QHash<QString, double>& parameterMap);

		signals:
			void statusUpdate(QString statusMsg);

		private:
			//=====================  VARIABLES  =====================
			/*! Wrapper of simulation */
			CarlsimWrapper* carlsimWrapper;

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

			//======================  METHODS  ======================
			void runExperiment();
			void stepCarlsim(unsigned numTimeSteps);
			void storeParameters(QHash<QString, double>& parameterMap);

	};

}

#endif//TEMPORALCODINGEXPTMANAGER_H
