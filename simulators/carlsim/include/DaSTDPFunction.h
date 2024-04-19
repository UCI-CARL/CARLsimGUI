#ifndef DASTDPFUNCTION_H
#define DASTDPFUNCTION_H

//SpikeStream includes
#include "AbstractSTDPFunction.h"

// 
#include "api.h" 

namespace spikestream {

	/*! \note This cannot inherit fromStandardSTDPFunction since 
		their virtual functions are final */
	class CARLSIMWRAPPER_LIB_EXPORT DaSTDPFunction : public AbstractSTDPFunction {
		public:
			DaSTDPFunction();
			~DaSTDPFunction();
			float* getPreArray();
			int getPreLength();
			float* getPostArray();
			int getPostLength();
			float getMinExcitatoryWeight();
			float getMaxExcitatoryWeight();
			float getMinInhibitoryWeight();
			float getMaxInhibitoryWeight();
			void print();

			float getDaReward();
			float getDaDecay();
			float getSynDecay();

		private:
			//=====================  VARIABLES  ====================
			/*! Array of the pre values */
			float* preArray;

			/*! Array of the post values */
			float* postArray;

			/*! Size of the pre array*/
			int preLength;

			/*! Size of the post array*/
			int postLength;

			//=====================  METHODS  ======================
			void buildDaSTDPFunction();
			void checkFunctionUpToDate();
			void cleanUp();
	};

}

#endif//DASTDPFUNCTION_H
