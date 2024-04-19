#ifndef CHANNELMODEL_H
#define CHANNELMODEL_H


#ifdef Q_MOC_RUN
#	define BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#endif 

//SpikeStream includes
#include "CnsManager.h"

//Qt includes
#include <QAbstractTableModel>
#include <QStringList>

namespace spikestream {

	/*! Used for the display of information about a neuron group.
		Based on the NeuronGroup table in the SpikeStream database. */
	class ChannelModel : public QAbstractTableModel {
		Q_OBJECT

		public:
			ChannelModel(CnsManager* cnsManager);
			~ChannelModel();
			void clearSelection();
			int columnCount(const QModelIndex& parent = QModelIndex()) const;
			QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
			void deleteChannel(int row);
			map<string, Property> getProperties(int row);
			void setProperties(int row, map<string, Property> propertyMap);
			QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
			bool isEmpty();
			void reload();
			int rowCount(const QModelIndex& parent = QModelIndex()) const;
			bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole);

			friend class ChannelTableView;

		private:
			//====================  VARIABLES  ====================
			/*! List containing the names of the channels */
			QList<QString> channelNameList;

			/*! List containing the types of the channels */
			QList<QString> channelSecuritiesList;

			/*! List containing the names of the associated neuron groups */
			QList<QString> neuronGroupNameList;

			/*! Manager holding information about the channels */
			CnsManager* cnsManager;


			/*! Amount of items waiting in the buffer queue. Updated only if Monitoring is on */
			//QList<QString> channelBufferQueuedList;
			QList<size_t> channelBufferQueuedList;

			/*! Time mm:hh.sss of the next pending item in the buffer queue. Updated only if Monitoring is on. */
			QList<QString> channelBufferNextList; 

			/*! Current View */
			QList<QString> channelStateList;  // Reader State

			/*! Time mm:hh.sss  */
			QList<size_t> channelReaderProcessedList;

			/*! Time mm:hh.sss of t*/
			QList<QString> channelReaderLastList; 


			static const int NUM_COLS				= 12;
			static const int PARAM_COL				= 0;
			static const int LOGGING_COL			= 1; // Diagnostic per Channel: Logging On/Off, Log-Level, Adhoc Printout (Modal Dialog)
			static const int DEBUG_COL				= 2; // Specific Hooks per Channel:
			static const int DELETE_COL				= 3;
			static const int NEURON_GROUP_NAME_COL	= 4;
			static const int READER_LAST_COL		= 5;		
			static const int READER_PROCESSED_COL	= 6;	 
			static const int BUFFER_NEXT_COL		= 7;		
			static const int BUFFER_QUEUED_COL		= 8;	 
			static const int CHANNEL_NAME_COL		= 9;
			static const int CHANNEL_STATE_COL		= 10;
			static const int CHANNEL_SECURITIES_COL	= 11;

			//====================  METHODS  =====================
			QString getNeuronGroupName(NeuronGroup* neuronGroup);
			void loadChannels();

		};

}

#endif//CHANNELMODEL_H

