#ifndef CARLSIMOATCOBAMONITOR_H
#define CARLSIMOATCOBAMONITOR_H

// SpikeStream 0.3
#include "api.h"

#include "CarlsimOatMonitor.h"

namespace spikestream {

	namespace carlsim_monitors {  // not hierarchically  carlsim::monitors

		class OatCobaMonitor : public OatMonitor {
			Q_OBJECT
		public:
			
			OatCobaMonitor(bool active, QString object, QString path, int start, int end, int period, bool persistent);

			virtual void setMonitor(CarlsimWrapper *wrapper);

			const char* type() { return "COBA-Monitor"; };
			
			bool isRecording();
			void startRecording(unsigned snnTime);
			void stopRecording(unsigned snnTime);
			void startRecording();
			void stopRecording();
			void print();
			void clear();

			void printStats(QTextDocument *report, QTextTableFormat tableFormat);
			void printCOBA(QTextDocument *report, QTextTableFormat tableFormat);

			NeuronGroup* group;  // must be set explictly 
			CobaMonitor* monitor;
		};

	}
}

#endif//CARLSIMOATCOBAMONITOR_H

