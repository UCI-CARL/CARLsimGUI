#ifndef CNSSXMLLOADER_H
#define CNSSXMLLOADER_H

//Qt includes
//#include <QXmlDefaultHandler>
//#include <QXmlSimpleReader>
#include <qxml.h>
#include <QHash>
#include <QString>
#include <QStack>

#include <CARLsimIO/Channel/InboundChannel/InboundChannelFactory.hpp>
#include <CARLsimIO/Reader/ReaderFactory.hpp>
#include <CARLsimIO/Channel/OutboundChannel/OutboundChannelFactory.hpp>
#include <CARLsimIO/Writer/WriterFactory.hpp>
#include <CARLsimIO/Description.hpp>
#include <CARLsimIO/Property.hpp>

namespace carlsimio {
	class Channel;
	class Reader;
}
using namespace carlsimio;

namespace spikestream {

	// Forwards
	class CnsManager;
	class NeuronGroup;

	/*!  */
	class CnsXmlLoader : public QXmlDefaultHandler {
		public:
			CnsXmlLoader(CnsManager* manager);
			~CnsXmlLoader();
			

			/*! Load the devices from the given file path. */
			bool loadFromPath(const QString& path);

		protected:
			//Parsing methods inherited from QXmlDefaultHandler
			bool characters ( const QString & ch );
			bool endElement( const QString&, const QString&, const QString& );
			bool startDocument();
			bool startElement( const QString&, const QString&, const QString& , const QXmlAttributes& );

			//Error handling methods inherited from QXmlDefaultHandler
			bool error ( const QXmlParseException & exception );
			QString errorString();
			bool fatalError ( const QXmlParseException & exception );
			bool warning ( const QXmlParseException & exception );
			QString contextXPath();

		private:

			/*! Holds the reference to the underlying manager to apply addChannel */
			CnsManager* manager;
	
			///*! Have passed a <parameter> opening tag */
			//bool loadingParameter;

			Channel *channel;
			Reader *reader;
			Writer *writer;
			NeuronGroup *neuronGroup;

			InboundChannelFactory inboundChannelFactory;
			Description inboundChannelDesc;
			map<string, Property> inboundChannelProps;

			OutboundChannelFactory outboundChannelFactory;
			Description outboundChannelDesc;
			map<string, Property> outboundChannelProps;

			ReaderFactory readerFactory;
			Description readerDesc;
			map<string, Property> readerProps;

			WriterFactory writerFactory;
			Description writerDesc;
			map<string, Property> writerProps;

			//! Properties are not nested so it's much simplier to keep a reference to the current 
			map<string, Property> *props; 
			Property prop;

			QStack<QString> context;

			QMap<QString, int> quantities;  //! used to create an Xpath
			QString previous;

			// Fix: 
			QStack<int> children;

		};

}

#endif//CNSSXMLLOADER_H

