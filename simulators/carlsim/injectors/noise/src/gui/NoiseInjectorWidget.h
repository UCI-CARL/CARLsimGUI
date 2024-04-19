#ifndef NOISEINJECTORWIDGET_H
#define NOISEINJECTORWIDGET_H

//SpikeStream includes
#include "AbstractInjectorWidget.h"
#include "NoiseInjectorManager.h"
#include "../models/NoiseInjectorModel.h"
#include "CarlsimWrapper.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QHash>
#include <QTextEdit>
#include <QPushbutton>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>

namespace spikestream {
  
	namespace carlsim_injectors { 

		/*! Graphical interface for carrying out a pop1 experiment */
		class NoiseInjectorWidget : public AbstractInjectorWidget {
			Q_OBJECT

			public:
				NoiseInjectorWidget(QWidget* parent = 0);
				~NoiseInjectorWidget();
			

			signals: 
				void injectorsLoaded();

			private slots:
				void addInjector();
				void deleteInjectors();
				void loadInjectors();
				void setWrapper(void *wrapper);
				void inject(); 
				void loadNeuronGroups();

				void injectButtonClicked();

				void updateInjection(int); 

				void updateNeuronGroup(const QString &);  // combo box changed

			protected:
			//	void buildParameters();

			

			private:
				//=====================  VARIABLES  ====================
				/*! Wrapper of CarlSIM that is used in the experiments. */
				CarlsimWrapper* carlsimWrapper;

				/*! Manager that runs the experiments */
				NoiseInjectorManager* oatManager;

				/*! Widget holding messages from the manager */
				QTextEdit* statusTextEdit;

				/*! Box holding injection controls */
				QGroupBox* injectGroupBox;
			
				/*! Button to inject noise into a neuron group */
				QPushButton* injectNoiseButton;

				/*! Combo box controlling which neuron group noise is injected into */
				QComboBox* injectNoiseNeuronGroupCombo;

				/*! Combo box controlling the percentage of injected noise */
				QComboBox* injectNoisePercentCombo;

				/*! Combo box controlling whether noise injection fires neurons
					or injects a specified amount of current. */
				QComboBox* injectNoiseCurrentCombo;

				/*! Switches injection of noise on and off. */
				//QCheckBox* sustainNoiseChkBox;

				/*! Adds the adhoc values to sustain list */
				QPushButton* addButton;

				//QPushButton* deleteButton; 


				NoiseInjectorModel* currentInjectorModel;

				//=====================  METHODS  ======================

		};
	}
}

#endif//NOISEINJECTORWIDGET_H