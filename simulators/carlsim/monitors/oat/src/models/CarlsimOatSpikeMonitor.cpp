//SpikeStream includes
#include "Globals.h"
#include "CarlsimOatSpikeMonitor.h"
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


OatSpikeMonitor::OatSpikeMonitor(bool active, QString object, QString path, int start, int end, int period, bool persistent, SpikeMonMode mode) 
	: mode(mode), OatMonitor(active, object, path, start, end, period, persistent) {
	monitor = NULL;
	group = NULL;
}


void OatSpikeMonitor::setMonitor(CarlsimWrapper *wrapper) {
		monitor = wrapper->carlsim->setSpikeMonitor(group->getVID(), path.toStdString());
		monitor->setPersistentData(persistent);
}

void OatSpikeMonitor::unload() {
	monitor = NULL;
	group = NULL;
	OatMonitor::unload();
}


void OatSpikeMonitor::startRecording(unsigned snnTime) {
	if (!monitor||!active) 
		return;
	if((start > -1 
		&& int(snnTime) >= start) 
		&& (end == -1 || int(snnTime) < end)
		&& !monitor->isRecording()
		)  
			monitor->startRecording();
}

void OatSpikeMonitor::stopRecording(unsigned snnTime) {
	if (monitor && active 
		&& monitor->isRecording() 
		&& (snnTime >= end - 1 || (period > 0 && (snnTime + 1) % period == 0))
		)
			monitor->stopRecording();
}

bool OatSpikeMonitor::isRecording() {
	return monitor && monitor->isRecording();
}

void OatSpikeMonitor::startRecording() {
	monitor->startRecording();
	OatMonitor::startRecording();
}

void OatSpikeMonitor::stopRecording() {
	if(monitor->isRecording())
		monitor->stopRecording();
	OatMonitor::stopRecording();
}

void OatSpikeMonitor::print() {
	if (!monitor || !active || monitor->isRecording() || monitor->getRecordingTotalTime() < 1)
		return; 
	monitor->print();
}

void OatSpikeMonitor::clear() {
	monitor->clear();
}



void OatSpikeMonitor::printStats(QTextDocument *report, QTextTableFormat tableFormat) {
	if (!monitor || monitor->isRecording())
		return;

	QTextCursor cursor(report);

	cursor.movePosition(QTextCursor::MoveOperation::End);
	cursor.insertBlock();
	cursor.beginEditBlock();  // can be used for Undo in an exception handler
	cursor.insertHtml("<h2>Statistics</h2><p>");
	//cursor.insertText("Timing");
	cursor.insertHtml("<h3>Timing</h3><p>");
	auto table = cursor.insertTable(7, 2, tableFormat);
	int row = 0; 
	table->cellAt(row, 0).firstCursorPosition().insertText("First Recording Start");
	table->cellAt(row, 1).firstCursorPosition().insertText(QString("%1 ms").arg(monitor->getRecordingStartTime()));
	row++;
	table->cellAt(row, 0).firstCursorPosition().insertText("Last Recording Start");
	table->cellAt(row, 1).firstCursorPosition().insertText(QString("%1 ms").arg(monitor->getRecordingLastStartTime()));
	row++;
	table->cellAt(row, 0).firstCursorPosition().insertText("Last Recording Stop");
	table->cellAt(row, 1).firstCursorPosition().insertText(QString("%1 ms").arg(monitor->getRecordingStopTime()));
	row++;
	table->cellAt(row, 0).firstCursorPosition().insertText("Recording Total Time");
	table->cellAt(row, 1).firstCursorPosition().insertText(QString("%1 ms").arg(monitor->getRecordingTotalTime()));
	row++;
	table->cellAt(row, 0).firstCursorPosition().insertText("Spikes");
	table->cellAt(row, 1).firstCursorPosition().insertText(QString("%1").arg(monitor->getPopNumSpikes()));
	row++;
	table->cellAt(row, 0).firstCursorPosition().insertText("Silent Neurons");
	table->cellAt(row, 1).firstCursorPosition().insertText(QString("%1").arg(monitor->getNumSilentNeurons()));
	row++;
	table->cellAt(row, 1).firstCursorPosition().insertText(QString("%1 %").arg(monitor->getPercentSilentNeurons()));
	cursor.endEditBlock();

	cursor.movePosition(QTextCursor::MoveOperation::End); 
	cursor.insertBlock();
	cursor.beginEditBlock();
	//cursor.insertText("Firing Stats");
	cursor.insertHtml("<h3>Firing Stats</h3><p>");
	table = cursor.insertTable(3, 4, tableFormat);
	row=0;
	table->cellAt(row, 0).firstCursorPosition().insertText("Firing");
	table->cellAt(row, 1).firstCursorPosition().insertText("Min");
	table->cellAt(row, 2).firstCursorPosition().insertText("Mean");
	table->cellAt(row, 3).firstCursorPosition().insertText("Max");
	row++;
	table->cellAt(row, 0).firstCursorPosition().insertText("Rate");
	table->cellAt(row, 1).firstCursorPosition().insertText(QString("%1 Hz").arg(monitor->getMinFiringRate()));
	table->cellAt(row, 2).firstCursorPosition().insertText(QString("%1 Hz").arg(monitor->getPopMeanFiringRate()));
	table->cellAt(row, 3).firstCursorPosition().insertText(QString("%1 Hz").arg(monitor->getMaxFiringRate()));
	row++;
	table->cellAt(row, 0).firstCursorPosition().insertText("Std. Dev.");
	table->cellAt(row, 2).firstCursorPosition().insertText(QString("%1 Hz").arg(monitor->getPopStdFiringRate()));
	cursor.endEditBlock();

	{
		cursor.movePosition(QTextCursor::MoveOperation::End);
		cursor.insertBlock();
		cursor.beginEditBlock();
		//cursor.insertText("Top Ten Firing Rates");
		cursor.insertHtml("<h3>Top Ten Firing Rates</h3><p>");
		const auto &rates = monitor->getAllFiringRatesSorted(); //std::vector<float>   // Linux Pi5 
		int rows = min<int>(10, rates.size()); 
		table = cursor.insertTable( 10+1, 2, tableFormat);
		table->cellAt(0, 0).firstCursorPosition().insertText("#"); 
		table->cellAt(0, 1).firstCursorPosition().insertText("Rate (Hz)");
		for (int row = 1; row <= 10; row++)
			table->cellAt(row, 0).firstCursorPosition().insertText(QString("%1.").arg(row));
		for (int i = 0; i < rows; i++)
			table->cellAt(i + 1, 1).firstCursorPosition().insertText(QString::number(rates[rates.size() - i - 1], 'f', 1));
		cursor.endEditBlock(); 
	}

	{
		cursor.movePosition(QTextCursor::MoveOperation::End);
		cursor.insertBlock();
		cursor.beginEditBlock();
		//cursor.insertText("All Firings");
		cursor.insertHtml("<h3>All Firings</h3><p>");
		auto rates = monitor->getAllFiringRates(); //std::vector<float>
		table = cursor.insertTable(rates.size()+1, 4, tableFormat);  // Local Id, Global Id, Network Id, Rate
		table->cellAt(0, 0).firstCursorPosition().insertText("Neuron");
		table->cellAt(0, 1).firstCursorPosition().insertText("CARLsim-ID");
		table->cellAt(0, 2).firstCursorPosition().insertText("SpikeStream-ID");
		table->cellAt(0, 3).firstCursorPosition().insertText("Rate (Hz)");
		for (int i = 0; i < rates.size(); i++) {
			table->cellAt(i+1, 0).firstCursorPosition().insertText(QString::number(i));
			table->cellAt(i+1, 3).firstCursorPosition().insertText(QString::number(rates[i], 'f', 1));
		}
		cursor.endEditBlock();
	}
}


void OatSpikeMonitor::printSpikeTimes(QTextDocument *report, QTextTableFormat tableFormat) {
	if (!monitor || monitor->isRecording())
		return;
	QTextCursor cursor(report);
	cursor.movePosition(QTextCursor::MoveOperation::End);
	cursor.insertBlock();
	cursor.beginEditBlock();
	cursor.insertHtml("<h2>Spike Times</h2><p>");
	auto neuronSpikes = monitor->getSpikeVector2D(); // std::vector<std::vector<int>>
	auto table = cursor.insertTable(neuronSpikes.size() + 1, 4, tableFormat);  // Local Id, Global Id, Network Id, Rate
	table->cellAt(0, 0).firstCursorPosition().insertText("Neuron");
	table->cellAt(0, 1).firstCursorPosition().insertText("CARLsim-ID");
	table->cellAt(0, 2).firstCursorPosition().insertText("SpikeStream-ID");
	table->cellAt(0, 3).firstCursorPosition().insertText("Spikes @ ms");
	for (int i = 0; i < neuronSpikes.size(); i++) {
		table->cellAt(i + 1, 0).firstCursorPosition().insertText(QString::number(i));
		auto spikes = neuronSpikes[i]; // vector<int>  ms
		auto cell_cursor(table->cellAt(i + 1, 3).firstCursorPosition());
		for (int j = 0; j < spikes.size(); j++) {
			if (j > 0)
				cell_cursor.insertText(", ");
			cell_cursor.insertText(QString::number(spikes[j]));
		}
	}
	cursor.insertHtml("</p>");
	cursor.endEditBlock();
}


void OatSpikeMonitor::printNeurons(QTextDocument *report, QTextTableFormat tableFormat, 
	float minFiringRate, float maxFiringRate) {

	if (!monitor || monitor->isRecording())
		return;

	QTextCursor cursor(report);
	cursor.movePosition(QTextCursor::MoveOperation::End);
	cursor.beginEditBlock();
	cursor.insertBlock();
	cursor.insertHtml("<h2>Firing Neurons</h2><p>");
	auto table = cursor.insertTable(1, 2, tableFormat);
	int row = 0;
	table->cellAt(row, 0).firstCursorPosition().insertText(
		QString::asprintf("Neurons with Firings between %.1f Hz and %.1 Hz", minFiringRate, maxFiringRate) );
	int neurons = monitor->getNumNeuronsWithFiringRate(minFiringRate, maxFiringRate);
	table->cellAt(row, 1).firstCursorPosition().insertText(QString::number(neurons));
	cursor.insertHtml("<p>");
	cursor.endEditBlock();

}

void OatSpikeMonitor::printNeuron(QTextDocument *report, QTextTableFormat tableFormat, int neuronId) {

	if (!monitor || monitor->isRecording())
		return;

	if (neuronId < 0 || neuronId > 16 - 1 )  // see constructor || neuronId > size - 1)
		return; 

	QTextCursor cursor(report);
	cursor.movePosition(QTextCursor::MoveOperation::End);
	cursor.beginEditBlock();
	cursor.insertBlock();

	cursor.insertHtml(QString("<h2>Neuron %1 </h2><p>").arg(neuronId));
	auto table = cursor.insertTable(2, 2, tableFormat);
	int row = 0;
	table->cellAt(row, 0).firstCursorPosition().insertText(
		QString("Mean Fring Rate for Neuron %1").arg(neuronId));
	float meanFiringRate = monitor->getNeuronMeanFiringRate(neuronId);
	table->cellAt(row, 1).firstCursorPosition().insertText(QString("%1 Hz").arg(meanFiringRate));
	row++;
	table->cellAt(row, 0).firstCursorPosition().insertText(
		QString("Spikes of Neuron %1").arg(neuronId));
	int spikes = monitor->getNeuronNumSpikes(neuronId);
	table->cellAt(row, 1).firstCursorPosition().insertText(QString::number(spikes));
	cursor.endEditBlock();
}

