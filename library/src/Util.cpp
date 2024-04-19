//SpikeStream includes
#include "Util.h"
#include "NumberConversionException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QTime>
#include <QStringList>
#include <QCoreApplication>

//Other includes
#include <cstdint>
#include <cmath>
#include <iostream>

using namespace std;


#ifdef _MSC_VER

/*	\function double rint(double x)

	Round to integral value
	Rounds x to an integral value, using the rounding direction specified by fegetround.
	http://www.cplusplus.com/reference/cmath/rint/
	http://www.cplusplus.com/reference/cmath/round/
	http://www.cplusplus.com/reference/cfenv/fegetround/

	http://en.cppreference.com/w/cpp/numeric/math/rint

	// missing in VS2012 though it is specified in C++11 
	https://connect.microsoft.com/VisualStudio/feedback/details/775474/missing-round-functions-in-standard-library
	http://social.msdn.microsoft.com/Forums/vstudio/en-US/260e04fc-dd05-4a96-8953-9c6ea1ad62fb/cant-find-stdround-in-cmath?forum=vclanguage
*/

/*	copied from 
	http://stackoverflow.com/questions/14919512/rint-not-present-in-visual-studio-2010-math-h-and-equivalent-of-cuda-rint
	replaced by postgres implementation blow since it fails for the following essential testcase
	  QCOMPARE(rint(0.50000000), 1.0);	
	and because rint has 4 different operation modes that are not covered in the implementation 
*/
//double rint(double a) {
//    const double two_to_52 = 4.5035996273704960e+15;
//    double fa = fabs(a);
//    double r = two_to_52 + fa;
//    if (fa >= two_to_52) {
//        r = a;
//    } else {
//        r = r - two_to_52;
//        r = _copysign(r, a);
//    }
//    return r;
//}

// see also
// GNU C Library master sources
// s_rint.c 5.1 93/09/24 */
// http://sourceware.org/git/?p=glibc.git;a=blob;f=sysdeps/ieee754/dbl-64/s_rint.c;h=4e6381efbe040146b518eba62a53534981453a4a;hb=af0498dc955f23c4e2eb28b1658ec2e86b4ab069
//

/* 	Simple Imlementation 
	http://doxygen.postgresql.org/rint_8c.html

	but 
	http://stackoverflow.com/questions/485525/round-for-float-in-c    

	Adding 0.5 before truncating fails to round to the nearest integer for several inputs including 0.49999999999999994. 
	See blog.frama-c.com/index.php?post/2013/05/02/nearbyintf1 –  Pascal Cuoq May 4 at 18:23  
*/


/*	IMpl. Boost
	Boost offers a simple set of rounding functions.
	#include <boost/math/special_functions/round.hpp>
	http://stackoverflow.com/questions/485525/round-for-float-in-c
*/
//#include <boost/math/special_functions/round.hpp>
//double rint(double x) {
//	return boost::math::round(x);
//}


// obsolete by VS2019
///*
// * http://social.msdn.microsoft.com/forums/en-US/Vsexpressvc/thread/dd464efa-99c9-447e-8c50-b683a317de78
// */
//#include <float.h>
//double cbrt(double x) {
//  if (fabs(x) < DBL_EPSILON) return 0.0; 
//  if (x > 0.0) return pow(x, 1.0/3.0); 
//  return -pow(-x, 1.0/3.0); 
//}


// obsolete by VS2019
///*
// * http://en.cppreference.com/w/cpp/numeric/math/exp2
// */ 
//double exp2(double x) {
//  return pow(2,x); 
//}

// obsolete by VS2019
///*
// * http://stackoverflow.com/questions/758001/log2-not-found-in-my-math-h
// */
//double log2(double x) {
// return log(x)/log(2.0); 
//}

#endif // _MSC_VER

#ifndef _MSC_VER
/*! Returns the factorial of a number */
mpf_class Util::factorial (unsigned int num){
	mpz_class result=1;
	for (unsigned int i=1; i<=num; ++i)
		result *= i;
	return result;
}
#endif


/*! Fills an array to select k neurons out of n. */
void Util::fillSelectionArray(bool* selectionArray, int arraySize, int selectionSize){
	int nonSelectionSize = arraySize - selectionSize;

	//Add zeros at start of array up to the non-selection size
	for(int i=0; i<nonSelectionSize; ++i)
		selectionArray[i] = false;

	//Add 1s to the rest of the array
	for(int i=nonSelectionSize; i<arraySize; ++i)
		selectionArray[i] = true;
}


/*! Converts the QString to a bool.
	Throws a number conversion exception if the conversion fails. */
bool Util::getBool(const QString& str){
	if(str.toLower() == "true")
		return true;
	else if (str.toLower() == "false")
		return false;
	throw NumberConversionException("Error converting '" + str + "' to boolean.");
}


/*! Gets a bool parameter out of the hash map with the specified name.
	Throws an exception if it cannot be found. */
bool Util::getBoolParameter(const QString& paramName, QHash<QString, double>& paramMap){
	if(!paramMap.contains(paramName))
		throw SpikeStreamException("Parameter '" + paramName + "'' not found in parameter map.");
	if(paramMap[paramName] == 0.0)
		return false;
	return true;
}


/*! Converts the QString to a float.
	Throws a number conversion exception if the conversion fails. */
float Util::getFloat(const QString& str){
	bool ok = true;
	float newFloat = str.toFloat(&ok);
	if(!ok)
		throw NumberConversionException("Error converting '" + str + "' to float.");
	return newFloat;
}


/*! Converts the QString to an integer.
	Throws a number conversion exception if the conversion fails. */
int Util::getInt(const QString& str){
	bool ok = true;
	int newInt = str.toInt(&ok);
	if(!ok)
		throw NumberConversionException("Error converting '" + str + "' to integer.");
	return newInt;
}


/*! Converts the QString to an unsigned integer.
	Throws a number conversion exception if the conversion fails. */
unsigned int Util::getUInt(const QString& str){
	bool ok = true;
	unsigned int newInt = str.toUInt(&ok);
	if(!ok)
		throw NumberConversionException("Error converting '" + str + "' to unsigned integer.");
	return newInt;
}


/*! Converts a string containing a comma separated list of unsigned integers into  a qlist of unsigned ints  */
QList<unsigned int> Util::getUIntList(const QString& str){
	QStringList strList = str.split(",", QString::SkipEmptyParts);
	QList<unsigned int> uIntList;
	foreach(QString tmpStr, strList)
		uIntList.append(Util::getUInt(tmpStr.trimmed()));
	return uIntList;
}


/*! Converts the QString to a double
	Throws a number conversion exception if the conversion fails. */
double Util::getDouble(const QString& str){
	bool ok = true;
	double newDouble = str.toDouble(&ok);
	if(!ok)
		throw NumberConversionException("Error converting '" + str + "' to double.");
	return newDouble;
}


/*! Returns a double parameter from the supplied map and throws an exception if the parameter does not exist. */
double Util::getDoubleParameter(const QString& paramName, QHash<QString, double>& paramMap){
	if(!paramMap.contains(paramName))
		throw SpikeStreamException("Parameter '" + paramName + "'' not found in parameter map.");
	return paramMap[paramName];
}

/*! Returns a double parameter from the supplied map and throws an exception if the parameter does not exist. */
double Util::getPositiveDoubleParameter(const QString& paramName, QHash<QString, double>& paramMap){
	if(!paramMap.contains(paramName))
		throw SpikeStreamException("Parameter '" + paramName + "'' not found in parameter map.");
	if(paramMap[paramName] < 0.0)
		throw SpikeStreamException("Parameter '" + paramName + "'' should be greater than 0: " + QString::number(paramMap[paramName]));
	return paramMap[paramName];
}


/*! Returns a float parameter from the supplied map and throws an exception if the parameter does not exist. */
float Util::getFloatParameter(const QString& paramName, QHash<QString, double>& paramMap){
	if(!paramMap.contains(paramName))
		throw SpikeStreamException("Parameter '" + paramName + "'' not found in parameter map.");
	return (float)paramMap[paramName];
}


/*! Returns an integer parameter from the supplied map and throws an exception if the parameter does not exist. */
int Util::getIntParameter(const QString& paramName, QHash<QString, double>& paramMap){
	if(!paramMap.contains(paramName))
		throw SpikeStreamException("Parameter '" + paramName + "'' not found in parameter map.");
	if(rint(paramMap[paramName]) != paramMap[paramName])
		throw SpikeStreamException("Parameter '" + paramName + "'' is not an integer.");
	return (int) paramMap[paramName];
}


/*! Returns an unsigned parameter from the supplied map and throws an exception if the parameter does not exist. */
unsigned Util::getUIntParameter(const QString& paramName, QHash<QString, double>& paramMap){
	if(!paramMap.contains(paramName))
		throw SpikeStreamException("Parameter '" + paramName + "'' not found in parameter map.");
	if(rint(paramMap[paramName]) != paramMap[paramName])
		throw SpikeStreamException("Parameter '" + paramName + "'' is not a whole number.");
	if(paramMap[paramName] < 0.0)
		throw SpikeStreamException("Expecting a positive number, but parameter '" + paramName + "'' is less than 0.");
	return (unsigned) paramMap[paramName];
}


/*! Prints out the names and values of the parameters stored in the map */
void Util::printParameterMap(const QHash<QString, double>& paramMap){
	cout<<"---------------  Parameter map  -------------"<<endl;
	for(QHash<QString, double>::const_iterator iter = paramMap.begin(); iter != paramMap.end(); ++iter){
		cout<<"\tKey: "<<iter.key().toStdString().data()<<" value: "<<iter.value()<<endl;
	}
	cout<<endl;
}


/*! Returns a random integer in the range specified.
	Will be less and not equal to the maximum. */
int Util::getRandom(int min, int max){
	if(max <= min)
		throw SpikeStreamException("Incorrect range for random number: maximum <= minimum.");

	int randomNum = min;
	randomNum += Util::rUInt((max-1-min) * ((double)qrand() / (double)RAND_MAX));
	return randomNum;
}


/*! Returns a random double in the specified range */
double Util::getRandomDouble(double min, double max){
	if(min > max)
		throw SpikeStreamException("Minimum cannot be greater than maximum");
	if(min == max)
		return min;
	double ranNum = (double)rand() / (double) RAND_MAX;
	return min + (max-min)*ranNum;
}


/*! Returns a random float in the specified range */
float Util::getRandomFloat(float min, float max){
	if(min > max)
		throw SpikeStreamException("Minimum cannot be greater than maximum");
	if(min == max)
		return min;
	float ranNum = (float)rand() / (float) RAND_MAX;
	return min + (max-min)*ranNum;
}


/*! Returns a random unsigned integer in the specified range */
unsigned int Util::getRandomUInt(unsigned min, unsigned max){
	if(min > max)
		throw SpikeStreamException("Minimum cannot be greater than maximum");
	if(min == max)
		return min;
	return min + rand() % (max-min);
}


/*! Returns the root directory of the application.
	Assumes that application is launched from bin directory. */
QString Util::getRootDirectory(){

	QString rootDirectory = qEnvironmentVariable("SPIKESTREAM_ROOT");
	if (rootDirectory.isEmpty()) {
		// Legacy behavior 
		//Get the working directory - this varies depending on the operating system
		rootDirectory = QCoreApplication::applicationDirPath();
#ifdef MAC32_SPIKESTREAM
		rootDirectory = rootDirectory.section("/bin", -2, -2, QString::SectionSkipEmpty);
#else//Windows or Linux
		rootDirectory.truncate(rootDirectory.size() - 4);//Trim the "/bin" off the end
#endif

	}

	return rootDirectory;
}

/*! Returns true if the string is a number */
bool Util::isNumber(const QString& str){
	//Is it a float - return true if no exception is thrown
	try{
		getFloat(str);
		return true;
	}
	catch(SpikeStreamException&){}

	//Is it a double - return true if no exception is thrown
	try{
		getDouble(str);
		return true;
	}
	catch(SpikeStreamException&){}

	//Is it an integer - return true if no exception is thrown
	try{
		getInt(str);
		return true;
	}
	catch(SpikeStreamException&){}

	//Is it an unsigned integer - return true if no exception is thrown
	try{
		getUInt(str);
		return true;
	}
	catch(SpikeStreamException&){}

	//If we have got to here no number conversion method will work on the string.
	return false;
}


#ifndef _MSC_VER
	/*! Returns the minimum of three numbers */
	float Util::min(float n1, float n2, float n3){
		if(n1 < n2 && n1 < n3)//Is n1 the minimum?
			return n1;
		if(n2 < n3)//n1 is not the minimum, is n2 the minimum?
			return n2;
		return n3;
	}
#elif _MSC_VER >= 1500  // VS 2008 
	/*! Returns the minimum of three numbers */
	float Util::min3(float n1, float n2, float n3){
		if(n1 < n2 && n1 < n3)//Is n1 the minimum?
			return n1;
		if(n2 < n3)//n1 is not the minimum, is n2 the minimum?
			return n2;
		return n3;
	}
#endif



/*! Prints out a binary number */
void Util::printBinary(uint64_t number){
	int numBits = sizeof(number) * 8;
        uint64_t probe = 2;// 0x8000000000000000;
	for(int i=0; i<numBits; ++i){
		if(probe & number)
			cout<<"1";
		else
			cout<<"0";
		probe >>=1;
	}
	cout<<endl;
}


/*! Prints out a bool array  */
void Util::printBoolArray(bool arr[], int arrLen){
	cout<<"Bool Array: ";
	for(int i=0; i< arrLen; ++i){
		if(arr[i])
			cout<<"1";
		else
			cout<<"0";
	}
	cout<<endl;
}


/*! Prints out the byte array */
void Util::printByteArray(byte* byteArr, int arrLen){
	cout<<"Byte array: ";
	for(int i=0; i< arrLen*8; ++i){
		if(byteArr[i/8] & 1<<(i%8))
			cout<<"1";
		else
			cout<<"0";
	}
	cout<<endl;
}


/*! Prints out the bits in a QByteArray */
void Util::printByteArray(const QByteArray& byteArr){
	cout<<"QByteArray: ";
	for(int i=0; i< byteArr.size(); ++i){
		byte tmpByte = (unsigned char) byteArr.at(i);
		for(int j=0; j<8; ++j){
			if(tmpByte & 1<<j)
				cout<<"1";
			else
				cout<<"0";
		}
	}
	cout<<endl;
}


/*! Rounds the given double to the specified number of decimal places. */
double Util::rDouble(double num, int numPlaces){
	double tempDoub = num * ( pow(10.0, (double)numPlaces) );
	tempDoub = rint(tempDoub);
	tempDoub /= ( pow(10.0, (double)numPlaces) );
	return tempDoub;
}


/*! Rounds the double and returns it as an integer */
int Util::rInt(double num){
	num = rint(num);
	return (int) num;
}


/*! Rounds the double and returns it as an unsigned integer */
unsigned int Util::rUInt(double num){
	num = rint(num);
	return (unsigned int) num;
}


/*! Carries out a safe copy of source cstring to target cstring that is no larger than targetSize. */
void Util::safeCStringCopy(char target[], const char source[], int targetSize){
	size_t newLength = strlen(source); // 0.3
	if(newLength > targetSize){
		throw SpikeStreamException("Source cstring too large for destination cstring.");
	}
	int i;
	for(i=0; i< newLength; ++i) //Copy everything up to newLength-1
		target[i] = source[i];
	target[i] = '\0';//Finish string with null character
}



/*! Seeds the random number generator.
	If the seed is not specified or zero, number of seconds to midnight is used as the seed.*/
void Util::seedRandom(int seed){
	if(seed == 0){
		QTime midnight(0, 0, 0);
		qsrand(midnight.secsTo(QTime::currentTime()));
	}
	else
		qsrand(seed);
}


/*! Adds the trainign pattern to the weightless neuron */
void Util::addTraining(WeightlessNeuron& neuron, QString trainingPattern, int output){
	unsigned char* byteArr;
	int arrLen;
	fillByteArray(byteArr, arrLen, trainingPattern);
	QByteArray qByteArr = QByteArray::fromRawData((const char*)byteArr, arrLen);
	neuron.addTraining(qByteArr, output);
	delete [] byteArr;
}


/*! Returns true if the string of 1s and 0s matches the 1s and 0s in the byte array and the output */
bool Util::bitsEqual(unsigned char* byteArr, QString bitPattStr, int output){
	if(byteArr[0] != output)
		return false;

	for(int i=0; i<bitPattStr.length(); ++i){
		if(bitPattStr[i] == '1' && (byteArr[1 + i/8] & ( 1<<(i % 8) )))//1 is equal
			;//do nothing
		else if(bitPattStr[i] == '0' && !(byteArr[1 + i/8] & ( 1<<(i % 8) )))//0 is equal
			;//Do nothing
		else
			return false;//String and byte array do not match
	}
	return true;
}


/*! Fills the suplied array with the 1s and 0s specified in the string. */
void Util::fillByteArray(unsigned char*& byteArr, int& arrLen, QString byteStr){
	if(byteStr.length() % 8 == 0)
		arrLen = byteStr.length() / 8;
	else
		arrLen = byteStr.length() / 8 + 1;
	byteArr = new unsigned char[arrLen];

	//Initialize array
	for(int i=0; i<arrLen; ++i)
		byteArr[i] = 0;

	//Set bits corresponding to 1's in byte string
	for(int i=0; i<byteStr.length(); ++i){
		if(byteStr[i] == '1')
			byteArr[i/8] |= 1<<(i % 8);
	}
}


/*! Sets the generalization in a map of weightless neurons. */
void Util::setGeneralization(QHash<unsigned int, WeightlessNeuron*>& weiNeurMap, double gen){
	for(QHash<unsigned int, WeightlessNeuron*>::iterator iter = weiNeurMap.begin(); iter != weiNeurMap.end(); ++iter)
		iter.value()->setGeneralization(gen);
}


/*! Converts number to a positive number if it is negative */
double Util::toPositive(double num){
	if (num > 0.0)
		return num;
	return num * -1.0;
}


/*! Converts number to a positive number if it is negative */
float Util::toPositive(float num){
	if (num > 0.0f)
		return num;
	return num * -1.0f;
}


/*! Converts number to a positive number if it is negative */
int Util::toPositive(int num){
	if (num >= 0)
		return num;
	return num * -1;
}



