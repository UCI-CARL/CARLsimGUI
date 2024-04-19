#ifndef CARLSIMOATMANAGER_H
#define CARLSIMOATMANAGER_H

//SpikeStream includes
#include "CarlsimWrapper.h"
#include "SpikeStreamThread.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QString>


namespace spikestream {

	namespace carlsim_monitors {  // not hierarchically  carlsim::monitors

		class CarlsimOatManager : public SpikeStreamThread {
			Q_OBJECT

		public:
			CarlsimOatManager();
			~CarlsimOatManager();

		signals:

		private:
			//=====================  VARIABLES  =====================


			//======================  METHODS  ======================

		};
	}
}

#endif//CARLSIMOATMANAGER_H
