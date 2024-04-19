//SpikeStream includes
#include "Globals.h"
#include "NoiseInjectorModel.h"
#include "CarlsimSpikeGeneratorContainer.h"
#include "SpikeStreamException.h"
#include "CarlsimWrapper.h"
#include "Util.h"

using namespace spikestream::carlsim_injectors;

//Qt includes
#include <QDebug>
#include <QIcon>

/*! Constructor */
NoiseInjectorModel::NoiseInjectorModel() : QAbstractTableModel(){
	//connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(loadNeuronGroups()));
	rows = 0;
}


/*! Destructor */
NoiseInjectorModel::~NoiseInjectorModel(){
	foreach(auto container, spikeGeneratorContainerList) {
		if(container)
			delete container; 
	}
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Clears the list of selected neuron groups */
void NoiseInjectorModel::clearSelection(){
	selectionMap.clear();
}


/*! Inherited from QAbstractTableModel. Returns the number of columns in the model */
int NoiseInjectorModel::columnCount(const QModelIndex&) const{
	return NUM_COLS;
}


/*! Inherited from QAbstractTableModel. Returns the data at the specified model index
    ready to be displayed in the way requested */
QVariant NoiseInjectorModel::data(const QModelIndex & index, int role) const{
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
		if(index.column() == ID_COL)
			return neurGrpList[index.row()]->getInfo().getID();
		else
		if(index.column() == NAME_COL)
			return neurGrpList[index.row()]->getInfo().getName(); 
		else
		if(index.column() == PERCENT_COL)
			return QString::number(percentageList[index.row()])+" %";		
		else
		if(index.column() == CURRENT_COL) {
			QString cell;
			auto current = currentList[index.row()]; 
			if(current>0.f) {
				cell = QString::number(current) + " mA";
			}
			return cell;		
		}
    }

	//Icons
	if (role == Qt::DecorationRole){
		if(index.column() == DELETE_COL ){
			return QIcon(Globals::getSpikeStreamRoot() + "/images/trash_can.png");
		}
	}

	//Check boxes
	if(role == Qt::CheckStateRole)
		if(index.column() == SUSTAIN_COL)
			return sustainList[index.row()];
	

    //If we have reached this point ignore request
    return QVariant();
}



/* groupId
 */
#include <QRandomGenerator>
void NoiseInjectorModel::injectCurrentVector(NeuronGroup* group, float percentage, float current) {
	auto n = group->size();
	int k = n * percentage / 100.f;
	if (k < 1)
		return;
	QVector<float> currentVector(n, 0.f);
	QVector<bool> usedIndices(n, false);
	int m = n; 
	for (int i = 0; i < k; i++) {
		auto j = Util::getRandom(0, m); // index of 0..n-1
		// skip used indices
		for (int ii = 0; ii < n; ii++) {
			auto used = usedIndices[ii];
			if(used) {
				j++;
			}
		}				
		usedIndices[j] = true;
		m--;
		currentVector[j] = current;
		qDebug() << "currentVector[" << j << "] = " << current << endl;
	}
	currentVectorMap[group->getVID()] = currentVector;
	currentResetSet.insert(group->getVID());
}



void NoiseInjectorModel::injectSpikeVector(NeuronGroup* group, float percentage) {
	// this ignors the percentage 
	auto i = neurGrpList.indexOf(group); 
	if(i < 0) {
		throw "only preconfigured .. can be injected"; 
	};  
	//assert group == nrnGrpList[i]
	auto n = group->size();			// cache its size
	auto & spikes = spikeVectorList[i];   // std::vector<bool>
	//printf("spikes size %d\n", (int) spikes.size());
	int nRandomNeurons = n*percentage/100.0f;	// calc the configured target size
	nRandomNeurons = min(n, max(1,nRandomNeurons));		// ensure range
	for(int id=0; id<n; id++) 
		spikes[id]=false;	
	unsigned counter=0;					// loop protection 
	while(nRandomNeurons>0) {			// nRandomNeurons are decremented
		auto randomId = Util::getRandom(0, n);		//Get random position in list of neuron ids
		if(!spikes[randomId]) {
			spikes[randomId] = true; 
			nRandomNeurons--;			// decrement 
		}
		if(++counter>1000000)  
			break;  // loop detected 
	}
}



void NoiseInjectorModel::updateCurrentVector(int i) {
	auto group = neurGrpList[i];	// cache the neuron group 
	auto n = group->size();			// cache its size
	float current = currentList[i];
	float percentage = percentageList[i];
	for (int id = 0; id < n; id++) {
		auto p = Util::getRandom(0, 100); // 1% minimal resolution 
		if (percentage > p) {
			currentVectorList[i][id] = current; // assing the current ot the random neuron
		}
		else
			currentVectorList[i][id] = .0f;	// initialize the current vector
	}
}

void NoiseInjectorModel::updateSpikeVector(int i) {
	auto group = neurGrpList[i];	// cache the neuron group 
	auto n = group->size();			// cache its size
	auto & spikes = spikeVectorList[i];   // std::vector<bool>
	//printf("spikes size %d\n", (int) spikes.size());
	int nRandomNeurons = n*percentageList[i]/100.0f;	// calc the configured target size
	nRandomNeurons = min(n, max(1,nRandomNeurons));		// ensure range
	for(int id=0; id<n; id++) 
		spikes[id]=false;	
	unsigned counter=0;					// loop protection 
	while(nRandomNeurons>0) {			// nRandomNeurons are decremented
		auto randomId = Util::getRandom(0, n);		//Get random position in list of neuron ids
		if(!spikes[randomId]) {
			spikes[randomId] = true; 
			nRandomNeurons--;			// decrement 
		}
		if(++counter>1000000)  
			break;  // loop detected 
	}
}


void NoiseInjectorModel::injectFor(CarlsimWrapper* carlsimWrapper) {
	
	// adhoc current
	// reset 
	foreach(auto vid, currentResetSet) {
		carlsimWrapper->carlsim->setExternalCurrent(vid, 0.f);
		currentResetSet.remove(vid); 
	}
	//assert(currentResetList.isEmpty());
	// inject
	foreach(int vid, currentVectorMap.keys()) {
		carlsimWrapper->carlsim->setExternalCurrent(vid, currentVectorMap[vid].toStdVector());
		currentVectorMap.remove(vid); 
		currentResetSet.insert(vid); 
	}
	//assert(currentVectorMap.isEmpty()); 

	// sustain
	for(int i=0; i<rows; i++) {			// for all injectors
		if(sustainList[i]) {			// check if current is to be injected
			if(typeList[i]==CURRENT) {
				updateCurrentVector(i);		// refresh the current vector
				carlsimWrapper->carlsim->setExternalCurrent( // delegate to CARLsim
					neurGrpList[i]->getVID(),				 // CARLsim's ID is stored in the group 
					currentVectorList[i].toStdVector());	 // convert to std vector
			} else
			if(typeList[i]==FIRE) {
				updateSpikeVector(i); 
				//the callback configured in config state reads the spikeVector and resets it to false
			}
		}
	}		
		
}



bool NoiseInjectorModel::appendInjector(int neuronGroupId, injection_t injection, double percentage, double current, bool sustain, CarlsimWrapper* wrapper) {
	
	NeuronGroup* group = Globals::getNetwork()->getNeuronGroup(neuronGroupId);
	bool isCurrentInjection = currentInjection(group); 

	neurGrpList.append(group);  // both IDs are needed later 
	typeList.append(injection); 

	percentageList.append(percentage);
	currentList.append(isCurrentInjection?current:.0f); 
	sustainList.append(sustain); 

	QVector<float> currentVector(isCurrentInjection?group->size():0); 
	currentVectorList.append(currentVector);

	std::vector<bool> spikeVector(isCurrentInjection?0:group->size()); 
	spikeVectorList.append(spikeVector); 

	
	spikeGeneratorContainerList.append(injection==FIRE?
		new CarlsimSpikeGeneratorContainer(this, rows, wrapper):NULL); 
		 
	rows++; 

	//emit reset();
	// reset is obsole Qt 5.15
	// \sa https://doc.qt.io/qt-5/qabstractitemmodel-obsolete.html
	emit endResetModel();


	return true; 	
}


bool NoiseInjectorModel::currentInjection(NeuronGroup* group) {
	return group->getNeuronTypeID() <= 2; 
}



bool NoiseInjectorModel::updateInjector(int neuronGroupId, double percentage, double current) {
	
	NeuronGroup* group = Globals::getNetwork()->getNeuronGroup(neuronGroupId);

	int idx = neurGrpList.indexOf(group); 
	

	percentageList[idx] = percentage;
	currentList[idx] = current; 

	QModelIndex topLeft, bottomRight; 
	topLeft.child(idx, 0);
	bottomRight.child(idx, NUM_COLS); 
	emit dataChanged(topLeft, bottomRight);

	return true; 	
}


bool NoiseInjectorModel::removeInjector(int index) {
	
	currentResetSet.insert(neurGrpList[index]->getVID());

	neurGrpList.removeAt(index); 

	percentageList.removeAt(index); 
	currentList.removeAt(index); 
	sustainList.removeAt(index); 
	currentVectorList.removeAt(index); 
	
	rows--; 

	//emit reset();
	// reset is obsole Qt 5.15
	// e,g, 'QTextStreamFunctions::reset': function does not take 0 arguments
	// \sa https://doc.qt.io/qt-5/qabstractitemmodel-obsolete.html
	emit endResetModel();

	return true; 	
}


/*! Inherited from QAbstractTableModel. Returns the header data */
QVariant NoiseInjectorModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role != Qt::DisplayRole)
		return QVariant();

    if (orientation == Qt::Horizontal){
		if(section == ID_COL)
			return "ID";
		else
		if(section == NAME_COL)
			return "Group";
		else
		if(section == PERCENT_COL)
			return "Neurons";
		else
		if(section == CURRENT_COL)
			return "Current";
		else
		if(section == SUSTAIN_COL)
			return "";
    }

	return QVariant();

}


/*! Inherited from QAbstractTableModel. Returns the number of rows in the model. */
int NoiseInjectorModel::rowCount(const QModelIndex& ) const{
    return rows;  // still does not work 
}

void NoiseInjectorModel::loadNoiseInjectors() {

}



/*! If at least one injector is on, switch all off; if all are off, switch all on 
 */
void NoiseInjectorModel::sustainAllOrNone(){

	bool on = false; 

	for(auto iter = sustainList.begin(); iter< sustainList.end(); iter++)
		on = on || *iter;  

	on = !on; 	
	int i=0;
	for(auto iter = sustainList.begin(); iter< sustainList.end(); iter++, i++) {
		*iter = on; 
		if(!on && typeList[i]==CURRENT)
			currentResetSet.insert(neurGrpList[i]->getVID());
	}

	QModelIndex leftTop, bottomRight;
	leftTop.child(0, 0); 
	bottomRight.child(1,0);
	emit dataChanged(leftTop, bottomRight);
}




/*! Inherited from QAbstractTableModel. */
bool NoiseInjectorModel::setData(const QModelIndex& index, const QVariant&, int) {

	//Change selection status of neuron group
	if(index.column() == SUSTAIN_COL){
		bool sustain = !sustainList[index.row()]; 
		sustainList[index.row()] = sustain; 
		if(!sustain && typeList[index.row()]==CURRENT)
			currentResetSet.insert(neurGrpList[index.row()]->getVID());
		emit dataChanged(index, index);
		return true;
	}
	
    //If we have reached this point no data has been set
    return false;
}
