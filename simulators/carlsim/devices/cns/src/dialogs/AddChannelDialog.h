#ifndef CNSADDCHANNELDIALOG_H
#define CNSADDCHANNELDIALOG_H

#ifdef Q_MOC_RUN
#	define BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#endif 

//SpikeStream includes
#include "NeuronGroup.h"

//Qt includes
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QPushButton>

//Cns includes
#include <CARLsimIO/Channel/InboundChannel/InboundChannelFactory.hpp>
#include <CARLsimIO/Channel/OutboundChannel/OutboundChannelFactory.hpp>
#include <CARLsimIO/Description.hpp>
#include <CARLsimIO/Channel/InboundChannel/InboundChannel.hpp>
#include <CARLsimIO/Channel/OutboundChannel/OutboundChannel.hpp>
#include <CARLsimIO/Writer/WriterFactory.hpp>
#include <CARLsimIO/Reader/ReaderFactory.hpp>
using namespace carlsimio;

//Other includes
#include <map>
#include <vector>
using namespace std;

namespace spikestream {

	/*! Dialog that enables user to configure channel linking with
		a neuron group in SpikeStream */
	class AddChannelDialog : public QDialog {
		Q_OBJECT

		public:
			AddChannelDialog(QWidget* parent = 0);
			AddChannelDialog(QString ipString, QString portString, QWidget* parent = 0);
			~AddChannelDialog();
			bool isInboundChannel(){ return inboundChannel; }
			InboundChannel* getInputChannel() { return newInboundChannel; }
			NeuronGroup* getNeuronGroup(){ return neuronGroup; }
			OutboundChannel* getOutboundChannel() { return newOutboundChannel; }

		private slots:
			void cancelButtonClicked();
			void configureChannel();
			void configureReaderWriter();
			void okButtonClicked();
			void resetButtonClicked();
			void selectChannelType();

		private:
			//=====================  VARIABLES  =======================
			/*! String holding IP address of DNS server */
			QString dnsIPAddress;

			/*! String holding port of DNS server */
			QString dnsPort;

			/*! Records whether ip address and port have been set */
			bool ipAddressSet;

			/*! Holds information about the available channel types */
			QComboBox* channelTypeCombo;

			/*! Button to select channel type */
			QPushButton* selectChannelTypeButton;

			/*! Holds information about available channels */
			QComboBox* channelCombo;

			/*! Label for channel combo */
			QLabel* channelLabel;

			/*! Launches dialog to configure the channel */
			QPushButton* configureChannelButton;

			/*! Holds information about the available readers and writers for the channel */
			QComboBox* readerWriterCombo;

			/*! Label for reader / writer combo */
			QLabel* readerWriterLabel;

			/*! Launches dialog to configure reader/writer */
			QPushButton* configureReaderWriterButton;

			/*! Confirms creation of channel and closes dialog */
			QPushButton* okButton;

			/*! Neuron group that will be linked to the channel. */
			NeuronGroup* neuronGroup;

			/*! Records whether channel is input or output */
			bool inboundChannel;

			/*! Creates output channels */
			OutboundChannelFactory* outboundFactory;

			/*! Creates inbound channels */
			InboundChannelFactory* inboundFactory;

			/*! Creates writers */
			WriterFactory* writerFactory;

			/*! Creates readers */
			ReaderFactory* readerFactory;

			/*! Descriptions of the available inbound channels. */
			// Uncomment the line below prevents the plug-in from loading !!
			vector<Description> inChanDesc;
			// The following patch works
			//vector<Description> *inChanDesc ;

			/*! Descriptions of the available output channels. */
			vector<Description> outChanDesc;

			/*! Descriptions of the available writers. */
			vector<Description> writerDescVector;

			/*! Descriptions of the available readers */
			vector<Description> readerDescVector ;

			/*! Input channel that we are creating. */
			InboundChannel* newInboundChannel;

			/*! Output channel that we are creating */
			OutboundChannel* newOutboundChannel;

			/** Properties of the channel */
			map<string, Property> channelProperties;

			/** Properties of the reader or writer */
			map<string, Property> readerWriterProperties;


			//======================  METHODS  =======================
			void buildGUI();
			bool configureProperties(map<string, Property> srcPropertyMap, map<string, Property>& destPropertyMap, bool selectNeuronGroup = false, string channelName="");
			void reset();

	};

}

#endif//CNSADDCHANNELDIALOG_H
