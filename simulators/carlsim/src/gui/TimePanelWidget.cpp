#include "TimePanelWidget.h"

using namespace spikestream;

#include "ui_TimePanel.h"


TimePanelWidget::TimePanelWidget(QWidget *parent) :
	uiTimePanel(new Ui_TimePanel()), QWidget(parent)
{
	uiTimePanel->setupUi(this); 

	uiTimePanel->widget->setHidden(true); // hide the Buttons per Default
	
	setObjectName("myWidget");	

	setStyleSheet("#myWidget {border: 2px groove lightgray; border-bottom-left-radius:4px; border-bottom-right-radius:4px;}");

	connect(uiTimePanel->stepMsEdit, SIGNAL(valueChanged(int)), this, SLOT(emit_stepMsChanged(int)));
	connect(uiTimePanel->speedEdit, SIGNAL(valueChanged(double)), this, SIGNAL(speedChanged(double)));

}

	

TimePanelWidget::~TimePanelWidget() 
{
}


#include <stdio.h>

QString formatTimeDiff(unsigned long long timeDiffMs)
{
	int ms = timeDiffMs%1000;
	int s = timeDiffMs/1000%60;
	int m = timeDiffMs/60000%60;
	int h = timeDiffMs/3600000%60;
	int dayOfWeek = timeDiffMs/(24*3600000)%7;
	int weeks = timeDiffMs/(7*24*3600000);

	//! http://msdn.microsoft.com/en-us/library/56e442dc(v=vs.100).aspx
	const size_t BufSize = 50;  
	static char buf[BufSize];
	sprintf_s<BufSize>(buf, "%02d.%01d  %02d:%02d:%02d.%03d", weeks, dayOfWeek, h, m, s, ms);

	return QString::fromLatin1(buf);
}

void TimePanelWidget::setStepMs(unsigned step) 
{
	if(step == uiTimePanel->stepMsEdit->value())
		return; // avoid recursion
	uiTimePanel->stepMsEdit->setValue(step);
	emit stepMsChanged(step);
}

void TimePanelWidget::setSpeed(double speed) 
{
	if(abs(speed - uiTimePanel->speedEdit->value()) < 0.001)
		return; // avoid recursion
	uiTimePanel->speedEdit->setValue(speed);
	emit speedChanged(speed);
}


//#define DEBUG_PERFORMANCE1

void TimePanelWidget::updateTime(
		unsigned int /*timeStep*/, unsigned long long snnTimeMs, 
		QDateTime modelTime0, QDateTime modelTime, 
		QDateTime realTime0, QDateTime realTime, 
		unsigned long long realTimeUs ){

	QLocale locale;
	uiTimePanel->stepCounter->setText(locale.toString(snnTimeMs));
	
#ifdef DEBUG_PERFORMANCE1
	if (snnTimeMs >= 0) {
		return;
	}
#endif

	uiTimePanel->snnTimeDiff->setText(formatTimeDiff(snnTimeMs));

	uiTimePanel->modelTimeEdit->setDateTime(modelTime);

	unsigned long long modelTimeDiffMs = modelTime.toMSecsSinceEpoch() - modelTime0.toMSecsSinceEpoch();


	uiTimePanel->modelTimeDiff->setText(formatTimeDiff(modelTimeDiffMs));

	uiTimePanel->realTimeEdit->setDateTime(realTime);
	unsigned long long realTimeDiffMs = realTime.toMSecsSinceEpoch() - realTime0.toMSecsSinceEpoch();
	
	uiTimePanel->realTimeDiff->setText(formatTimeDiff(modelTimeDiffMs));

	
	unsigned long long realTimeMs = realTimeUs/1000; 

	double snnSpeed=.0, modelSpeed=.0, realSpeed=.0;
	if(realTimeDiffMs>0) {
		snnSpeed = ((double) snnTimeMs) / realTimeDiffMs;
		modelSpeed = ((double) modelTimeDiffMs) / realTimeDiffMs;
		realSpeed = ((double) realTimeMs) / realTimeDiffMs;
	}
	uiTimePanel->snnSpeed->setText(QString("x %1").arg( snnSpeed, 0, 'f', 3));
	uiTimePanel->modelSpeed->setText(QString("x %1").arg( modelSpeed, 0, 'f', 3));
	uiTimePanel->realSpeed->setText(QString("x %1").arg( realSpeed, 0, 'f', 3));


}

