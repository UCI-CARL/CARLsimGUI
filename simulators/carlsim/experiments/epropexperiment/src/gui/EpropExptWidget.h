#ifndef VTEEXPTWIDGET_H
#define VTEEXPTWIDGET_H

//SpikeStream includes
#include "AbstractExperimentWidget.h"
#include "EpropExptManager.h"
#include "CarlsimWrapper.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QHash>
#include <QTextEdit>

namespace spikestream {

	class AbstractMonitorWidget;
	class carlsim_monitors::OatSpikeMonitor;

	/*! Graphical interface for carrying out a pop1 experiment */
	class EpropExptWidget : public AbstractExperimentWidget {
		Q_OBJECT

		public:
			EpropExptWidget(QWidget* parent = 0);
			~EpropExptWidget();
			void setWrapper(void *wrapper);

		signals:
			void requiresMonitorWidget(AbstractMonitorWidget*&, QString);

		public slots:
			void assignOatMonitor();
			void updateMonitor();
			void updateTemplate(int i);
			void editCosts();

		private slots:
			void managerFinished();
			void startExperiment();
			void stopExperiment();
			void updateStatus(QString statusMsg);

		protected:
			void buildParameters();

			// Each experiment can have a config (else 0 is default experiment)
			// the template initialize the parameters when the widget is loaded
			// or if the templated was changed
			QComboBox* templateCombo;
			QHash<QString, QString> defaults;
			ConfigLoader* configLoader;
			QList<ConfigLoader*>epropConfigLoaders;
			QString costs;  // in GUI the costs are represented as String
			QStringList segments; // learn route (Boone)
			QStringList landmarks; // trails (Boone)
			QStringList starts; // Start points for Morris trails
			QStringList ends; // End Point(s) for Morris trails
			QStringList selection; // (Random) selection of trails of start x end


		private:
			//=====================  VARIABLES  ====================
			/*! Wrapper of CARLsimthat is used in the experiments. */
			CarlsimWrapper* carlsimWrapper;

			/*! Manager that runs the experiments */
			EpropExptManager* vteExptManager;

			/*! Widget holding messages from the manager */
			QTextEdit* statusTextEdit;


			//=====================  METHODS  ======================
			void checkNetwork();

			//Keep hard earned SpikeMonitor as reference
			carlsim_monitors::OatSpikeMonitor* spikeMonitor;
			NeuronGroup* placeCells; // PlaceCells


	};

}

#endif//VTEEXPTWIDGET_H

