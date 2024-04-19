#ifndef SPIKESTREAMEXCEPTION_H
#define SPIKESTREAMEXCEPTION_H

// Spikestream 0.3
#include "api.h" 

#include <QString>

namespace spikestream {

	/*! SpikeStream specific exception. Other SpikeStream exception classes inherit from this class. */
	class SPIKESTREAM_LIB_EXPORT SpikeStreamException {
		public:
			SpikeStreamException(QString details = QString("No details available"));
			virtual ~SpikeStreamException();
			virtual QString getMessage();

		protected:
			QString type;

		private:
			QString details;
	};

}

#endif // SPIKESTREAMEXCEPTION_H
