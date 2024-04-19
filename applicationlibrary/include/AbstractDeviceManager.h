#ifndef ABSTRACTDEVICEMANAGER_H
#define ABSTRACTDEVICEMANAGER_H

// SpikeStream 0.3
#include "api.h"

//SpikeStream includes
#include "SpikeStreamTypes.h"

//Qt includes
#include <QList>

//#define NC_EXTENSIONS

namespace spikestream {

	/*! Defines a generic interface used by all device managers */
	class SPIKESTREAM_APP_LIB_EXPORT AbstractDeviceManager{

		public:
			AbstractDeviceManager();
			virtual ~AbstractDeviceManager();

			/*! Returns iterator pointing to start of list of firing neurons output from the device */
			virtual QList<neurid_t>::iterator outputNeuronsBegin() = 0;

			/*! Returns iterator pointing to end of list of firing neurons output from the device */
			virtual QList<neurid_t>::iterator outputNeuronsEnd() = 0;



			/*! Returns iterator pointing to start of list of firing neurons output from the device */
			virtual QList<neurid_t>::iterator currentNeuronsBegin() = 0;

			/*! Returns iterator pointing to end of list of firing neurons output from the device */
			virtual QList<neurid_t>::iterator currentNeuronsEnd() = 0;

			/*! Returns iterator pointing to start of list of firing neurons output from the device */
			virtual QList<double>::iterator currentValuesBegin() = 0;

			/*! Returns iterator pointing to end of list of firing neurons output from the device */
			virtual QList<double>::iterator currentValuesEnd() = 0;








			/*! Sets firing neuron ids entering the device. */
			virtual void setInputNeurons(timestep_t timeStep, QList<neurid_t>& firingNeuronIDs) = 0;

			/*! Steps the device forward one time step */
			virtual void step() = 0;

#ifdef NC_EXTENSIONS
			virtual void setSimtime(unsigned long long ) { } // model simtime in ms
			virtual bool allChannelBufferEmpty() { return true; } // default is true
			virtual void reset() {};
			virtual void update() {};
			virtual double getDopaminLevel() { return 0.0; }
#endif




			/*! Returns the amount of current that device injects when fire Neuron mode is off */
			double getCurrent() { return current; }

			/*! Returns whether neuron events from device fire neurons or inject current */
			bool isFireNeuronMode() { return fireNeuronMode; }

			/*! Sets the amount of current that device injects when fire neuron mode is off. */
			void setCurrent(double current) { this->current = current; }

			/*! Sets whether neuron events from device fire neurons or inject current */
			void setFireNeuronMode(bool fireNeuronMode ) { this->fireNeuronMode = fireNeuronMode; }

		private:
			/*! Controls whether device injects current or fires a neuron. */
			bool fireNeuronMode;

			/*! Amount of current to inject when fireNeuron=false */
			double current;

	};

}

#endif//ABSTRACTDEVICEMANAGER_H
