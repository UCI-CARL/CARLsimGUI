#ifndef SYNAPSEPARAMETERSEDITDIALOG_H
#define SYNAPSEPARAMETERSEDITDIALOG_H

// SpikeStream 0.3
#include "api.h"

//SpikeStream includes
#include "AbstractParametersEditDialog.h"
#include "ConnectionGroupInfo.h"

namespace spikestream {

	/*! Edits the parameters for a particular connection group */
	class SPIKESTREAM_APP_LIB_EXPORT SynapseParametersEditDialog : public AbstractParametersEditDialog {
		Q_OBJECT

		public:
			SynapseParametersEditDialog(const ConnectionGroupInfo& conGrpInfo, const QList<ParameterInfo>& paramInfoList, const QHash<QString, double>& currParamValueMap, QWidget* parent=0);
			~SynapseParametersEditDialog();


		private slots:
			void defaultButtonClicked();
			void okButtonClicked();


		private:
			//====================  VARIABLES  ====================
			/*! Information about the connection group whose parameters are being edited */
			ConnectionGroupInfo conGrpInfo;
	};

}

#endif//SYNAPSEPARAMETERSEDITDIALOG_H
