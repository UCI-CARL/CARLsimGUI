#ifndef CARLSIMOATWIDGET_H
#define CARLSIMOATWIDGET_H

//SpikeStream includes
#include "AbstractMonitorWidget.h"
#include "CarlsimOatModel.h"
#include "CarlsimOatManager.h"
#include "CarlsimWrapper.h"
#include "SpikeStreamTypes.h"


//Qt includes
#include <QHash>
#include <QTextEdit>
#include <QCheckBox>

#include "api.h" 

namespace spikestream {

	namespace carlsim_monitors {


		// API requrired for access

		/*! Graphical interface for carrying out a pop1 experiment */
		class CARLSIMOAT_LIB_EXPORT CarlsimOatWidget : public AbstractMonitorWidget {
			Q_OBJECT

		public:
			CarlsimOatWidget(QWidget* parent = 0);
			~CarlsimOatWidget();
			void setWrapper(void* wrapper);
			void findMonitor(OatMonitor*& monitor, QString type, QString name);  // without slot

		signals:
			void timeStepChanged(unsigned int step);
			void monitorsLoaded();
			void monitorsUnloaded();

		private slots:
			void loadMonitors();
			void unloadMonitors();
			void setup();
			void printMonitor(int index);
			void monitorStartRecording(unsigned step);
			void monitorStopRecording(unsigned step);
			void emit_timeStepChanged(unsigned int); 
			void saveSimulation(); 

		protected:
			//	void buildParameters();

		private:
			//=====================  VARIABLES  ====================
			/*! Wrapper of CARLsim that is used in the experiments. */
			CarlsimWrapper* carlsimWrapper;

			/*! Manager that runs the experiments */
			CarlsimOatManager* oatManager;

			/*! Widget holding messages from the manager */
			QTextEdit* statusTextEdit;

			CarlsimOatModel* oatModel;

			QCheckBox* synapsesCheckBox;
			//=====================  METHODS  ======================

		};
	}
}

#endif//CARLSIMOATWIDGET_H