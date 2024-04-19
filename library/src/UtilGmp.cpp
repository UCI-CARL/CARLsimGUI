//SpikeStream includes

#ifdef _MSC_VER

#include "UtilGmp.h"
#include "NumberConversionException.h"

using namespace spikestream;

//Qt includes

//Other includes
//#include <stdint.h>
//include "math.h"
#include <iostream>
using namespace std;

/*! Returns the factorial of a number */
mpf_class UtilGmp::factorial (unsigned int num){
	mpz_class result=1;
	for (unsigned int i=1; i<=num; ++i)
		result *= i;
	return result;
}


#endif




