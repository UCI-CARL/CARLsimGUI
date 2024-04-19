#include "NcMergedDockWidget.h"

#include "Globals.h"

#include <QDockWidget>
#include <QAction>

using namespace spikestream; 

NcMergedDockWidget::NcMergedDockWidget(QWidget* _panel, QString dockName, QString dockTitle, QWidget *parent)	
 : panel(_panel), QStackedWidget(parent)
{

	dockWidget = new QDockWidget(parent); 

	dockWidget->setObjectName(dockName);
	dockWidget->setWindowTitle(dockTitle);
	dockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);	
	dockWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
	dockWidget->setWidget(panel);

	QAction* floatMainAction = new QAction("Detach Panel", this); 
	dockWidget->addAction(floatMainAction);
	connect(floatMainAction, SIGNAL(triggered()), this, SLOT(floatMain())); 
	floatMainAction = NULL;

	QAction* dockNestedAction = new QAction("Merge Panel", this); 
	dockWidget->addAction(dockNestedAction);
	connect(dockNestedAction, SIGNAL(triggered()), this, SLOT(dockNested())); 
	dockNestedAction = NULL;

	// Must be explicit by Context Menu
	dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures); 
	this->addWidget(dockWidget);

	this->setStyleSheet("QStackedWidget {border: 2px groove lightgray; }");
}



void NcMergedDockWidget::dockNested() {

	this->setStyleSheet("QStackedWidget {border: 2px groove lightgray; }"); 
	Globals::getMainWindow()->removeDockWidget(dockWidget);
	dockWidget->setParent(this);

	this->setMinimumHeight(panel->minimumHeight()+20);  // Add the bar Height
	this->setMaximumHeight(panel->maximumHeight()+20);	// Add the bar Height
	this->resize(this->size());

	dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures); 
	this->addWidget(dockWidget);
}

void NcMergedDockWidget::dockMain() {

	this->removeWidget(dockWidget);

	dockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);

	dockWidget->setEnabled(true);
	dockWidget->setVisible(true);

	dockWidget->setParent(Globals::getMainWindow());

	Globals::getMainWindow()->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
}


void NcMergedDockWidget::floatMain() {

	this->removeWidget(dockWidget);
	this->setMinimumHeight(0);
	this->setMaximumHeight(0);
	this->resize(this->size());

	this->setStyleSheet("QStackedWidget {border-width: 0px;}");


	dockWidget->setParent(Globals::getMainWindow());

	dockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);

	dockWidget->setEnabled(true);
	dockWidget->setVisible(true);

	dockWidget->setFloating(true);


}


