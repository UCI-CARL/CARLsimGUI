#ifndef ARCHIVEINFO_H
#define ARCHIVEINFO_H

//Qt includes
#include <QString>
#include <QDateTime>

// Spikestream 0.3
#include "api.h" 

namespace spikestream {

	/*! Holds information about an archive stored in the SpikeStreamArchive database. */
    class SPIKESTREAM_LIB_EXPORT ArchiveInfo {
		public:
			ArchiveInfo();
			ArchiveInfo(unsigned int id, unsigned int networkID, unsigned int unixTimestamp, const QString& description);
			ArchiveInfo(const ArchiveInfo& archInfo);
			~ArchiveInfo();
			ArchiveInfo& operator=(const ArchiveInfo& rhs);

			unsigned int getID() { return id; }
			unsigned int getNetworkID() { return networkID; }
			QDateTime getDateTime() { return startDateTime; }
			QString getDescription() { return description; }
			void reset();
			void setDateTime(const QDateTime& dateTime) { this->startDateTime = dateTime; }
			void setDescription(const QString& description) { this->description = description; }
			void setID(unsigned int id) { this->id = id; }
			void setNetworkID(unsigned int netID) { this->networkID = netID; }

		private:
			//=========================  VARIABLES  ===========================
			/*! ID of the archive in the database */
			unsigned int id;
			unsigned int networkID;
			QDateTime startDateTime;
			QString description;

		};

}

#endif//ARCHIVEINFO_H


