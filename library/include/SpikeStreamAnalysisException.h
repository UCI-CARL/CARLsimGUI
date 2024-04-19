#ifndef SPIKESTREAMANALYSISEXCEPTION_H
#define SPIKESTREAMANALYSISEXCEPTION_H

//SpikeStream includes
#include "SpikeStreamException.h"

// Spikestream 0.3
#include "api.h" 

namespace spikestream {

	/*! Exception specific to analysis-related functionality. */
    class SPIKESTREAM_LIB_EXPORT SpikeStreamAnalysisException : public SpikeStreamException {
		public:
			SpikeStreamAnalysisException(QString details);
			~SpikeStreamAnalysisException();

		};

}

#endif//SPIKESTREAMANALYSISEXCEPTION_H


