#ifndef CARLSIMOATCONNECTIONMONITOR_H
#define CARLSIMOATCONNECTIONMONITOR_H

// SpikeStream 0.3
#include "api.h"

#include "CarlsimOatMonitor.h"


namespace spikestream {

	namespace carlsim_monitors {  // not hierarchically  carlsim::monitors

		class OatConnectionMonitor : public OatMonitor {
			Q_OBJECT
		public:
			OatConnectionMonitor(bool active, QString object, QString path, int start, int end, int period, bool persistent);

			const char* type() { return "Connection-M."; };

			virtual void setMonitor(CarlsimWrapper *wrapper);
			virtual void setup(); 

			virtual bool isRecording(); 
			virtual void startRecording(unsigned snnTime);
			virtual void stopRecording(unsigned snnTime);
			virtual void startRecording(); // take snapshot
			virtual void print();
			virtual void clear();

			void printStats(QTextDocument *report, QTextTableFormat tableFormat);
			void printQueries(QTextDocument *report, QTextTableFormat tableFormat,
				int preId, int postId, float minAbsChanged, float minWeight, float maxWeight, float weight);

			void printWeights(QTextDocument *report, QTextTableFormat tableFormat);
			void printPrevWeights(QTextDocument *report, QTextTableFormat tableFormat);
			void printWeightChanges(QTextDocument *report, QTextTableFormat tableFormat);


			ConnectionGroup* group;
			ConnectionMonitor* monitor;
			CarlsimWrapper* wrapper;
		};
	}
}

#endif//CARLSIMOATCONNECTIONMONITOR_H

