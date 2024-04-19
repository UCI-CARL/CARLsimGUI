//SpikeStream includes
#include "Globals.h"
#include "CarlsimOatModel.h"
#include "CarlsimOatSpikeMonitor.h"
#include "CarlsimOatGroupMonitor.h"
#include "CarlsimOatConnectionMonitor.h"
#include "CarlsimOatNeuronMonitor.h"
#include "SpikeStreamException.h"
#include "CarlsimWrapper.h"
#include "Util.h"


using namespace spikestream::carlsim_monitors;

//Qt includes
#include <QDebug>
#include <QIcon>


/*! Constructor */
CarlsimOatModel::CarlsimOatModel() : QAbstractTableModel(){
	//connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(loadNeuronGroups()));
	rows = 0;
}


/*! Sets the wrapper that will be used in the experiments.
The void pointer must point to an object of class CarlsimWrapper */
void CarlsimOatModel::setWrapper(void *wrapper) {
	this->wrapper = (CarlsimWrapper*)wrapper;
}

/*! Destructor */
CarlsimOatModel::~CarlsimOatModel(){

}


void CarlsimOatModel::unloadOatMonitors() {

	for (int i = 0; i < rowCount(); i++)
		getMonitor(i)->unload();

	monitorList.resize(0);
	rows = 0;

	emit endResetModel();
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Clears the list of selected neuron groups */
void CarlsimOatModel::clearSelection(){
	//selectionMap.clear();
}


/*! Inherited from QAbstractTableModel. Returns the number of columns in the model */
int CarlsimOatModel::columnCount(const QModelIndex&) const{
	return NUM_COLS;
}


/*! Inherited from QAbstractTableModel. Returns the data at the specified model index
    ready to be displayed in the way requested */
QVariant CarlsimOatModel::data(const QModelIndex & index, int role) const{
    //Return invalid index if index is invalid or no network loaded
    if (!index.isValid())
		return QVariant();
    if(!Globals::networkLoaded())
		return QVariant();

    //Check rows and columns are in range
    if (index.row() < 0 || index.row() >= rowCount() || index.column() < 0 || index.column() >= columnCount())
		return QVariant();

	//Return appropriate data
    if (role == Qt::DisplayRole){
		switch (index.column()) {
			case TYPE_COL:
				return monitorList[index.row()]->type();
			case GROUP_COL:
				return monitorList[index.row()]->object;
			case START_COL:
				return monitorList[index.row()]->start;
			case END_COL:
				return monitorList[index.row()]->end;
			case PERIOD_COL:
				return monitorList[index.row()]->period;
			case PATH_COL:
				return monitorList[index.row()]->path;
		}
    }

	//Icons
	if (role == Qt::DecorationRole){
		bool active = monitorList[index.row()]->active; 
		bool recording = monitorList[index.row()]->isRecording();
		switch(index.column()) {
			case ACTIVE_COL:  // CAUTION: monitor->active means not "manual" now
				return QIcon(Globals::getSpikeStreamRoot() + "/images/" + 
					(active||recording?"record.xpm":"stop.xpm"));
			case REC_COL:
				return QIcon(Globals::getSpikeStreamRoot() + "/images/rec.png")
					.pixmap(QSize(22, 22), 
						active || recording ? QIcon::Disabled: QIcon::Active,
						active || recording ? QIcon::Off : QIcon::On);
			case STOP_COL: 
				return QIcon(Globals::getSpikeStreamRoot() + "/images/stop.png")
					.pixmap(QSize(22, 22),
						active || recording ? QIcon::Active : QIcon::Disabled,
						active || recording ? QIcon::On : QIcon::Off);
			case DELETE_COL: 
				return QIcon(Globals::getSpikeStreamRoot() + "/images/trash_can.png")
					.pixmap(QSize(22, 22),
						!active && !recording ? QIcon::Active : QIcon::Disabled,
						!active && !recording ? QIcon::On : QIcon::Off);
			case PRINT_COL: 
				return QIcon(Globals::getSpikeStreamRoot() + "/images/print.png")
					.pixmap(QSize(22, 22),
						active || !recording ? QIcon::Active : QIcon::Disabled,
						active || !recording ? QIcon::On : QIcon::Off);
		}
	}
	
	//Check boxes  -> select
	if (role == Qt::CheckStateRole)
		if (index.column() == PERSIST_COL)
			return monitorList[index.row()]->persistent;	

    //If we have reached this point ignore request
    return QVariant();
}

OatMonitor* CarlsimOatModel::getMonitor(int index) {
	return monitorList[index]; 
};

void CarlsimOatModel::appendSpikeMonitor(NeuronGroup* group, int start, int end, int period,
	bool active, bool persistent, QString path, int mode) {  
	// alternative: prop list
	auto object = group->getInfo().getName() + " (Neuron-Group #" + QString::number(group->getID()) + ")";
	auto monitor = new OatSpikeMonitor(active, object, path, start, end, period, persistent, (SpikeMonMode) mode);
	monitor->group = group; 
	monitor->setMonitor(wrapper);
	monitorList.append(monitor);
	rows++;
	emit endResetModel();

}

void CarlsimOatModel::appendConnectionMonitor(ConnectionGroup* group, int start, int end, int period,
	bool active, bool persistent, QString path) {
	// alternative: prop list
	auto object = group->getInfo().getDescription() + " (Connection-Group #" + QString::number(group->getID()) + ")";
	auto monitor = new OatConnectionMonitor(active, object, path, start, end, period, persistent);
	monitor->group = group;
	monitor->setMonitor(wrapper);
	monitorList.append(monitor);
	rows++;
	emit endResetModel();

}

void CarlsimOatModel::appendGroupMonitor(NeuronGroup* group, int start, int end, int period,
	bool active, bool persistent, QString path) {
	// alternative: prop list
	auto object = group->getInfo().getName() + " (Neuron-Group #" + QString::number(group->getID()) + ")";
	auto monitor = new OatGroupMonitor(active, object, path, start, end, period, persistent);
	monitor->group = group; 
	monitor->setMonitor(wrapper);
	monitorList.append(monitor);
	rows++;
	emit endResetModel();

}

void CarlsimOatModel::appendNeuronMonitor(NeuronGroup* group, int start, int end, int period,
	bool active, bool persistent, QString path) {
	// alternative: prop list
	auto object = group->getInfo().getName() + " (Neuron-Group #" + QString::number(group->getID()) + ")";
	auto monitor = new OatNeuronMonitor(active, object, path, start, end, period, persistent);
	monitor->group = group; 
	monitor->setMonitor(wrapper);
	monitorList.append(monitor);
	rows++;
	emit endResetModel();

}


void CarlsimOatModel::printMonitor(int index) {
	auto monitor = monitorList[index];
	//printf("monitor[%d] %s on %s to be printed\n", index, monitor->type(), monitor->object.toStdString().c_str());   // class wraps this completely
	monitor->print();
}

void CarlsimOatModel::monitorStartRecording(unsigned step) {
	//printf("CarlsimOatModel::monitorStartRecording(unsigned step=%d)\n", step);
	foreach(auto monitor, monitorList) {
		monitor->startRecording(step);
	}
}

void CarlsimOatModel::monitorStopRecording(unsigned step) {
	//printf("CarlsimOatModel::monitorStopRecording(unsigned step=%d)\n", step);
	foreach(auto monitor, monitorList) {
		monitor->stopRecording(step);
	}
}



void CarlsimOatModel::loadOatMonitors() {

}



/*! Inherited from QAbstractTableModel. Returns the header data */
QVariant CarlsimOatModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role != Qt::DisplayRole)
		return QVariant();

    if (orientation == Qt::Horizontal){
		switch (section) {
			//case ACTIVE_COL:	return "";   // Active = Recording, Inactiv = Stopped
			case TYPE_COL:		return "Type";
			case GROUP_COL:		return "Target";
			case START_COL:		return "Start";
			case END_COL:		return "End";
			case PERIOD_COL:	return "Period";
			case PATH_COL:		return "File"; 
			case PERSIST_COL:	return "Persist"; 
			//case DELETE_COL:	return "";
		}
    }

	return QVariant();
}


/*! Inherited from QAbstractTableModel. Returns the number of rows in the model. */
int CarlsimOatModel::rowCount(const QModelIndex& ) const {
    return rows; 
}



/*! Inherited from QAbstractTableModel. */
bool CarlsimOatModel::setData(const QModelIndex& index, const QVariant&, int) {

	auto monitor = monitorList[index.row()];
	auto recording = monitor->isRecording();

	if (index.column() == REC_COL) {		
		// if not simulation is running
		if (!recording) {
			monitor->startRecording(); // manual override
			emit dataChanged(index.child(index.row(), 0), index.child(index.row(), NUM_COLS-1));
		}
		return true;
	} else
	if (index.column() == STOP_COL) {
		if (recording || monitor->active) {
			monitor->stopRecording(); // override
			emit dataChanged(index.child(index.row(), 0), index.child(index.row(), NUM_COLS-1));
		}
		return true;
	} else
	if (index.column() == DELETE_COL) {
		if (!recording && !monitor->active) {					
			monitor->clear();
			emit dataChanged(index.child(index.row(), 0), index.child(index.row(), NUM_COLS-1));
		}
		return true;
	}
	else
	if (index.column() == PERSIST_COL) {
		if (!monitor->active) {
			monitor->persistent = !monitorList[index.row()]->persistent;  
			emit dataChanged(index, index);
		}
		return true;
	}

	//If we have reached this point no data has been set
	return false;
}





