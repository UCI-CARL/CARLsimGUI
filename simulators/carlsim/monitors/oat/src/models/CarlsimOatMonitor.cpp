//SpikeStream includes
#include "Globals.h"
#include "CarlsimOatMonitor.h"
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

OatMonitor::OatMonitor() {
		active = false;
		start = -1;
		end = -1;
		period = -1;
		persistent = false;
		path = "DEFAULT";
		object = "";
}

OatMonitor::OatMonitor(bool active, QString object, QString path, int start, int end, int period, bool persistent) :
		active(active), object(object), path(path), start(start), end(end), period(period), persistent(persistent) {
}

void OatMonitor::unload() {
	active = false;
	start = -1;
	end = -1;
	period = -1;
	persistent = false;
	path = "DEFAULT";
	object = "";
}
