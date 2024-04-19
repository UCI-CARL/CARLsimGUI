#ifndef NEURONPARAMETERSEDITDIALOG_H
#define NEURONPARAMETERSEDITDIALOG_H

// SpikeStream 0.3
#include "api.h"

//SpikeStream includes
#include "AbstractParametersEditDialog.h"
#include "NeuronGroupInfo.h"

namespace spikestream {

	/*! Edits the parameters for a particular neuron group */
	class SPIKESTREAM_APP_LIB_EXPORT NeuronParametersEditDialog : public AbstractParametersEditDialog {
		Q_OBJECT

		public:
			NeuronParametersEditDialog(const NeuronGroupInfo& neurGrpInfo, const QList<ParameterInfo>& paramInfoList, const QHash<QString, double>& currParamValueMap, QWidget* parent=0);
			~NeuronParametersEditDialog();


		private slots:
			void defaultButtonClicked();
			void okButtonClicked();


		private:
			//====================  VARIABLES  ====================
			/*! Information about the neuron group whose parameters are being edited */
			NeuronGroupInfo neurGrpInfo;
	};

}

#endif//NEURONPARAMETERSEDITDIALOG_H
