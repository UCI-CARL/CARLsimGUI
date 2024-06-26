#ifndef GLOBALS_H
#define GLOBALS_H


// SpikeStream 0.3: Plugin Support for Microsoft VC
// http://qt-project.org/doc/qt-4.8/qlibrary.html
#ifdef _MSC_VER
#	define _MSC_DLL_API __declspec(dllexport)
#else
#	define _MSC_DLL_API
#endif

// SpikeStream 0.3
#include "api.h"


//SpikeStream includes
#include "AbstractSimulation.h"
#include "AnalysisDao.h"
#include "Archive.h"
#include "ArchiveWidget.h"
#include "NetworksWidget.h"
#include "EventRouter.h"
#include "ArchiveDao.h"
#include "NetworkDao.h"
#include "Network.h"
#include "NetworkDisplay.h"
#include "NetworkViewer.h"
#include "SpikeStreamMainWindow.h"
using namespace spikestream;

//Qt includes
#include <QString>
#include <QDateTime>

namespace spikestream {
    class SPIKESTREAM_APP_LIB_EXPORT Globals {
		public:

			static bool archiveLoaded();
			static void cleanUp();
			static void clearAnalysisRunning(const QString& analysisName);
			static unsigned int getAnalysisID(const QString& analysisName);
			static AnalysisDao* getAnalysisDao();
			static Archive* getArchive();
			static ArchiveDao* getArchiveDao();
			static EventRouter* getEventRouter() {return eventRouter; }
			static Network* getNetwork();
			static NetworkDao* getNetworkDao();
			static NetworkDisplay* getNetworkDisplay();
			static AbstractSimulation* getSimulation();
			static QString getSpikeStreamRoot();
			static QString getWorkingDirectory();

			static bool isAnalysisRunning();
			static bool isArchivePlaying();
			static bool isAnalysisLoaded(const QString& analysisName);
			static bool isSimulationLoaded();
			static bool isSimulationRunning();
			static bool isRendering();
			static bool networkChangeOk();
			static bool networkLoaded();
			static void setAnalysisID(const QString& analysisName, unsigned int id);
			static void setAnalysisRunning(const QString& analysisName);
			static void setArchivePlaying(bool archivePlaying);
			static void setNetwork(Network* network);
			static void setSimulation(AbstractSimulation* simulation);
			static void setSimulationLoading(bool simulationLoading);
			static void setSimulationRunning(bool simulationRunning);

			/* Friend classes that are allowed to change the state of Globals
				by accessing private setter methods*/
			friend class SpikeStreamMainWindow;
			friend class spikestream::NetworksWidget;
			friend class spikestream::NetworkViewer;
			friend class spikestream::ArchiveWidget;


static SpikeStreamMainWindow* getMainWindow();
static void setMainWindow(SpikeStreamMainWindow* );

static SpikeStreamMainWindow* mainWindow;


		private:
			//=====================  VARIABLES ============================
			/*! Wrapper for SpikeStreamAnalysis database */
			static AnalysisDao* analysisDao;

			/*! Wrapper for the SpikeStreamNetwork database */
			static NetworkDao* networkDao;

			/*! Wrapper for SpikeStreamArchive database */
			static ArchiveDao* archiveDao;

			/*! Corresponds to the root of the whole spikestream directory,
				environment variable SPIKESTREAMROOT */
			static QString spikeStreamRoot;

			/*! Default location for loading and saving files */
			static QString workingDirectory;

			///*! Relative Path of the config file passed as first command line param */
			//static QString configFile;

			/*! The current network that is being viewed, edited, etc. */
			static Network* network;

			/*! The current loaded archive */
			static Archive* archive;

			/*! The current simulation */
			static AbstractSimulation* simulation;

			/*! Stores the id for each analysis type.
				An ID of 0 indicates that no analysis is loaded for that type. */
			static QHash<QString, unsigned int> analysisMap;

			/*! Records whether a particular analysis is running. */
			static QHash<QString, bool> analysisRunningMap;

			/*! Records if a simulation is in the process of being loaded */
			static bool simulationLoading;

			/*! Records if a simulation is running */
			static bool simulationRunning;

			/*! Class controlling how the current network is displayed */
			static NetworkDisplay* networkDisplay;

			/*! Router for application-wide events */
			static EventRouter* eventRouter;

			/*! Records when rendering is in progress */
			static bool rendering;

			/*! Is archive playback in progress */
			static bool archivePlaying;


			//======================  METHODS  ============================
			static void setAnalysisDao(AnalysisDao* analysisDao);
			static void setArchive(Archive* archive);
			static void setArchiveDao(ArchiveDao* archiveDao);
			static void setEventRouter(EventRouter* eventRouter);
			static void setNetworkDisplay(NetworkDisplay* networkDisplay);
			static void setNetworkDao(NetworkDao* networkDao);
			static void setRendering(bool rendering);
			static void setSpikeStreamRoot(QString rootDir);
			static void setWorkingDirectory(QString workingDir);


    };

}

#endif // GLOBALS_H
