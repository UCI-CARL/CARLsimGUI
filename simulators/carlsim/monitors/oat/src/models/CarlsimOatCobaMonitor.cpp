//SpikeStream includes
#include "Globals.h"
#include "CarlsimOatCobaMonitor.h"
#include "SpikeStreamException.h"
#include "CarlsimWrapper.h"
#include "Util.h"

#include "connection_monitor.h"
#include "group_monitor.h"
#include "coba_monitor.h"
#include "spike_monitor.h"

using namespace spikestream::carlsim_monitors;

//Qt includes
#include <QDebug>
#include <QIcon>
#include <QTextCursor>
#include <QTextTable>



OatCobaMonitor::OatCobaMonitor(bool active, QString object, QString path, int start, int end, int period, bool persistent) :
		OatMonitor(active, object, path, start, end, period, persistent) {
	monitor = NULL;
	group = NULL;
}

void OatCobaMonitor::setMonitor(CarlsimWrapper *wrapper) {
	monitor = wrapper->carlsim->setCobaMonitor(group->getVID(), path.toStdString());
	monitor->setPersistentData(persistent);
	if (monitor->isRecording())
		monitor->stopRecording();
}

bool OatCobaMonitor::isRecording() {
	return monitor && monitor->isRecording();
}

void OatCobaMonitor::startRecording() {
	monitor->startRecording();
	OatMonitor::startRecording();
}

void OatCobaMonitor::stopRecording() {
	if (monitor->isRecording())
		monitor->stopRecording();
	OatMonitor::stopRecording();
}

void OatCobaMonitor::startRecording(unsigned snnTime) {
	if (!monitor  || !active)
		return;
	if (monitor->isRecording()) {
		if (period > 0 && snnTime % period == 0) {
			monitor->stopRecording();
			monitor->startRecording();
		}
	}
	else
		if ((start > -1 && int(snnTime) >= start) && (end == -1 || int(snnTime) < end))  
			monitor->startRecording();
}

void OatCobaMonitor::stopRecording(unsigned snnTime) {
	if (monitor && active && monitor->isRecording()
		&& (snnTime >= end - 1 || (period > 0 && snnTime % period == 0)))
		monitor->stopRecording();
}

void OatCobaMonitor::print() {
	if (!monitor || !active || monitor->isRecording())
		return;
	monitor->print();
}

void OatCobaMonitor::clear() {
	monitor->clear();
}

void OatCobaMonitor::printStats(QTextDocument *report, QTextTableFormat tableFormat) {
	if (!monitor || monitor->isRecording())
		return;
	QTextCursor cursor(report);
	cursor.movePosition(QTextCursor::MoveOperation::End);
	cursor.insertHtml("<h2>Recording Times<h2><p>");
	auto table = cursor.insertTable(1, 2, tableFormat);
	int row = 0;
	table->cellAt(row, 0).firstCursorPosition().insertText("Recording Last Time");
	table->cellAt(row, 1).firstCursorPosition().insertText(QString("%1 ms").arg(monitor->getLastUpdated()));
	cursor.insertHtml("</p>");
}

void OatCobaMonitor::printCOBA(QTextDocument *report, QTextTableFormat tableFormat) {
	if (!monitor || monitor->isRecording())
		return;
	QTextCursor cursor(report);
	cursor.movePosition(QTextCursor::MoveOperation::End);
	cursor.insertHtml("<h2>COBA </h2><p>");
	auto AMPA = monitor->getVectorAMPA();  //std::vector<std::vector<float>>   neuron_ID x float_value
	auto NMDA = monitor->getVectorNMDA(); 
	auto GABAa = monitor->getVectorGABAa(); 
	auto GABAb = monitor->getVectorGABAb();
	auto n = AMPA.size();
	auto t = n > 0 ? AMPA[0].size() : 0;
	auto last = monitor->getLastUpdated(); 
	auto table = cursor.insertTable(n*3 + 1, 2+t, tableFormat); 
	QTextTableCellFormat cellFormat; 
	cellFormat.setBackground(QBrush(Qt::GlobalColor::lightGray));
	table->cellAt(0, 0).firstCursorPosition().insertText("COBA");
	for (int i = 0; i < n; i++) {  // neurons
		table->cellAt(i*4+1, 0).firstCursorPosition().insertText(QString::number(i));  
		auto &cell = table->cellAt(i * 4 + 1, 1); 
		cell.setFormat(cellFormat); 
		cell.firstCursorPosition().insertText("AMPA");
		table->cellAt(i * 4 + 2, 1).firstCursorPosition().insertText("NMDA");
		table->cellAt(i * 4 + 3, 1).firstCursorPosition().insertText("GABAa");
		table->cellAt(i * 4 + 4, 1).firstCursorPosition().insertText("GABAb");
		for (int j = 0; j < t; j++)  {
			if (i == 0)
				table->cellAt(0, j + 2).firstCursorPosition().insertText(QString("%1 ms").arg(last - t + j)); 
			auto &cell = table->cellAt(i * 4 + 1, j + 2);
			cell.setFormat(cellFormat);
			cell.firstCursorPosition().insertText(QString::number(AMPA[i][j], 'f', 1));
			table->cellAt(i * 4 + 2, j + 2).firstCursorPosition().insertText(QString::number(NMDA[i][j], 'f', 1));
			table->cellAt(i * 4 + 3, j + 2).firstCursorPosition().insertText(QString::number(GABAa[i][j], 'f', 1));
			table->cellAt(i * 4 + 4, j + 2).firstCursorPosition().insertText(QString::number(GABAb[i][j], 'f', 1));
		}
	}	
	cursor.insertHtml("</p>");
}


