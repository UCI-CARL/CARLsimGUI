#ifndef ARCHIVEDAO_H
#define ARCHIVEDAO_H

//SpikeStream includes
#include "ArchiveInfo.h"
#include "AbstractDao.h"
#include "DBInfo.h"
using namespace spikestream;

// Spikestream 0.3
#include "api.h" 

namespace spikestream {

	/*! Data access object for the SpikeStreamArchive database */
    class SPIKESTREAM_LIB_EXPORT ArchiveDao : public AbstractDao {
		public:
			ArchiveDao(const DBInfo& dbInfo);
			virtual ~ArchiveDao();

			void addArchive(ArchiveInfo& archiveInfo);
			void addArchiveData(unsigned int archiveID, unsigned int timeStep, const QString& firingNeuronString);
			void addArchiveData(unsigned int archiveID, unsigned int timeStep, const QList<unsigned>& firingNeuronList);//UNTESTED
			void deleteArchive(unsigned int archiveID);
			void deleteAllArchives();
			QList<ArchiveInfo> getArchivesInfo(unsigned int networkID);
			int getArchiveSize(unsigned int archiveID);
			QList<unsigned> getFiringNeuronIDs(unsigned int archiveID, unsigned int timeStep);
			unsigned int getMaxTimeStep(unsigned int archiveID);
			unsigned int getMinTimeStep(unsigned int archiveID);
			bool networkHasArchives(unsigned int networkID);
			void setArchiveProperties(unsigned archiveID, const QString& description);//UNTESTED
		};

}

#endif//ARCHIVEDAO_H

