#ifndef CUBOIDWIDGET_H
#define CUBOIDWIDGET_H

//SpikeStream includes
#include "CuboidBuilderThread.h"

//Qt includes
#include <QComboBox>
#include <QLineEdit>
#include <QProgressDialog>
#include <QWidget>

namespace spikestream {

	/*! Gathers information for creating a cuboid neuron group */
	class CuboidWidget : public QWidget {
		Q_OBJECT

		public:
			CuboidWidget(QWidget* parent = 0);
			~CuboidWidget();


		private slots:
			void addButtonClicked();
			void builderThreadFinished();
			void updateProgress(int stepsCompleted, int totalSteps, QString message);


		private:
			//=====================  VARIABLES  =====================
			/*! Class that runs as a separate thread to create neuron group */
			CuboidBuilderThread* builderThread;

			/*! Displays progress with the operation */
			QProgressDialog* progressDialog;

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


#ifdef CARLSIMGUI_USES_CARLSIM
			/*! To enter conductance of neuron group - the probability that a neuron is added at a particular position */
			QLineEdit* conductancesEdit;
#endif

			/*! Map of line edits to set the percentage of neurons of different types
				Key is the neuron type ID. */
			QHash<unsigned int, QLineEdit*> neuronTypeEditMap;

			/*! Flag to prevent calls to progress dialog while it is redrawing. */
			bool updatingProgress;

			//=====================  METHODS  =======================
			void checkInput(QLineEdit* inputEdit, const QString& errorMessage);
	};

}

#endif//CUBOIDWIDGET_H
