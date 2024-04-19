#ifndef CNSWIDGET_H
#define CNSWIDGET_H

//SpikeStream includes
#include "AbstractDeviceWidget.h"
#include "ChannelModel.h"
#include "CnsManager.h"

//Qt includes
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTimeEdit>
#include <QWidget>
#include <QTranslator>

namespace spikestream {

	class AddChannelDialog;

	/*! Interface used to set up communication between library CARLsim. */
	class CnsWidget : public AbstractDeviceWidget {
		Q_OBJECT

		public:
			CnsWidget(QWidget* parent = 0);
			~CnsWidget();
			AbstractDeviceManager* getDeviceManager();

		signals:
			void channelsLoaded();

		private slots:
			void addChannelModeless();		
			void addChannelAccept();		
			void setFireOrCurrent(int index);
			void setReportingLevel(int level);
			void setIPAddressButtonClicked();
			void unsetIPAddressButtonClicked();
			void networkChanged();
			void autoaddChannels(); 
			void connectAutoUpdateChannels(); // automaticaly updates after each step
			void updateChannels(); 
			void resetChannels();
		private:
			//====================  VARIABLES  ======================
			/*! Manager that interfaces with library */
			CnsManager* cnsManager;

			/*! Button to add channels */
			QPushButton* addChannelButton;

			/*! Model linked to Manager, which displays current connected channels */
			ChannelModel* channelModel;

			/*! Place to enter the DNS server address */
			QLineEdit* dnsEdit;

			/*! Place to enter the port address */
			QLineEdit* portEdit;

			/*! Does the system try to connect to an IP address */
			bool ipAddressSet;

			/*! Button to connect to DNS server */
			QPushButton* connectButton;

			/*! Button to disconnect from DNS server */
			QPushButton* disconnectButton;

			/*! Switches between firing neurons or injecting current for the whole device */
			QComboBox* fireOrCurrentCombo;

			QPushButton* updateChannelsButton;

			QPushButton* resetChannelsButton;

			// Modeless Dialogs
			//AddChannelDialog* m_addChannelDialog;   Do not use a member since elsewise only one modeless dialog would be supported

			QTranslator* translator;

			/*! Address the configuration, see CR-74 */
			QString xmlConfigPath;

			//====================  METHODS  ========================

	};
}

#endif//CNSWIDGET_H
