#ifndef PERFORMANCETIMER_H
#define PERFORMANCETIMER_H

//Qt includes
#include <QDateTime>

// Spikestream 0.3
#include "api.h" 

namespace spikestream {

	/*! Timer to record the time taken for methods or processes. */
	class SPIKESTREAM_LIB_EXPORT PerformanceTimer {

		public:
			PerformanceTimer();
			~PerformanceTimer();
			void printTime(const QString& taskName);
			void start();


		private:
			//============================== VARIABLES =============================
			/*! The date time at which the timer starts */
			QDateTime timerStart;


			//=============================== METHODS ==============================
			/*! Declare copy constructor private so it cannot be used inadvertently.*/
			PerformanceTimer (const PerformanceTimer&);

			/*! Declare assignment private so it cannot be used.*/
			PerformanceTimer operator = (const PerformanceTimer&);

	};

}

#endif//PERFORMANCETIMER_H

