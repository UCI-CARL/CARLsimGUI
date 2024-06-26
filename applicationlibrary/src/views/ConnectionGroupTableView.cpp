//SpikeStream includes
#include "ConnectionGroupDialog.h"
#include "ConnectionGroupTableView.h"
#include "SpikeStreamException.h"
#include "ViewParametersDialog.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QHeaderView>

/*! Constructor */
ConnectionGroupTableView::ConnectionGroupTableView(QWidget* parent, ConnectionGroupModel* model) : QTableView(parent) {
	//Set up the view
	this->horizontalHeader()->setSectionsClickable(true); // https://doc.qt.io/archives/qt-5.11/qheaderview-obsolete.html
	setShowGrid(false);
	setSelectionMode(QAbstractItemView::NoSelection);
	connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(tableClicked(QModelIndex)));
	connect(this->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(headerClicked(int)));
	connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(tableDoubleClicked(QModelIndex)));

	//Set the model for this view and show it
	this->connectionGroupModel = model;
	setModel(model);
	show();
	resizeHeaders();
}


/*! Destructor */
ConnectionGroupTableView::~ConnectionGroupTableView(){
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Sets up the size of the headers. Does not work in the constructor because there
	is not any data or model */
void ConnectionGroupTableView::resizeHeaders(){
	QHeaderView* hHeader = horizontalHeader();
	hHeader->setMinimumSectionSize(10);
	hHeader->resizeSection(hHeader->logicalIndex(ConnectionGroupModel::VIS_COL), 20);//Visibility icon
	hHeader->resizeSection(hHeader->logicalIndex(ConnectionGroupModel::ID_COL), 50);//ID
	hHeader->resizeSection(hHeader->logicalIndex(ConnectionGroupModel::DESC_COL), 300);//Description
	hHeader->resizeSection(hHeader->logicalIndex(ConnectionGroupModel::SIZE_COL), 100);//Size
	hHeader->resizeSection(hHeader->logicalIndex(ConnectionGroupModel::FROM_NEUR_ID_COL), 100);//From neuron group
	hHeader->resizeSection(hHeader->logicalIndex(ConnectionGroupModel::T0_NEUR_ID_COL), 100);//To neuron group
	hHeader->resizeSection(hHeader->logicalIndex(ConnectionGroupModel::SYNAPSE_TYPE_COL), 100);//Synapse type
	hHeader->resizeSection(hHeader->logicalIndex(ConnectionGroupModel::SELECT_COL), 20);//Select
	hHeader->resizeSection(hHeader->logicalIndex(ConnectionGroupModel::PARAM_COL), 70);//Parameters
	hHeader->setDefaultAlignment(Qt::AlignLeft);
}


/*--------------------------------------------------------*/
/*-------             PRIVATE SLOTS                -------*/
/*--------------------------------------------------------*/

/*! Called when the header of the table is clicked */
void ConnectionGroupTableView::headerClicked(int column){
	if(column == ConnectionGroupModel::SELECT_COL)
		connectionGroupModel->selectAllOrNone();
	else if(column == ConnectionGroupModel::VIS_COL)
		connectionGroupModel->showAllOrNone();
}

/*
HINT The ParameterDialog works perfectly fine. 
however,only doubles are supported so far. 
UnitTest: TestXMLParameterParser::testGetParameterMap
E2E Test with SQL: 
update spikestreamnetwork.connectiongroups set
	Parameters = '<?xml version="1.0" encoding="ISO-8859-1"?><connection_group_parameters><parameter><name>direction</name><value>3.13</value></parameter></connection_group_parameters>' -- '<connection_group_parameters> <parameter> <name>direction</name><value>forward</value></parameter></connection_group_parameters>'
 where ConnectionGroupID = 95;
*/

/*! Called when the table is clicked and passes this information on to model. */
void ConnectionGroupTableView::tableClicked(QModelIndex index){
	if(index.column() == ConnectionGroupModel::PARAM_COL){
		try{
			ViewParametersDialog dlg(connectionGroupModel->getParameters(index.row()), this);
			dlg.exec();
		}
		catch(SpikeStreamException& ex){
			qCritical()<<ex.getMessage();
		}
		return;
	}
	else{
		model()->setData(index, 0);
	}
}



/*! Called when the table is double clicked.
	Displays a dialog to change name and description. */
void ConnectionGroupTableView::tableDoubleClicked(QModelIndex index){
	if(index.column() == ConnectionGroupModel::DESC_COL){
		try{
			ConnectionGroupDialog* dialog = new ConnectionGroupDialog(connectionGroupModel->getInfo(index), this);
			if(dialog->exec() == QDialog::Accepted){
				connectionGroupModel->reload();
			}
			delete dialog;
		}
		catch(SpikeStreamException& ex){
			qCritical()<<ex.getMessage();
		}
	}
}

