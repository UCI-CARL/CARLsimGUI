//SpikeStream includes
#include "SpikeRasterDialog.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QComboBox>
#include <QDebug>
#include <QLayout>


/*! Constructor */
SpikeRasterDialog::SpikeRasterDialog(QList<NeuronGroup*> neuronGroupList, QWidget* parent) : QDialog(parent){

	// FIXED Warning: QWindowsWindow::setGeometry: Unable to set geometry
	setMinimumSize(1024, 600); // FIXME get Screen properties

	QVBoxLayout* mainVBox = new QVBoxLayout(this);

	try{
		spikeRasterWidget = new SpikeRasterWidget(neuronGroupList, this);

		QComboBox* colorCombo = new QComboBox();
		colorCombo->addItem("Color");
		colorCombo->addItem("Black and white");
		connect(colorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(colorComboChanged(int)));

		// FIXME display in color mode correctly from the beginning, including the legend
		colorCombo->setCurrentIndex(1); // Fixed: at least the display is consistent now

		mainVBox->addWidget(colorCombo);
		mainVBox->addWidget(spikeRasterWidget);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<"Raster error: "<<ex.getMessage();
	}
	catch(...){
		qCritical()<<"An unknown exception occurred";
	}
}


/*! Destructor */
SpikeRasterDialog::~SpikeRasterDialog(){
}


/*----------------------------------------------------------*/
/*------                PRIVATE SLOTS                 ------*/
/*----------------------------------------------------------*/

/*! Sets the spike raster to black and white or coloured. */
void SpikeRasterDialog::colorComboChanged(int index){
	if(index == 0){
		spikeRasterWidget->setBlackAndWhite(false);
	}
	else if (index == 1){
		spikeRasterWidget->setBlackAndWhite(true);
	}
	else{
		throw SpikeStreamException("Index not recognized: " + QString::number(index));
	}
}

/*! Adds firing neuron data and replots the spike raster. */
void SpikeRasterDialog::addData(const QList<unsigned>& firingNeuronIDs, unsigned timeStep){
	spikeRasterWidget->addSpikes(firingNeuronIDs,timeStep);
}
