#ifndef CARLSIMOATSPIKEMONITOR_H
#define CARLSIMOATSPIKEMONITOR_H

// SpikeStream 0.3
#include "api.h"

#include "CarlsimOatMonitor.h"

namespace spikestream {


	namespace carlsim_monitors {  // not hierarchically  carlsim::monitors

		class OatSpikeMonitor : public OatMonitor {
			Q_OBJECT
		public:
			OatSpikeMonitor(bool active, QString object, QString path, int start, int end, int period, bool persistent, SpikeMonMode mode);

			virtual void setMonitor(CarlsimWrapper *wrapper);
			virtual void unload();

			const char* type() { return "Spike-Monitor"; };
			// mode // There are two different modes that define what information is collected exactly, AER and COUNT mode.

			bool isRecording();
			void startRecording(unsigned snnTime);
			void stopRecording(unsigned snnTime);
			void startRecording();
			void stopRecording();
			void print();
			void clear();

			void printStats(QTextDocument *report, QTextTableFormat tableFormat);
			void printNeurons(QTextDocument *report, QTextTableFormat tableFormat, float minFiringRate, float maxFiringRate);
			void printNeuron(QTextDocument *report, QTextTableFormat tableFormat, int neuronId);
			void printSpikeTimes(QTextDocument *report, QTextTableFormat tableFormat);

			SpikeMonMode mode;  // SpikeMonMode 
			NeuronGroup* group;  // must be set explictly 
			SpikeMonitor* monitor;
		};

	}
}

#endif//CARLSIMOATSPIKEMONITOR_H

