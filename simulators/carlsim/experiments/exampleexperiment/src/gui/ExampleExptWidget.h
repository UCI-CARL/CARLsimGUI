#ifndef EXAMPLEEXPTWIDGET_H
#define EXAMPLEEXPTWIDGET_H

//SpikeStream includes
#include "AbstractExperimentWidget.h"
#include "ExampleExptManager.h"
#include "CarlsimWrapper.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QHash>
#include <QTextEdit>

namespace spikestream {

	/*! Graphical interface for carrying out a pop1 experiment */
	class ExampleExptWidget : public AbstractExperimentWidget {
		Q_OBJECT

		public:
			ExampleExptWidget(QWidget* parent = 0);
			~ExampleExptWidget();
			void setWrapper(void *wrapper);

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
			ExampleExptManager* exampleExptManager;

			/*! Widget holding messages from the manager */
			QTextEdit* statusTextEdit;


			//=====================  METHODS  ======================
			void checkNetwork();

	};

}

#endif//EXAMPLEEXPTWIDGET_H

