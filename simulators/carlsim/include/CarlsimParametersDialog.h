#ifndef CARLSIMPARAMETERSDIALOG_H
#define CARLSIMPARAMETERSDIALOG_H

//SpikeStream includes


//Qt includes
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>

//Other includes
#include "carlsim_types.h"

// 
#include "api.h" 
 
namespace spikestream {

	class CarlsimLib;

	/*! Displays the current  parameters for the CUDA simulator. */
	class CARLSIMWRAPPER_LIB_EXPORT CarlsimParametersDialog : public QDialog {
		Q_OBJECT

		public:
			CarlsimParametersDialog(CarlsimLib* lib_, carlsim41::carlsim_configuration_t carlsimConfig, unsigned stdpFunctionID, QWidget* parent=0);
			~CarlsimParametersDialog();
			carlsim41::carlsim_configuration_t getCarlsimConfig(){ return currentCarlsimConfig; }
			unsigned getSTDPFunctionID() { return stdpFunctionID; }

		private slots:
			void backendChanged(int index);
			void defaultButtonClicked();
			void okButtonClicked();
			void stdpParameterButtonClicked();


		private:
			//=====================  VARIABLES  ======================
			/*! Current configuration */
			carlsim41::carlsim_configuration_t currentCarlsimConfig;

			/*! Default optimal configuration created */
			carlsim41::carlsim_configuration_t defaultCarlsimConfig;

			/*! The current stdp function ID */
			unsigned stdpFunctionID;

			/*! Combo box allowing selection of backend */
			QComboBox* backendCombo;

			/*! Combo containing current CUDA devices */
			QComboBox* cudaDeviceCombo;

			/*! Label for CUDA devices, which needs to be shown and hidden. */
			QLabel* cudaDeviceLabel;

			/*! Label for STDP default parameter */
			QLabel* stdpLabel;

			/*! Combo box to select STDP function */
			QComboBox* stdpCombo;


			// reference, container for the network, config, etc.
			CarlsimLib* carlsim; 

			//=========================  METHODS  ===========================
			void addButtons(QVBoxLayout* mainVLayout);
			void checkCarlsimOutput(int result, const QString& errorMessage);
			void getCudaDevices(QComboBox* combo);
			void getStdpFunctions(QComboBox* combo);
			void loadParameters(carlsim41::carlsim_configuration_t config);
			void storeParameterValues();
	};

}

#endif//CARLSIMPARAMETERSDIALOG_H

