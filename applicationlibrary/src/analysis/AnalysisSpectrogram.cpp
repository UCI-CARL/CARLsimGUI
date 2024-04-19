//SpikeStream includes
#include "AnalysisSpectrogram.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

#include <qwt_color_map.h>
#include <qwt_plot_spectrogram.h>


/*! Constructor */
AnalysisSpectrogram::AnalysisSpectrogram (QWidget* parent, QwtRasterData* data) : QwtPlot( parent ){
	// Show a title
	//setTitle( "This is an Example" );
	d_spectrogram = new QwtPlotSpectrogram();

	QwtLinearColorMap colorMap(Qt::darkCyan, Qt::red);
	colorMap.addColorStop(0.1, Qt::cyan);
	colorMap.addColorStop(0.6, Qt::green);
	colorMap.addColorStop(0.95, Qt::yellow);

	d_spectrogram->setColorMap(&colorMap);

	d_spectrogram->setData(data);
	d_spectrogram->attach(this);


	// A color bar on the right axis
	QwtScaleWidget *rightAxis = axisWidget(QwtPlot::yRight);
	rightAxis->setColorBarEnabled(true);

	enableAxis(QwtPlot::yRight);

	//Rescale to match data
	setAxisMaxMinor( QwtPlot::yLeft, 0);
	setAxisMaxMinor( QwtPlot::xBottom, 0);
	rescale();

	//Change canvas background
	setCanvasBackground(Qt::white);


	//Change fonts
	QFont axisFont("Arial", 12, QFont::Normal);
	setAxisFont(QwtPlot::yLeft, axisFont);
	setAxisFont(QwtPlot::yRight, axisFont);
	setAxisFont(QwtPlot::xBottom, axisFont);

	plotLayout()->setAlignCanvasToScales(true);
	replot();

}


/*! Destructor */
AnalysisSpectrogram::~AnalysisSpectrogram(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Rescales the plot */
void AnalysisSpectrogram::rescale(){
}


/*! Shows the contours of the plot */
void AnalysisSpectrogram::showContour(bool on){
	d_spectrogram->setDisplayMode(QwtPlotSpectrogram::ContourMode, on);
	replot();
}


/*! Makes the spectrogram visible */
void AnalysisSpectrogram::showSpectrogram(bool on){
}


/*! Prints the plot to a pdf file
	FIXME: THIS PROBABLY DOESN'T WORK. */
void AnalysisSpectrogram::printPlot() {
	QPrinter printer;
	printer.setOrientation(QPrinter::Landscape);
	printer.setOutputFileName("/tmp/spectrogram.pdf");
	QPrintDialog dialog(&printer);
}
