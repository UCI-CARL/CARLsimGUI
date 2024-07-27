#ifndef CARLSIMCNSYOLOBUILDERTHREAD_H
#define CARLSIMCNSYOLOBUILDERTHREAD_H

//SpikeStream includes
#include "ArchiveDao.h"
#include "NetworkDao.h"
#include "ConnectionGroup.h"
#include "ConnectionGroupInfo.h"
#include "AbstractConnectionBuilder.h"

#include <QVector>

namespace spikestream {

	/*! Adds a connection group to the current network, which automatically adds it to the database */
	class CARLsimCNSYoloBuilderThread : public AbstractConnectionBuilder {
		Q_OBJECT

		public:
			CARLsimCNSYoloBuilderThread();
			~CARLsimCNSYoloBuilderThread();
			virtual void run();

			struct ConnectionParam_t {
				bool active;
				float weights;
				float w_factor;
				unsigned delays;
				//ConnectionParam_t(float weights, float w_factor, float delays) :
				//	weights(weights), w_factor(w_factor), delays(delays) {};
				ConnectionParam_t() : active(false), weights(.0f), w_factor(.0f), delays(.0f) {};
			} dir2vel, dist2vel, prob2obj, obj2tof;

		protected:
			void buildConnectionGroup();
			void checkParameters();

		private:
			//=======================  VARIABLES  ========================
			/*! Total number of progress steps */
			int numberOfProgressSteps;

			NeuronGroup* yoloGroup;

			NeuronGroup* velGroup;

			NeuronGroup* objGroup;

			NeuronGroup* tofGroup;


			QVector<unsigned int> yolo_ids; // yolo event

			QVector<unsigned int> vel_ids;	// vel actuator

			QVector<unsigned int> obj_ids;  // interneuron that knock-out tof sensor if in obj was detected

			QVector<unsigned int> tof_ids;  // distance densor 

			/*! Connection groups to be added, stored as a list */
			QList<ConnectionGroup*> conGrpList;

	};
}

#endif//CARLSIMCNSYOLOBUILDERTHREAD_H
