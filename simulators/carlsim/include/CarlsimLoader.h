#ifndef CARLSIMLOADER_H
#define CARLSIMLOADER_H


//Other includes
#include "boost/random.hpp"
 
/*! The random number generator type */
typedef boost::mt19937 rng_t;

/*! The variate generator type, which includes the random number generator type. */
typedef boost::variate_generator<rng_t&, boost::uniform_real<double> > urng_t;


//SpikeStream includes
#include "Network.h"
using namespace spikestream;

#include "carlsim_types.h"

//Qt includes
#include <QObject> 

// 
#include "api.h" 
 
namespace spikestream {

	class CarlsimWrapper;

	/*! Loads the network into the graphics hardware ready to run with Carlsim */
	class CARLSIMWRAPPER_LIB_EXPORT CarlsimLoader : public QObject {

		Q_OBJECT

		public:
			CarlsimLoader(CarlsimWrapper* wrapper) ;
			~CarlsimLoader();
			bool buildCarlsimNetwork(Network* network, QHash<unsigned, synapse_id*>& volatileConGrpMap, const bool* stop);

		signals:
			void progress(int stepsCompleted, int totalSteps);

		private:
			//======================  VARIABLES  =======================
			/*! File where log is written if required. */
			QFile* logFile;

			/*! Text stream connected to log file */
			QTextStream* logTextStream;

			CarlsimWrapper* wrapper; // reference, container for the network, config, etc.

			//======================  METHODS  =======================
			void addExcitatoryNeuronGroup(NeuronGroup* neuronGroup /*, urng_t& ranNumGen*/);
			void addInhibitoryNeuronGroup(NeuronGroup* neuronGroup /*, urng_t& ranNumGen*/);

			/*! featInputNeurons */
			void addDaErgicNeuronGroup(NeuronGroup* neuronGroup /*, urng_t& ranNumGen*/);
			void addCustomExcitatoryNeuronGroup(NeuronGroup* neuronGroup /*, urng_t& ranNumGen*/);
			void addCustomInhibitoryNeuronGroup(NeuronGroup* neuronGroup /*, urng_t& ranNumGen*/);
			void addPoissonExcitatoryNeuronGroup(NeuronGroup* neuronGroup /*, urng_t& ranNumGen*/);
			void addPoissonInhibitoryNeuronGroup(NeuronGroup* neuronGroup /*, urng_t& ranNumGen*/);

			void addConnectionGroup(ConnectionGroup* conGroup, QHash<unsigned, synapse_id*>& volatileConGrpMap);
			void printConnection(unsigned source,unsigned targets[], unsigned delays[], float weights[], unsigned char is_plastic[], size_t length);
	};
}


#endif//CARLSIMLOADER_H
