#ifndef CURRENTINJECTORMANAGER_H
#define CURRENTINJECTORMANAGER_H

//SpikeStream includes
#include "CarlsimWrapper.h"
#include "SpikeStreamThread.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QString>


namespace spikestream {

	class CurrentInjectorManager : public SpikeStreamThread {
		Q_OBJECT

		public:
			CurrentInjectorManager();
			~CurrentInjectorManager();

		signals:

		private:
			//=====================  VARIABLES  =====================


			//======================  METHODS  ======================

	};

}

#endif//CURRENTINJECTORMANAGER_H
