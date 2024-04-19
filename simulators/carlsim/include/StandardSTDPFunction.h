#ifndef STANDARDSTDPFUNCTION_H
#define STANDARDSTDPFUNCTION_H

//SpikeStream includes
#include "AbstractSTDPFunction.h"

// 
#include "api.h" 

namespace spikestream {

	class CARLSIMWRAPPER_LIB_EXPORT StandardSTDPFunction : public AbstractSTDPFunction {
		public:
			StandardSTDPFunction();
			~StandardSTDPFunction();
			float* getPreArray();
			int getPreLength();
			float* getPostArray();
			int getPostLength();
			float getMinExcitatoryWeight();
			float getMaxExcitatoryWeight();
			float getMinInhibitoryWeight();
			float getMaxInhibitoryWeight();
			void print();

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
			void buildStandardSTDPFunction();
			void checkFunctionUpToDate();
			void cleanUp();
	};

}

#endif//STANDARDSTDPFUNCTION_H
