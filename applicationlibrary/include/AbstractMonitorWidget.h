#ifndef ABSTRACTMONITORWIDGET_H
#define ABSTRACTMONITORWIDGET_H

// SpikeStream 0.5
#include "api.h"

//SpikeStream includes
//#include "AbstractMonitorManager.h"

//Qt includes
#include <QWidget>


/*

Abstract GUI = Generic Functionliy

Activate / Deactivate Monitor 
Type: Neuron or Connection Group 
Activate 

File <-> CARLsim specific
Group 




*/


namespace spikestream {

	class SPIKESTREAM_APP_LIB_EXPORT AbstractMonitorWidget : public QWidget {

		public:
			AbstractMonitorWidget(QWidget* parent= 0);
			virtual ~AbstractMonitorWidget();
			//virtual AbstractMonitorManager* getMonitorManager() = 0;
			virtual void setWrapper(void* wrapper) = 0;

	};

}
#endif//ABSTRACTMONITORWIDGET_H

