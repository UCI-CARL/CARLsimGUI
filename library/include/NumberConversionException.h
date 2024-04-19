#ifndef NUMBERCONVERSIONEXCEPTION_H
#define NUMBERCONVERSIONEXCEPTION_H

//SpikeStream includes
#include "SpikeStreamException.h"

// Spikestream 0.3
#include "api.h" 

namespace spikestream{

	/*! Exception thrown when an error is encountered converting numbers */
	class SPIKESTREAM_LIB_EXPORT NumberConversionException : public SpikeStreamException {
		public:
			NumberConversionException(QString details);
			~NumberConversionException();

	};

}

#endif//NUMBERCONVERSIONEXCEPTION_H



