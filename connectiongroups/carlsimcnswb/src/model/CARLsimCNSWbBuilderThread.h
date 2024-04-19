#ifndef CARLSIMCNSWBBUILDERTHREAD_H
#define CARLSIMCNSWBBUILDERTHREAD_H

//SpikeStream includes
#include "ArchiveDao.h"
#include "NetworkDao.h"
#include "ConnectionGroup.h"
#include "ConnectionGroupInfo.h"
#include "AbstractConnectionBuilder.h"

#include <QVector>

namespace spikestream {

	/*! Adds a connection group to the current network, which automatically adds it to the database */
	class CARLsimCNSWbBuilderThread : public AbstractConnectionBuilder {
		Q_OBJECT

		public:
			CARLsimCNSWbBuilderThread();
			~CARLsimCNSWbBuilderThread();
			virtual void run();

			struct ConnectionParam_t {
				bool active;
				float weights;
				float w_factor;
				unsigned delays;
				//ConnectionParam_t(float weights, float w_factor, float delays) :
				//	weights(weights), w_factor(w_factor), delays(delays) {};
				ConnectionParam_t() : active(false), weights(.0f), w_factor(.0f), delays(.0f) {};
			} tof2vel, ps2vel;

		protected:
			void buildConnectionGroup();
			void checkParameters();

		private:
			//=======================  VARIABLES  ========================
			/*! Total number of progress steps */
			int numberOfProgressSteps;

			NeuronGroup* tofGroup;
			NeuronGroup* psGroup;

			NeuronGroup* velGroup;

			QVector<unsigned int> tof_ids;
			QVector<unsigned int> ps_ids;

			QVector<unsigned int> vel_ids;

			/*! Connection groups to be added, stored as a list */
			QList<ConnectionGroup*> conGrpList;

	};
}

#endif//CARLSIMCNSWBBUILDERTHREAD_H
