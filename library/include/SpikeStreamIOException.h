#ifndef SPIKESTREAMIOEXCEPTION_H
#define SPIKESTREAMIOEXCEPTION_H

//SpikeStream includes
#include "SpikeStreamException.h"

// Spikestream 0.3
#include "api.h" 

namespace spikestream {

	/*! Exception specific to input and output tasks, such as reading from a file. */
	class SPIKESTREAM_LIB_EXPORT SpikeStreamIOException : public SpikeStreamException {
		public:
			SpikeStreamIOException(QString details);
			~SpikeStreamIOException();

	};
}

#endif//SPIKESTREAMIOEXCEPTION_H


