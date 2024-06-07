#ifndef ANALYSISSPECTROGRAM_H
#define ANALYSISSPECTROGRAM_H

// SpikeStream 0.3
#include "api.h"

//SpikeStream includes
#include "AnalysisSpectrogram.h"

//Qt includes
#include <qprinter.h>
#include <qprintdialog.h>

//Qwt includes
#include "qwt_plot.h"
#include <qwt_color_map.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>
#include <qwt_raster_data.h>


namespace spikestream {

	/*! Zoom class required by Qwt. */
	class SPIKESTREAM_APP_LIB_EXPORT MyZoomer: public QwtPlotZoomer {
		public:
			MyZoomer(QwtPlotCanvas *canvas): QwtPlotZoomer(canvas)	{
				setTrackerMode(AlwaysOn);
			}

			virtual QwtText trackerText(const QPoint& /*QwtDoublePoint&*/ pos) const {
				QColor bg(Qt::white);
				bg.setAlpha(200);
				QwtText text = QwtPlotZoomer::trackerText(pos);
				text.setBackgroundBrush( QBrush( bg ));
				return text;
			}
	};

// NO AUTMOC

	/*! Displays a spectrogram using the supplied data. Used for information spectrogram plots. */
	class SPIKESTREAM_APP_LIB_EXPORT  AnalysisSpectrogram : public QwtPlot {
/* Patch: error LNK2001: unresolved external symbol
	1>  Creating library ..\lib\\spikestreamapplication0.lib and object ..\lib\\spikestreamapplication0.exp
	1>  moc_AnalysisSpectrogram.obj : error LNK2001: unresolved external symbol "public: static struct QMetaObject const QwtPlot::staticMetaObject" (?staticMetaObject@QwtPlot@@2UQMetaObject@@B)
	1> ..\lib\\spikestreamapplication0.dll : fatal error LNK1120: 1 unresolved externals
*/
// Fix for Linux comment out 
//#ifndef _MSC_VER
//		Q_OBJECT
//#endif

			// QT AUTOMOC 
			//https://cmake.org/cmake/help/latest/prop_sf/SKIP_AUTOMOC.html#prop_sf:SKIP_AUTOMOC
			//set_property(SOURCE file.h PROPERTY SKIP_AUTOMOC ON)

			//1 > C:\Test\SpikeStream - 0.4.0 - build\applicationlibrary\spikestreamapplication_autogen\include_Debug\6YEA5652QU / moc_AnalysisSpectrogram.cpp(59, 1) : error C2509 : 'qt_static_metacall' : member function not declared in 'spikestream::AnalysisSpectrogram' (compiling source file C : \Test\SpikeStream - 0.4.0 - build\applicationlibrary\spikestreamapplication_autogen\mocs_compilation.cpp)
			//1 > C:\Test\SpikeStream - 0.4.0 - src\applicationlibrary\include\AnalysisSpectrogram.h(47) : message: see declaration of 'spikestream::AnalysisSpectrogram' (compiling source file C : \Test\SpikeStream - 0.4.0 - build\applicationlibrary\spikestreamapplication_autogen\mocs_compilation.cpp)
			//1 > C:\Test\SpikeStream - 0.4.0 - build\applicationlibrary\spikestreamapplication_autogen\include_Debug\6YEA5652QU / moc_AnalysisSpectrogram.cpp(77, 1) : error C2509 : 'metaObject' : member function not declared in 'spikestream::AnalysisSpectrogram' (compiling source file C : \Test\SpikeStream - 0.4.0 - build\applicationlibrary\spikestreamapplication_autogen\mocs_compilation.cpp)
			//1 > C:\Test\SpikeStream - 0.4.0 - src\applicationlibrary\include\AnalysisSpectrogram.h(47) : message: see declaration of 'spikestream::AnalysisSpectrogram' (compiling source file C : \Test\SpikeStream - 0.4.0 - build\applicationlibrary\spikestreamapplication_autogen\mocs_compilation.cpp)
			//1 > C:\Test\SpikeStream - 0.4.0 - build\applicationlibrary\spikestreamapplication_autogen\include_Debug\6YEA5652QU / moc_AnalysisSpectrogram.cpp(82, 1) : error C2509 : 'qt_metacast' : member function not declared in 'spikestream::AnalysisSpectrogram' (compiling source file C : \Test\SpikeStream - 0.4.0 - build\applicationlibrary\spikestreamapplication_autogen\mocs_compilation.cpp)
			//1 > C:\Test\SpikeStream - 0.4.0 - src\applicationlibrary\include\AnalysisSpectrogram.h(47) : message: see declaration of 'spikestream::AnalysisSpectrogram' (compiling source file C : \Test\SpikeStream - 0.4.0 - build\applicationlibrary\spikestreamapplication_autogen\mocs_compilation.cpp)
			//1 > C:\Test\SpikeStream - 0.4.0 - build\applicationlibrary\spikestreamapplication_autogen\include_Debug\6YEA5652QU / moc_AnalysisSpectrogram.cpp(90, 1) : error C2509 : 'qt_metacall' : member function not declared in 'spikestream::AnalysisSpectrogram' (compiling source file C : \Test\SpikeStream - 0.4.0 - build\applicationlibrary\spikestreamapplication_autogen\mocs_compilation.cpp)
			//1 > C:\Test\SpikeStream - 0.4.0 - src\applicationlibrary\include\AnalysisSpectrogram.h(47) : message: see declaration of 'spikestream::AnalysisSpectrogram' (compiling source file C : \Test\SpikeStream - 0.4.0 - build\applicationlibrary\spikestreamapplication_autogen\mocs_compilation.cpp)*/



		public:
			AnalysisSpectrogram(QWidget *parent, QwtRasterData* data);
			~AnalysisSpectrogram();
			void showContour(bool on);
			void showSpectrogram(bool on);
			void printPlot();
			void rescale();

		private:
			QwtPlotSpectrogram *d_spectrogram;
	};

}


#endif//ANALYSISSPECTROGRAM_H


