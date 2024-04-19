#ifndef ABSTRACTSTDPFUNCTION_H
#define ABSTRACTSTDPFUNCTION_H

//SpikeStream includes
#include "ParameterInfo.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QHash>

// 
#include "api.h" 

namespace spikestream {

	/*! Abstract interface for an STDP function 
	 * for CARLsim this means the Defaults for the STDP methods itself
	 * and the default STDP method to be applied if learning is activated on a Connection Group 
	 */
	class CARLSIMWRAPPER_LIB_EXPORT AbstractSTDPFunction {
		public:
			AbstractSTDPFunction();
			virtual ~AbstractSTDPFunction();
			virtual timestep_t getApplySTDPInterval();
			virtual QHash<QString, double> getParameters();
			virtual void setParameters(QHash<QString, double>& newParameterMap);
			virtual QHash<QString, double> getDefaultParameters();
			virtual QList<ParameterInfo> getParameterInfoList();
			virtual float* getPreArray() = 0;
			virtual int getPreLength() = 0;
			virtual float* getPostArray() = 0;
			virtual int getPostLength() = 0;
			virtual float getMinExcitatoryWeight() = 0;
			virtual float getMaxExcitatoryWeight() = 0;
			virtual float getMinInhibitoryWeight() = 0;
			virtual float getMaxInhibitoryWeight() = 0;
			virtual float getReward();
			virtual void print() = 0;

#ifdef NC_EXTENSIONS_CARLSIM
			virtual float getDaReward() { return 0.0f; };  // only defined in DsSTDP
			virtual float getDaDecay()  { return 0.0f; };
			virtual float getSynDecay() { return 0.0f; }; 
#endif

		protected:
			//======================  VARIABLES  ========================
			/*! List defining the parameters */
			QList<ParameterInfo> parameterInfoList;

			/*! Map containing default values of the parameters */
			QHash<QString, double> defaultParameterMap;

			/*! Map containing the current values of the parameters */
			QHash<QString, double> parameterMap;

			/*! Records if the function is up to date.
				This value is set to false when the parameters are changed */
			bool functionUpToDate;


			//=====================  METHODS  =======================
			double getParameter(QString parameterName);
	};

}

#endif//ABSTRACTSTDPFUNCTION_H
