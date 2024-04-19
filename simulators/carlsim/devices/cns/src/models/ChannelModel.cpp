//SpikeStream includes
#include "ChannelModel.h"
#include "Globals.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QIcon>
#include <QDateTime>

/*! Constructor */
ChannelModel::ChannelModel(CnsManager* cnsManager) : QAbstractTableModel(){
	this->cnsManager = cnsManager;
	loadChannels(); 
}

/*! Destructor */
ChannelModel::~ChannelModel(){
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Inherited from QAbstractTableModel. Returns the number of columns in the model */
int ChannelModel::columnCount(const QModelIndex&) const{
	return NUM_COLS;
}


/*! Inherited from QAbstractTableModel. Returns the data at the specified model index
    ready to be displayed in the way requested */
QVariant ChannelModel::data(const QModelIndex & index, int role) const{
    //Return invalid index if index is invalid or no network loaded
    if (!index.isValid())
		return QVariant();

    //Check rows and columns are in range
    if (index.row() < 0 || index.row() >= rowCount() || index.column() < 0 || index.column() >= columnCount())
		return QVariant();

    //Return appropriate data
    if (role == Qt::DisplayRole){
		if(index.column() == CHANNEL_NAME_COL)
			return channelNameList.at(index.row());
		else
		if(index.column() == CHANNEL_SECURITIES_COL)
			return channelSecuritiesList.at(index.row());
		else
		if(index.column() == NEURON_GROUP_NAME_COL)
			return neuronGroupNameList.at(index.row());
		else
		if(index.column() == BUFFER_QUEUED_COL)
			return channelBufferQueuedList.at(index.row());
		else
		if(index.column() == BUFFER_NEXT_COL)
			return channelBufferNextList.at(index.row());
		else
		if(index.column() == READER_PROCESSED_COL)
			return channelReaderProcessedList.at(index.row());
		else
		if(index.column() == READER_LAST_COL)
			return channelReaderLastList.at(index.row());
		else
		if(index.column() == CHANNEL_STATE_COL)
			return channelStateList.at(index.row());
    }

	//Icons
    if (role == Qt::DecorationRole){
		if(index.column() == PARAM_COL ){
			return QIcon(Globals::getSpikeStreamRoot() + "/images/parameters.png");
		} else
		if(index.column() == DELETE_COL){
			return QIcon(Globals::getSpikeStreamRoot() + "/images/trash_can.png");
		} else
		if(index.column() == LOGGING_COL){
			return QIcon(Globals::getSpikeStreamRoot() + "/images/logging.png");
		} else
		if(index.column() == DEBUG_COL){
			return QIcon(Globals::getSpikeStreamRoot() + "/images/debug.png");
		}
	}

    //If we have reached this point ignore request
    return QVariant();
}


/*! Deletes an input or output channel depending on the row */
void ChannelModel::deleteChannel(int row){
	if(row >= 0 && row < cnsManager->getInboundChannelCount()){
		cnsManager->deleteInboundChannel(row);
		reload();
	} else 
	if (row - cnsManager->getInboundChannelCount() < cnsManager->getOutboundChannelCount()){
		cnsManager->deleteOutboundChannel(row - cnsManager->getInboundChannelCount());
		reload();
	}
	else
		throw SpikeStreamException("Failed to delete channel; row out of range: " + QString::number(row));
}


/*! Returns the parameters for a particular active channel */
map<string, Property> ChannelModel::getProperties(int row){
	if(row >= 0 && row < cnsManager->getInboundChannelCount())
		return cnsManager->getInboundProperties(row);
	else if (row - cnsManager->getInboundChannelCount() < cnsManager->getOutboundChannelCount())
		return cnsManager->getOutboundProperties(row - cnsManager->getInboundChannelCount());
	throw SpikeStreamException("Failed to get properties; row out of range: " + QString::number(row));
}

/*! Sets the properties for a particular channel */
void  ChannelModel::setProperties(int row, map<string, Property> propertyMap){
	if(row >= 0 && row < cnsManager->getInboundChannelCount())
		cnsManager->setInboundProperties(row, propertyMap);
	else if (row - cnsManager->getInboundChannelCount() < cnsManager->getOutboundChannelCount())
		cnsManager->setOutboundProperties(row - cnsManager->getInboundChannelCount(), propertyMap);
	else
		throw SpikeStreamException("Failed to set properties; row out of range: " + QString::number(row));
}


/*! Returns true if there are no channels */
bool ChannelModel::isEmpty(){
	if(rowCount() == 0)
		return true;
	return false;
}


/*! Reloads the list of neuron groups */
void ChannelModel::reload(){
	loadChannels();
}


/*! Inherited from QAbstractTableModel. */
bool ChannelModel::setData(const QModelIndex& index, const QVariant&, int) {
	if (!index.isValid())
		return false;
    if (index.row() < 0 || index.row() >= rowCount())
		return false;

    //If we have reached this point no data has been set
    return false;
}


/*! Inherited from QAbstractTableModel. Returns the header data */
QVariant ChannelModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role != Qt::DisplayRole)
		return QVariant();

    if (orientation == Qt::Horizontal){
		if(section == CHANNEL_NAME_COL)
			return "Channel";
		else
		if(section == CHANNEL_SECURITIES_COL)  
			return "Securities";
		else
		if(section == NEURON_GROUP_NAME_COL)
			return "Neuron Group";
		else
		if(section == CHANNEL_STATE_COL)
			return "State";
		else
		if(section == READER_LAST_COL)
			return "Last";
		else
		if(section == READER_PROCESSED_COL)
			return "Processed";
		else
		if(section == BUFFER_NEXT_COL)
			return "Next";
		else
		if(section == BUFFER_QUEUED_COL)
			return "#";

    }

	return QVariant();

}


/*! Inherited from QAbstractTableModel. Returns the number of rows in the model. */
int ChannelModel::rowCount(const QModelIndex&) const{
	return channelNameList.size();
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Returns a standard formatted neuron group name including the ID. */
QString ChannelModel::getNeuronGroupName(NeuronGroup* neuronGroup){
	return neuronGroup->getInfo().getName() + " (" + QString::number(neuronGroup->getID()) + ")";
}


/*! Loads the current list of connected channels from the ISpikeManager */
void ChannelModel::loadChannels(){
//qDebug() << __FUNCTION__ << __LINE__;
	channelNameList.clear();
	neuronGroupNameList.clear();
	channelBufferQueuedList.clear();
	channelBufferNextList.clear();
	channelReaderProcessedList.clear();
	channelReaderLastList.clear();

	//Inbound channels
	QList< QPair<InboundChannel*, NeuronGroup*> > inboundChannelList = cnsManager->getInboundChannels(); 
	QDateTime dt;
	dt.setTimeSpec(Qt::OffsetFromUTC);
	QDateTime last;
	last.setTimeSpec(Qt::OffsetFromUTC);

	for(int i=0; i< inboundChannelList.size(); ++i){
		channelNameList.append(inboundChannelList[i].first->getChannelDescription().getName().data());

		channelSecuritiesList.append("tbd4432");

		neuronGroupNameList.append(getNeuronGroupName(inboundChannelList[i].second));

		size_t queued = inboundChannelList[i].first->getBufferQueued();
		channelBufferQueuedList.append(queued);

		dt = QDateTime::fromMSecsSinceEpoch(inboundChannelList[i].first->getBufferNext() - 1*3600*1000);
		channelBufferNextList.append(dt.toString("hh:mm:ss.zzz"));


		QString state = "UNDEF";
		channelStateList.append(state);

		size_t processed = inboundChannelList[i].first->getReaderProcessed();
		channelReaderProcessedList.append(processed);

		last = QDateTime::fromMSecsSinceEpoch(inboundChannelList[i].first->getReaderLast() - 1*3600*1000);  
		channelReaderLastList.append(last.toString("hh:mm:ss.zzz"));

	}

	//Outbound channels
	QList< QPair<OutboundChannel*, NeuronGroup*> > outboundChannelList = cnsManager->getOutboundChannels();
	for(int i=0; i< outboundChannelList.size(); ++i){
		channelNameList.append(outboundChannelList[i].first->getChannelDescription().getName().data());

		channelSecuritiesList.append("tbd3249"); 
		
		neuronGroupNameList.append(getNeuronGroupName(outboundChannelList[i].second));
		size_t queued = outboundChannelList[i].first->getBufferQueued(); // 0 or 1;
		channelBufferQueuedList.append(queued);
		dt = QDateTime::fromMSecsSinceEpoch(outboundChannelList[i].first->getBufferNext() - 1*3600*1000);  
		channelBufferNextList.append(dt.toString("hh:mm:ss.zzz"));
		QString state = "UNDEF";
		channelStateList.append(state);
		size_t processed = outboundChannelList[i].first->getReaderProcessed();
		channelReaderProcessedList.append(processed);
		last = QDateTime::fromMSecsSinceEpoch(outboundChannelList[i].first->getReaderLast() - 1*3600*1000);  
		channelReaderLastList.append(last.toString("hh:mm:ss.zzz"));
	}

	beginResetModel();

	endResetModel();
}


