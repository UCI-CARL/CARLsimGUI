#ifndef NETWORKVIEWERPROPERTIES
#define NETWORKVIEWERPROPERTIES

// SpikeStream 0.3
#include "api.h"

//SpikeStream includes
#include "TruthTableDialog.h"
using namespace spikestream;

//Qt includes
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QSlider>
#include <QWidget>
#include <QRadioButton>
#include <QPushButton>
#include <QProgressBar>

namespace spikestream {

	/*! Controls the properties of the 3D network viewer. */
	class SPIKESTREAM_APP_LIB_EXPORT NetworkViewerProperties : public QWidget {
		Q_OBJECT

		public:
			NetworkViewerProperties(QWidget* parent=0);
			~NetworkViewerProperties();

		private slots:
			void fromToSelectionChanged(int index);
			void networkDisplayChanged();
			void neuronTransparencyChanged(int newTransparency);
			void connectionTransparencyChanged(int newTransparency);
			void posNegSelectionChanged(int index);
			void setRenderMode();
			void showTruthTable();
			void updateConnectionCount();
			void setRenderWeights(); 
			void weightDisplayChanged(int buttonID);
			void styleDisplayChanged(int buttonID); 
			void setRenderDelays();
			void delayDisplayChanged(int buttonID);

		private:
			//=======================  VARIABLES  =======================
			/*! Indicates that we are showing all selected connections */
			QRadioButton* allConsButt;

			/*! Indicates that we are showing connections to/from a single neuron */
			QRadioButton* conSingleNeurButt;

			/*! Indicates that we are showing connections between two neurons */
			QRadioButton* conBetweenNeurButt;

			/*! Label showing the neuron id when a single neuron is selected */
			QLabel* singleNeuronIDLabel;

			/*! Allows user to filter by direction of connection */
			QComboBox* fromToCombo;

			/*! Label showing id of from neuron in between mode */
			QLabel* fromNeuronIDLabel;

			/*! Label showing to neuron id in between mode */
			QLabel* toNeuronIDLabel;

			/*! Allows user to filter connections by connection weight */
			QComboBox* posNegCombo;

			/*! Keep reference to label saying "from" to enable and disable it */
			QLabel* fromLabel;

			/*! Keep reference to label saying "to" to enable and disable it */
			QLabel* toLabel;

			/*! Label listing number of connections in table */
			QLabel* numberOfConnectionsLabel;

			/*! Button to launch truth table dialog when to connections are shown and when neuron type is appropriate */
			QPushButton* truthTableButton;

			/*! Non modal dialog to display truth table associated with a neuron */
			TruthTableDialog* truthTableDialog;

			/*! Check box to change to full render mode */
			QCheckBox* renderCheckBox;

			/*! Widget holding full render controls */
			QWidget* fullRenderControlsWidget;

			/*! Check box to change to full render mode */
			QCheckBox* weightsCheckBox;

			/*! Check box to change to full render mode */
			QCheckBox* delaysCheckBox;

			QButtonGroup* weightButtonGroup;

			QButtonGroup* styleButtonGroup;

			QRadioButton* synapseStyleButton;
			QRadioButton* middleStyleButton;
			QRadioButton* evenStyleButton;

			QButtonGroup* delayButtonGroup;


			//=======================  METHODS  =========================
			void hideTruthTableDialog();
			void showAllConnections();
			void showBetweenConnections();
			void showSingleConnections();
			void showTruthTableDialog(unsigned int neuronID);
	};

}

#endif// NETWORKVIEWERPROPERTIES

