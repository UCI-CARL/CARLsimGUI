//SpikeStream includes
#include "CarlsimOatTableView.h"
#include "SpikeStreamException.h"

using namespace spikestream::carlsim_monitors;

//Qt includes
#include <QDebug>
#include <QHeaderView>

/*! Constructor */
CarlsimOatTableView::CarlsimOatTableView(QWidget* parent, CarlsimOatModel* model) : QTableView(parent) {
	////Set up the view
	setShowGrid(false);
	setSelectionMode(QAbstractItemView::NoSelection);
	connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(tableClicked(QModelIndex)));
	connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(tableDoubleClicked(QModelIndex)));

	////Header click events
	this->horizontalHeader()->setSectionsClickable(true);
	connect(this->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(headerClicked(int)));

	//Set the model for this view and show it
	this->oatModel = model;
	setModel(model);
	show();
	resizeHeaders();
}


/*! Destructor */
CarlsimOatTableView::~CarlsimOatTableView(){
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Sets up the size of the headers. Does not work in the constructor because there
	is not any data or model */
void CarlsimOatTableView::resizeHeaders(){
	QHeaderView* hHeader = horizontalHeader();
	hHeader->setMinimumSectionSize(10);
	hHeader->resizeSection(hHeader->logicalIndex(CarlsimOatModel::ACTIVE_COL), 20);		//
	hHeader->resizeSection(hHeader->logicalIndex(CarlsimOatModel::REC_COL), 20);		//
	hHeader->resizeSection(hHeader->logicalIndex(CarlsimOatModel::STOP_COL), 20);		//
	hHeader->resizeSection(hHeader->logicalIndex(CarlsimOatModel::TYPE_COL), 100);		// Type
	hHeader->resizeSection(hHeader->logicalIndex(CarlsimOatModel::GROUP_COL), 250);		// Group ID
	hHeader->resizeSection(hHeader->logicalIndex(CarlsimOatModel::START_COL), 50+20);
	hHeader->resizeSection(hHeader->logicalIndex(CarlsimOatModel::END_COL), 50+20);
	hHeader->resizeSection(hHeader->logicalIndex(CarlsimOatModel::PERIOD_COL), 50+20); 
	hHeader->resizeSection(hHeader->logicalIndex(CarlsimOatModel::PRINT_COL), 20);		//Deletion
	hHeader->resizeSection(hHeader->logicalIndex(CarlsimOatModel::PERSIST_COL), 20);		//Deletion
	hHeader->resizeSection(hHeader->logicalIndex(CarlsimOatModel::DELETE_COL), 20);		//Deletion
	hHeader->setDefaultAlignment(Qt::AlignLeft);
}


/*--------------------------------------------------------*/
/*-------             PRIVATE SLOTS                -------*/
/*--------------------------------------------------------*/

/*! Called when the header of the table is clicked */
void CarlsimOatTableView::headerClicked(int /*column*/){
}


/*! Called when the table is clicked and passes this information on to model. */
void CarlsimOatTableView::tableClicked(QModelIndex index){
	//qDebug() << "tableClicked index "<<  index.row() << "  "<< index.column();

	if(index.column() == CarlsimOatModel::REC_COL){
		model()->setData(index, false);
	} else 
	if (index.column() == CarlsimOatModel::STOP_COL) {
		model()->setData(index, false);
	}
	else
	if (index.column() == CarlsimOatModel::DELETE_COL) {
		model()->setData(index, false);
	}
	else
	if(index.column() == CarlsimOatModel::PRINT_COL) {
		emit monitorPrint(index.row());
	} 	
}


/*! Called when the table is double clicked.
	Displays a dialog to change name and description. */
void CarlsimOatTableView::tableDoubleClicked(QModelIndex index){
	//qDebug()<<"tableDoubleClicked index " << index.row() << index.column();

}
