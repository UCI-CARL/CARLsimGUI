#ifndef ABSTRACTINJECTORWIDGET_H
#define ABSTRACTINJECTORWIDGET_H

// SpikeStream 0.5
#include "api.h"

//SpikeStream includes
//#include "AbstractInjectorManager.h"

//Qt includes
#include <QWidget>

namespace spikestream {

	class SPIKESTREAM_APP_LIB_EXPORT AbstractInjectorWidget : public QWidget {

		public:
			AbstractInjectorWidget(QWidget* parent= 0);
			virtual ~AbstractInjectorWidget();
			//virtual AbstractInjectorManager* getInjectorManager() = 0;
			virtual void setWrapper(void* wrapper) = 0;

	};

}
#endif//ABSTRACTINJECTORWIDGET_H

