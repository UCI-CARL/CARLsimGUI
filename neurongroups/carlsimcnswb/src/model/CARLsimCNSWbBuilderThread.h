#ifndef CARLSIMCNSWBBUILDERTHREAD_H
#define CARLSIMCNSWBBUILDERTHREAD_H

//SpikeStream includes
#include "ArchiveDao.h"
#include "NetworkDao.h"
#include "NeuronGroup.h"
#include "NeuronGroupInfo.h"
#include "SpikeStreamThread.h"

//Qt includes
#include <QThread>


namespace spikestream {

	/*! Adds a neuron group to the current network, which automatically adds it to the database */
	class CARLsimCNSWbBuilderThread : public SpikeStreamThread {
		Q_OBJECT

		public:
			CARLsimCNSWbBuilderThread();
			~CARLsimCNSWbBuilderThread();

			struct NeuronParam_t {
				//int nm; 
				float a; float b; float c; float d;
				void setExcitatoryParameters(QHash<QString, double>& parameterMap);
				void setInhibitoryParameters(QHash<QString, double>& parameterMap);
				NeuronParam_t(float a, float b, float c, float d) : a(a), b(b), c(c), d(d) {};
				NeuronParam_t() : a(.0f), b(.0f), c(.0f), d(.0f) {};
			} ps, ls, tof, whls, vel, led, rgb, frnt, body, accl, gyro; 
			//rn, bfb, lc, sn, vta, na;

			//void prepareAddNeuronGroups(const QString& name, const QString& description, QHash<QString, double>& paramMap);
			// No reason to use a untyped param list
			void prepareAddNeuronGroups(			
				const unsigned securities, 
				const unsigned states,
				const QString& prefix,
				const unsigned space,
				const int pos_x, 
				const int pos_y, 
				const int pos_z,
				const NeuronParam_t &ps,
				const NeuronParam_t &ls,
				const NeuronParam_t &tof,
				const NeuronParam_t &vel
			);

			void run();

		signals:
			void progress(int stepsCompleted, int totalSteps, QString message);

		private:

			QString prefix;
			unsigned securities; // 1..3, default 1
			unsigned states; // 2..5, default 3			

			unsigned space;

			//=======================  VARIABLES  ========================
			/*! The neuron group(s) being added.
				The key is the neuron type ID. */
			QHash<unsigned int, NeuronGroup*> newNeuronGroupMap;

			/*! The threshold of the percentages used to add the different neuron types.
				If a random number is above the threshold, then the neuron is added to the group.
				The algorithm starts with the  lowest threshold
				The key is the percentage; the value is the neuron type ID. */
			QMap<double, unsigned int> neuronTypePercentThreshMap;

			/*! Network Dao used by Network when in thread */
			NetworkDao* threadNetworkDao;

			/*! Archive Dao used by Network when in thread */
			ArchiveDao* threadArchiveDao;

			/*! Starting x position of neurons to be added */
			int xStart;

			/*! Starting y position of neurons to be added */
			int yStart;

			/*! Starting z position of neurons to be added */
			int zStart;

			/*! Width of the neuron group to be added along the x axis */
			int width;

			/*! Length of the neuron group to be added along the y axis */
			int length;

			/*! Height of the neuron group to be added along the z axis */
			int height;

			/*! Spacing between neurons in the neuron group */
			int spacing;

			/*! Density of the neurons */
			double density;

			/*! Neuron groups to be added, stored as a list */
			QList<NeuronGroup*> newNeuronGroupList;

			///*! Connection groups to be added, stored as a list */
			//QList<ConnectionGroup*> newConGroupList;

			/*! The total number of neurons to be added. */
			unsigned totalNumberOfNeurons;


			//=======================  METHODS  ==========================
			void addNeuronGroupsToDatabase();
			void createNeuronGroups();
			double getParameter(const QString& paramName, const QHash<QString, double>& paramMap);
			void printSummary();

	};
}

#endif//CARLSIMCNSWBBUILDERTHREAD_H
