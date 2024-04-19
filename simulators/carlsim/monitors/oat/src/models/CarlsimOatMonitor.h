#ifndef CARLSIMOATMONITOR_H
#define CARLSIMOATMONITOR_H

// SpikeStream 0.3
#include "api.h"


//Qt includes
#include <QObject>
#include <QVector>
#include <QTextDocument>
#include <QTextTableFormat>


#include "carlsim_datastructures.h"

class SpikeMonitor;
class ConnectionMonitor;
class NeuronMonitor;
class GroupMonitor;

namespace spikestream {

	class CarlsimWrapper;
	class NeuronGroup;
	class ConnectionGroup;

	namespace carlsim_monitors {  // not hierarchically  carlsim::monitors

	/*! Used for the display of information about a neuron group.
		Based on the NeuronGroup table in the SpikeStream database. */
		class OatMonitor : public QObject {
			Q_OBJECT
		public:

			OatMonitor();

			OatMonitor(bool active, QString object, QString path, int start, int end, int period, bool persistent);

			virtual void setMonitor(CarlsimWrapper *wrapper) = 0;  // called in CONIG state when loading the XML
			virtual void setup() {};  // called in SETUP state 
			virtual void unload(); // called when simulation is unloaded an all pointer needs to be reset

			virtual bool isRecording() = 0; // monitor && monitor->isRecording()
			virtual void startRecording(unsigned snnTime) = 0;
			virtual void stopRecording(unsigned snnTime) = 0;
			virtual void print() = 0;
			virtual void startRecording() { active = false; };	// manual override
			virtual void stopRecording()  { active = false; };  // manual override
			virtual void clear()		  { active = false; };  // manual override

			virtual const char* type() = 0;

		//protected

			QString path;
			bool active;
			QString object; // can be a neuron group or a connection group !
			int start;
			int end;
			int period;
			bool persistent;
		};

	}
}

#endif//CARLSIMOATMONITOR_H

