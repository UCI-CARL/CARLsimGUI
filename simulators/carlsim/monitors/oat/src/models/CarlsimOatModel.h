#ifndef CARLSIMOATMODEL_H
#define CARLSIMOATMODEL_H

// SpikeStream 0.3
#include "api.h"

//Qt includes
#include <QAbstractTableModel>
#include <QVector>

namespace spikestream {

	class CarlsimWrapper; 
	class NeuronGroup;
	class ConnectionGroup;


	namespace carlsim_monitors {  // not hierarchically  carlsim::monitors
	
		class OatMonitor;

		/*! Used for the display of information about a neuron group.
			Based on the NeuronGroup table in the SpikeStream database. */
		class CarlsimOatModel : public QAbstractTableModel {
			Q_OBJECT
	
		  public:

			    CarlsimOatModel();
				~CarlsimOatModel();

				void setWrapper(void *wrapper); 

				void clearSelection();
				int columnCount(const QModelIndex& parent = QModelIndex()) const;
				QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

				//QList<unsigned int> getSelectedNeuronGroupIDs();
				QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
			
				int rowCount(const QModelIndex& parent = QModelIndex()) const;



				void appendSpikeMonitor(NeuronGroup* group, int start, int end, int period,
					bool active, bool persistent, QString path, int mode);

				void appendConnectionMonitor(ConnectionGroup* group, int start, int end, int period,
					bool active, bool persistent, QString path);

				void appendGroupMonitor(NeuronGroup* group, int start, int end, int period,
					bool active, bool persistent, QString path);

				void appendNeuronMonitor(NeuronGroup* group, int start, int end, int period,
					bool active, bool persistent, QString path);

				void monitorStartRecording(unsigned step); 
				void monitorStopRecording(unsigned step);

				void printMonitor(int index);

				OatMonitor* getMonitor(int index);
	

				/*! Inherited from QAbstractTableModel. */
				bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole);

				friend class CarlsimOatTableView;
				friend class CarlsimOatWidget;

			private slots:
				void loadOatMonitors();
				void unloadOatMonitors();

			private:

				//====================  VARIABLES  ====================

				int rows; 

				QVector<OatMonitor*> monitorList;

				static const int NUM_COLS = 12;

				static const int ACTIVE_COL = 0;
				static const int REC_COL = 1;
				static const int STOP_COL = 2;
				static const int DELETE_COL = 3;
				static const int TYPE_COL = 4;
				static const int GROUP_COL = 5;		// one Monitor per Type and Group
				static const int PRINT_COL = 6;
				static const int START_COL = 7;
				static const int END_COL = 8;
				static const int PERIOD_COL = 9;
				static const int PATH_COL = 10;
				static const int PERSIST_COL = 11; 


				CarlsimWrapper *wrapper;
			};

	}

}

#endif//CARLSIMOATMODEL_H

