#ifndef CARLSIMOATXMLLOADER_H
#define CARLSIMOATXMLLOADER_H

//Qt includes
#include <qxml.h>
#include <QHash>
#include <QString>
#include <QStack>


namespace spikestream {

	// Forwards
	class CarlsimWrapper;
	class NeuronGroup;
	class ConnectionGroup;

	namespace carlsim_monitors { 

		class CarlsimOatModel; 

		/*!  */
		class CarlsimOatXmlLoader : public QXmlDefaultHandler {
			public:
				CarlsimOatXmlLoader(CarlsimWrapper* _wrapper, CarlsimOatModel* _model);
				~CarlsimOatXmlLoader();
			

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
				CarlsimWrapper* wrapper;
				CarlsimOatModel* model;
				
				enum {
					SPIKE,
					CONNECTION,
					GROUP,
					NEURON,
					UNDEF
				} type;

				NeuronGroup *neuronGroup;
				ConnectionGroup *connectionGroup;
				
				QString path;
				bool active, persistent;
				int start, end, period;
				int mode;

				struct  {
					QString name; 
					QString type; 
					QString value; 
				} prop;

				
				QStack<QString> context;

				QMap<QString, int> quantities;  //! used to create an Xpath
				QString previous;

				// Fix: 
				QStack<int> children;

		};
	}
}

#endif//CARLSIMOATXMLLOADER_H

