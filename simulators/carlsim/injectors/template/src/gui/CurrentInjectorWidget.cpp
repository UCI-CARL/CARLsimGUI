//SpikeStream includes
#include "Globals.h"
#include "CurrentInjectorWidget.h"
using namespace spikestream;

//Qt includes
#include <QLabel>
#include <QLayout>


// \sa FinSpikes 
//Functions for dynamic library loading
extern "C" {
	/*! Creates an QWidget class when library is dynamically loaded. */
	_MSC_DLL_API QWidget* getClass(){
		return new CurrentInjectorWidget();
	}

	/*! Returns a descriptive name for this widget */
	_MSC_DLL_API QString getName(){
		return QString("Current Injector");  
	}
}






/*! Constructor */
CurrentInjectorWidget::CurrentInjectorWidget(QWidget* parent) : AbstractMonitorWidget(parent){
	//Add toolbar to start and stop experiment
	//QVBoxLayout* mainVBox = new QVBoxLayout(this);
	//QToolBar* toolBar = getToolBar();
	//mainVBox->addWidget(toolBar);

	////Add status display area
	//statusTextEdit = new QTextEdit(this);
	//statusTextEdit->setReadOnly(true);
	//mainVBox->addWidget(statusTextEdit);
	//mainVBox->addStretch(5);

	////Initialize variables
	//nemoWrapper = NULL;
	//buildParameters();

	////Create experiment manager to run experiment
	//temporalCodingExptManager = new TemporalCodingExptManager();
	//connect(temporalCodingExptManager, SIGNAL(finished()), this, SLOT(managerFinished()));
	//connect(temporalCodingExptManager, SIGNAL(statusUpdate(QString)), this, SLOT(updateStatus(QString)));
}


/*! Destructor */
CurrentInjectorWidget::~CurrentInjectorWidget(){
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Sets the wrapper that will be used in the experiments.
	The void pointer must point to an object of class NemoWrapper */
void CurrentInjectorWidget::setWrapper(void *wrapper){
	this->carlsimWrapper = (CarlsimWrapper*)wrapper;
}


/*----------------------------------------------------------*/
/*------                 PRIVATE SLOTS                ------*/
/*----------------------------------------------------------*/




/*----------------------------------------------------------*/
/*------              PROTECTED METHODS               ------*/
/*----------------------------------------------------------*/



/*----------------------------------------------------------*/
/*------                PRIVATE METHODS               ------*/
/*----------------------------------------------------------*/


