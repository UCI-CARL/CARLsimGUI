#ifndef CONNECTION_H
#define CONNECTION_H

//SpikeStream includes
#include "SpikeStreamException.h"

// Spikestream 0.3
#include "api.h" 

//Other includes
#include <math.h>


//Defines for compressing the weight
#define DELAY_FACTOR 10
#define DELAY_MAX 6553.5f
#define DELAY_MIN 0.0f

// Patch for Izhikevich Experiment
#define WEIGHT_FACTOR 1000.0f // reduced points  
#define WEIGHT_MAX 4.0f
#define WEIGHT_MIN -1.0f


namespace spikestream {

    /*! Holds information about each connection. Should match the information stored
		in the Connections table of the SpikeStreamNetwork database. */
    class SPIKESTREAM_LIB_EXPORT Connection{
		public:
			Connection();
			Connection (unsigned fromNeuronID, unsigned toNeuronID, float delay, float weight);
			Connection (unsigned id, unsigned fromNeuronID, unsigned toNeuronID, float delay, float weight);
			Connection(const Connection& conn);
			Connection& operator=(const Connection& rhs);
			void print();

			unsigned getID() { return id; }
			float getDelay();
			float getTempDelay();
			unsigned getFromNeuronID(){ return fromNeuronID; }
			unsigned getToNeuronID(){ return toNeuronID; }
			float getTempWeight();
			float getWeight();
			void setID(unsigned id) { this->id = id; }
			void setFromNeuronID(unsigned fromNeurID) { this->fromNeuronID = fromNeurID; }
			void setToNeuronID(unsigned toNeurID) { this->toNeuronID = toNeurID; }
			void setTempWeight(float newTempWeight);
			void setWeight(float newWeight);
			void setTempDelay(float newTempDelay);


		private:
			//=======================  VARIABLES  ========================
			/*! ID of the connection from the Connections database. */
			unsigned id;

			/*! Connection is from neuron with this ID. */
			unsigned int fromNeuronID;

			/*! Connection is to neuron with this ID. */
			unsigned int toNeuronID;

			/*! Compressed fixed point representation of the delay of a connection.
				Delay ranges from 0-6,553.5 ms with a resolution of 0.1 */
			unsigned short delay;

			/*! Compressed fixed point representation of the delay of a connection.
			Delay ranges from 0-6,553.5 ms with a resolution of 0.1 */
			unsigned short tempDelay;

			/*! Compressed fixed point representation of the weight of connection.
				Weight ranges from -1.0 to 1.0 with a resolution of 0.0001. */
			short weight;

			/*! Compressed fixed point representation of the temporary weight of a connection.
				Weight ranges from -1.0 to 1.0 with a resolution of 0.0001. */
			short tempWeight;

    };



	/*--------------------------------------------------------*/
	/*-------              INLINE METHODS              -------*/
	/*--------------------------------------------------------*/

	/*! Returns the delay */
	inline float Connection::getDelay(){
		return (float)delay / DELAY_FACTOR;
	}


	/*! Returns the delay */
	inline float Connection::getTempDelay() {
		return (float)tempDelay / DELAY_FACTOR;
	}

	/*! Returns the weight */
	inline float Connection::getWeight(){
		return (float)weight / WEIGHT_FACTOR;
	}


	/*! Returns the temporary weight */
	inline float Connection::getTempWeight(){
		return (float)tempWeight / WEIGHT_FACTOR;
	}


	/*! Sets the temporary weight */
	inline void Connection::setTempWeight(float newTempWeight){
		if(newTempWeight > WEIGHT_MAX || newTempWeight < WEIGHT_MIN)
			throw SpikeStreamException("Weight out of range: " + QString::number(newTempWeight));
		this->tempWeight = (short) rint(newTempWeight * WEIGHT_FACTOR);
	}


	/*! Sets the weight */
	inline void Connection::setWeight(float newWeight){
		if(newWeight > WEIGHT_MAX || newWeight < WEIGHT_MIN)
			throw SpikeStreamException("Weight out of range: " + QString::number(newWeight));
		this->weight = (short) rint(newWeight * WEIGHT_FACTOR);
	}


	/*! Sets the temporary delay for axonal plasticity */
	inline void Connection::setTempDelay(float newTempDelay) {
		if (newTempDelay > DELAY_MAX || newTempDelay < DELAY_MIN)
			throw SpikeStreamException("Delayout of range: " + QString::number(newTempDelay));
		this->tempDelay = (short)rint(newTempDelay * DELAY_FACTOR);
	}


}

#endif//CONNECTION_H

