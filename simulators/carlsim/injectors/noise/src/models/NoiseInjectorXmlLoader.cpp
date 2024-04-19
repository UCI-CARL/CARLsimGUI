
//spikestream.CARLsimCNS includes
#include "NoiseInjectorXmlLoader.h"
#include "NoiseInjectorModel.h"
#include "CarlsimWidget.h"
#include "CarlsimWrapper.h"
#include "SpikeStreamXMLException.h"
#include "Globals.h"
#include "NeuronGroup.h"

using namespace spikestream::carlsim_injectors;

//Qt includes
#include <QDebug>
#include <QFile>


/*! Constructor */
NoiseInjectorXmlLoader::NoiseInjectorXmlLoader(CarlsimWrapper* _wrapper, NoiseInjectorModel* _model): 
	wrapper(_wrapper), model(_model),
	active(true)  // compatibilty
{
}


/*! Destructor */
NoiseInjectorXmlLoader::~NoiseInjectorXmlLoader(){
}


/*----------------------------------------------------------*/
/*-----                  PUBLIC METHODS                -----*/
/*----------------------------------------------------------*/

/*! Parses the XML from file and add corresponding channels the FinSpikesManager */
bool NoiseInjectorXmlLoader::loadFromPath(const QString& path){

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
bool NoiseInjectorXmlLoader::characters(const QString& chars){

	if(context.top() == "Property") {
		prop.value = chars;	
	}
		
    return true; 
}


/*! Called when the parser encounters the end of an element. */
bool NoiseInjectorXmlLoader::endElement( const QString&, const QString&, const QString& elemName){

  qDebug() << "endElement:" << elemName; 

	QString &top = context.pop();
	Q_ASSERT(top == elemName);

	if (elemName == "Injectors") {
		//! Create the channel's reader
		qDebug() << "Parsing Injectors";
	}
	else
	if(elemName == "Injector") {
		auto groupId = neuronGroup->getID();
		switch(type) {
			case CURRENT:	
				model->appendInjector(groupId, NoiseInjectorModel::CURRENT, percentage, current, active, NULL);	
				break;
			case FIRE:
				model->appendInjector(groupId, NoiseInjectorModel::FIRE, percentage, .0f, active, wrapper);
				break;
		}
	} else
	if(elemName == "Property") {
		bool ok;
		if (prop.name == "Current") {
			Q_ASSERT(prop.type == "double"); 
			current = prop.value.toDouble(&ok);
			if (!ok) throw SpikeStreamXMLException(QString("Conversion to int failed: %1 at %2").arg(prop.value).arg(contextXPath()));
		} else if (prop.name == "Active") {  
			Q_ASSERT(prop.type == "bool");
			active = prop.value.toLower() == "true";  
		} else if (prop.name == "Percentage") {
			Q_ASSERT(prop.type == "double");
			percentage = prop.value.toDouble(&ok);
			if (!ok) throw SpikeStreamXMLException(QString("Conversion to double failed: %1 at %2").arg(prop.value).arg(contextXPath()));
		} else
			throw SpikeStreamXMLException(QString("Invalid Property: %1 at %2").arg(prop.name).arg(contextXPath()));
 	}

  return true;
}


/*! Called when the parser generates a warning. */
bool NoiseInjectorXmlLoader::warning ( const QXmlParseException& ex){
    qWarning()<<ex.message();
    return true;
}


/*! Called when the parser generates an error. */
bool NoiseInjectorXmlLoader::error (const QXmlParseException& parseEx){
    throw SpikeStreamXMLException(parseEx.message());
    return true;
}


/*! Called when the parser generates a fatal error. */
bool NoiseInjectorXmlLoader::fatalError (const QXmlParseException& parseEx){
    throw SpikeStreamXMLException(parseEx.message());
    return true;
}


QString NoiseInjectorXmlLoader::contextXPath() {
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
QString NoiseInjectorXmlLoader::errorString (){
	return QString("Default error string");
}


/*! Called when parser reaches the start of the document. */
bool NoiseInjectorXmlLoader::startDocument(){
    
	context.clear();

	neuronGroup = NULL;

    return true;
}


/*! Called when parser reaches the start of an element. */
bool NoiseInjectorXmlLoader::startElement(const QString&, const QString&, const QString& qName, const QXmlAttributes& qAttributes){
	qDebug() << "start Element " << qName;

	if(qName == "CarlsimWidget") {
		;
	} else 
	if(qName == "Injectors") {
		;
	} else 
	if(qName == "Injector") {
		QString type = qAttributes.value("type");
		qDebug() << "type=" << type;
		if (type == "Current") 
			this->type = CURRENT;
		else
		if (type == "Fire") 
			this->type = FIRE;
	} else
	if(qName == "Properties") {
		; // props.clear();
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
