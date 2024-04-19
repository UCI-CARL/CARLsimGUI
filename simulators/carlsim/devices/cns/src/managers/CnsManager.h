#ifndef CNSMANAGER_H
#define CNSMANAGER_H

//SpikeStream includes
#include "AbstractDeviceManager.h"
#include "NeuronGroup.h"
#include "SpikeStreamTypes.h"

//Cns includes
#include "CARLsimIO/Channel/InboundChannel/InboundChannel.hpp"
#include "CARLsimIO/Channel/OutboundChannel/OutboundChannel.hpp"
using namespace carlsimio;

//Qt includes
#include <QHash>
#include <QList>
#include <QPair>
#include <QFile>
#include <QTextStream>

//Other includes
#include <map>
using namespace std;

namespace spikestream {

	typedef QHash<neurid_t, QPair<neurid_t, vector<unsigned>* >* > NeurIDMap;

	class CnsManager : public AbstractDeviceManager{
		public:
			CnsManager();
			~CnsManager();
			void addChannel(InboundChannel* inboundChannel, NeuronGroup* neuronGroup);
			void addChannel(OutboundChannel* outboundsChannel, NeuronGroup* neuronGroup);
			void deleteAllChannels();
			void deleteInboundChannel(int index);
			void deleteOutboundChannel(int index);
			QList< QPair<InboundChannel*, NeuronGroup*> > getInboundChannels(){ return inboundChannels; }
			int getInboundChannelCount() { return inboundChannels.size(); }
			map<string, Property> getInboundProperties(int index);
			map<string, Property> getReaderProperties(int index);
			QList< QPair<OutboundChannel*, NeuronGroup*> >  getOutboundChannels(){  return outboundChannels; }
			int getOutboundChannelCount() { return outboundChannels.size(); }
			map<string, Property> getOutboundProperties(int index);
			map<string, Property> getWriterProperties(int index);
			QList<neurid_t>::iterator outputNeuronsBegin();
			QList<neurid_t>::iterator outputNeuronsEnd();


			QList<neurid_t>::iterator currentNeuronsBegin();
			QList<neurid_t>::iterator currentNeuronsEnd();
			QList<double>::iterator currentValuesBegin();
			QList<double>::iterator currentValuesEnd();

			void setInputNeurons(timestep_t timeStep, QList<neurid_t>& firingNeuronIDs);
			void setInboundProperties(int index, map<string, Property>& propertyMap);
			void setOutboundProperties(int index, map<string, Property>& propertyMap);
			void step();

			//SpikeStream.NC_EXTENSIONS
			virtual void setSimtime(unsigned long long simtime_ms);  // called before step, from spikestream, distribute simtime to all channels
			virtual bool allChannelBufferEmpty(); 
			virtual void reset();

			virtual double getDopaminLevel();

		private:
			//=====================  VARIABLES  ======================
			/*! File where log is written if required. */
			QFile* logFile;

			/*! Text stream connected to log file */
			QTextStream* logTextStream;

			/*! List of output neuron ids */
			QList<neurid_t> outputNeuronIDs;

			/*! List of current vector for sparse neural activity */
			QList<neurid_t> currentNeuronIDs; 
			QList<double>    currentValues;

			///*! Map linking a SpikeStream ID to an ID 
			//	Key is a SpikeStream neuron ID, value is a pair of ID and a pointer to a vector of input neurons held in firingNeuronIDVectors. */
			NeurIDMap neurIDMap;

			///*! Vectors holding input neurons for each channel */
			vector< vector<unsigned>* > firingNeurIDVectors;


			///*! List of arrays whose index is the neuron ID
			//	and whose value is the spike stream ID. Used for mapping spikes from
			//	an input channel to SpikeStream. */
			QList<neurid_t*> inputArrayList;

			/*! List of input channels and their associated neuron groups */
			QList< QPair<InboundChannel*, NeuronGroup*> > inboundChannels;

			/*! List of output channels and their associated neuron groups */
			QList< QPair<OutboundChannel*, NeuronGroup*> > outboundChannels;
	};

}

#endif//CNSMANAGER_H
