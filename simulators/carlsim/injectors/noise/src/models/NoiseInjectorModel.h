#ifndef NOISEINJECTORMODEL_H
#define NOISEINJECTORMODEL_H

// SpikeStream 0.3
#include "api.h"

//SpikeStream includes
#include "NeuronGroupInfo.h"

using namespace spikestream;

//Qt includes
#include <QAbstractTableModel>
#include <QStringList>
#include <QSet>

#include <vector>

namespace spikestream {

	class CarlsimWrapper; 
	class NeuronGroup;
	class CarlsimSpikeGeneratorContainer;

	namespace carlsim_injectors {  // not hierarchically  carlsim::injectors
	

		/*! Used for the display of information about a neuron group.
			Based on the NeuronGroup table in the SpikeStream database. */
		class NoiseInjectorModel : public QAbstractTableModel {
			Q_OBJECT

			
	
		  public:
				// follows implicid from the group  SpikeGeneratorGroup
				enum injection_t {
					CURRENT,  //_POISSON | _RANDOM  selection of neurons are random/following a Poisson process	
					FIRE	  //_POISSON  
				};

				NoiseInjectorModel();
				~NoiseInjectorModel();
				void clearSelection();
				int columnCount(const QModelIndex& parent = QModelIndex()) const;
				QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

				//QList<unsigned int> getSelectedNeuronGroupIDs();
				QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
			
				int rowCount(const QModelIndex& parent = QModelIndex()) const;

				// current of fire, depends on the neuron group (type)
				bool appendInjector(int neuronGroupId, injection_t injection, double percentage, double current, bool sustain, CarlsimWrapper* wrapper); 
				bool updateInjector(int neuronGroupId, double percentage, double current); 

				bool removeInjector(int index); 


				/*! Inherited from QAbstractTableModel. */
				bool setData(const QModelIndex& index, const QVariant&, int); 

				void injectFor(CarlsimWrapper*); 

				void injectCurrentVector(NeuronGroup* group, float percentage, float current); 
				void injectSpikeVector(NeuronGroup* group, float percentage); 

				bool currentInjection(NeuronGroup* group); 

				friend class NoiseInjectorTableView;
				friend class NoiseInjectorWidget; 
				friend class CarlsimSpikeGenerator;

			private slots:
				void loadNoiseInjectors();

			//private:
			public:  // Linux 

				void sustainAllOrNone(); 

				void updateCurrentVector(int i);
				void updateSpikeVector(int i);

				//====================  VARIABLES  ====================

				int rows; 

				/*! List containing the information about the neuron groups in the current network
					This list is automatically refreshed when the network changes. */
				QList<NeuronGroup*> neurGrpList;

				QList<injection_t> typeList; 


				/*! List holding the percentage of neuerons in the group */
				QList<double> percentageList;

				/*! List holding current to be injected for the neurons*/
				QList<double> currentList;


				/*! List of injectors that do sustain the current */
				QList<bool> sustainList; 

				/*! Current vector in CARLsim format 
					vector = size of group 
					neuon id = relative to group (starting at 0)
					important: the current vector is created once when the injector is inserted as a row in the model
					afterwards only its values are set according to the poisson configuration (current + percentage)
				*/
				QList<QVector<float>> currentVectorList; 

				/*! 
				 * 1. user presess the enject button, and therefore only .. 
				 * \note the entry can be overwritten by pressing inject button again
				 * \note the float vector is consumed by the step, meaning afterwards its empty 
				 * and the id of the group is appended at the reset list. 
				 */
				QHash<int, QVector<float>> currentVectorMap;  

				/*! holds the CARLsim ids of neuon groups that requires the explicit resetting 
				 * of external current once defined, if 
				 * 1. an injector was deleted
				 * 2. an adhoc current vector from the currentVectorMap was executed 
				 * 3. the sustian was set to false
				 * \note this is done as the first action in the step
				 * \note this solves the ugly loop in the CarlsimWrapper::stepCarlsim 
				 */
				QSet<int> currentResetSet;  

				/*! Map of indexes of selected rows in neuron group info list */
				QHash<unsigned int, bool> selectionMap;

				static const int NUM_COLS = 7;
				static const int SUSTAIN_COL = 0;
				static const int ID_COL = 1;
				static const int NAME_COL = 2;
				static const int PERCENT_COL = 3;
				static const int CURRENT_COL = 4;
				static const int DELETE_COL = 5;
				//static const int TYPE_COL = 6;   // HIDDEN

				/*! For current, vector is of size 0, for fire size is group initialized once by the group							
				 */			
				QList<std::vector<bool>> spikeVectorList; 

				/*! Contains the instanciated SpikeGenerators with their container. 
				 * the Container is created with new in the append 
				 * and deleted in the desctructor
				 */
				QList<CarlsimSpikeGeneratorContainer*> spikeGeneratorContainerList; 

			};

	}

}

#endif//NOISEINJECTORMODEL_H

