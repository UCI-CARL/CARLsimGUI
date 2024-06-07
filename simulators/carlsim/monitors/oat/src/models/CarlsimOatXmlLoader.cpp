
//spikestream.CARLsimCNS includes
#include "CarlsimOatXmlLoader.h"
#include "CarlsimOatModel.h"
#include "CarlsimWidget.h"
#include "CarlsimWrapper.h"
#include "SpikeStreamXMLException.h"
#include "Globals.h"
#include "NeuronGroup.h"

using namespace spikestream::carlsim_monitors;

//Qt includes
#include <QDebug>
#include <QFile>


/*! Constructor */
CarlsimOatXmlLoader::CarlsimOatXmlLoader(CarlsimWrapper* _wrapper, CarlsimOatModel* _model): 
	wrapper(_wrapper), model(_model) {
}


/*! Destructor */
CarlsimOatXmlLoader::~CarlsimOatXmlLoader(){
}


/*----------------------------------------------------------*/
/*-----                  PUBLIC METHODS                -----*/
/*----------------------------------------------------------*/

/*! Parses the XML from file and add corresponding channels the FinSpikesManager */
bool CarlsimOatXmlLoader::loadFromPath(const QString& path){

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
bool CarlsimOatXmlLoader::characters(const QString& chars){

	if(context.top() == "Property") {
		prop.value = chars;	
	}
		
    return true; 
}


/*! Called when the parser encounters the end of an element. */
bool CarlsimOatXmlLoader::endElement( const QString&, const QString&, const QString& elemName){

  qDebug() << "endElement:" << elemName; 

	const QString &top = context.pop(); // Linux Pi5
	Q_ASSERT(top == elemName);

	if (elemName == "Monitors") {
		//! Create the channel's reader
		qDebug() << "Parsing Injectors";
	}
	else
	if(elemName == "Monitor") {
		//auto groupId = neuronGroup->getID();
		switch(type) {
			case SPIKE:	
				model->appendSpikeMonitor(neuronGroup, start, end, period, active, persistent, path, mode);
				break;
			case CONNECTION:
				model->appendConnectionMonitor(connectionGroup, start, end, period, active, persistent, path);
				break;
			case GROUP:
				model->appendGroupMonitor(neuronGroup, start, end, period, active, persistent, path);
				break;
			case NEURON:
				model->appendNeuronMonitor(neuronGroup, start, end, period, active, persistent, path);
				break;
			default: 
				throw SpikeStreamXMLException(QString("Undefined Element"));
		}
	} else
	if(elemName == "Property") {
		bool ok;
		if (prop.name == "Start") {
			Q_ASSERT(prop.type == "int");
			start = prop.value.toInt(&ok);
			if (!ok) throw SpikeStreamXMLException(QString("Conversion to int failed: %1 at %2").arg(prop.value).arg(contextXPath()));
		} else
		if (prop.name == "End") {
			Q_ASSERT(prop.type == "int");
			end = prop.value.toInt(&ok);
			if (!ok) throw SpikeStreamXMLException(QString("Conversion to int failed: %1 at %2").arg(prop.value).arg(contextXPath()));
		} else
		if (prop.name == "Period") {
			Q_ASSERT(prop.type == "int");
			period = prop.value.toInt(&ok);
			if (!ok) throw SpikeStreamXMLException(QString("Conversion to int failed: %1 at %2").arg(prop.value).arg(contextXPath()));
		}
		if (prop.name == "Active") {
			Q_ASSERT(prop.type == "bool");
			//active = prop.value.toLower() == QString("true");
			active = prop.value.toLower() == "true";
		} else
		if (prop.name == "Persistent") {
			Q_ASSERT(prop.type == "bool");
			//persistent = prop.value.toLower() == QString("true");  // QT_NO_CAST_FROM_ASCII  is not set
			persistent = prop.value.toLower() == "true";   
printf("persistent=%d\n", persistent);
		} else
		if (prop.name == "Path") {
			Q_ASSERT(prop.type == "string");
			path = prop.value;
		} else 
		if (prop.name == "Mode") {
			Q_ASSERT(prop.type == "string");
			if(prop.value == "AER") 
				mode = AER;
			else
			if (prop.value == "COUNT") 
				mode = COUNT; 
			else
				mode = -1; // UNDEF
		}
	}

  return true;
}


/*! Called when the parser generates a warning. */
bool CarlsimOatXmlLoader::warning ( const QXmlParseException& ex){
    qWarning()<<ex.message();
    return true;
}


/*! Called when the parser generates an error. */
bool CarlsimOatXmlLoader::error (const QXmlParseException& parseEx){
    throw SpikeStreamXMLException(parseEx.message());
    return true;
}


/*! Called when the parser generates a fatal error. */
bool CarlsimOatXmlLoader::fatalError (const QXmlParseException& parseEx){
    throw SpikeStreamXMLException(parseEx.message());
    return true;
}


QString CarlsimOatXmlLoader::contextXPath() {
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
QString CarlsimOatXmlLoader::errorString (){
	return QString("Default error string");
}


/*! Called when parser reaches the start of the document. */
bool CarlsimOatXmlLoader::startDocument(){
    
	context.clear();

	neuronGroup = NULL;

    return true;
}


/*! Called when parser reaches the start of an element. */
bool CarlsimOatXmlLoader::startElement(const QString&, const QString&, const QString& qName, const QXmlAttributes& qAttributes){
	qDebug() << "start Element " << qName;

	if(qName == "CarlsimOat") {
		;
	} else 
	if(qName == "Monitors") {
		;
	} else 
	if(qName == "Monitor") {

		// define defaults for all or specific Monitors
		path = "NULL"; 
		active = true; 
		persistent = false;
		start = end = period = -1;
		mode = -1;

		QString type = qAttributes.value("type");
		qDebug() << "type=" << type;
		if (type == "spike") 
			this->type = SPIKE;
		else
		if (type == "group") 
			this->type = GROUP;
		else
		if (type == "connection") 
			this->type = CONNECTION;
		else
		if (type == "neuron")
			this->type = NEURON;
		else
			this->type = UNDEF;
	} else
	if(qName == "Properties") {
		//props.clear();
	} else 
	if(qName == "Property") {
		try {
			// => struct
			prop.name = qAttributes.value("name");
			prop.type= qAttributes.value("type"); // always double 	
			//props[prop.name] = prop;
		} catch (exception& e) {
			throw SpikeStreamXMLException(QString("Exception: %1 in element Property at %2").arg(e.what()).arg(contextXPath()));
		}
	} else 
	if(qName == "Link") { 

		QString object = qAttributes.value("object");
		QString name = qAttributes.value("name");

		if (object == "NeuronGroup") {
			neuronGroup = NULL;
			QList<NeuronGroup*> neuronGroupList = Globals::getNetwork()->getNeuronGroups();
			foreach(NeuronGroup* tmpNeurGrp, neuronGroupList) {
				if (tmpNeurGrp->getInfo().getName() == name) {
					neuronGroup = tmpNeurGrp;
					break; //! stop search after element was found
				}
			}
			if (neuronGroup == NULL)
				throw SpikeStreamXMLException(QString("Neurongroup %1 not in Network").arg(name));
		} else
		if (object == "ConnectionGroup") {
			connectionGroup = NULL;
			QList<ConnectionGroup*> connectionGroupList = Globals::getNetwork()->getConnectionGroups();
			foreach(ConnectionGroup* tmpNeurGrp, connectionGroupList) {
				if (tmpNeurGrp->getInfo().getDescription() == name) {
					connectionGroup = tmpNeurGrp;
					break; //! stop search after element was found
				}
			}
			if (connectionGroup == NULL)
				throw SpikeStreamXMLException(QString("Connectiongroup %1 not in Network").arg(name));
		} 
		else {
			throw SpikeStreamXMLException(QString("Object is unsupported: %1").arg(object));
		}


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
