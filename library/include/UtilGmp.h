#ifndef UTILGMP_H
#define UTILGMP_H

#ifdef _MSC_VER

// Spikestream 0.3
#include "api.h" 

//Other includes
#include <stdint.h>
#include <gmpxx.h>

namespace spikestream {
	
	/*! Set of static utility methods. */
	class SPIKESTREAM_LIB_EXPORT UtilGmp {
		public:
			static mpf_class factorial(unsigned int num);
			//static float min3(float n1, float n2, float n3);
			// 1>  Topographic1BuilderThread.cpp
			// 1>C:\Test\Qt\4.8.1\examples\spikestream-vs2012\library\include\Util.h(53): warning C4002: too many actual parameters for macro 'min'
	};
}

#endif// _MSC_VER

#endif//UTILGMP_H

