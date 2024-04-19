#ifndef SPIKESTREAMXMLEXCEPTION_H
#define SPIKESTREAMXMLEXCEPTION_H

//SpikeStream includes
#include "SpikeStreamException.h"

// Spikestream 0.3
#include "api.h" 

namespace spikestream{

	/*! Exception related to XML processing. */
	class SPIKESTREAM_LIB_EXPORT SpikeStreamXMLException : public SpikeStreamException {
		public:
		SpikeStreamXMLException(QString details);
		~SpikeStreamXMLException();

	};

}

#endif//SPIKESTREAMXMLEXCEPTION_H


