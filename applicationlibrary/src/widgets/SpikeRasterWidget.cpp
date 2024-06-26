//SpikeStream includes
#include "Globals.h"
#include "SpikeRasterWidget.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QResizeEvent>
#include <QDebug>


/*! Constructor */
SpikeRasterWidget::SpikeRasterWidget(QList<NeuronGroup*>& neuronGroupList, QWidget* parent) : QWidget(parent){
	//Store list of neuron groups that are being monitored
	this->neuronGroupList = neuronGroupList;

	//Calculate number of neurons along with their offset and colour
	QList<unsigned> hueList = getHueList(neuronGroupList.size());
	numNeurons = 0;
	int hueCntr = 0;
	for(QList<NeuronGroup*>::iterator iter = neuronGroupList.begin(); iter != neuronGroupList.end(); ++iter){
		neurGrpOffsetMap[(*iter)->getID()] = numNeurons;
		neurGrpColorMap[(*iter)->getID()] = hueList.at(hueCntr);
		++hueCntr;
		numNeurons += (*iter)->size();
	}

	//Initialize display variables
	blackAndWhiteMode = false;
	xAxisTickLength = 2;
	yAxisTickLength = 2;
	fontSize = 8;  
	neurGrpNameFontSize = 10;
	yAxisPadding = yAxisTickLength + 1 + fontSize * QString::number(numNeurons).length();
	xAxisPadding = xAxisTickLength + 1 + fontSize;
	backgroundColor = qRgb(255,255,255);
	axesColor = qRgb(0,0,0);

	//Initialize other variables
	numTimeSteps = 1000;
	minTimeStep = 0;

	//Create buffer image
	imageWidth = numTimeSteps + yAxisPadding + 1;

	imageHeight = numNeurons + xAxisPadding + 1 + numNeurons/10; // Additional Space for swimlanes (zHeight= 10)	
	bufferImage = new QImage(imageWidth, imageHeight, QImage::Format_RGB32);
	bufferImage->fill(backgroundColor);
	updateAxes = true;

	//Set up widget graphic properties
	if(imageWidth > 500)
		setMinimumWidth(500);
	else
		setMinimumWidth(imageWidth);
	if(imageHeight > 500)
		setMinimumHeight(500);
	else
		setMinimumHeight(imageHeight);
	//setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_PaintOnScreen);
	widgetWidth = this->size().width();
	widgetHeight = this->size().height();
}


/*! Destructor */
SpikeRasterWidget::~SpikeRasterWidget(){
	delete bufferImage;
}


/*----------------------------------------------------------*/
/*------               PUBLIC METHODS                 ------*/
/*----------------------------------------------------------*/

/*! Adds spikes to the plot, filtered by the neuron groups that are being monitored. */
void SpikeRasterWidget::addSpikes(const QList<unsigned>& firingNeuronIDs, int timeStep){
	int writeLocation = timeStep % numTimeSteps +  yAxisPadding + 1;

	////Add spikes to the current image
	//unsigned tmpNeurID;
	//QList<unsigned>::const_iterator firingNeuronIDsEnd = firingNeuronIDs.end();
	//for(QList<unsigned>::const_iterator neurIter = firingNeuronIDs.begin(); neurIter != firingNeuronIDsEnd; ++neurIter){
	//	for(QList<NeuronGroup*>::iterator neurGrpIter = neuronGroupList.begin(); neurGrpIter != neuronGroupList.end(); ++neurGrpIter){
	//		if((*neurGrpIter)->contains(*neurIter)){
	//			tmpNeurID = *neurIter - (*neurGrpIter)->getStartNeuronID() + neurGrpOffsetMap[(*neurGrpIter)->getID()];
	//			bufferImage->setPixel(writeLocation, imageHeight - xAxisPadding - tmpNeurID - 1, neurGrpColorMap[(*neurGrpIter)->getID()]);
	//			break;
	//		}
	//	}
	//}


	int swimlanes = 0; // increment each height / neuron group
	int zHeight = 10;
	//Add spikes to the current image  
	// ascending according to iterator _not_ the internal IDs
	unsigned tmpNeurID = 0;
	unsigned tmpClr;
	QList<unsigned>::const_iterator firingNeuronIDsEnd = firingNeuronIDs.end();
	for(QList<NeuronGroup*>::iterator neurGrpIter = neuronGroupList.begin(); neurGrpIter != neuronGroupList.end(); ++neurGrpIter){
		//tmpNeurID = neurGrpOffsetMap[(*neurGrpIter)->getID()];
		tmpClr = neurGrpColorMap[(*neurGrpIter)->getID()];
		NeuronPositionIterator neurGrpEnd = (*neurGrpIter)->positionEnd();
		for(NeuronPositionIterator posIter = (*neurGrpIter)->positionBegin(); posIter != neurGrpEnd; ++posIter){
			if(firingNeuronIDs.contains((*posIter)->getID())) {
				bufferImage->setPixel(writeLocation, imageHeight - xAxisPadding - tmpNeurID - 1 - swimlanes, tmpClr);		
			}
			if(tmpNeurID % zHeight == 0) swimlanes++;
			tmpNeurID++;
		}
	}


	//Advance the time step
	increaseTimeStep(timeStep);
	repaint();
}


/*! Controls whether spikes are rendered in black or varying hues. */
void SpikeRasterWidget::setBlackAndWhite(bool on){
	blackAndWhiteMode = on;

	//Reload hues
	QList<unsigned> hueList = getHueList(neuronGroupList.size());
	int hueCntr = 0;
	for(QList<NeuronGroup*>::iterator iter = neuronGroupList.begin(); iter != neuronGroupList.end(); ++iter){
		neurGrpColorMap[(*iter)->getID()] = hueList.at(hueCntr);
		++hueCntr;
	}
	repaint();
}


/*----------------------------------------------------------*/
/*------              PROTECTED METHODS               ------*/
/*----------------------------------------------------------*/

/*! Prompts user to save image as a file. */
void SpikeRasterWidget::mouseDoubleClickEvent (QMouseEvent*){
	//QString fileType = "BMP";
	QString fileType = "PNG";   // FIXED PNG is supported in Qt 6
	// feat: provide default file name derived from Network name and simulation run
	//		 requires option to automatically overwrite 

	QString filePath = getFilePath("*." + fileType.toLower());

	//Fix extension
	if(!filePath.endsWith(fileType.toLower()))
		filePath += "." + fileType.toLower();

	//If file exists, check to see if user wants to overwrite file
	if(QFile::exists(filePath)){
		int response = QMessageBox::warning(this, "Overwrite File?", filePath + " already exists.\nAre you sure that you want to overwrite it?", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
		if(response != QMessageBox::Ok)
			return;
	}

	//Save file
	bufferImage->save(filePath, fileType.toLatin1(), 100);
}


//Inherited from QWidget
void SpikeRasterWidget::paintEvent(QPaintEvent*){
	QPainter painter(this); 
		// FIXED  Warning::QWidget::paintEngine: Should no longer be called
		//https://stackoverflow.com/questions/52902375/qwidgetpaintengine-should-no-longer-be-called-in-a-qtreewidget-derived-class
		//QPainter painter(viewport());

	if(updateAxes){
		QPainter axesPainter(bufferImage);
		paintAxes(axesPainter);
		paintNeuronGroupNames(axesPainter);
	}

	painter.drawImage(0, 0, bufferImage->scaled(widgetWidth, widgetHeight));
	painter.end();
}


//Inherited from QWidget
void SpikeRasterWidget::resizeEvent(QResizeEvent* /*event*/) {
	widgetWidth = this->size().width();
	widgetHeight = this->size().height();
}


/*----------------------------------------------------------*/
/*------              PRIVATE METHODS                 ------*/
/*----------------------------------------------------------*/

/*! Enables user to enter a file path */
QString SpikeRasterWidget::getFilePath(QString fileFilter){
	QFileDialog dialog(this);
	dialog.setDirectory(Globals::getWorkingDirectory());
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter( QString("Image file (" + fileFilter + ")") );
	dialog.setWindowTitle("Save");
	dialog.setViewMode(QFileDialog::Detail);
	QStringList fileNames;
	if (dialog.exec())
		fileNames = dialog.selectedFiles();
	if(fileNames.size() > 0)
		return fileNames[0];
	else
		return QString("");
}


/*! Returns a list of hues with the required lightness */
QList<unsigned> SpikeRasterWidget::getHueList(unsigned numItems){
	QList<unsigned> tmpHueList;

	//Return black spike colour in black and white mode
	if(blackAndWhiteMode){
		for(unsigned i=0; i<numItems; ++i)
			tmpHueList.append(qRgb(0, 0, 0));
		return tmpHueList;
	}

	//Colour mode
	unsigned numHues = 1530;

	//Sanity check
	if(numItems > numHues)
		throw SpikeStreamException("Number of requested hues exceeds that available.");

	//Prevent divide by zero
	if(numItems == 0)
		return tmpHueList;
	else if(numItems == 1){
		tmpHueList.append(qRgb(255, 0, 0));
		return tmpHueList;
	}

	for(unsigned i=0; i <= numHues - numHues%(numItems-1); i += numHues/(numItems-1)){
		if(i < 255)
			tmpHueList.append(qRgb(255, i, 0));
		else if (i < 510)
			tmpHueList.append(qRgb(i-255, 255, 0));
		else if (i < 765)
			tmpHueList.append(qRgb(0, 255, i - 510));
		else if (i < 1020)
			tmpHueList.append(qRgb(0, i-765, 255));
		else if (i < 1275)
			tmpHueList.append(qRgb(i-1020, 0, 255));
		else
			tmpHueList.append(qRgb(255, 0, i-1275));
	}

	if((int)numItems != tmpHueList.size())
		throw SpikeStreamException("Incorrect number of hues found. NumItems: " + QString::number(numItems) + "; size: " + QString::number(tmpHueList.size()));

	return tmpHueList;
}


/*! Advances the time step and redraws axes if necessary */
void SpikeRasterWidget::increaseTimeStep(int currentTimeStep){
	//Check to see if we have moved out of the X axis range
	if( ((currentTimeStep /numTimeSteps) * numTimeSteps) != minTimeStep){
		minTimeStep = (currentTimeStep/numTimeSteps) * numTimeSteps;
		bufferImage->fill(backgroundColor);
		updateAxes = true;
	}
}


/*! Paints the X and Y axes */
void SpikeRasterWidget::paintAxes(QPainter& painter) {
	painter.setPen( QPen(Qt::black) );

	//Set up font
	QFont font;
	font.setFamily("Helvetica");
	font.setWeight(QFont::Light);
	font.setPixelSize(fontSize);
	painter.setFont(font);

	//Paint axes
	paintYAxis(painter);
	paintXAxis(painter);

	//Set flag recording that axes are updated
	updateAxes = false;
}


/*! Paints the names of the neuron groups */
void SpikeRasterWidget::paintNeuronGroupNames(QPainter& painter){
	//Set up font
	QFont font;
	font.setFamily("Helvetica");
	font.setWeight(QFont::Light);
	font.setPixelSize(neurGrpNameFontSize);
	painter.setFont(font);

	for(QList<NeuronGroup*>::iterator neurGrpIter = neuronGroupList.begin(); neurGrpIter != neuronGroupList.end(); ++neurGrpIter){
		painter.setPen( QPen( QColor(neurGrpColorMap[(*neurGrpIter)->getID()]) ) );
		painter.drawText(
				imageWidth/2, imageHeight - xAxisPadding - neurGrpOffsetMap[(*neurGrpIter)->getID()],
				(*neurGrpIter)->getInfo().getName()
		);
	}
}


/*! set axis ticks for every 50ms */
void SpikeRasterWidget::paintXAxis(QPainter& painter) {
	//Draw X axis line
	painter.drawLine(0, imageHeight-xAxisPadding, imageWidth, imageHeight-xAxisPadding);

	//Draw ticks
	for(int i=0; i<numTimeSteps; i += numTimeSteps/10)
		paintXAxisTick(painter, i + yAxisPadding, minTimeStep + i);
}


/*! Paints a single tick on the X axis */
void SpikeRasterWidget::paintXAxisTick(QPainter& painter, int xPos, int labelX) {
	painter.drawLine(xPos,
					 imageHeight - xAxisPadding,
					 xPos,
					 imageHeight-xAxisPadding + xAxisTickLength
	);
	int textW = fontSize*5;
	painter.drawText(
			xPos - textW/2, imageHeight-xAxisPadding + xAxisTickLength,
			textW, fontSize,
			Qt::AlignHCenter,
			QString::number(labelX) + "ms"
	);
}


/*! Paints a single tick on the Y axis */
void SpikeRasterWidget::paintYAxisTick(QPainter& painter, int yPos, int label) {
//	painter.drawLine(yAxisPadding, yPos, yAxisPadding - yAxisTickLength, yPos);
	painter.setPen( QPen( QColor("lightgray") ) );
	painter.drawLine(yAxisPadding, yPos, imageWidth, yPos);
	painter.drawText(
			0, yPos + fontSize/2,
			QString::number(label)
	);
}


/*! Paints the Y axis */
void SpikeRasterWidget::paintYAxis(QPainter& painter){
	//Draw Y axis line
	painter.drawLine(yAxisPadding, 0, yAxisPadding, imageHeight);

	////Draw ticks
	//for(int i=0; i<numNeurons; i += numNeurons/10){
	//	paintYAxisTick(painter, imageHeight - xAxisPadding - i, i);
	//}

	//Draw y-tick for every 10 neurons
	int inc = 10+1;  // 10 neurons + 1 for the swimlane
	for(int i=inc; i<=numNeurons; i += inc){
		paintYAxisTick(painter, imageHeight - xAxisPadding - i, i);
	}


	//// Draw ticks for each Neuron Group 
	//for(QList<NeuronGroup*>::iterator neurGrpIter = neuronGroupList.begin(); neurGrpIter != neuronGroupList.end(); ++neurGrpIter){
	//	neurGrpOffsetMap[(*neurGrpIter)->getID()];
	//}

}


