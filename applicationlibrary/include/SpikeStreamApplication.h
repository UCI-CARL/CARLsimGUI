#ifndef SPIKESTREAMAPPLICATION_H
#define SPIKESTREAMAPPLICATION_H

 // SpikeStream 0.3
#include "api.h"

//Qt includes
#include <QApplication>

//Other includes
//#include <sys/time.h>

namespace spikestream {

    /*! Inherits from QApplication so that it can filter out XEvents during
		slow renders. Is the QApplication for the application. */
    class SPIKESTREAM_APP_LIB_EXPORT SpikeStreamApplication : public QApplication {
		Q_OBJECT

		public:
			SpikeStreamApplication(int & argc, char ** argv);
			~SpikeStreamApplication();
			bool notify(QObject* receiver, QEvent* event);

		protected:
			#ifdef LINUX32_SPIKESTREAM
				//Inherited from QApplication - Linux specific
				bool x11EventFilter( XEvent * );
			#endif//LINUX32_SPIKESTREAM

		private slots:
			void startRender();
			void stopRender();

		private:
			//============================ VARIABLES ==============================
			/*! Records the duration of each render.*/
			unsigned int renderDuration_ms;

			/*! Records the time of the last key press at the start of the render.*/
			unsigned int startRenderKeyEventTime;

			/*! Records the time of each key press event.*/
			unsigned int keyEventTime;


			/*! Records when rendering is in progress.*/
			bool rendering;

			/*! Relative Path to config file that can be passed as first argument */
			const char* config = NULL; 


			//============================ METHODS ================================
			/*! Declare copy constructor private so it cannot be used inadvertently.*/
			SpikeStreamApplication(const SpikeStreamApplication&);

			/*! Declare assignment private so it cannot be used inadvertently.*/
			SpikeStreamApplication operator=(const SpikeStreamApplication&);

    };
}


#endif//SPIKESTREAMAPPLICATION_H


