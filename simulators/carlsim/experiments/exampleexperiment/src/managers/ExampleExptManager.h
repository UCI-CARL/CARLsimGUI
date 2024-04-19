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
	class ExampleExptManager : public SpikeStreamThread {
		Q_OBJECT

		public:
			ExampleExptManager();
			~ExampleExptManager();
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


			//======================  METHODS  ======================
			void runExperiment();
			void stepCarlsim(unsigned numTimeSteps);
			void storeParameters(QHash<QString, double>& parameterMap);

	};

}

#endif//TEMPORALCODINGEXPTMANAGER_H
