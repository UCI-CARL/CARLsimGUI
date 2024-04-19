#ifndef SPIKESTREAMDBEXCEPTION_H
#define SPIKESTREAMDBEXCEPTION_H

//SpikeStream includes
#include "SpikeStreamException.h"

// Spikestream 0.3
#include "api.h" 

namespace spikestream {

	/*! Exception specific to database-related functionality. */
	class SPIKESTREAM_LIB_EXPORT SpikeStreamDBException : public SpikeStreamException {
		public:
		SpikeStreamDBException(QString details);
		~SpikeStreamDBException();

	};
}

#endif//SPIKESTREAMDBEXCEPTION_H


