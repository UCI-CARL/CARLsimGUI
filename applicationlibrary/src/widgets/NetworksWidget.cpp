//SpikeStream includes
#include "Globals.h"
#include "NetworkDao.h"
#include "NetworkDialog.h"
#include "NetworksWidget.h"
#include "PluginsDialog.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>

// NEW
#include <QTreeWidget>


//Other includes
#include <iostream>
using namespace std;

//Enable to show debugging information
//#define DEBUG


/*! Constructor */
NetworksWidget::NetworksWidget(QWidget* parent) : QWidget(parent) {
	//this->setStyleSheet("* { background-color: white; }");

	//this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); 
	//this->setMinimumSize(300, 200);

	// FIXED Warning: QWindowsWindow::setGeometry: Unable to set geometry 1
	//https://stackoverflow.com/questions/31230975/qt-setgeometry-unable-to-set-geometry/31231069#31231069
	//https://stackoverflow.com/questions/54307407/why-am-i-getting-qwindowswindowsetgeometry-unable-to-set-geometry-warning-wit
	//https://stackoverflow.com/questions/31230975/qt-setgeometry-unable-to-set-geometry
	setMinimumSize(250, 30);


	QVBoxLayout* verticalBox = new QVBoxLayout(this);


	// does not work
	//setMinimumSize(200, 200); // https://stackoverflow.com/questions/31230975/qt-setgeometry-unable-to-set-geometry/31231069#31231069




	// New QTreeWidget
	QToolBar* toolBar = new QToolBar(this);
	verticalBox->addWidget(toolBar);

	// Action depends on selected item => member
	loadAction = toolBar->addAction(tr("Load"));  
	loadAction->setEnabled(true);
	connect (loadAction, SIGNAL(triggered()), this, SLOT(loadNetwork()));

	prototypeAction = toolBar->addAction(tr("Prototype"));
	prototypeAction->setEnabled(true);
	connect (prototypeAction, SIGNAL(triggered()), this, SLOT(prototypeNetwork()));

	saveAction = toolBar->addAction(tr("Save"));
	saveAction->setIcon(QIcon(Globals::getSpikeStreamRoot() + "/images/save.png"));  
	saveAction->setEnabled(false);
	connect (saveAction, SIGNAL(triggered()), this, SLOT(saveNetwork()));

	deleteAction = toolBar->addAction(tr("Delete"));
	deleteAction->setIcon(QIcon(Globals::getSpikeStreamRoot() + "/images/trash_can.png"));  
	connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteNetwork()));

	toolBar->addSeparator();

	propertiesAction = toolBar->addAction(tr("Properties..."));
	propertiesAction->setEnabled(false);
	connect (propertiesAction, SIGNAL(triggered()), this, SLOT(setNetworkProperties()));

	toolBar->addSeparator();

	// Action independent of state
	QAction* action = NULL;
	action = toolBar->addAction(tr("New"));
	connect(action, SIGNAL(triggered()), this, SLOT(addNewNetwork()));
	action = toolBar->addAction(tr("Add..."));
	connect(action, SIGNAL(triggered()), this, SLOT(addNetworks()));
	toolBar->addSeparator();
	action = NULL;



	// http://qt-project.org/doc/qt-4.8/qtreewidgetitem.html#details
	// See also QTreeWidget, QTreeWidgetItemIterator,
	treeWidget = new QTreeWidget(this);
	treeWidget->setColumnCount(3);
	treeWidget->setHeaderLabels(QStringList() << tr("Id") << tr("Network/Neurons") << tr("Description"));
	verticalBox->addWidget(treeWidget);
	connect(treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
		this, SLOT(onNetworkSelected(QTreeWidgetItem*))); // Note: second arg is ignored
	connect(treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(loadNeuronGroupsList(QTreeWidgetItem*)));
	connect(treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(unloadNeuronGroupsList(QTreeWidgetItem*)));
	connect(treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), loadAction, SLOT(trigger()));
	loadNetworkListNEW();


	//Create thread-based class for heavy operations
	networkManager = new NetworkManager();
	connect(networkManager, SIGNAL(progress(int, int, QString, bool)), this, SLOT(updateProgress(int, int, QString, bool)), Qt::QueuedConnection);
	connect(networkManager, SIGNAL(finished()), this, SLOT(networkManagerFinished()));
	progressDialog = new QProgressDialog(this, Qt::CustomizeWindowHint);
	progressDialog->setLabelText("QProcessDialog NetworkWidget.cpp #134");
	progressDialog->setAutoClose(true);
	progressDialog->reset();
	progressDialog->setAutoReset(false);
	progressDialog->setModal(true); 
	progressDialog->setMinimumDuration(0);


	//Connect to global reload signal
	connect(Globals::getEventRouter(), SIGNAL(reloadSignal()), this, SLOT(loadNetworkListNEW()), Qt::QueuedConnection);
	connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(loadNetworkListNEW()), Qt::QueuedConnection);
	connect(Globals::getEventRouter(), SIGNAL(networkListChangedSignal()), this, SLOT(loadNetworkListNEW()), Qt::QueuedConnection);

	connect(this, SIGNAL(networkChanged()), Globals::getEventRouter(), SLOT(networkChangedSlot()), Qt::QueuedConnection);
}


/*! Destructor */
NetworksWidget::~NetworksWidget(){
	delete networkManager;
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Shows a plugins dialog to enable the user to add a network */
void NetworksWidget::addNetworks(){
	try{
		PluginsDialog* pluginsDialog = new PluginsDialog(this, "/plugins/networks", "Add Network");
		pluginsDialog->exec();
		delete pluginsDialog;
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Shows a plugins dialog to enable the user to add a network */
void NetworksWidget::addNewNetwork(){
	try{
		NetworkDialog* networkDialog = new NetworkDialog(this);
		if(networkDialog->exec() == QDialog::Accepted) 
		{
			loadNetworkListNEW(); // insert only single row
		}
		delete networkDialog;
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Deletes a network */
void NetworksWidget::deleteNetwork() {
	//Get the ID of the network to be deleted
	//deleteNetworkID = Util::getUInt(sender()->objectName());	// OBSOLETE
	unsigned int deleteNetworkID = selectedNetworkID();
	if (!networkInfoMap.contains(deleteNetworkID)) {
		qCritical() << "Network with ID " << deleteNetworkID << " cannot be found.";
		return;
	}

	//Run checks if we are deleting the current network
	if (Globals::networkLoaded() && Globals::getNetwork()->getID() == deleteNetworkID) {
		if (!Globals::networkChangeOk())
			return;
	}

	//Confirm that user wants to take this action.
	QMessageBox msgBox(QMessageBox::Question,
		QString("Deleting Network"),
		QString("Are you sure that you want to delete network with ID ") + QString::number(deleteNetworkID) + "? This step cannot be undone.",
		QMessageBox::Ok | QMessageBox::Cancel,
		Globals::getMainWindow());

	int ret = msgBox.exec();
	if(ret != QMessageBox::Ok)
		return;

	//Delete the network from the database
	try{
		//Start network manager to load network in a separate thread.
		networkManager->startDeleteNetwork(deleteNetworkID);
		progressDialog->reset();
		progressDialog->setLabelText("Starting network delete.");
		progressDialog->setCancelButton(0);//Cancel not implemented at this stage
		cancelButtonVisible = false;
		progressDialog->show();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<"Exception thrown when deleting network: "<<ex.getMessage();
	}
}


/*! Loads a network into memory */
void NetworksWidget::loadNetwork(){
	//unsigned int networkID = sender()->objectName().toUInt(); // OBSOLETE
	unsigned int networkID = selectedNetworkID();
	if(!networkInfoMap.contains(networkID)){
		qCritical()<<"Network with ID "<<networkID<<" cannot be found.";
		return;
	}

	//Run checks to make sure that we want to change network
	if(!Globals::networkChangeOk())
		return;
	if(Globals::networkLoaded() && !Globals::getNetwork()->isSaved()){
		QMessageBox msgBox(QMessageBox::Warning, "Changing network", "Network is loaded in prototype mode and has not been saved.\nThis will discard all of your changes to the network.\nDo you want to continue?", QMessageBox::Ok | QMessageBox::Cancel, this);
		if(msgBox.exec() != QMessageBox::Ok)
			return;
	}

	//load the network
	loadNetwork(networkInfoMap[networkID]);
}

/*! Loads a network into memory */
void NetworksWidget::loadNetwork(unsigned networkID){
	//unsigned int networkID = sender()->objectName().toUInt();
	if(!networkInfoMap.contains(networkID)){
		qCritical()<<"Network with ID "<<networkID<<" cannot be found.";
		return;
	}

	//Run checks to make sure that we want to change network
	if(!Globals::networkChangeOk())
		return;
	if(Globals::networkLoaded() && !Globals::getNetwork()->isSaved()){
		QMessageBox msgBox(QMessageBox::Warning, "Changing network", "Network is loaded in prototype mode and has not been saved.\nThis will discard all of your changes to the network.\nDo you want to continue?", QMessageBox::Ok | QMessageBox::Cancel, this);
		if(msgBox.exec() != QMessageBox::Ok)
			return;
	}

	//load the network
	loadNetwork(networkInfoMap[networkID]);
}


/*! Loads the list of networks in the tree widget */
void NetworksWidget::loadNetworkListNEW(){
	//Reset widget
	resetNEW();  

	//Get a list of the networks in the database
	NetworkDao* networkDao = Globals::getNetworkDao();
	QList<NetworkInfo> networkInfoList;
	try{
		networkInfoList = networkDao->getNetworksInfo();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		return;
	}

	//Show "no network" message if list is empty
	if(networkInfoList.size() == 0){
		//gridLayout->addWidget(new QLabel("No networks in database"), 0, 0);
		if(Globals::networkLoaded()){
			Globals::setNetwork(0);
			emit networkChanged();
		}
		return;
	}

	//Copy network infos into map
	for(int i=0; i<networkInfoList.size(); ++i){
		networkInfoMap[networkInfoList[i].getID()] = networkInfoList[i];
	}

	/* If the current network is in the network list, then set this as the one loaded
	   Otherwise currentNetworkID is set to zero and the user has to choose the loaded network */
	unsigned int currentNetworkID = 0;
	if(Globals::networkLoaded() && networkInfoMap.contains(Globals::getNetwork()->getID())){
		currentNetworkID = Globals::getNetwork()->getID();
	}

	
	// Load the TreeWidget
	QIcon networkIcon = QIcon(Globals::getSpikeStreamRoot() + "/images/neuralnetwork.png");

	 
	//// treeWidget->expandAll();
	////nw1->setCheckState(0, Qt::Checked); // indicate loaded
	////nw2->setCheckState(0, Qt::Unchecked); // indicate loaded


	//Display the list in the widget
	for(int i=0; i<networkInfoList.size(); ++i){

		QTreeWidgetItem *networkItem = new QTreeWidgetItem(treeWidget);
		networkItem->setIcon(0, networkIcon);
		networkItem->setText(0, QString::number(networkInfoList[i].getID())); // ID
		networkItem->setText(1, networkInfoList[i].getName());
		networkItem->setText(2, networkInfoList[i].getDescription());

		// always show the expand indicator to be able to load the children on demand
		networkItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator); 

		// Format Row depending on whether it is the current network
		if(currentNetworkID == networkInfoList[i].getID()){
			treeWidget->setCurrentItem(networkItem);
			networkItem->setExpanded(true); // show groups 
			QFont font = networkItem->font(0);
			QColor color;
			font.setBold(true);
			if(Globals::getArchiveDao()->networkHasArchives(currentNetworkID)){
				//setStyleSheet( "QLabel { color: #F08080; font-weight: bold; font-style: italic; }");
				font.setItalic(true);
				//color = QColor("#F08080");
				color = QColor(0xF0, 0x80, 0x80);
			} else{
				//setStyleSheet( "QLabel { color: #008000; font-weight: bold; }");
				color = QColor(0x00, 0x80, 0x00);
			}
			networkItem->setFont(0, font); // only ID and Name, not Description
			networkItem->setFont(1, font);
			networkItem->setTextColor(0, color);
			networkItem->setTextColor(1, color);
		}

	}
	
	treeWidget->resizeColumnToContents(0); // must _not_ be called before treeWidget was added to its parent
	treeWidget->resizeColumnToContents(1);	
	//treeWidget->setColumnWidth(2, 50); // Description is respectively uses up all available space
	treeWidget->resizeColumnToContents(2);	

}


/*! Loads the list of networks in the tree widget */
void NetworksWidget::loadNeuronGroupsList(QTreeWidgetItem* networkItem)
{

	QIcon groupIcon = QIcon(Globals::getSpikeStreamRoot() + "/images/neurongroup.png");

	//Get a list of the networks in the database
	NetworkDao* networkDao = Globals::getNetworkDao();
	QList<NeuronGroupInfo> neuronGroupInfoList;
	try{
		networkDao->getNeuronGroupsInfo(networkItem->text(0).toUInt(), neuronGroupInfoList);
	} catch(SpikeStreamException& ex)
	{
		qCritical()<<ex.getMessage();
		return;
	}

	for(int ii=0; ii<neuronGroupInfoList.size(); ++ii){
		QTreeWidgetItem *groupItem = new QTreeWidgetItem(networkItem);
		groupItem->setFlags(Qt::NoItemFlags); // avoid to be selectable
		groupItem->setIcon(0, groupIcon);
		//groupItem->setText(0, QString::number(neuronGroupInfoList[ii].getID())); // ID
		groupItem->setText(1, neuronGroupInfoList[ii].getName()); 
		groupItem->setText(2, neuronGroupInfoList[ii].getDescription());
	}

	treeWidget->resizeColumnToContents(0); // must not be called before treeWidget was added to its parent
	treeWidget->resizeColumnToContents(1);	
}


/*! Loads the list of networks in the tree widget */
void NetworksWidget::unloadNeuronGroupsList(QTreeWidgetItem* networkItem)
{
	//// Var1 - Holzhammer
	//QTreeWidgetItemIterator it(networkItem);
	//while (*it) {
	//	if ((*it)->parent() == networkItem) 
	//	{
	//		networkItem->removeChild(*it);		
	//		delete *it;  // Note: deleteLater() does not exist for WidgetItems since they are no QObjects
	//	}
	//	++it;
	//}

	// Var2 - Qt Master
	QList<QTreeWidgetItem *> children = networkItem->takeChildren();
	foreach(QTreeWidgetItem* child, children)
		delete child;   // Note: deleteLater() does not exist for WidgetItems since they are no QObjects
	
}


//
///*! Loads up the list of networks */
//void NetworksWidget::loadNetworkList(){
//	//Reset widget
//	reset();
//
//	//Get a list of the networks in the database
//	NetworkDao* networkDao = Globals::getNetworkDao();
//	QList<NetworkInfo> networkInfoList;
//	try{
//		networkInfoList = networkDao->getNetworksInfo();
//	}
//	catch(SpikeStreamException& ex){
//		qCritical()<<ex.getMessage();
//		return;
//	}
//
//	//Show "no network" message if list is empty
//	if(networkInfoList.size() == 0){
//		gridLayout->addWidget(new QLabel("No networks in database"), 0, 0);
//		if(Globals::networkLoaded()){
//			Globals::setNetwork(0);
//			emit networkChanged();
//		}
//		return;
//	}
//
//	//Copy network infos into map
//	for(int i=0; i<networkInfoList.size(); ++i){
//		networkInfoMap[networkInfoList[i].getID()] = networkInfoList[i];
//	}
//
//	/* If the current network is in the network list, then set this as the one loaded
//	   Otherwise currentNetworkID is set to zero and the user has to choose the loaded network */
//	unsigned int currentNetworkID = 0;
//	if(Globals::networkLoaded() && networkInfoMap.contains(Globals::getNetwork()->getID())){
//		currentNetworkID = Globals::getNetwork()->getID();
//	}
//
//	//Display the list in the widget
//	for(int i=0; i<networkInfoList.size(); ++i){
//
//		//Property button
//		QPushButton* propButton = new QPushButton("P");
//		propButton->setMaximumWidth(20);
//		propButton->setObjectName(QString::number(networkInfoList[i].getID()));
//		connect ( propButton, SIGNAL(clicked()), this, SLOT( setNetworkProperties() ) );
//		gridLayout->addWidget(propButton, i, propCol);
//
//		//Create labels
//		QLabel* idLabel = new QLabel(QString::number(networkInfoList[i].getID()));
//		gridLayout->addWidget(idLabel, i, idCol);
//		QLabel* nameLabel = new QLabel(networkInfoList[i].getName());
//		gridLayout->addWidget(nameLabel, i, nameCol);
//		QLabel* descriptionLabel = new QLabel(networkInfoList[i].getDescription());
//		gridLayout->addWidget(descriptionLabel, i, descCol);
//
//		//Create the load button and name it with the object id so we can tell which button was invoked
//		QPushButton* loadButton = new QPushButton("Load");
//		loadButton->setObjectName(QString::number(networkInfoList[i].getID()));
//		connect ( loadButton, SIGNAL(clicked()), this, SLOT( loadNetwork() ) );
//		gridLayout->addWidget(loadButton, i, loadCol);
//
//		//Create the prototype button and name it with the object id so we can tell which button was invoked
//		QPushButton* prototypeButton = new QPushButton("Prototype");
//		prototypeButton->setObjectName(QString::number(networkInfoList[i].getID()));
//		connect ( prototypeButton, SIGNAL(clicked()), this, SLOT( prototypeNetwork() ) );
//		gridLayout->addWidget(prototypeButton, i, protoCol);
//
//		//Create the delete button and name it with the object id so we can tell which button was invoked
//		QPushButton* deleteButton = new QPushButton(QIcon(Globals::getSpikeStreamRoot() + "/images/trash_can.png"), "");
//		deleteButton->setObjectName(QString::number(networkInfoList[i].getID()));
//		connect ( deleteButton, SIGNAL(clicked()), this, SLOT( deleteNetwork() ) );
//		gridLayout->addWidget(deleteButton, i, delCol);
//
//		//Set labels and buttons depending on whether it is the current network
//		if(currentNetworkID == networkInfoList[i].getID()){
//			if(Globals::getNetwork()->isPrototypeMode() && !Globals::getNetwork()->isSaved()){
//				QPushButton* saveButton = new QPushButton(QIcon(Globals::getSpikeStreamRoot() + "/images/save.png"), "");
//				saveButton->setObjectName(QString::number(networkInfoList[i].getID()));
//				connect ( saveButton, SIGNAL(clicked()), this, SLOT( saveNetwork() ) );
//				gridLayout->addWidget(saveButton, i, saveCol);
//			}
//
//			if(Globals::getArchiveDao()->networkHasArchives(currentNetworkID)){
//				idLabel->setStyleSheet( "QLabel { color: #F08080; font-weight: bold; font-style: italic; }");
//				nameLabel->setStyleSheet( "QLabel { color: #F08080; font-weight: bold; font-style: italic; }");
//				descriptionLabel->setStyleSheet( "QLabel { color: #F08080; font-weight: bold; font-style: italic; }");
//			}
//			else{
//				idLabel->setStyleSheet( "QLabel { color: #008000; font-weight: bold; }");
//				nameLabel->setStyleSheet( "QLabel { color: #008000; font-weight: bold; }");
//				descriptionLabel->setStyleSheet( "QLabel { color: #008000; font-weight: bold; }");
//			}
//
//			//Disable load or prototype button
//			if(Globals::getNetwork()->isPrototypeMode())
//				prototypeButton->setEnabled(false);
//			else
//				loadButton->setEnabled(false);
//		}
//		else if (Globals::getArchiveDao()->networkHasArchives(currentNetworkID)) {
//			idLabel->setStyleSheet( "QLabel { color: #777777; font-style: italic; }");
//			nameLabel->setStyleSheet( "QLabel { color: #777777; font-style: italic; }");
//			descriptionLabel->setStyleSheet( "QLabel { color: #777777; font-style: italic; }");
//		}
//		else{
//			idLabel->setStyleSheet( "QLabel { color: #777777; }");
//			nameLabel->setStyleSheet( "QLabel { color: #777777; }");
//			descriptionLabel->setStyleSheet( "QLabel { color: #777777; }");
//		}
//	}
//
//
//}


/*! Called when the network manager finishes a task */
void NetworksWidget::networkManagerFinished(){
	//If we have reached this point, loading is complete
	progressDialog->hide();

	//Check for errors
	if(networkManager->isError()){
		qCritical()<<"Error occurred saving network: '"<<networkManager->getErrorMessage()<<"'.";
	}

	//Handle task specific stuff
	switch(networkManager->getCurrentTask()){
		case NetworkManager::LOAD_NETWORK_TASK:
			if(networkManager->isError() || progressDialog->wasCanceled()){
				delete newNetwork;
			}
			else{
				//Store network in global scope
				Globals::setNetwork(newNetwork);

				//Use event router to inform other classes that the network has changed.
				emit networkChanged();
			}
		break;
		case NetworkManager::SAVE_NETWORK_TASK:
			//Refresh network list
			emit networkChanged();
		break;
		case NetworkManager::DELETE_NETWORK_TASK:
			// If we have deleted the current network, use event router to inform other classes that the network has changed.
			if(Globals::networkLoaded() && Globals::getNetwork()->getID() == deleteNetworkID){
				Globals::setNetwork(NULL);
				emit networkChanged();
			}

			//Reload the network list
			loadNetworkListNEW(); 
		break;
		default:
			qCritical()<<"Network manager task not recognized: "<<networkManager->getCurrentTask();
	}
}


unsigned int NetworksWidget::selectedNetworkID() {	
	QList<QTreeWidgetItem*> selectedItems = treeWidget->selectedItems();
	if(selectedItems.isEmpty())
		return 0;
	return selectedItems.first()->text(0).toUInt();
}


/*! Loads up the network in prototyping mode. */
void NetworksWidget::prototypeNetwork(){
	
	unsigned int networkID = selectedNetworkID(); 
	if(!networkInfoMap.contains(networkID)){
		qCritical()<<"Network with ID "<<networkID<<" cannot be found.";
		return;
	}

	//Run checks to make sure that we want to change network
	if(!Globals::networkChangeOk())
		return;
	if(Globals::networkLoaded() && !Globals::getNetwork()->isSaved()){
		QMessageBox msgBox(QMessageBox::Warning, "Changing network", "Network is loaded in prototype mode and has not been saved.\nThis will discard all of your changes to the network.\nDo you want to continue?", QMessageBox::Ok | QMessageBox::Cancel, this);
		if(msgBox.exec() != QMessageBox::Ok)
			return;
	}
	if(Globals::getArchiveDao()->networkHasArchives(networkID)){
		QMessageBox msgBox(QMessageBox::Warning, "Prototype Mode", "This network has archives associated with it.\nIf you change this network in prototype mode, your changes cannot be saved until the archives have been deleted.\nWould you like to continue?", QMessageBox::Ok | QMessageBox::Cancel, this);
		if(msgBox.exec() != QMessageBox::Ok)
			return;
	}
	//Set prototype mode and load the network
	loadNetwork(networkInfoMap[networkID], true);
}


/*! Saves a network that has been loaded in prototyping mode. */
void NetworksWidget::saveNetwork(){
	//Run some internal checks
	if(!Globals::networkLoaded()){
		qCritical()<<"SpikeStream internal error. Should not be able to save network when no network is loaded";
		return;
	}
	
	unsigned int networkID = selectedNetworkID(); 
	if(!networkInfoMap.contains(networkID)){
		qCritical()<<"Network with ID "<<networkID<<" cannot be found.";
		return;
	}
	if(!Globals::getNetwork()->isPrototypeMode()){
		qCritical()<<"SpikeStream internal error. Should not be able to save a network that is not in prototype mode.";
	}
	if(Globals::getArchiveDao()->networkHasArchives(networkID)){
		qCritical()<<"Network has archives.\nYour prototype mode changes cannot be saved until these archives have been deleted.";
		return;
	}

	//Save network
	try{
		//Start network manager to load network in a separate thread.
		networkManager->startSaveNetwork(Globals::getNetwork());
		progressDialog->reset();
		progressDialog->setLabelText("Starting save network.");
		progressDialog->setCancelButton(0);//Cancel not implemented at this stage
		cancelButtonVisible = false;
		progressDialog->show();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Allows user to set name and description of network. */
void NetworksWidget::setNetworkProperties(){

	//	unsigned int networkID = sender()->objectName().toUInt();
	unsigned int networkID = selectedNetworkID();

	if(!networkInfoMap.contains(networkID)){
		qCritical()<<"Network with ID "<<networkID<<" cannot be found.";
		return;
	}
	try{
		NetworkDialog* networkDialog = new NetworkDialog(networkInfoMap[networkID].getName(), networkInfoMap[networkID].getDescription(), this);
		if(networkDialog->exec() == QDialog::Accepted){
			Globals::getNetworkDao()->setNetworkProperties(networkID, networkDialog->getName(), networkDialog->getDescription());
			loadNetworkListNEW(); // Update row only
		}
		delete networkDialog;
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}



/*! Updates progress dialog with load progress. */
void NetworksWidget::updateProgress(int stepsCompleted, int totalSteps, QString message, bool showCancelButton){
	#ifdef DEBUG
		qDebug()<<"steps completed: "<<stepsCompleted<<"; totalSteps: "<<totalSteps<<"; message: |"<<message<<"|";
	#endif//DEBUG

	//Avoid multiple calls to graphics
	if(progressUpdating)
		return;
	progressUpdating = true;

	//Add or remove cancel button depending on the current task that is being done
	if(cancelButtonVisible != showCancelButton){
		if(showCancelButton)
			progressDialog->setCancelButton(new QPushButton("Cancel"));
		else
			progressDialog->setCancelButton(0);
		cancelButtonVisible = showCancelButton;
	}

	//Check for cancellation - stop timer and abort operation
	if(progressDialog->wasCanceled()){
		networkManager->cancel();
		progressDialog->show();
	}

	if(networkManager->isRunning() && progressDialog->isVisible()){
		progressDialog->setMaximum(totalSteps);
		progressDialog->setValue(stepsCompleted);
		progressDialog->setLabelText(message);
	}

	progressUpdating = false;
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Loads up the specified network into memory. */
void NetworksWidget::loadNetwork(NetworkInfo& netInfo, bool prototypeMode){
	if(!networkInfoMap.contains(netInfo.getID())){
		qCritical()<<"Network with ID "<<netInfo.getID()<<" cannot be found.";
		return;
	}

	//Check to see if the network is already loaded and matches the database
	if(Globals::networkLoaded() && Globals::getNetwork()->getID() == netInfo.getID()){
		//Change from saved prototype to loaded mode
		if(Globals::getNetwork()->isPrototypeMode() && Globals::getNetwork()->isSaved()){
			//Change status of network and refresh list
			Globals::getNetwork()->setPrototypeMode(false);
			loadNetworkListNEW();
			return;
		}
		//Change from loaded to prototype
		if(!Globals::getNetwork()->isPrototypeMode() && prototypeMode){
			//Change status of network and refresh list
			Globals::getNetwork()->setPrototypeMode(true);
			loadNetworkListNEW();
			return;
		}
	}

	//Load the network
	progressUpdating = false;
	try{
		/* Create new network and store it in global scope.
			Network is set to null because if an exception is thrown during construction
			then we need to know if the object was created */
		newNetwork = NULL;
		newNetwork = new Network(netInfo, Globals::getNetworkDao()->getDBInfo(), Globals::getArchiveDao()->getDBInfo());

		//Set prototype mode
		newNetwork->setPrototypeMode(prototypeMode);

		//Start network manager to load network in a separate thread.
		networkManager->startLoadNetwork(newNetwork);
		progressDialog->reset();
		progressDialog->setLabelText("Starting load network.");
		progressDialog->setCancelButton(0);
		cancelButtonVisible = false;
		progressDialog->show();
	}
	catch (SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		if(newNetwork != NULL){
			delete newNetwork;
		}
		progressDialog->hide();
		return;
	}
}
//
//
///*! Resets the state of the widget.
//	Deleting the widget automatically removes it from the layout. */
//void NetworksWidget::reset(){
//	//Clean up widgets that were scheduled to be deleted during the previous event cycle
//	foreach(QWidget* widget , cleanUpList)
//		widget->deleteLater();
//	cleanUpList.clear();
//
//	//Remove no networks label if it exists
//	if(networkInfoMap.size() == 0){
//		QLayoutItem* item = gridLayout->itemAtPosition(0, 0);
//		if(item != 0){
//			delete item->widget();
//		}
//		return;
//	}
//
//	//Remove list of networks
//	for(int rowIndx=0; rowIndx<networkInfoMap.size(); ++rowIndx){
//		for(int colIndx = 0; colIndx < numCols; ++colIndx){
//			QLayoutItem* item = gridLayout->itemAtPosition(rowIndx, colIndx);
//			if(item != 0){
//				QWidget* tmpWidget = item->widget();
//				tmpWidget->setVisible(false);
//				gridLayout->removeWidget(tmpWidget);
//				cleanUpList.append(tmpWidget);
//			}
//		}
//	}
//	networkInfoMap.clear();
//}


/*! Resets the state of the widget.
	Deleting the widget automatically removes it from the layout. */
void NetworksWidget::resetNEW(){

	treeWidget->clear();

	networkInfoMap.clear();	 
}


void NetworksWidget::onNetworkSelected(QTreeWidgetItem* current) 
{
	// Update Toolbar Buttons

	if(current==NULL) {
		propertiesAction->setEnabled(false);
		loadAction->setEnabled(false); 
		prototypeAction->setEnabled(false);
		saveAction->setEnabled(false);
		return;
	}


	/* If the current network is in the network list, then set this as the one loaded
	   Otherwise currentNetworkID is set to zero and the user has to choose the loaded network */
	unsigned int currentNetworkID = 0;
	if(Globals::networkLoaded() && networkInfoMap.contains(Globals::getNetwork()->getID())){
		currentNetworkID = Globals::getNetwork()->getID();
	}

	propertiesAction->setEnabled(true);
	loadAction->setEnabled(true); 
	prototypeAction->setEnabled(true);
	saveAction->setEnabled(false);
	
	if(currentNetworkID == current->text(0).toUInt() ) 
	{ 
		// Enable Save Button for Network in Prototype Mode. 
		// Note: Load Button is activ to reload the network from database
		if(Globals::getNetwork()->isPrototypeMode() && !Globals::getNetwork()->isSaved())
		    saveAction->setEnabled(true);
	
		//Disable load or prototype button
		if(Globals::getNetwork()->isPrototypeMode())
			prototypeAction->setEnabled(false);	// a network can only put once in prototype mode
		else
			loadAction->setEnabled(false); // a notwork already loaded can not be loaded again
	}

}
