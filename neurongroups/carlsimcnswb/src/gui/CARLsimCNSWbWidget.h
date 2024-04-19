#ifndef CARLSIMCNSWBWIDGET_H
#define CARLSIMCNSWBWIDGET_H

//SpikeStream includes
#include "CARLsimCNSWbBuilderThread.h"
#include "ConfigLoader.h"

//Qt includes
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLayout>
#include <QLineEdit>
#include <QProgressDialog>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QWidget>


namespace spikestream {

	/*! Gathers information for creating a cuboid neuron group */
	class CARLsimCNSWbWidget : public QWidget {
		Q_OBJECT

		public:
			CARLsimCNSWbWidget(QWidget* parent = 0);
			~CARLsimCNSWbWidget();


		private slots:
			void addButtonClicked();
			void builderThreadFinished();
			void updateProgress(int stepsCompleted, int totalSteps, QString message);
			void updateTemplate(int);

		private:
			//=====================  VARIABLES  =====================
			/*! Class that runs as a separate thread to create neuron group */
			CARLsimCNSWbBuilderThread* builderThread;

			/*! Displays progress with the operation */
			QProgressDialog* progressDialog;



			QLineEdit* prefixEdit; // corresponds to the name 


			QComboBox* templateCombo; // Template for prefil the fields 

			QHash<QString, QString> defaults;
			ConfigLoader* configLoader;
			QList<ConfigLoader*> wmConfigLoaders;

			// Izhikevich 4 param model
			struct NeuronParam_t {
				//QComboBox* nmCombo;
				QDoubleSpinBox* aSpin;
				QDoubleSpinBox* bSpin;
				QDoubleSpinBox* cSpin;
				QDoubleSpinBox* dSpin;
				void addGroup(QString name, QGridLayout* gridLayout, ConfigLoader* configLoader); // Group Content
			} ps, ls, tof, whls, vel, led, rgb, frnt, body, accl, gyro;
		
				//rn, bfb, lc, sn, vta, na;    

			QSpinBox* securitiesSpin; // 1..10, default 1
			QSpinBox* statesSpin; // 2..5, default 3

			// Layout
			QComboBox* policyCombo; // 0..Standard
			QSpinBox* spaceSpin; // 0..10, default 3;


			/*! To enter the name of the neuron group */
			QLineEdit* nameEdit;

			/*! To enter a description of the neuron group */
			QLineEdit* descriptionEdit;

			/*! To enter x position of neuron group to be created */
			QLineEdit* xPosEdit;

			/*! To enter y position of neuron group to be created */
			QLineEdit* yPosEdit;

			/*! To enter z position of neuron group to be created */
			QLineEdit* zPosEdit;

			/*! To enter width of neuron group */
			QLineEdit* widthEdit;

			/*! To enter width of neuron group */
			QLineEdit* lengthEdit;

			/*! To enter height of neuron group */
			QLineEdit* heightEdit;

			/*! To enter spacing between neurons */
			QLineEdit* spacingEdit;

			/*! To enter density of neuron group - the probability that a neuron is added at a particular position */
			QLineEdit* densityEdit;


			/*! Map of line edits to set the percentage of neurons of different types
				Key is the neuron type ID. */
			QHash<unsigned int, QLineEdit*> neuronTypeEditMap;

			/*! Flag to prevent calls to progress dialog while it is redrawing. */
			bool updatingProgress;

			//=====================  METHODS  =======================
			void checkInput(QLineEdit* inputEdit, const QString& errorMessage);
	};

}

#endif//CARLSIMCNSWBWIDGET_H
