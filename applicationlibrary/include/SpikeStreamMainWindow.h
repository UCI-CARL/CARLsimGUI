#ifndef SPIKESTREAMMAINWINDOW_H
#define SPIKESTREAMMAINWINDOW_H

// SpikeStream 0.3
#include "api.h" 

//SpikeStream includes
#include "DatabaseManager.h"
#include "SpikeStreamApplication.h"
using namespace spikestream;

//Qt includes
#include <QProgressDialog>
#include <QMainWindow>
#include <QTabWidget>
#include <QCloseEvent>
#include <QMenu>
#include <QToolBar>
#include <QStatusBar>

namespace spikestream {

	class ConfigLoader;

    /*! The main class of the application and the widget below the Spike Stream
		Application. Responsible for initializing the application and setting up the various widgets.*/
    class SPIKESTREAM_APP_LIB_EXPORT SpikeStreamMainWindow: public QMainWindow {
		Q_OBJECT

		public:
			SpikeStreamMainWindow();
			~SpikeStreamMainWindow();

		protected:
			void closeEvent (QCloseEvent* event);

		private slots:
			void about();
			void clearDatabases();
			void loadDatabases();
			void saveDatabases();
			//Obsolete by new Docking Layout
			//void showAnalysisWidget();
			//void showArchiveWidget();
			//void showEditorWidget();
			//void showNetworkWidget();
			//void showSimulationWidget();
			//void showViewerWidget();
			void databaseManagerFinished();
			void connectNeuronId(bool);
			void updateFromNeuronId();
			void updateToNeuronId();
			void setSelectedFromNeuronId(const QString & text); // sets the neuron id by direct entry of the user
			void setSelectedToNeuronId(const QString & text); // sets the neuron id by direct entry of the user
			void saveLayout();
			void resetLayout();
			void restoreLayout();

		private:
			//======================  VARIABLES  ======================

			/*! Carries out heavy database tasks */
			DatabaseManager* databaseManager;

			QProgressDialog* progressDialog;

			QMenu* fileMenu;
			QMenu* viewMenu;
			QMenu* windowMenu;
			QToolBar* simulationToolBar;
			QToolBar* rotationToolBar;
			//QStatusBar* bar;
			QLineEdit* fromNeuronIdEdit;
			QLineEdit* toNeuronIdEdit;

			//=======================  METHODS  =======================
			void initializeApplication();
			// Structure according best practice provide by DockingWidgets example
			void createActions();
			void createMenus();
			void createToolBars();
			void createStatusBar();
			void createDocking(ConfigLoader* configLoader); 			
			//void createCentralWidget();

			/*! Declare copy constructor private so it cannot be used inadvertently. */
			SpikeStreamMainWindow (const SpikeStreamMainWindow&);

			/*! Declare assignment private so it cannot be used inadvertently.*/
			SpikeStreamMainWindow operator = (const SpikeStreamMainWindow&);

    };
}

#endif//SPIKESTREAMMAINWINDOW_H
