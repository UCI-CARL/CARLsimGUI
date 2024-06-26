#ifndef EVENTROUTER_H
#define EVENTROUTER_H

// SpikeStream 0.3
#include "api.h"

//Qt includes
#include <QObject>

namespace spikestream {

	/*! Class held within globals that enables other classes to listen for application
		wide events, such as reload, etc. */
	class SPIKESTREAM_APP_LIB_EXPORT EventRouter : public QObject {
		Q_OBJECT

		public:

		public slots:
			//Archive slots
			void archiveListChangedSlot() { emit archiveListChangedSignal(); }
			void archiveChangedSlot() { emit archiveChangedSignal(); }

			//Analysis slots
			void analysisStarted() { emit analysisStartedSignal(); emit analysisNotRunningSignal(false); }
			void analysisStopped() { emit analysisStoppedSignal(); emit analysisNotRunningSignal(true); }

			//Network slots
			void networkChangedSlot() { emit networkChangedSignal(); }
			void networkListChangedSlot() { emit networkListChangedSignal(); }
			void networkDisplayChangedSlot() { emit networkDisplayChangedSignal(); }
			void neuronGroupDisplayChangedSlot() { emit neuronGroupDisplayChangedSignal(); }
			void connectionGroupDisplayChangedSlot() { emit connectionGroupDisplayChangedSignal(); }
			void visibleConnectionsChangedSlot() { emit visibleConnectionsChangedSignal(); }
			void weightsChangedSlot() { emit weightsChangedSignal(); }
			void delaysChangedSlot() { emit delaysChangedSignal(); }

			//Other slots
			void reloadSlot() { emit reloadSignal(); }
			void refreshSlot() { emit refreshSignal(); }  			// F5
			void managementSlot() { emit managementSignal(); }  	// Ctrl-M


		signals:
			//Analysis signals
			void analysisNotRunningSignal(bool isNotRunning);
			void analysisStartedSignal();
			void analysisStoppedSignal();

			//Archive signals
			void archiveChangedSignal();
			void archiveListChangedSignal();

			//Simulation signals
			void startSimulationSignal();
			void stopSimulationSignal();
			void startStopSimulationSignal();
			void stepSimulationSignal();

			//OpenGL signals
			void moveBackwardSignal();
			void moveForwardSignal();
			void moveUpSignal();
			void moveDownSignal();
			void moveLeftSignal();
			void moveRightSignal();
			void resetViewSignal();
			void rotateUpSignal();
			void rotateDownSignal();
			void rotateLeftSignal();
			void rotateRightSignal();

			//Network signals
			void networkChangedSignal();
			void networkDisplayChangedSignal();
			void networkListChangedSignal();
			void networkViewChangedSignal();
			void neuronGroupDisplayChangedSignal();
			void connectionGroupDisplayChangedSignal();
			void visibleConnectionsChangedSignal();
			void weightsChangedSignal();
			void delaysChangedSignal();

			//Other signals
			void reloadSignal();
			void showAllOrNoneConnectionsSignal();
			void refreshSignal();
			void managementSignal();

			void nemoSimulationLoadedSignal(); 
			void finspikesDeviceLoadedSignal();

			void carlsimSimulationLoadedSignal(); 
			//void carlsimWantsInjectorsSignal(QString path); 
			void finspikes2DeviceLoadedSignal(); 

			//void simulatorChanged(void*);


		private slots:
			//Simulation slots
			void startSimulationSlot() { emit startSimulationSignal(); }
			void stopSimulationSlot() { emit stopSimulationSignal(); }
			void startStopSimulationSlot() { emit startStopSimulationSignal(); }
			void stepSimulationSlot() { emit stepSimulationSignal(); }

			//OpenGL slots
			void moveBackwardSlot() { emit moveBackwardSignal(); emit networkViewChangedSignal(); }
			void moveForwardSlot() { emit moveForwardSignal(); emit networkViewChangedSignal(); }
			void moveUpSlot() { emit moveUpSignal(); emit networkViewChangedSignal(); }
			void moveDownSlot() { emit moveDownSignal(); emit networkViewChangedSignal(); }
			void moveLeftSlot() { emit moveLeftSignal(); emit networkViewChangedSignal(); }
			void moveRightSlot() { emit moveRightSignal(); emit networkViewChangedSignal(); }
			void resetViewSlot() { emit resetViewSignal(); emit networkViewChangedSignal(); }
			void rotateUpSlot() { emit rotateUpSignal(); emit networkViewChangedSignal(); }
			void rotateDownSlot() { emit rotateDownSignal(); emit networkViewChangedSignal(); }
			void rotateLeftSlot() { emit rotateLeftSignal(); emit networkViewChangedSignal(); }
			void rotateRightSlot() { emit rotateRightSignal(); emit networkViewChangedSignal(); }

			void showAllOrNoneConnectionsSlot() { emit showAllOrNoneConnectionsSignal(); }

			// Autoload  
			void nemoSimulationLoadedSlot() { emit nemoSimulationLoadedSignal(); }  
			void finspikesDeviceLoadedSlot() { emit finspikesDeviceLoadedSignal(); } 
			void finspikes2DeviceLoadedSlot() { emit finspikes2DeviceLoadedSignal(); } 
			void carlsimSimulationLoadedSlot() { emit carlsimSimulationLoadedSignal(); }  

	};

}

#endif//EVENTROUTER_H

