#ifndef NETWORKDAOTHREAD_H
#define NETWORKDAOTHREAD_H

//SpikeStream includes
#include "NetworkDao.h"
#include "DBInfo.h"
#include "ConnectionGroup.h"
#include "NeuronGroup.h"
using namespace spikestream;

// Spikestream 0.3
#include "api.h" 

//Qt includes
#include <QThread>


namespace spikestream {

    /*! Thread-based wrapper for the SpikeStreamNetwork database that handles
		heavy tasks that may take several minutes. */
	class SPIKESTREAM_LIB_EXPORT NetworkDaoThread : public QThread, public NetworkDao {
		Q_OBJECT

		public:
			NetworkDaoThread(const DBInfo& dbInfo, const QString& name = "Unnamed");
			~NetworkDaoThread();
			void clearError();
			int getNumberOfCompletedSteps() { return numberOfCompletedSteps; }
			int getTotalNumberOfSteps() { return totalNumberOfSteps; }
			bool isError() { return error; }
			QString getErrorMessage() { return errorMessage; }
			QString getProgressMessage() { return progressMessage; }
			void prepareAddConnectionGroup(unsigned int networkID, ConnectionGroup* connGrp);
			void prepareAddConnectionGroups(unsigned int networkID, QList<ConnectionGroup*>& connGrpList);
			void prepareAddNeuronGroup(unsigned int networkID, NeuronGroup* neurGrp);
			void prepareAddNeuronGroups(unsigned int networkID, QList<NeuronGroup*>& neurGrpList);
			void prepareDeleteConnectionGroups(unsigned int networkID, QList<unsigned int>& conGrpList);
			void prepareDeleteNeuronGroups(unsigned int networkID, QList<unsigned int>& neurGrpList);
			void prepareLoadConnections(const QList<ConnectionGroup*>& connGrpList);
			void prepareLoadConnections(ConnectionGroup* connGrp);
			void prepareLoadNeurons(const QList<NeuronGroup*>& neurGrpList);
			void prepareLoadNeurons(NeuronGroup* neurGrp);
			void run();
			void startDeleteNetwork(unsigned networkID);
			void startSaveNetwork(unsigned networkID, QList<NeuronGroup*> newNeuronGroups, QList<ConnectionGroup*> newConnectionGroups, QList<unsigned> deleteNeuronGroupIDs, QList<unsigned> deleteConnectionGroupIDs, QList<ConnectionGroup*> volatileConnectionGroups);
			void startSaveTempWeights(QList<ConnectionGroup*>& connectionGroupList);
			void stop();

		private:
			//========================  VARIABLES  =========================
			/*! List of connection groups to load connections into or used for
				adding connection groups to the database. */
			QList<ConnectionGroup*> connectionGroupList;

			/*! List of connection group ids used for deleting connection groups */
			QList<unsigned int> connectionGroupIDList;

			/*! List of neuron groups to load neurons into or for adding to database */
			QList<NeuronGroup*> neuronGroupList;

			/*! List of neuron group ids used for deleting neuron groups */
			QList<unsigned int> neuronGroupIDList;

			/*! List of volatile connection groups used for saving temporary weights */
			QList<ConnectionGroup*> volatileConnectionGroupList;

			/*! Id of network to which neuron or connection group are added */
			unsigned int networkID;

			/*! Records which task is being undertaken - task IDs are defined below */
			unsigned int currentTask;

			/*! When set to true the run method is exited. */
			bool stopThread;

			/*! Exceptions do not work across threads, so need to record when an error takes place when
			 the thread is executing a task.*/
			bool error;

			/*! Error message */
			QString errorMessage;

			/*! Message reporting current task for progress bar. */
			QString progressMessage;

			/*! The total number of steps for a task */
			int totalNumberOfSteps;

			/*! The number of steps that have been completed so far */
			int numberOfCompletedSteps;

			/*! The number of values statement used when adding connections */
			int numConBuffers;

			/*! The number of values statement used when adding neurons */
			int numNeurBuffers;

			const static unsigned int NO_TASK_DEFINED = 1;
			const static unsigned int ADD_CONNECTION_GROUPS_TASK = 2;
			const static unsigned int ADD_NEURON_GROUPS_TASK = 3;
			const static unsigned int DELETE_CONNECTION_GROUPS_TASK = 4;
			const static unsigned int DELETE_NETWORK_TASK = 5;
			const static unsigned int DELETE_NEURON_GROUPS_TASK = 6;
			const static unsigned int LOAD_NEURONS_TASK = 7;
			const static unsigned int LOAD_CONNECTIONS_TASK = 8;
			const static unsigned int SAVE_NETWORK_TASK = 9;
			const static unsigned int SAVE_TEMP_WEIGHTS_TASK = 10;


			//========================  METHODS  ===========================
			void addNeuronGroups();
			void addConnectionGroups();
			void deleteConnectionGroups();
			void deleteNetwork();
			void deleteNeuronGroups();
			void loadConnections();
			void loadNeurons();
			void saveNetwork();
			void saveTempWeights();
			void setError(const QString& msg);
    };

}

#endif//NETWORKDAOTHREAD_H
