#ifndef CARLSIMSYNFIRECONNBUILDERTHREAD_H
#define CARLSIMSYNFIRECONNBUILDERTHREAD_H

//SpikeStream includes
#include "ArchiveDao.h"
#include "NetworkDao.h"
#include "ConnectionGroup.h"
#include "ConnectionGroupInfo.h"
#include "AbstractConnectionBuilder.h"

#include <QVector>

namespace spikestream {

	/*! Adds a connection group to the current network, which automatically adds it to the database */
	class CARLsimSynfireConnBuilderThread : public AbstractConnectionBuilder {
		Q_OBJECT

		public:
			CARLsimSynfireConnBuilderThread();
			~CARLsimSynfireConnBuilderThread();
			virtual void run();

			struct ConnectionParam_t {
				bool active;
				float weights;
				float w_factor;
				unsigned delays;
				//ConnectionParam_t(float weights, float w_factor, float delays) :
				//	weights(weights), w_factor(w_factor), delays(delays) {};
				ConnectionParam_t() : active(false), weights(.0f), w_factor(.0f), delays(.0f) {};
			} exc2exc, exc2inh, inh2exc;  // exc can be a stim for the first group 

		protected:
			void buildConnectionGroup();
			void checkParameters();

		private:
			//=======================  VARIABLES  ========================
			/*! Total number of progress steps */
			int numberOfProgressSteps;

			NeuronGroup* stimGroup;

			QVector<NeuronGroup*> excGroups;

			QVector<NeuronGroup*> inhGroups;

			QVector<unsigned int> stimNeuronIds; // per group 

			QVector<QVector<unsigned int>> excNeuronIds; // per group 

			QVector<QVector<unsigned int>> inhNeuronIds; // per group 


			/*! Connection groups to be added, stored as a list */
			QList<ConnectionGroup*> conGrpList;

	};
}

#endif//CARLSIMSYNFIRECONNBUILDERTHREAD_H
