#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

// SpikeStream 0.3
#include "api.h"

//SpikeStream includes
#include "SpikeStreamThread.h"

namespace spikestream {

	/*! Runs as a separate thread to delete connections and other tasks. */
	class SPIKESTREAM_APP_LIB_EXPORT ConnectionManager : public SpikeStreamThread {
		Q_OBJECT

		public:
			ConnectionManager();
			virtual ~ConnectionManager();
			void deleteConnectionGroups(QList<unsigned>& connectionGroupIDs);
			void run();

		signals:
			void progress(int stepsCompleted, int totalSteps, QString message);


		private:
			//=======================  VARIABLES  =========================
			/*! List of connection group IDs to be deleted */
			QList<unsigned> deleteConnectionGroupIDs;

			/*! Total number of connections to be deleted */
			int totalNumberOfConnections;

	};

}

#endif//CONNECTIONMANAGER_H
