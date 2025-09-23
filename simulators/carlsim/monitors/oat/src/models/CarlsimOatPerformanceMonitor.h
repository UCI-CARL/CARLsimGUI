#ifndef CARLSIMOATPERFMONITOR_H
#define CARLSIMOATPERFMONITOR_H

// SpikeStream 0.3
#include "api.h"

#include "CarlsimOatMonitor.h"

namespace spikestream {

	namespace carlsim_monitors {  // not hierarchically  carlsim::monitors

		class OatPerformanceMonitor : public OatMonitor {
			Q_OBJECT
		public:
			
			OatPerformanceMonitor(bool active, QString object, QString path, int start, int end, int period, bool persistent);

			virtual void setMonitor(CarlsimWrapper *wrapper);

			const char* type() { return "Performance-Monitor"; };
			
			bool isRecording();
			void startRecording(unsigned snnTime);
			void stopRecording(unsigned snnTime);
			void startRecording();
			void stopRecording();
			void print();
			void clear();

			void printStats(QTextDocument *report, QTextTableFormat tableFormat);
			void printPerformance(QTextDocument *report, QTextTableFormat tableFormat);

			PerformanceMonitor* monitor;
			CarlsimWrapper* wrapper;
		};

	}
}

#endif//CARLSIMOATPERFMONITOR_H

