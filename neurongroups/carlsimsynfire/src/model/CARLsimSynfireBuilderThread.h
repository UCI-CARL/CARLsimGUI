#ifndef CARLSIMSYNFIREBUILDERTHREAD_H
#define CARLSIMSYNFIREBUILDERTHREAD_H

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
	class CARLsimSynfireBuilderThread : public SpikeStreamThread {
		Q_OBJECT

		public:
			CARLsimSynfireBuilderThread();
			~CARLsimSynfireBuilderThread();

			struct NeuronParam_t {
				//int nm; 
				float a; float b; float c; float d;
				void setExcitatoryParameters(QHash<QString, double>& parameterMap);
				void setInhibitoryParameters(QHash<QString, double>& parameterMap);
				NeuronParam_t(float a, float b, float c, float d) : a(a), b(b), c(c), d(d) {};
				NeuronParam_t() : a(.0f), b(.0f), c(.0f), d(.0f) {};
			} exc, inh; 

			struct CustomNeuronParam_t {
				float mean; float sd; int spikes;
				void setCustomParameters(QHash<QString, double>& parameterMap);
				CustomNeuronParam_t(float mean, float sd, int spikes) : mean(mean), sd(sd), spikes(spikes) {};
				CustomNeuronParam_t(): mean(.0f), sd(.0f), spikes(0) {};
			} stim;

			//void prepareAddNeuronGroups(const QString& name, const QString& description, QHash<QString, double>& paramMap);
			// No reason to use a untyped param list
			void prepareAddNeuronGroups(

				const QString& prefix,

				const unsigned segments,

				//const unsigned space,
				//const int pos_x, 
				//const int pos_y, 
				//const int pos_z,

				const int stim_columns,
				const int stim_rows,

				const int exc_columns,
				const int exc_rows,

				const int inh_columns,
				const int inh_rows,

				const bool coba,
				const int coba_ampa,
				const int coba_nmda,
				const int coba_gaba_a,
				const int coba_gaba_b,

				const NeuronParam_t &exc,
				const NeuronParam_t &inh
				);

			void run();

		signals:
			void progress(int stepsCompleted, int totalSteps, QString message);

		private:

			//QString name;
			//QString description;

			QString prefix;

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



//// Parameters Kremkow 2010
//
//			const QString prefix = "G";     // get this from the parameters    this descripption, this is the group prexix 
//				// Caution: Hoepner only use C. C for Core
//
//			// Debug former:  unittest18d_epuck_gui.config !!!
//	// 
//	// start with hard wired parameter for 2 segments 
//	// Loop over Segments  -> param  
//	// columns = 5
//	// rows = 20 for exc   total -> GUI readonly
//	// rows = 5 for inh    total -> GUI readonly 
//	// 
//			const int segments = 10;
//
//			const int exc_columns = 10;
//			const int exc_rows = 10;
//
//			const int inh_columns = 5;
//			const int inh_rows = 5;
//

			unsigned segments;

			int stim_columns;
			int stim_rows;

			int exc_columns;		
			int exc_rows;

			int inh_columns;		
			int inh_rows;

			bool coba;
			int coba_ampa;
			int coba_nmda;
			int coba_gaba_a;
			int coba_gaba_b;

			//=======================  METHODS  ==========================
			void addNeuronGroupsToDatabase();
			void createNeuronGroups();
			double getParameter(const QString& paramName, const QHash<QString, double>& paramMap);
			void printSummary();

	};
}

#endif//CARLSIMSYNFIREBUILDERTHREAD_H
