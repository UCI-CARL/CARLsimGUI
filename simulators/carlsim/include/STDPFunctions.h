#ifndef STDPFUNCTIONS_H
#define STDPFUNCTIONS_H

//SpikeStream includes
#include "AbstractSTDPFunction.h"
#include "ParameterInfo.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QHash>


//Other includes
#include <vector>

// 
#include "api.h" 

namespace spikestream {

	/*! Holds STDP functions for CARLsim. */
	class CARLSIMWRAPPER_LIB_EXPORT STDPFunctions {

		public:
			static void cleanup();
			static timestep_t getApplySTDPInterval(unsigned functionID);
			static QString getFunctionDescription(unsigned functionID);
			static QList<unsigned> getFunctionIDs();
			static QHash<QString, double> getParameters(unsigned functionID);
			static void setParameters(unsigned functionID, QHash<QString, double>& newParameterMap);
			static QHash<QString, double> getDefaultParameters(unsigned functionID);
			static QList<ParameterInfo> getParameterInfoList(unsigned functionID);
			static float* getPreArray(unsigned functionID);
			static int getPreLength(unsigned functionID);
			static float* getPostArray(unsigned functionID);
			static int getPostLength(unsigned functionID);
			static float getMinExcitatoryWeight(unsigned functionID);
			static float getMaxExcitatoryWeight(unsigned functionID);
			static float getMinInhibitoryWeight(unsigned functionID);
			static float getMaxInhibitoryWeight(unsigned functionID);
			static float getReward(unsigned functionID);
			static void print(unsigned functionID);

#ifdef NC_EXTENSIONS_CARLSIM
			// downcast would not be sufficient, since an object singleton is used to store parameters
			static float getDaReward(unsigned functionID);
			static float getDaDecay(unsigned functionID);
			static float getSynDecay(unsigned functionID);
			static const unsigned DA_STDP;
#endif

		private:
			//=====================  VARIABLES  ====================
			/*! Type IDs for the STDP functions.
				NOTE: These should start at 0 and increase continuously. */
			static unsigned STANDARD_STDP;
			static unsigned STEP_STDP;
			//DA_STDP is public but const

			/*! Map containing instances of classes implementing each of the functions */
			static QHash<unsigned, AbstractSTDPFunction*> functionMap;

			/*! Records when function map has been filled */
			static bool initialized;

			//=====================  METHODS  ======================
			static void checkFunctionID(unsigned functionID);
			static void checkInitialization();
			static void initialize();
	};


}


#endif//STDPFUNCTIONS_H

