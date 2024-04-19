#ifndef CURRENTINJECTORWIDGET_H
#define CURRENTINJECTORWIDGET_H

//SpikeStream includes
#include "AbstractMonitorWidget.h"
#include "CurrentInjectorManager.h"
#include "CarlsimWrapper.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QHash>
#include <QTextEdit>

namespace spikestream {

	/*! Graphical interface for carrying out a pop1 experiment */
	class CurrentInjectorWidget : public AbstractMonitorWidget {
		Q_OBJECT

		public:
			CurrentInjectorWidget(QWidget* parent = 0);
			~CurrentInjectorWidget();
			void setWrapper(void *wrapper);

		//private slots:
		//	void managerFinished();
		//	void startExperiment();
		//	void stopExperiment();
		//	void updateStatus(QString statusMsg);

		protected:
		//	void buildParameters();

		private:
			//=====================  VARIABLES  ====================
			/*! Wrapper of NeMo that is used in the experiments. */
			CarlsimWrapper* carlsimWrapper;

			/*! Manager that runs the experiments */
			CurrentInjectorManager* oatManager;

			/*! Widget holding messages from the manager */
			QTextEdit* statusTextEdit;


			//=====================  METHODS  ======================

	};

}

#endif//CURRENTINJECTORWIDGET_H