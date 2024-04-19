#ifndef VTEEXPTWIDGET_H
#define VTEEXPTWIDGET_H

//SpikeStream includes
#include "AbstractExperimentWidget.h"
#include "VteExptManager.h"
#include "CarlsimWrapper.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QHash>
#include <QTextEdit>

namespace spikestream {

	class AbstractMonitorWidget;

	/*! Graphical interface for carrying out a pop1 experiment */
	class VteExptWidget : public AbstractExperimentWidget {
		Q_OBJECT

		public:
			VteExptWidget(QWidget* parent = 0);
			~VteExptWidget();
			void setWrapper(void *wrapper);

		signals:
			void requiresMonitorWidget(AbstractMonitorWidget*&, QString);

		public slots:
			void assignOatMonitor() {};


		private slots:
			void managerFinished();
			void startExperiment();
			void stopExperiment();
			void updateStatus(QString statusMsg);

		protected:
			void buildParameters();

		private:
			//=====================  VARIABLES  ====================
			/*! Wrapper of CARLsimthat is used in the experiments. */
			CarlsimWrapper* carlsimWrapper;

			/*! Manager that runs the experiments */
			VteExptManager* vteExptManager;

			/*! Widget holding messages from the manager */
			QTextEdit* statusTextEdit;


			//=====================  METHODS  ======================
			void checkNetwork();

	};

}

#endif//VTEEXPTWIDGET_H

