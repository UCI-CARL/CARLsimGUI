#ifndef TIMEPANELWIDGET_H
#define TIMEPANELWIDGET_H

#include <QWidget>
#include <QDateTime>

#include "api.h" 

class Ui_TimePanel;  

namespace spikestream {


	class  TimePanelWidget : public QWidget
	{
		Q_OBJECT
	public:
		explicit TimePanelWidget(QWidget *parent = 0);
		~TimePanelWidget();
    
	signals:		
			void stepMsChanged(unsigned);
			void speedChanged(double);
    
	public slots:
		void updateTime(		
			unsigned int timeStep, unsigned long long snnTimeMs, 
			QDateTime modelTime0, QDateTime modelTime, 
			QDateTime realTime0, QDateTime realTime, 
			unsigned long long realTimeUs );

		void emit_stepMsChanged(int ms) { emit stepMsChanged((unsigned) ms); } // SIGNAL cast
		void setStepMs(unsigned);
		void setSpeed(double);

	private:
		Ui_TimePanel* uiTimePanel;

	};

}

#endif // TIMEPANELWIDGET_H
