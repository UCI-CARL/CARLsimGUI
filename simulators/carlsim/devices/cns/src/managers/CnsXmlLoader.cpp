
//spikestream.Cns includes
#include "CnsXmlLoader.h"
#include "CnsManager.h"
#include "SpikeStreamXMLException.h"
#include "Globals.h"
#include "NeuronGroup.h"

using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QFile>

//Cns includes
#include "CARLsimIO/Channel/InboundChannel/InboundChannel.hpp"
#include "CARLsimIO/Channel/OutboundChannel/OutboundChannel.hpp"
#include "CARLsimIO/CARLsimIOException.hpp"
using namespace carlsimio;


/*! Constructor */
CnsXmlLoader::CnsXmlLoader(CnsManager* _manager): manager(_manager) {
}


/*! Destructor */
CnsXmlLoader::~CnsXmlLoader(){
}


/*----------------------------------------------------------*/
/*-----                  PUBLIC METHODS                -----*/
/*----------------------------------------------------------*/

/*! Parses the XML from file and add corresponding channels the CnsManager */
bool CnsXmlLoader::loadFromPath(const QString& path){

	QFile file (path);
	if(!file.open(QIODevice::ReadOnly))
		return false;

	QXmlSimpleReader xmlReader;
    QXmlInputSource xmlInput(&file);
    xmlReader.setContentHandler(this);
    xmlReader.setErrorHandler(this);
    xmlReader.parse(xmlInput);

	file.close();
    return true;
}


/*----------------------------------------------------------*/
/*-----               PROTECTED METHODS                -----*/
/*----------------------------------------------------------*/

/*! Called when parser encounters characters. */
bool CnsXmlLoader::characters(const QString& chars){

	if(context.top() == "Property") {
		bool ok;
		switch(prop.getType()) {
			case Property::Integer: 
				prop.setInt(chars.toInt(&ok));
				if(!ok) throw SpikeStreamXMLException(QString("Conversion to int failed: %1 at %2").arg(chars).arg(contextXPath()));
				break;
			case Property::Double: 
				prop.setDouble(chars.toDouble(&ok));
				if(!ok) throw SpikeStreamXMLException(QString("Conversion to double failed: %1 at %2").arg(chars).arg(contextXPath()));
				break;
			case Property::String: 
				prop.setString(chars.toStdString());
				break;
			default:
				throw SpikeStreamXMLException(QString("Invalid Property Type: %1 at %2").arg(prop.getType()).arg(contextXPath()));
		}
	}
		
    return true; 
}


/*! Called when the parser encounters the end of an element. */
bool CnsXmlLoader::endElement( const QString&, const QString&, const QString& elemName){

	QString &top = context.pop();
	Q_ASSERT(top == elemName);
	
	if(elemName == "InboundChannel") {
		//! Create a Cns InboundChannel
		InboundChannelFactory factory;	
		InboundChannel* inboundChannel = factory.create(inboundChannelDesc, reader, inboundChannelProps);
		//! Assign the new channel to its neuron group and add it to the mannager
		manager->addChannel(inboundChannel, neuronGroup); //Add channel to finspikesManager
	} else
	if(elemName == "OutboundChannel") {
		//! Create a Cns OutboundChannel
		OutboundChannelFactory factory;	
		OutboundChannel* outboundChannel = factory.create(outboundChannelDesc, writer, outboundChannelProps);
		//! Assign the new channel to its neuron group and add it to the mannager
		manager->addChannel(outboundChannel, neuronGroup); //Add channel to finspikesManager
	} else	
	if(elemName == "Reader") {
		//! Create the channel's reader
		ReaderFactory factory;	
		reader = factory.create(readerDesc, readerProps);
	} else
	if(elemName == "Writer") {
		//! Create the channel's reader
		WriterFactory factory;	
		writer = factory.create(writerDesc, writerProps);
	} else
	if(elemName == "Property") {
		props->at(prop.getName()) = prop;
	}

    return true;
}


/*! Called when the parser generates a warning. */
bool CnsXmlLoader::warning ( const QXmlParseException& ex){
    qWarning()<<ex.message();
    return true;
}


/*! Called when the parser generates an error. */
bool CnsXmlLoader::error (const QXmlParseException& parseEx){
    throw SpikeStreamXMLException(parseEx.message());
    return true;
}


/*! Called when the parser generates a fatal error. */
bool CnsXmlLoader::fatalError (const QXmlParseException& parseEx){
    throw SpikeStreamXMLException(parseEx.message());
    return true;
}


QString CnsXmlLoader::contextXPath() {
	QString path = ""; // StringStream ?
	foreach(QString elem, context) {
		int count = quantities[elem];
		path = path + "/" + elem;
		if(count>1)
			path = path + QString("[%1]").arg(count);
	}
	return path;
}


/*! Returns a default error string. */
QString CnsXmlLoader::errorString (){
	return QString("Default error string");
}


/*! Called when parser reaches the start of the document. */
bool CnsXmlLoader::startDocument(){
    
	context.clear();
	channel = NULL;
	reader = NULL;
	neuronGroup = NULL;

    return true;
}


/*! Called when parser reaches the start of an element. */
bool CnsXmlLoader::startElement(const QString&, const QString&, const QString& qName, const QXmlAttributes& qAttributes){

	if(qName == "CARLsimCNS") {
		;
	} else 
	if(qName == "InboundChannel") {
		QString name = qAttributes.value("name");
		//! The type of the Reader is not relevant for Xml
		inboundChannelDesc = Description(name.toStdString(), "", ""); 
	} else
	if(qName == "OutboundChannel") {
		QString name = qAttributes.value("name");
		//! The type of the Reader is not relevant for Xml
		outboundChannelDesc = Description(name.toStdString(), "", ""); 
	} else
	if(qName == "Reader") {
		QString name = qAttributes.value("name");
		//! Reader Type is required for Bar and BarInd due History parameter in Channel
		QString type = qAttributes.value("type"); 
		readerDesc = Description(name.toStdString(), "", type.toStdString()); 				
	} else
	if(qName == "Writer") {
		QString name = qAttributes.value("name");
		//! Reader Type is required for Bar and BarInd due History parameter in Channel
		QString type = qAttributes.value("type"); 
		writerDesc = Description(name.toStdString(), "", type.toStdString()); 				
	} else
	if(qName == "Properties") {
		props = NULL;
		if(context[context.size()-1] == "InboundChannel") {	
			inboundChannelProps = inboundChannelFactory.getDefaultProperties(inboundChannelDesc);
			props = &inboundChannelProps; 
		} else
		if(context[context.size()-1] == "OutboundChannel") {	
			outboundChannelProps = outboundChannelFactory.getDefaultProperties(outboundChannelDesc);
			props = &outboundChannelProps; 
		} else
		if(context[context.size()-1] == "Reader") {
			readerProps = readerFactory.getDefaultProperties(readerDesc);
			props = &readerProps;
		} else
		if(context[context.size()-1] == "Writer") {
			writerProps = writerFactory.getDefaultProperties(writerDesc);
			props = &writerProps;
		}
		Q_ASSERT(props != NULL);
	} else 
	if(qName == "Property") {
		try {
			QString type = qAttributes.value("type");
			QString name = qAttributes.value("name");
			Q_ASSERT(props != NULL);
			prop = props->at(name.toStdString());
			Q_ASSERT(
				(type == "int" &&  prop.getType() == Property::Integer)
				|| 
				(type == "double" && prop.getType() == Property::Double)
				|| 
				(type == "" && prop.getType() == Property::String)
				);
		} catch (exception& e) {
			throw SpikeStreamXMLException(QString("Exception: %1 in element Property at %2").arg(e.what()).arg(contextXPath()));
		}
	} else 
	if(qName == "NeuronGroup") {
		QString neuronGroupName = qAttributes.value("name");
		neuronGroup = NULL;
		QList<NeuronGroup*> neuronGroupList = Globals::getNetwork()->getNeuronGroups();
		foreach(NeuronGroup* tmpNeurGrp, neuronGroupList){
			if(tmpNeurGrp->getInfo().getName() == neuronGroupName) {
				neuronGroup = tmpNeurGrp;
				break; //! stop search after element was found
			}
		}
		if(neuronGroup == NULL)
			throw SpikeStreamXMLException(QString("Neurongroup %1 not in Network").arg(neuronGroupName));
	} else {

		throw SpikeStreamXMLException(QString("Unsupported element: %1 at %2").arg(qName).arg(contextXPath()));
	}
	
	context.push(qName);

	/*! Cardinality in XPath
		Note: Only elements are counted that are relative to their parent element.
	*/
	if(context.size()>1) {
		if(previous == context.at(context.size()-2))
			quantities[qName] = quantities[qName] + 1;
		else {
			quantities[qName] = 1;
			previous = context.at(context.size()-2); // parent
		}
	}

    return true;
}
