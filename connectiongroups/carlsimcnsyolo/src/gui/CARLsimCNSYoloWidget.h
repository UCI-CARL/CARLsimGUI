#ifndef CARLSIMCNSYOLOWIDGET_H
#define CARLSIMCNSYOLOWIDGET_H

//SpikeStream includes
#include "AbstractConnectionWidget.h"
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

	/*! Gathers information for creating a random connection pattern between two neuron groups. */
	class CARLsimCNSYoloWidget : public AbstractConnectionWidget {
		Q_OBJECT

		public:
			CARLsimCNSYoloWidget(QWidget* parent = 0);
			~CARLsimCNSYoloWidget();

		private slots:
			void updateTemplate(int);
		
		protected:
			bool checkInputs();
			ConnectionGroupInfo getConnectionGroupInfo();

		private:

			QComboBox* templateCombo; // Template for prefil the fields 

			QHash<QString, QString> defaults;
			ConfigLoader* configLoader;
			QList<ConfigLoader*> configLoaders;

			QLineEdit* prefixEdit; // corresponds to the name 

			// Connection parameters 
			struct ConnectionParam_t {
				bool active;
				QDoubleSpinBox* weightsSpin;
				QDoubleSpinBox* weightFactorSpin; // CUBA/COBA adaption
				QSpinBox* delaysSpin;
				void addGroup(QString name, QGridLayout* gridLayout, ConfigLoader* configLoader, bool inhib = false ); // Group Content
			} dir2vel, dist2vel, prob2obj, obj2tof;


			//=====================  VARIABLES  =====================
			/*! To enter a description of the neuron group */
			QLineEdit* descriptionEdit;

			/*! Combo to select neuron group that connection is from */
			QComboBox* fromCombo;

			/*! Combo to select neuron group that connection is to */
			QComboBox* toCombo;

			/*! The min value of weight range 1 */
			QLineEdit* minWeightRange1Edit;

			/*! The max value of weight range 1 */
			QLineEdit* maxWeightRange1Edit;

			/*! The proportion of weights falling within weight range 1 */
			QLineEdit* weightRange1PercentEdit;

			/*! The min value of weight range 1 */
			QLineEdit* minWeightRange2Edit;

			/*! The max value of weight range 1 */
			QLineEdit* maxWeightRange2Edit;

			/*! The min value of delay */
			QLineEdit* minDelayEdit;

			/*! The max value of delay */
			QLineEdit* maxDelayEdit;

			/*! The probability of making a connection between two neurons */
			QLineEdit* connectionProbabilityEdit;

			/*! To enter the type of synapse */
			QComboBox* synapseTypeCombo;

			/*! To enter random seed */
			QLineEdit* seedEdit;

			//=====================  METHODS  ======================
			void buildGUI(QVBoxLayout* mainVBox);

	};

}

#endif//CARLSIMCNSYOLOWIDGET_H
