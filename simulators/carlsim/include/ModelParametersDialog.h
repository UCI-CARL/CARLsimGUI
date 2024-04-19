#ifndef MODELPARAMETERSDIALOG_H
#define MODELARAMETERSDIALOG_H

//SpikeStream includes


//Qt includes
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDialog>
#include <QLabel>
#include <QLayout>
#include <QDateTimeEdit>

#include "CarlsimWrapper.h"

// 
#include "api.h" 



namespace spikestream {

	/*! Displays the current  parameters for the CUDA simulator. */
	class CARLSIMWRAPPER_LIB_EXPORT ModelParametersDialog : public QDialog {
		Q_OBJECT
		Q_PROPERTY(ModelAutosync m_autosync READ getAutosync WRITE setAutosync)     		
		Q_PROPERTY(ModelStart m_start READ getStart WRITE setStart)
		Q_ENUMS(ModelAutosync)  
		Q_ENUMS(ModelStart)  


		public:

			enum ModelStart {REALTIME=0, FIXED, CHANNEL}; 
			enum ModelAutosync { OFF=0, FIRST, LAST}; 

			ModelParametersDialog(
				const QDateTime &simtime,
				double speed, // realtime vs. Simulation --> update vs. intervall
				unsigned step_ms,	// ms of model time per each step (alpha)
				ModelStart start,  // snyc if any buffer of any channel is filled
				ModelAutosync autosync,  // snyc if any buffer of any channel is filled
				int lag_ms, // ms behind real time
				QWidget* parent=0);
			~ModelParametersDialog();

			QDateTime getSimtime() { return m_simtime; }
			ModelStart getStart() { return m_start; }
			ModelAutosync getAutosync() { return m_autosync; }
			double getSpeed() { return m_speed; }
			unsigned getStep_ms() { return m_step_ms; }
			int getLag_ms() { return m_lag_ms; }
			
			void setStart(ModelStart start) { m_start = start; }
			void setAutosync(ModelAutosync autosync) { m_autosync = autosync; }

		private slots:
			void defaultButtonClicked();
			void okButtonClicked();
			void resetSimtime();

		private:
			//=====================  VARIABLES  ======================

			QDateTime m_simtime;
			double m_speed; 
			unsigned m_step_ms;	
			ModelStart m_start;  
			ModelAutosync m_autosync;  
			int m_lag_ms; 

			/*! Combo box to select the autosync mode */
			QDateTimeEdit* simtimeEdit;
			QDoubleSpinBox* speedSpin;
			QSpinBox* stepSpin;
			QComboBox* startCombo;
			QComboBox* autosyncCombo;
			QSpinBox* lagSpin;
			

			//=========================  METHODS  ===========================
			void addButtons(QVBoxLayout* mainVLayout);
			void storeParameterValues();
	};

}

#endif//CARLSIMPARAMETERSDIALOG_H

