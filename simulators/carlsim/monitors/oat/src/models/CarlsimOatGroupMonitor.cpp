//SpikeStream includes
#include "Globals.h"
#include "CarlsimOatGroupMonitor.h"
#include "SpikeStreamException.h"
#include "CarlsimWrapper.h"
#include "Util.h"

#include "connection_monitor.h"
#include "group_monitor.h"
#include "neuron_monitor.h"
#include "spike_monitor.h"

using namespace spikestream::carlsim_monitors;

//Qt includes
#include <QDebug>
#include <QIcon>
#include <QTextCursor>
#include <QTextTable>


OatGroupMonitor::OatGroupMonitor(bool active, QString object, QString path, int start, int end, int period, bool persistent) :
 OatMonitor(active, object, path, start, end, period, persistent) {
	monitor = NULL;
	group = NULL;
}

void OatGroupMonitor::setMonitor(CarlsimWrapper *wrapper) {
	monitor = wrapper->carlsim->setGroupMonitor(group->getVID(), path.toStdString());
	if (monitor->isRecording())
		monitor->stopRecording();
	monitor->setPersistentData(persistent);
}

bool OatGroupMonitor::isRecording() {
	return monitor && monitor->isRecording();
}

void OatGroupMonitor::startRecording(unsigned snnTime) {
	if (!monitor || !active)
		return;
	if (monitor->isRecording()) {
		if (period > 0 && snnTime % period == 0 && (end == -1 || int(snnTime) < end)) {
			monitor->stopRecording();
			monitor->startRecording();
		}
	}
	else
		if ((start > -1 && int(snnTime) >= start) && (end == -1 || int(snnTime) < end))  
			monitor->startRecording();
}

void OatGroupMonitor::stopRecording(unsigned snnTime) {
	if (monitor && active && monitor->isRecording() 
		&& (snnTime >= end - 1 || (period > 0 && snnTime % period == 0)))
		monitor->stopRecording();
}

void OatGroupMonitor::print() {
	if (!monitor || !active || monitor->isRecording() || monitor->getRecordingTotalTime() < 1)
		return;

	auto times = monitor->getTimeVector(); 
	auto data = monitor->getDataVector(); 

	for (int i = 0; i < times.size(); i++) {
		printf("da@%d: %f\n", times[i], data[i]);
	}

}

void OatGroupMonitor::clear() {
	//monitor->clear();
}

void OatGroupMonitor::startRecording() {
	monitor->startRecording();
	OatMonitor::startRecording();
}

void OatGroupMonitor::stopRecording() {
	if (monitor->isRecording())
		monitor->stopRecording();
	OatMonitor::stopRecording();
}


void OatGroupMonitor::printStats(QTextDocument *report, QTextTableFormat tableFormat) {
	if (!monitor || monitor->isRecording())
		return;
	QTextCursor cursor(report);
	cursor.movePosition(QTextCursor::MoveOperation::End);
	cursor.insertHtml("<h2>Recording Times</h2><p>");
	auto table = cursor.insertTable(4, 2, tableFormat);
	int row = 0;
	table->cellAt(row, 0).firstCursorPosition().insertText("Recording Last Time");
	table->cellAt(row, 1).firstCursorPosition().insertText(QString("%1 ms").arg(monitor->getRecordingLastStartTime()));
	row++;
	table->cellAt(row, 0).firstCursorPosition().insertText("Recording Start Time");
	table->cellAt(row, 1).firstCursorPosition().insertText(QString("%1 ms").arg(monitor->getRecordingStartTime()));
	row++;
	table->cellAt(row, 0).firstCursorPosition().insertText("Recording Stop Time");
	table->cellAt(row, 1).firstCursorPosition().insertText(QString("%1 ms").arg(monitor->getRecordingStopTime()));
	row++;
	table->cellAt(row, 0).firstCursorPosition().insertText("Recording Total Time");
	table->cellAt(row, 1).firstCursorPosition().insertText(QString("%1 ms").arg(monitor->getRecordingTotalTime()));
	cursor.insertHtml("</p>");
}


void OatGroupMonitor::printData(QTextDocument *report, QTextTableFormat tableFormat) {
	if (!monitor || monitor->isRecording())
		return;
	QTextCursor cursor(report);
	cursor.movePosition(QTextCursor::MoveOperation::End);
	cursor.insertHtml("<h2>Timeseries Neuro-Modulator</h2><p>");
	auto times = monitor->getTimeVector(); //std::vector<int>
	auto data = monitor->getDataVector(); //std::vector<float>
	auto table = cursor.insertTable(times.size() + 1, 2, tableFormat);  // 
	table->cellAt(0, 0).firstCursorPosition().insertText("Time(ms)");
	table->cellAt(0, 1).firstCursorPosition().insertText("DA-Level");
	for (int i = 0; i < times.size(); i++) {
		table->cellAt(i + 1, 0).firstCursorPosition().insertText(QString::number(times[i]));
		table->cellAt(i + 1, 1).firstCursorPosition().insertText(QString::number(data[i], 'f', 1));
	}
	cursor.insertHtml("</p>");
}

void OatGroupMonitor::printPeaks(QTextDocument *report, QTextTableFormat tableFormat, int maxPeaks) {
	if (!monitor || monitor->isRecording())
		return;
	QTextCursor cursor(report);
	cursor.movePosition(QTextCursor::MoveOperation::End);
	cursor.insertHtml("<h2>Neuro-Modulator Peaks over Time</h2><p>");
	auto times = monitor->getPeakTimeVector(); //std::vector<int>
	auto data = monitor->getPeakValueVector(); //std::vector<float>
	auto n = min<size_t>(times.size(), (size_t) maxPeaks); 
	auto table = cursor.insertTable(n + 1, 2, tableFormat);  // 
	table->cellAt(0, 0).firstCursorPosition().insertText("Time(ms)");
	table->cellAt(0, 1).firstCursorPosition().insertText("Peak DA-Level"); 
	for (int i = 0; i < times.size(); i++) {
		table->cellAt(i + 1, 0).firstCursorPosition().insertText(QString::number(times[i]));
		table->cellAt(i + 1, 1).firstCursorPosition().insertText(QString::number(data[i], 'f', 1));
	}
	cursor.insertHtml("</p>");
}

void OatGroupMonitor::printPeaksSorted(QTextDocument *report, QTextTableFormat tableFormat, int maxPeaks) {
	if (!monitor || monitor->isRecording())
		return;
	QTextCursor cursor(report);
	cursor.movePosition(QTextCursor::MoveOperation::End);
	cursor.insertHtml(QString("<h2>Neuro-Modulator Top %1 Peaks</h2><p>").arg(maxPeaks));
	auto times = monitor->getSortedPeakTimeVector(); //std::vector<int>
	auto data = monitor->getSortedPeakValueVector(); //std::vector<float>
	auto n = min<size_t>(times.size(), (size_t)maxPeaks);
	auto table = cursor.insertTable(n + 1, 2, tableFormat);  // 
	table->cellAt(0, 0).firstCursorPosition().insertText("Time(ms)");
	table->cellAt(0, 1).firstCursorPosition().insertText("Peak DA-Level"); 
	for (int i = 0; i < times.size(); i++) {
		table->cellAt(i + 1, 0).firstCursorPosition().insertText(QString::number(times[i]));
		table->cellAt(i + 1, 1).firstCursorPosition().insertText(QString::number(data[i], 'f', 1));
	}
	cursor.insertHtml("</p>");
}


