#ifndef MONITORLOADERWIDGET_H
#define MONITORLOADERWIDGET_H

//SpikeStream includes
#include "AbstractMonitorWidget.h"

//Qt includes
#include <QComboBox>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

// 
#include "api.h" 

namespace spikestream {

	/*! Loads monitors and allows user to switch between them using a combo box. */
	class CARLSIMWRAPPER_LIB_EXPORT MonitorLoaderWidget : public QWidget {
		Q_OBJECT

		  public:
			  MonitorLoaderWidget(QString pluginPath, QWidget* parent=0);
			  ~MonitorLoaderWidget();
			  QList<AbstractMonitorWidget*> getAbstractMonitorWidgets() {return abstractMonitorList; }

		  private slots:
			  void showMonitorWidget(int layerID);

		  private:
			  //=========================  VARIABLES  =============================
			  /*! Combo box to select plugin */
			  QComboBox* pluginsCombo;

			  /*! Stores the position in the stacked widget where the device plugins are stored. */
			  QHash<QString, int> pluginWidgetMap;

			  /*! List of pointers to the device plugins */
			  QList<AbstractMonitorWidget*> abstractMonitorList;

			  /*! Vertical box organizing layout */
			  QVBoxLayout *mainVerticalBox;

			  /*! Widget with layers holding the different experiment plugins */
			  QStackedWidget* stackedWidget;
	};

}

#endif//DEVICESLOADERWIDGET_H
