#ifndef CARLSIMOATGROUPMONITOR_H
#define CARLSIMOATGROUPMONITOR_H

// SpikeStream 0.3
#include "api.h"

#include "CarlsimOatMonitor.h"

namespace spikestream {

	namespace carlsim_monitors {  // not hierarchically  carlsim::monitors

		class OatGroupMonitor : public OatMonitor {
			Q_OBJECT
		public:
			OatGroupMonitor( bool active, QString object, QString path, int start, int end, int period, bool persistent);

			virtual void setMonitor(CarlsimWrapper *wrapper);
			const char* type() { return "Group-Monitor"; };

			virtual bool isRecording();
			void startRecording(unsigned snnTime);
			void stopRecording(unsigned snnTime);
			void startRecording();
			void stopRecording();
			void print();
			void clear();

			void printStats(QTextDocument *report, QTextTableFormat tableFormat);
			void printData(QTextDocument *report, QTextTableFormat tableFormat);
			void printPeaks(QTextDocument *report, QTextTableFormat tableFormat, int maxPeaks);
			void printPeaksSorted(QTextDocument *report, QTextTableFormat tableFormat, int maxPeaks);


			NeuronGroup* group;  // must be set explictly 
			GroupMonitor* monitor;
		};

	}
}

#endif//CARLSIMOATGROUPMONITOR_H

