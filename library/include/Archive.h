#ifndef ARCHIVE_H
#define ARCHIVE_H

//SpikeStream includes
#include "ArchiveInfo.h"
using namespace spikestream;

// Spikestream 0.3
#include "api.h" 

namespace spikestream {

	/*! Holds information about a particular archive. */
    class SPIKESTREAM_LIB_EXPORT Archive {
		public:
			Archive(const ArchiveInfo& archiveInfo);
			~Archive();

			unsigned int getID() { return info.getID(); }
			unsigned int getTimeStep() { return timeStep; }
			void setTimeStep(unsigned int timeStep){ this->timeStep = timeStep; }

		private:
			//========================  VARIABLES  ===========================
			/*! Class containing details about the archive */
			ArchiveInfo info;

			/*! The current time step */
			unsigned int timeStep;
		};

}

#endif//ARCHIVE_H

