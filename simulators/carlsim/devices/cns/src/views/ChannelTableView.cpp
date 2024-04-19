//SpikeStream includes
#include "ChannelTableView.h"
#include "EditPropertiesDialog.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QHeaderView>
#include <QMessageBox>

/*! Constructor */
ChannelTableView::ChannelTableView(ChannelModel* model, QWidget* parent) : QTableView(parent) {
	//Set up the view
	setShowGrid(false);
	setWordWrap(false);
	setSelectionMode(QAbstractItemView::NoSelection);
	connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(tableClicked(QModelIndex)));

	//Set the model for this view and show it
	this->channelModel = model;
	setModel(model);
	show();
	resizeHeaders();
}


/*! Destructor */
ChannelTableView::~ChannelTableView(){
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Sets up the size of the headers. Does not work in the constructor because there
	is not any data or model */
void ChannelTableView::resizeHeaders(){
	QHeaderView* hHeader = horizontalHeader();
	hHeader->setMinimumSectionSize(10);
	hHeader->resizeSection(hHeader->logicalIndex(ChannelModel::PARAM_COL), 20); //Channel parameters
	hHeader->resizeSection(hHeader->logicalIndex(ChannelModel::LOGGING_COL), 20); //Logging button
	hHeader->resizeSection(hHeader->logicalIndex(ChannelModel::DEBUG_COL), 20); //Debug button
	hHeader->resizeSection(hHeader->logicalIndex(ChannelModel::DELETE_COL), 20); //Delete button
	hHeader->resizeSection(hHeader->logicalIndex(ChannelModel::NEURON_GROUP_NAME_COL), 90); //Name of neuron group
	hHeader->resizeSection(hHeader->logicalIndex(ChannelModel::READER_PROCESSED_COL), 65); 
	hHeader->resizeSection(hHeader->logicalIndex(ChannelModel::READER_LAST_COL), 75); 
	hHeader->resizeSection(hHeader->logicalIndex(ChannelModel::BUFFER_QUEUED_COL), 20); 
	hHeader->resizeSection(hHeader->logicalIndex(ChannelModel::BUFFER_NEXT_COL), 75); 
	hHeader->resizeSection(hHeader->logicalIndex(ChannelModel::CHANNEL_NAME_COL), 60); // Name of channel (Shortname), Channel is redundant
	hHeader->resizeSection(hHeader->logicalIndex(ChannelModel::CHANNEL_STATE_COL), 50); 
	hHeader->resizeSection(hHeader->logicalIndex(ChannelModel::CHANNEL_SECURITIES_COL), 230); // List of the Securities
	hHeader->setDefaultAlignment(Qt::AlignLeft);
}


/*--------------------------------------------------------*/
/*-------             PRIVATE SLOTS                -------*/
/*--------------------------------------------------------*/

/*! Called when the table is clicked and passes this information on to model. */
void ChannelTableView::tableClicked(QModelIndex index){
	try{
		if(index.column() == ChannelModel::PARAM_COL){
			EditPropertiesDialog* tmpDlg = new EditPropertiesDialog(channelModel->getProperties(index.row()), true);
			if(tmpDlg->exec() == QDialog::Accepted){
				channelModel->setProperties(index.row(), tmpDlg->getPropertyMap());
			}
			return;
		} else
		if(index.column() == ChannelModel::DELETE_COL){
			//Check if user really wants to delete a channel
			int response = QMessageBox::warning(this, "Delete Channel?", "Are you sure that you want to delete this channel?\nThis cannot be undone.", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
			if(response != QMessageBox::Ok)
				return;

			//Delete channel
			channelModel->deleteChannel(index.row());
			return;
		} else
		if(index.column() == ChannelModel::LOGGING_COL){
			return;
		} else
		if(index.column() == ChannelModel::DEBUG_COL){
			// Channel Debug Dialog
			EditPropertiesDialog* tmpDlg = new EditPropertiesDialog(channelModel->getProperties(index.row()), true);
			if(tmpDlg->exec() == QDialog::Accepted){
				// channelModel->setProperties(index.row(), tmpDlg->getPropertyMap());
			}
			return;
		}
			
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
	catch(...){
		qCritical()<<"An unknown exception occurred configuring or deleting a channel.";
	}
}



