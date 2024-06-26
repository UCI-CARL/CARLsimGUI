#ifndef SPIKESTREAMSIMULATIONEXCEPTION_H
#define SPIKESTREAMSIMULATIONEXCEPTION_H

//SpikeStream includes
#include "SpikeStreamException.h"

// Spikestream 0.3
#include "api.h" 

namespace spikestream {

	/*! Exception that relates to simulation functions. */
	class SPIKESTREAM_LIB_EXPORT SpikeStreamSimulationException : public SpikeStreamException {
		public:
			SpikeStreamSimulationException(QString details);
			~SpikeStreamSimulationException();

	};

}

#endif//SPIKESTREAMSIMULATIONEXCEPTION_H


