#ifndef NCMERGEDDOCKWIDGET_H
#define NCMERGEDDOCKWIDGET_H

#include <QStackedWidget>

class QDockWidget;

// 
#include "api.h" 

namespace spikestream {

	class CARLSIMWRAPPER_LIB_EXPORT NcMergedDockWidget : public QStackedWidget
	{
		Q_OBJECT
	public:
		//explicit NcMergedDockWidget(QWidget *parent = 0);
		explicit NcMergedDockWidget(QWidget* panel, QString dockName, QString dockTitle, QWidget *parent = 0);
    
	signals:
    
	public slots:

	private slots:

		void dockNested();
		void dockMain();
		void floatMain();

	private:
		QWidget* panel;
		QDockWidget* dockWidget;
    
	};

}

#endif // NCMERGEDDOCKWIDGET_H
