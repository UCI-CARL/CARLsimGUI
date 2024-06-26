#ifndef CONNECTIONGROUPINFO_H
#define CONNECTIONGROUPINFO_H

//SpikeStream includes
#include "SynapseType.h"

// Spikestream 0.3
#include "api.h" 

//Qt includes
#include <QString>
#include <QHash>

namespace spikestream {

    /*! Holds information about a connection group as stored in the ConnectionGroup table
	    of the SpikeStreamNetwork database */
    class SPIKESTREAM_LIB_EXPORT ConnectionGroupInfo {
		public:
			ConnectionGroupInfo();
			ConnectionGroupInfo(unsigned int id, const QString& desc, unsigned int fromID, unsigned int toID, QHash<QString, double> paramMap, const SynapseType& synapseType);
			ConnectionGroupInfo(const ConnectionGroupInfo& conGrpInfo);
			~ConnectionGroupInfo();
			ConnectionGroupInfo& operator=(const ConnectionGroupInfo& rhs);

			unsigned int getID() const { return id; }
			QString getDescription() const { return description; }
			unsigned int getFromNeuronGroupID() const { return fromNeuronGroupID; }
			unsigned int getToNeuronGroupID() const { return toNeuronGroupID; }
			SynapseType getSynapseType() { return synapseType; }
			unsigned int getSynapseTypeID() const { return synapseType.getID(); }
			double getParameter(const QString& parameterName);
			QHash<QString, double> getParameterMap() { return parameterMap; }
			QString getParameterXML();
			bool hasParameter(const QString& parameterName);
			void setDescription(const QString& description){ this->description = description; }
			void setFromNeuronGroupID(unsigned id) { this->fromNeuronGroupID = id; }
			void setToNeuronGroupID(unsigned id) { this->toNeuronGroupID = id; }
			void setID(unsigned int id) { this->id = id; }

		private:
			/*! ID of the connection group. Should match a row in the ConnectionGroup
			table in the SpikeStream database */
			unsigned int id;

			/*! Description of the connection */
			QString description;

			/*! ID of the neuron group that the connection is from */
			unsigned int fromNeuronGroupID;

			/*! ID of the neuron group that the connection is to */
			unsigned int toNeuronGroupID;

			/*! Type of synapses on the connection */
			SynapseType synapseType;

			/*! Parameters used to create the connection group - these are different from the synapse parameters. */
			QHash<QString, double> parameterMap;

    };

}

#endif//CONNECTIONGROUPINFO_H


