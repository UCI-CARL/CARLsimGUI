#ifndef INJECTORLOADERWIDGET_H
#define INJECTORLOADERWIDGET_H

//SpikeStream includes
#include "AbstractInjectorWidget.h"

//Qt includes
#include <QComboBox>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

// 
#include "api.h" 


namespace spikestream {

	/*! Loads injectors and allows user to switch between them using a combo box. */
	class CARLSIMWRAPPER_LIB_EXPORT InjectorLoaderWidget : public QWidget {
		Q_OBJECT

		  public:
			  InjectorLoaderWidget(QString pluginPath, QWidget* parent=0);
			  ~InjectorLoaderWidget();
			  QList<AbstractInjectorWidget*> getAbstractInjectorWidgets() {return abstractInjectorList; }

		  private slots:
			  void showInjectorWidget(int layerID);

		  private:
			  //=========================  VARIABLES  =============================
			  /*! Combo box to select plugin */
			  QComboBox* pluginsCombo;

			  /*! Stores the position in the stacked widget where the experiment plugins are stored. */
			  QHash<QString, int> pluginWidgetMap;

			  /*! List of pointers to the experiment plugins */
			  QList<AbstractInjectorWidget*> abstractInjectorList;

			  /*! Vertical box organizing layout */
			  QVBoxLayout *mainVerticalBox;

			  /*! Widget with layers holding the different experiment plugins */
			  QStackedWidget* stackedWidget;
	};

}

#endif//INJECTORLOADERWIDGET_H
