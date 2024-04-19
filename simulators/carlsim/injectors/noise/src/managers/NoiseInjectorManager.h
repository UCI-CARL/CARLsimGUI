#ifndef NOISEINJECTORMANAGER_H
#define NOISEINJECTORMANAGER_H

//SpikeStream includes
#include "CarlsimWrapper.h"
#include "SpikeStreamThread.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QString>


namespace spikestream {

	namespace carlsim_injectors { 

		class NoiseInjectorManager : public SpikeStreamThread {
			Q_OBJECT

			public:
				NoiseInjectorManager();
				~NoiseInjectorManager();

			signals:

			private:
				//=====================  VARIABLES  =====================


				//======================  METHODS  ======================

		};
	}
}

#endif//NOISEINJECTORMANAGER_H
