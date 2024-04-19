//SpikeStream includes
#include "NoiseInjectorTableView.h"
#include "SpikeStreamException.h"

using namespace spikestream::carlsim_injectors;

//Qt includes
#include <QDebug>
#include <QHeaderView>

/*! Constructor */
NoiseInjectorTableView::NoiseInjectorTableView(QWidget* parent, NoiseInjectorModel* model) : QTableView(parent) {
	//Set up the view
	setShowGrid(false);
	setSelectionMode(QAbstractItemView::NoSelection);
	connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(tableClicked(QModelIndex)));
	connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(tableDoubleClicked(QModelIndex)));

	//Header click events
	this->horizontalHeader()->setSectionsClickable(true);
	connect(this->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(headerClicked(int)));

	//Set the model for this view and show it
	this->currentInjectorModel = model;
	setModel(model);
	show();
	resizeHeaders();
}


/*! Destructor */
NoiseInjectorTableView::~NoiseInjectorTableView(){
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Sets up the size of the headers. Does not work in the constructor because there
	is not any data or model */
void NoiseInjectorTableView::resizeHeaders(){
	QHeaderView* hHeader = horizontalHeader();
	hHeader->setMinimumSectionSize(10);
	hHeader->resizeSection(hHeader->logicalIndex(NoiseInjectorModel::SUSTAIN_COL), 20);//Visibility icon
	hHeader->resizeSection(hHeader->logicalIndex(NoiseInjectorModel::ID_COL), 50);//ID
	hHeader->resizeSection(hHeader->logicalIndex(NoiseInjectorModel::NAME_COL), 200);//Name
	hHeader->resizeSection(hHeader->logicalIndex(NoiseInjectorModel::PERCENT_COL), 50+20);//Percentage
	hHeader->resizeSection(hHeader->logicalIndex(NoiseInjectorModel::CURRENT_COL), 50+20);//Current mA
	hHeader->resizeSection(hHeader->logicalIndex(NoiseInjectorModel::DELETE_COL), 20); //Deletion
	hHeader->setDefaultAlignment(Qt::AlignLeft);
}


/*--------------------------------------------------------*/
/*-------             PRIVATE SLOTS                -------*/
/*--------------------------------------------------------*/

/*! Called when the header of the table is clicked */
void NoiseInjectorTableView::headerClicked(int column){
	qDebug() << "headerClicked column: " << column;
	if(column == NoiseInjectorModel::SUSTAIN_COL)
		this->currentInjectorModel->sustainAllOrNone();
}


/*! Called when the table is clicked and passes this information on to model. */
void NoiseInjectorTableView::tableClicked(QModelIndex index){
	qDebug() << "tableClicked index "<<  index.row() << "  "<< index.column();

	if(index.column() == NoiseInjectorModel::SUSTAIN_COL){
		model()->setData(index, false);
	}
	if(index.column() <= NoiseInjectorModel::CURRENT_COL) {
		emit injectorSelected(index.row());
	} 	
	if(index.column() == NoiseInjectorModel::DELETE_COL) {
		currentInjectorModel->removeInjector(index.row());
	}

}


/*! Called when the table is double clicked.
	Displays a dialog to change name and description. */
void NoiseInjectorTableView::tableDoubleClicked(QModelIndex index){
	qDebug()<<"tableDoubleClicked index " << index.row() << index.column();
}
