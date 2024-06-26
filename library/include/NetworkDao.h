#ifndef NETWORKDAO_H
#define NETWORKDAO_H

//SpikeStream includes
#include "AbstractDao.h"
#include "Box.h"
#include "ConnectionGroup.h"
#include "ConnectionGroupInfo.h"
#include "NetworkInfo.h"
#include "NeuronGroup.h"
#include "NeuronGroupInfo.h"
#include "NeuronType.h"
#include "ParameterInfo.h"
#include "SynapseType.h"
#include "WeightlessNeuron.h"
using namespace spikestream;

// Spikestream 0.3
#include "api.h" 

//Qt includes
#include <QString>

namespace spikestream{

	/*! Abstraction layer on top of the SpikeStreamNetwork database.
		Used for light operations taking place in the same thread as the calling
		function. Operations that are more database intensive can be found in
		the NetworkDaoThread class. */
	class SPIKESTREAM_LIB_EXPORT NetworkDao : public AbstractDao {
		public:
			NetworkDao(const DBInfo& dbInfo);
			virtual ~NetworkDao();

			void addNetwork(NetworkInfo& netInfo);
			void addWeightlessConnection(unsigned int connectionID, unsigned int patternIndex);
			unsigned int addWeightlessNeuronTrainingPattern(unsigned int neuronID, const unsigned char* patternArray, bool output, unsigned int patternArraySize);
			void getAllFromConnections(unsigned int networkID, QHash<unsigned int, QHash<unsigned int, bool> >& connMap);
			void getAllToConnections(unsigned int networkID, QHash<unsigned int, QHash<unsigned int, bool> >& connMap);
			unsigned getConnectionCount(unsigned int networkID);
			unsigned getConnectionCount(ConnectionGroup* conGrp);//UNTESTED
			unsigned getConnectionCount(const QList<ConnectionGroup*>& conGrpList);
			unsigned getConnectionCount(const QList<unsigned>& conGrpIDList);//UNTESTED
			unsigned getConnectionCount(const ConnectionGroupInfo& conGrpInfo);//UNTESTED
			void getConnectionGroupsInfo(unsigned int networkID, QList<ConnectionGroupInfo>& conGrpInfoList);
			unsigned int getConnectionGroupSize(unsigned int connGrpID);
			QList< QPair<unsigned, Connection> > getConnections(unsigned int fromNeuronID, unsigned int toNeuronID);
			QHash<QString, double> getDefaultNeuronParameters(unsigned int neuronTypeID);
			QHash<QString, double> getDefaultSynapseParameters(unsigned int synapseTypeID);
			QList<unsigned int> getFromConnections(unsigned int fromNeuronID);
			QList<unsigned int> getToConnections(unsigned int toNeuronID);
			QList<NetworkInfo> getNetworksInfo();
			bool NetworkDao::getNetworkInfo(const QString& name, NetworkInfo& info);  //! retrieve the entwork ID and NetworkInfo by a given name
			unsigned getNeuronCount(unsigned int networkID);
			unsigned getNeuronCount(const QList<NeuronGroup*>& neurGrpList);
			unsigned getNeuronCount(const NeuronGroupInfo& neurGrpInfo);//UNTESTED
			unsigned getNeuronCount(unsigned int networkID, const Box& box);
			Box getNeuronGroupBoundingBox(unsigned int neurGrpID);
			unsigned int getNeuronGroupID(unsigned int neuronID);
			void getNeuronGroupsInfo(unsigned int networkID, QList<NeuronGroupInfo>& neurGrpInfoList);
			QList<unsigned int> getNeuronIDs(unsigned int networkID);
			NeuronType getNeuronType(unsigned int neuronTypeID);
			NeuronType getNeuronType(const QString& neuronTypeDescription);//UNTESTED
			QList<NeuronType> getNeuronTypes();
			QHash<QString, double> getNeuronParameters(const NeuronGroupInfo& neurGrpInfo);
			unsigned getStartNeuronID(unsigned neuronGroupID);
			QHash<QString, double> getSynapseParameters(const ConnectionGroupInfo& conGrpInfo);
			SynapseType getSynapseType(unsigned int synapseTypeID);
			SynapseType getSynapseType(const QString& synapseTypeDescription);//UNTESTED
			QList<SynapseType> getSynapseTypes();
			WeightlessNeuron* getWeightlessNeuron(unsigned int neuronID);
			bool isWeightlessNetwork(unsigned int networkID);
			bool isWeightlessNeuron(unsigned int neuronID);
			void setConnectionGroupProperties(unsigned connectionGroupID, const QString& description);
			void setNetworkProperties(unsigned networkID, const QString& name, const QString& description);
			void setNeuronGroupProperties(unsigned neuronGroupID, const QString& name, const QString& description);
			void setNeuronParameters(const NeuronGroupInfo& info, QHash<QString, double>& paramMap);
			void setSynapseParameters(const ConnectionGroupInfo& info, QHash<QString, double>& paramMap);
			void setWeight(unsigned connectionID, double weight);

		private:
			//=========================  METHODS  ===========================
			QList<ParameterInfo> getNeuronParameterInfo(const NeuronType& neuronType);
			QList<ParameterInfo> getSynapseParameterInfo(const SynapseType& synapseType);

	};

}

#endif//NETWORKDAO_H


