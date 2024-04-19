#ifndef CNSEDITPROPERTIESDIALOG_H
#define CNSEDITPROPERTIESDIALOG_H

#ifdef Q_MOC_RUN
#	define BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#endif 


//SpikeStream includes
#include "NeuronGroup.h"
#include "ParameterInfo.h"

//Qt includes
#include <QComboBox>
#include <QDialog>
#include <QHash>
#include <QLayout>
#include <QLineEdit>

//Cns includes
#include "CARLsimIO/Property.hpp"
using namespace carlsimio;

//Other includes
#include <map>
#include <string>
using namespace std;

namespace spikestream {

	/*! Allows editing of properties */
	class EditPropertiesDialog : public QDialog {
		Q_OBJECT

		public:
			EditPropertiesDialog(map<string, Property> propertyMap, bool disableReadOnly, QWidget* parent = 0);
			EditPropertiesDialog(string channelName, map<string, Property> propertyMap, bool disableReadOnly, QList<NeuronGroup*> neuronGroupList, QWidget* parent = 0);
			~EditPropertiesDialog();
			NeuronGroup* getNeuronGroup() { return neuronGroup; }
			map<string, Property> getPropertyMap() { return propertyMap; }


		protected slots:
			void updateNeuronCombo();
			void okButtonClicked();


		private:
			//===================  VARIABLES  ====================
			/*! Map that is being edited */
			map<string, Property> propertyMap;

			/*! List of neuron groups - only used in neuronGroupSelectionMode */
			QList<NeuronGroup*> neuronGroupList;

			/*! Combo box holding compatible neuron groups */
			QComboBox* neuronGroupCombo;

			/*! Map of the line edits
				Key is the name of the parameter; value is a pointer to the line edit */
			QHash<QString, QLineEdit*> lineEditMap;

			/*! Map of the combo boxes
				Key is the name of the parameter; value is a pointer to the combo */
			QHash<QString, QComboBox*> comboMap;

			/*! Records whether the properties of a channel are being configured. */
			bool neuronGroupSelectionMode;

			/*! Neuron group that has been selected */
			NeuronGroup* neuronGroup;

			/*! Controls whether read only properties should be disabled */
			bool disableReadOnly;

			string channelName;

			//====================  METHODS  =====================
			void addButtons(QVBoxLayout* mainVLayout);
			void addNeuronGroups(QVBoxLayout* mainVLayout);
			void addParameters(QVBoxLayout* mainVLayout);
			QString getNeuronGroupName(NeuronGroup* neuronGroup);
			void storeParameterValues();
			void updateCompatibleNeuronGroups(int numberOfNeurons);
			size_t calcNumberOfNeurons();

	};

}

#endif//CNSEDITPROPERTIESDIALOG_H
