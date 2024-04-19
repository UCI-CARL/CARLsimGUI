#ifndef NOISEINJECTORXMLLOADER_H
#define NOISEINJECTORXMLLOADER_H

//Qt includes
#include <qxml.h>
#include <QHash>
#include <QString>
#include <QStack>


namespace spikestream {

	// Forwards
	class CarlsimWrapper;
	class NeuronGroup;

	namespace carlsim_injectors { 

		class NoiseInjectorModel; 

		/*!  */
		class NoiseInjectorXmlLoader : public QXmlDefaultHandler {
			public:
				NoiseInjectorXmlLoader(CarlsimWrapper* _wrapper, NoiseInjectorModel* _model);
				~NoiseInjectorXmlLoader();
			

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
				NoiseInjectorModel* model;
				
				NeuronGroup *neuronGroup;
				enum { CURRENT, FIRE } type;
				//QString type; // Current / Fire 
				double percentage;
				double current;
				bool active;
			
				struct  {
					QString name; 
					QString type; 
					QString value; 
				} prop;

				QStack<QString> context;

				// Does only work on one level
				QMap<QString, int> quantities;  //! used to create an Xpath
				QString previous;

				// Fix: 
				QStack<int> children;

		};
	}
}

#endif//NOISEINJECTORXMLLOADER_H

