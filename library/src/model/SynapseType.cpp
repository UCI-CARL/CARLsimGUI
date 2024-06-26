//SpikeStream includes
#include "SynapseType.h"
using namespace spikestream;

//Qt includes
#include <QDebug>


/*! Empty constructor */
SynapseType::SynapseType(){
	id = 0;
	description = "Undescribed";
	parameterTableName = "Invalid parameter table name.";
	classLibraryName = "Invalid class library name.";
}


/*! Standard constructor */
SynapseType::SynapseType(unsigned int synapseTypeID, const QString& description, const QString& paramTableName, const QString& classLibraryName){
	this->id = synapseTypeID;
	this->description = description;
	this->parameterTableName = paramTableName;
	this->classLibraryName = classLibraryName;
}


/*! Copy constructor */
SynapseType::SynapseType(const SynapseType& synType){
	this->id = synType.id;
	this->description = synType.description;
	this->parameterTableName = synType.parameterTableName;
	this->classLibraryName = synType.classLibraryName;
	this->parameterInfoList = synType.parameterInfoList;
}


/*! Destructor */
SynapseType::~SynapseType(){
}


/*----------------------------------------------------------*/
/*-----                PUBLIC METHODS                  -----*/
/*----------------------------------------------------------*/

/*! Returns the ID of this neuron type */
unsigned int SynapseType::getID() const {
	return id;
}


/*! Returns a description of this neuron type */
QString SynapseType::getDescription() const {
	return description;
}


/*! Returns the name of the database table holding parameters associated with this neuron type */
QString SynapseType::getParameterTableName() const {
	return parameterTableName;
}


/*! Returns, where applicable, the location of a file holding code for this synapse type. */
QString SynapseType::getClassLibaryName() const {
	return classLibraryName;
}


/*! Assignment operator */
SynapseType& SynapseType::operator=(const SynapseType& rhs){
	//Check for self assignment
	if(this == &rhs)
		return *this;

	this->id = rhs.id;
	this->description = rhs.description;
	this->parameterTableName = rhs.parameterTableName;
	this->classLibraryName = rhs.classLibraryName;
	this->parameterInfoList = rhs.parameterInfoList;

	return *this;
}


/*! Returns the list of parameter types */
QList<ParameterInfo> SynapseType::getParameterInfoList() const{
	return parameterInfoList;
}


/*! Sets the information about the parameters associated with this synapse type */
void SynapseType::setParameterInfoList(QList<ParameterInfo>& parameterInfoList){
	this->parameterInfoList = parameterInfoList;
}


