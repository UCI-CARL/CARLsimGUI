//SpikeStream includes
#include "Globals.h"
#include "CarlsimOatConnectionMonitor.h"
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

OatConnectionMonitor::OatConnectionMonitor(bool active, QString object, QString path, int start, int end, int period, bool persistent)
		: OatMonitor(active, object, path, start, end, period, persistent) { 
monitor = NULL; 
group = NULL; 
}

void OatConnectionMonitor::setMonitor(CarlsimWrapper *wrapper) {
	this->wrapper = wrapper; 

}

void OatConnectionMonitor::setup() {

	NeuronGroup *toGroup = Globals::getNetwork()->getNeuronGroup(group->getToNeuronGroupID());
	NeuronGroup *fromGroup = Globals::getNetwork()->getNeuronGroup(group->getFromNeuronGroupID());
	//monitor = wrapper->carlsim->setConnectionMonitor(fromGroup->getVID(), toGroup->getVID(), path.toStdString());
	monitor = wrapper->carlsim->setConnectionMonitor(fromGroup->getVID(), toGroup->getVID(), "DEFAULT");
	monitor->setUpdateTimeIntervalSec(-1); // use finer period instead 
}

bool OatConnectionMonitor::isRecording() {
	return monitor && active;  
}

void OatConnectionMonitor::startRecording() {
	monitor->takeSnapshot();
}


void OatConnectionMonitor::startRecording(unsigned snnTime) {
	// snapshot 
	if (!monitor || !active)
		return;
	
	if ((start > -1 && snnTime == start) 
		|| (period > 0  
			&& snnTime % period == 0 
			&& int(snnTime) >= start 
			&& (end == -1 || int(snnTime) < end)) )
		monitor->takeSnapshot();
}

void OatConnectionMonitor::stopRecording(unsigned snnTime) {
	if (!active)
		return;
	if (snnTime == end)
		monitor->takeSnapshot();
}

void OatConnectionMonitor::print() {
	if (!monitor || !active)
		return;
	monitor->print();
}

void OatConnectionMonitor::clear() {
	//monitor->clear();
}



void OatConnectionMonitor::printStats(QTextDocument *report, QTextTableFormat tableFormat) {
	if (!monitor)
		return;
	QTextCursor cursor(report);
	cursor.movePosition(QTextCursor::MoveOperation::End);
	cursor.insertHtml("<h2>Statistics</h2><p>");
	auto table = cursor.insertTable(5, 3, tableFormat);
	int row = 0;
	table->cellAt(row, 0).firstCursorPosition().insertText("Current/Last Snapshot Time:");
	table->cellAt(row, 1).firstCursorPosition().insertText(QString("%1 ms").arg(monitor->getTimeMsCurrentSnapshot()));
	table->cellAt(row, 2).firstCursorPosition().insertText(QString("%1 ms").arg(monitor->getTimeMsLastSnapshot()));
	row++;
	table->cellAt(row, 0).firstCursorPosition().insertText("Snapshot Duration:");
	table->cellAt(row, 1).firstCursorPosition().insertText(QString("%1 ms").arg(monitor->getTimeMsSinceLastSnapshot()));
	row++;
	table->cellAt(row, 0).firstCursorPosition().insertText("Weight Change:");
	table->cellAt(row, 1).firstCursorPosition().insertText(QString::number(monitor->getTotalAbsWeightChange(), 'f', 3));
	row++;
	table->cellAt(row, 0).firstCursorPosition().insertText("Synapses");
	table->cellAt(row, 1).firstCursorPosition().insertText(QString::number(monitor->getNumSynapses()));
	row++;
	table->cellAt(row, 0).firstCursorPosition().insertText("Pre-/Post-Neurons:");
	table->cellAt(row, 1).firstCursorPosition().insertText(QString::number(monitor->getNumNeuronsPre()));
	table->cellAt(row, 2).firstCursorPosition().insertText(QString::number(monitor->getNumNeuronsPost()));
	cursor.insertHtml("</p>");
}

// metrics see getNumWeightsWithValue 0.0 => sparsity
void OatConnectionMonitor::printQueries(QTextDocument *report, QTextTableFormat tableFormat,
	int preId, int postId, float minAbsChanged, float minWeight, float maxWeight, float weight) {
	if (!monitor)
		return;
	QTextCursor cursor(report);
	cursor.movePosition(QTextCursor::MoveOperation::End);
	cursor.insertHtml("<h2>Connection Queries</h2><p>");
	auto table = cursor.insertTable(6, 3, tableFormat);
	int row = 0;
	table->cellAt(row, 0).firstCursorPosition().insertText("Fan-In/Fan-Out:");
	table->cellAt(row, 1).firstCursorPosition().insertText(QString("%1").arg(monitor->getFanIn(preId)));
	table->cellAt(row, 2).firstCursorPosition().insertText(QString("%1").arg(monitor->getFanOut(postId)));
	row++;
	table->cellAt(row, 0).firstCursorPosition().insertText("Min-/Max-Weight:");
	table->cellAt(row, 1).firstCursorPosition().insertText(QString::number(monitor->getMinWeight(true), 'f', 3));
	table->cellAt(row, 2).firstCursorPosition().insertText(QString::number(monitor->getMaxWeight(true), 'f', 3));
	row++;
	table->cellAt(row, 0).firstCursorPosition().insertText("Min-/Max-Weight(prev.):");
	table->cellAt(row, 1).firstCursorPosition().insertText(QString::number(monitor->getMinWeight(), 'f', 3));
	table->cellAt(row, 2).firstCursorPosition().insertText(QString::number(monitor->getMaxWeight(), 'f', 3));
	row++;
	table->cellAt(row, 0).firstCursorPosition().insertText(QString("Weight Changes (eps=%1):").arg(minAbsChanged));
	table->cellAt(row, 1).firstCursorPosition().insertText(QString::number(monitor->getNumWeightsChanged(minAbsChanged)));
	table->cellAt(row, 2).firstCursorPosition().insertText(QString("%1 %").arg(monitor->getPercentWeightsChanged(minAbsChanged)));
	row++;
	table->cellAt(row, 0).firstCursorPosition().insertText(QString("Weights in Range (%1 - %2):").arg(minWeight).arg(maxWeight));
	table->cellAt(row, 1).firstCursorPosition().insertText(QString::number(monitor->getNumWeightsInRange(minWeight, maxWeight)));
	table->cellAt(row, 2).firstCursorPosition().insertText(QString("%1 %").arg(monitor->getPercentWeightsInRange(minWeight, maxWeight)));
	row++;
	table->cellAt(row, 0).firstCursorPosition().insertText(QString("Weights at Value %1:").arg(weight));
	table->cellAt(row, 1).firstCursorPosition().insertText(QString::number(monitor->getNumWeightsWithValue(weight)));
	table->cellAt(row, 2).firstCursorPosition().insertText(QString("%1 %").arg(monitor->getPercentWeightsWithValue(weight)));
	cursor.insertHtml("</p>");
}


void OatConnectionMonitor::printWeights(QTextDocument *report, QTextTableFormat tableFormat) {
	if (!monitor)
		return;
	QTextTableCellFormat cellFormat;
	cellFormat.setBackground(QBrush(Qt::GlobalColor::lightGray));
	QTextCursor cursor(report);
	cursor.movePosition(QTextCursor::MoveOperation::End);

	cursor.insertHtml(QString("<h2>Weights@%1ms</h2>").arg(monitor->getTimeMsCurrentSnapshot()));
	auto wmat = monitor->getWeights();  //std::vector<std::vector<float>>   w[pre,post] 
	auto npre = wmat.size(); 
	//assert(npre > 0);
	auto npost = wmat[0].size();
	auto table = cursor.insertTable(npre + 1, npost + 1, tableFormat);  
	auto &cell = table->cellAt(0, 0);
	cell.setFormat(cellFormat);
	cell.firstCursorPosition().insertText("Pre \\ Post");
	for (int pre = 0; pre < npre; pre++) {
		auto &row = table->cellAt(pre + 1, 0);
		row.setFormat(cellFormat);
		row.firstCursorPosition().insertText(QString::number(pre));
		for (int post = 0; post < npost; post++) {
			if (pre == 0) {
				auto &col = table->cellAt(0, post + 1);
				col.setFormat(cellFormat);
				col.firstCursorPosition().insertText(QString::number(post));
			}
			auto w = wmat[pre][post];
			if(!isnan(w))
				table->cellAt(pre + 1, post + 1).firstCursorPosition().insertText(QString::number(w, 'f', 3));  // printf log 
		}
	}
}


void OatConnectionMonitor::printPrevWeights(QTextDocument *report, QTextTableFormat tableFormat) {
	if (!monitor)
		return;
	QTextTableCellFormat cellFormat;
	cellFormat.setBackground(QBrush(Qt::GlobalColor::lightGray));
	QTextCursor cursor(report);
	cursor.movePosition(QTextCursor::MoveOperation::End);

	cursor.insertHtml(QString("<h2>Previous Weights@%1ms</h2><p>").arg(monitor->getTimeMsLastSnapshot()));
	auto wmat = monitor->getPrevWeights();  //std::vector<std::vector<float>>   w[pre,post] 
	auto npre = wmat.size();
	//assert(npre > 0);
	auto npost = wmat[0].size();
	auto table = cursor.insertTable(npre + 1, npost + 1, tableFormat);  
	auto &cell = table->cellAt(0, 0);
	cell.setFormat(cellFormat);
	cell.firstCursorPosition().insertText("Pre \\ Post");
	for (int pre = 0; pre < npre; pre++) {
		auto &row = table->cellAt(pre + 1, 0);
		row.setFormat(cellFormat);
		row.firstCursorPosition().insertText(QString::number(pre));
		for (int post = 0; post < npost; post++) {
			if (pre == 0) {
				auto &col = table->cellAt(0, post + 1);
				col.setFormat(cellFormat);
				col.firstCursorPosition().insertText(QString::number(post));
			}
			auto w = wmat[pre][post];
			if (!isnan(w))
				table->cellAt(pre + 1, post + 1).firstCursorPosition().insertText(QString::number(w, 'f', 3));  // printf log 
		}
	}
	cursor.insertHtml("</p>");
}


// this modifies the monitor
void OatConnectionMonitor::printWeightChanges(QTextDocument *report, QTextTableFormat tableFormat) {
	if (!monitor)
		return;
	QTextTableCellFormat cellFormat;
	cellFormat.setBackground(QBrush(Qt::GlobalColor::lightGray));
	QTextCursor cursor(report);
	cursor.movePosition(QTextCursor::MoveOperation::End);

	cursor.insertHtml(QString("<h2>Weight Changes</h2><p>"));
	auto wmat = monitor->calcWeightChanges();  //std::vector<std::vector<float>>   w[pre,post] 
	auto npre = wmat.size();
	//assert(npre > 0);
	auto npost = wmat[0].size();
	auto table = cursor.insertTable(npre + 1, npost + 1, tableFormat);  // gNId, spikestream NID 
	auto &cell = table->cellAt(0, 0);
	cell.setFormat(cellFormat);
	cell.firstCursorPosition().insertText("Pre \\ Post");
	for (int pre = 0; pre < npre; pre++) {
		auto &row = table->cellAt(pre + 1, 0);
		row.setFormat(cellFormat);
		row.firstCursorPosition().insertText(QString::number(pre));
		for (int post = 0; post < npost; post++) {
			if (pre == 0) {
				auto &col = table->cellAt(0, post + 1);
				col.setFormat(cellFormat);
				col.firstCursorPosition().insertText(QString::number(post));
			}
			auto w = wmat[pre][post];
			if (!isnan(w))
				table->cellAt(pre + 1, post + 1).firstCursorPosition().insertText(QString::number(w, 'f', 3));  // printf log 
		}
	}
	cursor.insertHtml("</p>");
}

