#ifndef CARLSIMWIDGET_H
#define CARLSIMWIDGET_H

//SpikeStream includes
#include "MembranePotentialGraphDialog.h"
#include "CarlsimWrapper.h"
#include "SpikeRasterDialog.h"
#include "SpikeStreamTypes.h"
#include "RGBColor.h"

//Qt includes
#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMutex>
#include <QProgressDialog>
#include <QPushButton>
#include <QRadioButton>
#include <QTimer>
#include <QToolBar>
#include <QDockWidget>
#include <QStackedWidget>

// 
#include "api.h" 
 
namespace spikestream {


	class TimePanelWidget;
	class NcMergedDockWidget;
	class AbstractMonitorWidget;
	class AbstractExperimentWidget;
	class CarlsimMonitorWidget;


	/*! Graphical interface for the plugin wrapping spiking neural simulator */
	class CARLSIMWRAPPER_LIB_EXPORT CarlsimWidget : public QWidget {
		Q_OBJECT

		public:
			CarlsimWidget(QWidget* parent = 0);
			~CarlsimWidget();

		protected:
			virtual bool event(QEvent *event);

		signals:
			void simulationLoaded();
			void networkChangedProcessed();
			void injectorsLoaded();
			void injectorsWanted(QString path);


		public slots:
			void provideMonitorWidget(AbstractMonitorWidget*&, QString); 


		private slots:
			void archiveStateChanged(int state);
			void checkLoadingProgress();
			void checkResetWeightsProgress();
			void checkSaveWeightsProgress();
			void deleteMembranePotentialGraphDialog(int);
			void deleteRasterPlotDialog(int);
			void experimentEnded();
			void experimentStarted();
			void injectPatternButtonClicked();
			void loadPattern(QString comboStr);
			void loadSimulation();
			void memPotGraphButtonClicked();
			void monitorChanged(int state);
			void monitorNeuronsStateChanged(int monitorType);
			void carlsimWrapperFinished();
			void networkChanged();
			void rasterButtonClicked();
			void resetWeights();
			void setArchiveDescription();
			void setNeuronParameters();
			void setCarlsimParameters();
			void setSynapseParameters();
			void saveWeights();
			void setMonitorWeights(bool enable);

#ifdef CARLSIM_AXONPLAST
			void checkResetDelaysProgress();
			void checkSaveDelaysProgress();
			void resetDelays();
			void saveDelays();
			void setMonitorDelays(bool enable);
#endif
			void simulationRateChanged(int comboIndex);
			void simulationStopped();
			void startSimulation();
			void startStopSimulation();
			void stepSimulation();
			void stopSimulation();
			void sustainPatternChanged(bool enabled);
			void unloadSimulation(bool confirmWithUser=false);  
			void updateProgress(int stepsCompleted, int totalSteps);
			void updateTimeStep(unsigned int timeStep);
			void updateTimeStep(unsigned int timeStep, const QList<unsigned>& neuronIDList);
			void updateTimeStep(unsigned int timeStep, const QHash<unsigned, float>& membranePotentialMap);
			void setModelParameters();  
			void resetSimtime();

	private:
			//========================  VARIABLES  ========================
			/*! Wrapper for the SNN library */
			CarlsimWrapper* carlsimWrapper;

			/*! Box holding all widgets - makes it easy to enable and disable everything. */
			QGroupBox* mainGroupBox;  	// Optimized Layout

			/*! Box holding monitoring controls */
			QGroupBox* monitorGroupBox;

			/*! Box holding modeltime controls */ 
			QGroupBox* modeltimeGroupBox;

			/*! Box holding injection controls */
			QGroupBox* injectGroupBox;

			/*! Box holding widgets that need to be enabled when simulation is loaded */
			QWidget* controlsWidget;  

			/*! Button for loading simulation */
			QPushButton* loadButton;

			/*! Button for unloading simulation */
			QPushButton* unloadButton;

			/*! Button that launches dialog to edit the neuron parameters */
			QPushButton* neuronParametersButton;

			/*! Button that launches dialog to edit the synapse parameters */
			QPushButton* synapseParametersButton;

			/*! Button that launches dialog to edit the neuron parameters */
			QPushButton* carlsimParametersButton;

			/*! Button that launches dialog to edit the model parameters */  
			QPushButton* modelParametersButton;


			/*! Tool bar with transport controls for loading, playing, etc. */
			QToolBar* toolBar;

			/*! Play action */
			QAction* playAction;

			/*! Stop action */
			QAction* stopAction;

			/*! Action of stepping through the simulation. */
			QAction* stepAction;

			/*! Action for reseting the stepCounter and clocks */
			QAction* resetAction;

			/*! Switches off monitoring of neurons */
			QRadioButton* noMonitorNeuronsButton;

			/*! Switches on monitoring of firing neurons. */
			QRadioButton* monitorFiringNeuronsButton;

			/*! Switches on monitoring of membrane potential */
			QRadioButton* monitorMemPotNeuronsButton;

			/*! Button launching raster plot */
			QPushButton* rasterButton;

			/*! Controls the monitoring of weights */
			QCheckBox* monitorWeightsCheckBox;

			/*! Sets the simulation into archive mode */
			QCheckBox* archiveCheckBox;

			/*! For user to enter a description of the archive */
			QLineEdit* archiveDescriptionEdit;

			/*! Sets the description of the archive */
			QPushButton* setArchiveDescriptionButton;

			/*! Rate of the simulation */
			QComboBox* simulationRateCombo;

			/*! Button for saving volatile weights */
			QPushButton* saveWeightsButton;

			/*! Button for resetting volatile weights */
			QPushButton* resetWeightsButton;

#ifdef CARLSIM_AXONPLAST
			/*! Controls the monitoring of weights */
			QCheckBox* monitorDelaysCheckBox;

			/*! Button for saving volatile weights */
			QPushButton* saveDelaysButton;

			/*! Button for resetting volatile weights */
			QPushButton* resetDelaysButton;
#endif

			/*! Dialog for giving feedback about progress */
			QProgressDialog* progressDialog;

			/*! Mutex for preventing multiple access to functions */
			QMutex mutex;

			/*! Records if task has been cancelled */
			bool taskCancelled;

			/*! The colour of neurons that are currently firing */
			RGBColor* neuronColor;

			/*! Timer to check on loading progress */
			QTimer* loadingTimer;

			/*! Timer to check on progress with heavy tasks with progress bar */
			QTimer* heavyTaskTimer;

			/*! Flag to prevent calls to progress dialog while it is redrawing. */
			bool updatingProgress;

			/*! Combo to select which neuron group gets patterns injected into it. */
			QComboBox* injectPatternNeurGrpCombo;

			/*! Combo to select the pattern to be injected or to add a new pattern. */
			QComboBox* patternCombo;

			/*! Combo enabling user to switch between firing the neuron or setting an
				amount of injection current */
			QComboBox* patternCurrentCombo;

			/*! Button for injecting patterns into network. */
			QPushButton* injectPatternButton;

			/*! Controls whether patterns are sustained across time steps. */
			QCheckBox* sustainPatternChkBox;

			/*! Map linking file path with patterns */
			QHash<QString, Pattern> patternMap;

			/*! Map with colours for plotting heat maps.
				indexes range from 0 to 10 inclusive with increasing temperature. */
			QHash<int, RGBColor*> heatColorMap;

			/*! Counter used to give a unique name to the raster dialogs */
			unsigned rasterDialogCtr;

			/*! Map of the currently open raster dialogs
				The key is the raster dialog id. */
			QHash<unsigned,  SpikeRasterDialog*> rasterDialogMap;

			/*! Button to plot membrane potential graph for selected neuron */
			QPushButton* memPotGraphButton;


#ifdef NC_EXTENSIONS_CARLSIM
			// New Time Panel
			TimePanelWidget*  timePanel;
			NcMergedDockWidget* timePanelDock;

			/*! Noise Injectors */
			QString xmlConfigPath;		
#endif

			/*! Store the Monitor Widget Plug-ins as member variables.
				This gives more control for the handling of the plug-ins 
				for instance when unloading the simmulation or connecting signals to slots.
				Even a controlled "plugin onload" might be possible.
				The same might be applied to the injectors. 
				An similar approach was implemented for the devices, however
				here the managers were stored in the carlsimWrapper. 
				*/
			QList<AbstractMonitorWidget*> monitorWidgetList;




			//=======================  METHODS  =========================
			bool checkForErrors();
			void checkWidgetEnabled();
			void createMembranePotentialColors();
			void fillPatternCurrentCombo();
			QString getFilePath(QString fileFilter);
			unsigned getNeuronGroupID(QString neurGrpStr);
			QString getPatternKey(const QString& patternComboText);
			QToolBar* getToolBar();
			void loadNeuronGroups();
			void setInjectNoise(bool sustain);
			void setInjectionPattern(bool sustain);
	};

}

#endif//CARLSIMWIDGET_H

