//SpikeStream includes
#include "GlobalVariables.h"
#include "NetworkDao.h"
#include "SpikeStreamDBException.h"
#include "Util.h"
#include "XMLParameterParser.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;


/*! Constructor. DBInfo is stored by AbstractDao */
NetworkDao::NetworkDao(const DBInfo& dbInfo) : AbstractDao(dbInfo){
}


/*! Destructor */
NetworkDao::~NetworkDao(){
	closeDatabaseConnection();
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds the specified neural network to the database */
void NetworkDao::addNetwork(NetworkInfo& netInfo){
	QSqlQuery query = getQuery("INSERT INTO Networks (Name, Description) VALUES ('" + netInfo.getName() + "', '" + netInfo.getDescription() + "')");
	executeQuery(query);

	//Check id is correct and add to network info if it is
	int lastInsertID = query.lastInsertId().toInt();
	if(lastInsertID >= START_NEURALNETWORK_ID)
		netInfo.setID(lastInsertID);
	else
		throw SpikeStreamDBException("Insert ID for Network is invalid: " + QString::number(lastInsertID));
}


/*! Adds a weightless connection pattern index to the database */
void NetworkDao::addWeightlessConnection(unsigned int connectionID, unsigned int patternIndex){
	QSqlQuery query = getQuery("INSERT INTO WeightlessConnections (ConnectionID, PatternIndex) VALUES (" + QString::number(connectionID) + ", " + QString::number(patternIndex) + ")");
	executeQuery(query);
}


/*! Adds a weightless neuron training pattern to the database and returns the new pattern id. */
unsigned int NetworkDao::addWeightlessNeuronTrainingPattern(unsigned int neuronID, const unsigned char* patternArray, bool output, unsigned int patternArraySize){
	//Create a byte array that does not copy data
	QByteArray tmpByteArray = QByteArray::fromRawData((char*) patternArray, patternArraySize);

	//Add data to database
	QSqlQuery query = getQuery("INSERT INTO WeightlessNeuronTrainingPatterns (NeuronID, Pattern, Output) VALUES (" + QString::number(neuronID) + ", :PATTERNARRAY, " + QString::number(output) + ")");
	query.bindValue(":PATTERNARRAY", tmpByteArray);
	executeQuery(query);

	//Check id is correct and return it if it is
	int lastInsertID = query.lastInsertId().toInt();
	if(lastInsertID >= START_WEIGHTLESS_TRAINING_PATTERN_ID)
		return lastInsertID;

	//Throw exception if the last insert id is not in the valid range
	throw SpikeStreamDBException("Insert ID for WeightlessNeuronTrainingPatterns is invalid: " + QString::number(lastInsertID));
}


/*! Returns the number of connections in a network */
unsigned int NetworkDao::getConnectionCount(unsigned int networkID){
	QSqlQuery query = getQuery("SELECT COUNT(*) FROM Connections WHERE ConnectionGroupID IN (SELECT ConnectionGroupID FROM ConnectionGroups WHERE NetworkID=" + QString::number(networkID) + ")");
	executeQuery(query);
	query.next();
	return Util::getUInt(query.value(0).toString());
}


/*! Returns the total number of connections in a single connection group */
unsigned int NetworkDao::getConnectionCount(ConnectionGroup* conGrp){
	//Build query
	QString queryStr = "SELECT COUNT(*) FROM Connections WHERE ConnectionGroupID=" + QString::number(conGrp->getID());

	//Execute query and return result
	QSqlQuery query = getQuery(queryStr);
	executeQuery(query);
	query.next();
	return Util::getUInt(query.value(0).toString());
}


/*! Returns the total number of connections in the list of connection groups */
unsigned int NetworkDao::getConnectionCount(const QList<ConnectionGroup*>& conGrpList){
	if(conGrpList.isEmpty())
		return 0;

	//Build query
	QString queryStr = "SELECT COUNT(*) FROM Connections WHERE 1=0";
	foreach(ConnectionGroup* conGrp, conGrpList){
		queryStr += " OR ConnectionGroupID=" + QString::number(conGrp->getID());
	}

	//Execute query and return result
	QSqlQuery query = getQuery(queryStr);
	executeQuery(query);
	query.next();
	return Util::getUInt(query.value(0).toString());
}


/*! Returns the total number of connections in the list of connection groups */
unsigned int NetworkDao::getConnectionCount(const QList<unsigned>& conGrpIDList){
	if(conGrpIDList.isEmpty())
		return 0;

	//Build query
	QString queryStr = "SELECT COUNT(*) FROM Connections WHERE 1=0";
	foreach(unsigned conGrpID, conGrpIDList){
		queryStr += " OR ConnectionGroupID=" + QString::number(conGrpID);
	}

	//Execute query and return result
	QSqlQuery query = getQuery(queryStr);
	executeQuery(query);
	query.next();
	return Util::getUInt(query.value(0).toString());
}


/*! Returns the total number of connections in the connection groups */
unsigned int NetworkDao::getConnectionCount(const ConnectionGroupInfo& conGrpInfo){
	//Build and execute query
	QString queryStr = "SELECT COUNT(*) FROM Connections WHERE ConnectionGroupID=" + QString::number(conGrpInfo.getID());
	QSqlQuery query = getQuery(queryStr);
	executeQuery(query);
	query.next();
	return Util::getUInt(query.value(0).toString());
}


/*! Returns all of the connections from the specified neuron to the specified neuron. */
QList< QPair<unsigned, Connection> > NetworkDao::getConnections(unsigned int fromNeuronID, unsigned int toNeuronID){
	QSqlQuery query = getQuery("SELECT ConnectionID, Delay, Weight FROM Connections WHERE FromNeuronID=" + QString::number(fromNeuronID) + " AND ToNeuronID="+ QString::number(toNeuronID));
	executeQuery(query);
	QList< QPair<unsigned, Connection> > conPairList;
	unsigned tmpID;
	while ( query.next() ) {
		tmpID = query.value(0).toUInt();
		Connection tmpCon(
				fromNeuronID,//FromNeuronID
				toNeuronID,//ToNeuronID
				query.value(1).toString().toFloat(),//Delay
				query.value(2).toString().toFloat()//Weight
		);
		conPairList.append(QPair<unsigned, Connection>(tmpID, tmpCon));
	}
	return conPairList;
}


/*! Fills supplied list with information about the connection groups associated with the specified network */
void NetworkDao::getConnectionGroupsInfo(unsigned int networkID, QList<ConnectionGroupInfo>& conGrpInfoList){
	conGrpInfoList.clear();
	QSqlQuery query = getQuery("SELECT ConnectionGroupID, Description, FromNeuronGroupID, ToNeuronGroupID, Parameters, SynapseTypeID FROM ConnectionGroups WHERE NetworkID=" + QString::number(networkID));
	executeQuery(query);
	XMLParameterParser parameterParser;
	for(int i=0; i<query.size(); ++i){
		query.next();
		conGrpInfoList.append(
				ConnectionGroupInfo(
						query.value(0).toUInt(),//Connection group id
						query.value(1).toString(),//Description
						query.value(2).toUInt(),//From group id
						query.value(3).toUInt(),//To group id
						parameterParser.getParameterMap(query.value(4).toString()),//Parameters
						getSynapseType(query.value(5).toUInt())//Synapse type
				)
		);
	}
}


/*! Fills the supplied nested hash map with all the connections made FROM a neuron to other neurons.
	The key is the FROM neuron id, the value is a second map whose key is the TO neuron id and whose
	value is a boolean always set to true. */
void NetworkDao::getAllFromConnections(unsigned int networkID, QHash<unsigned int, QHash<unsigned int, bool> >& connMap){
	//Reset the map
	connMap.clear();

	//Get a list of all the neuron ids in the network
	QList<unsigned int> neuronIDList = getNeuronIDs(networkID);

	//Work through all of the neurons in the network
	foreach(unsigned int neuronID, neuronIDList){
		//Add the connections FROM this neuron id TO other neurons
		QHash<unsigned int, bool> tmpFromConMap;
		QList<unsigned int> fromConList = getFromConnections(neuronID);
		foreach(unsigned int fromConNeurID, fromConList)
			tmpFromConMap[fromConNeurID] = true;
		connMap.insert(neuronID, tmpFromConMap);
	}
}


/*! Fills the supplied nested  hash map with all the connections made TO each neuron from other neurons.
	The key is the TO neuron id, the value is a second map whose key is the FROM neuron id and whose
	value is a boolean always set to true.  */
void NetworkDao::getAllToConnections(unsigned int networkID, QHash<unsigned int, QHash<unsigned int, bool> >& connMap){
	//Reset the map
	connMap.clear();

	//Get a list of all the neuron ids in the network
	QList<unsigned int> neuronIDList = getNeuronIDs(networkID);

	//Work through all of the neurons in th network
	foreach(unsigned int neuronID, neuronIDList){
		//Add the connections FROM this neuron id TO other neurons
		QHash<unsigned int, bool> tmpToConMap;
		QList<unsigned int> toConList = getToConnections(neuronID);
		foreach(unsigned int toConNeurID, toConList)
			tmpToConMap[toConNeurID] = true;
		connMap.insert(neuronID, tmpToConMap);
	}
}


/*! Returns a list of all the connections FROM the specified neuron */
QList<unsigned int> NetworkDao::getFromConnections(unsigned int fromNeuronID){
	QSqlQuery query = getQuery("SELECT ToNeuronID FROM Connections WHERE FromNeuronID=" + QString::number(fromNeuronID));
	executeQuery(query);

	//Add neuron ids to list
	QList<unsigned int> conList;
	while(query.next()){
		unsigned int neurID = Util::getUInt(query.value(0).toString());
		conList.append(neurID);
	}

	//Return list
	return conList;
}


/*! Returns a list of all the connections TO the specified neuron */
QList<unsigned int> NetworkDao::getToConnections(unsigned int toNeuronID){
	QSqlQuery query = getQuery("SELECT FromNeuronID FROM Connections WHERE ToNeuronID=" + QString::number(toNeuronID));
	executeQuery(query);

	//Add neuron ids to list
	QList<unsigned int> conList;
	while(query.next()){
		unsigned int neurID = Util::getUInt(query.value(0).toString());
		conList.append(neurID);
	}

	//Return list
	return conList;
}


/*! Returns a map containing the default neuron parameters for a neuron type */
QHash<QString, double> NetworkDao::getDefaultNeuronParameters(unsigned int neuronTypeID){
	NeuronType neuronType = getNeuronType(neuronTypeID);
	QHash<QString, double> paramMap;

	//Check for case where there are no parameters.
	if(neuronType.getParameterInfoList().isEmpty())
		return paramMap;

	//Extract default values from parameter table
	QSqlQuery query = getQuery("SHOW COLUMNS FROM " + neuronType.getParameterTableName());
	executeQuery(query);
	int defaultCol = query.record().indexOf("Default");
	int variableNameCol = query.record().indexOf("Field");
	while(query.next()){
		QString variableName = query.value(variableNameCol).toString();
		if(variableName != "NeuronGroupID"){
			paramMap[variableName] = Util::getDouble( query.value(defaultCol).toString() );
		}
	}

	//Check it matches the parameters stored in the neuron type
	foreach(ParameterInfo info, neuronType.getParameterInfoList()){
		if(!paramMap.contains(info.getName()))
			throw SpikeStreamException("Parameter missing: " + info.getName() + " for neuron type " + QString::number(neuronType.getID()));
	}

	//Return map
	return paramMap;
}


/*! Returns a map containing the default parameters for the specified synapse type */
QHash<QString, double> NetworkDao::getDefaultSynapseParameters(unsigned int synapseTypeID){
	SynapseType synapseType = getSynapseType(synapseTypeID);
	QHash<QString, double> paramMap;

	//Check for case where there are no parameters.
	if(synapseType.getParameterInfoList().isEmpty())
		return paramMap;

	//Extract default values from parameter table
	QSqlQuery query = getQuery("SHOW COLUMNS FROM " + synapseType.getParameterTableName());
	executeQuery(query);
	int defaultCol = query.record().indexOf("Default");
	int variableNameCol = query.record().indexOf("Field");
	while(query.next()){
		QString variableName = query.value(variableNameCol).toString();
		if(variableName != "ConnectionGroupID"){
			paramMap[variableName] = Util::getDouble( query.value(defaultCol).toString() );
		}
	}

	//Check it matches the parameters stored in the synapse type
	foreach(ParameterInfo info, synapseType.getParameterInfoList()){
		if(!paramMap.contains(info.getName()))
			throw SpikeStreamException("Parameter missing: " + info.getName() + " for synapse type " + QString::number(synapseType.getID()));
	}

	//Return map
	return paramMap;
}


/*! Returns the total number of neurons in the list of neuron groups */
unsigned int NetworkDao::getNeuronCount(const QList<NeuronGroup*>& neurGrpList){
	if(neurGrpList.isEmpty())
		return 0;

	//Build query
	QString queryStr = "SELECT COUNT(*) FROM Neurons WHERE 1=0";
	foreach(NeuronGroup* neurGrp, neurGrpList){
		queryStr += " OR NeuronGroupID=" + QString::number(neurGrp->getID());
	}

	//Execute query and return result
	QSqlQuery query = getQuery(queryStr);
	executeQuery(query);
	query.next();
	return Util::getUInt(query.value(0).toString());
}


/*! Returns the number of neurons in a network */
unsigned int NetworkDao::getNeuronCount(unsigned int networkID){
	QSqlQuery query = getQuery("SELECT COUNT(*) FROM Neurons WHERE NeuronGroupID IN (SELECT NeuronGroupID FROM NeuronGroups WHERE NetworkID=" + QString::number(networkID) + ")");
	executeQuery(query);
	query.next();
	return Util::getUInt(query.value(0).toString());
}


/*! Returns the number of neurons in a neuron group */
unsigned NetworkDao::getNeuronCount(const NeuronGroupInfo& neurGrpInfo){
	QSqlQuery query = getQuery("SELECT COUNT(*) FROM Neurons WHERE NeuronGroupID=" + QString::number(neurGrpInfo.getID()) );
	executeQuery(query);
	query.next();
	return Util::getUInt(query.value(0).toString());
}


/*! Returns the number of neurons within the specified box.
	Useful for determining overlap when adding neuron groups to networks */
unsigned int NetworkDao::getNeuronCount(unsigned int networkID, const Box& box){
	//Build query
	QString queryStr = "SELECT COUNT(*) FROM Neurons ";
	queryStr += " WHERE NeuronGroupID IN (SELECT NeuronGroupID FROM NeuronGroups WHERE NetworkID=" + QString::number(networkID) + ") AND ";
	queryStr += " X >= " + QString::number(box.x1) + " AND X <= " + QString::number(box.x2) + " AND ";
	queryStr += " Y >= " + QString::number(box.y1) + " AND Y <= " + QString::number(box.y2) + " AND ";
	queryStr += " Z >= " + QString::number(box.z1) + " AND Z <= " + QString::number(box.z2);

	//Execute query and return result
	QSqlQuery query = getQuery(queryStr);
	executeQuery(query);
	query.next();
	return(Util::getUInt(query.value(0).toString()));
}


/*! Returns a box enclosing a particular neuron group */
Box NetworkDao::getNeuronGroupBoundingBox(unsigned int neurGrpID){
	QSqlQuery query = getQuery("SELECT MIN(X), MIN(Y), MIN(Z), MAX(X), MAX(Y), MAX(Z) FROM Neurons WHERE NeuronGroupID = " + QString::number(neurGrpID));
	executeQuery(query);
	query.next();
	Box box(
			query.value(0).toInt(),//x1
			query.value(1).toInt(),//y1
			query.value(2).toInt(),//z1
			query.value(3).toInt(),//x2
			query.value(4).toInt(),//y2
			query.value(5).toInt()//z2
			);
	return box;
}


/*! Returns the number of connections in the specified connection group */
unsigned int NetworkDao::getConnectionGroupSize(unsigned int connGrpID){
	QSqlQuery query = getQuery("SELECT COUNT(*) FROM Connections WHERE ConnectionGroupID=" + QString::number(connGrpID));
	executeQuery(query);
	query.next();
	return query.value(0).toUInt();
}


/*! Returns a list of information about the available networks. */
QList<NetworkInfo> NetworkDao::getNetworksInfo(){
	QSqlQuery query = getQuery("SELECT NetworkID, Name, Description FROM Networks");
	executeQuery(query);
	QList<NetworkInfo> tmpList;
	for(int i=0; i<query.size(); ++i){
		query.next();
		tmpList.append( NetworkInfo(query.value(0).toUInt(), query.value(1).toString(), query.value(2).toString()));
	}
	return tmpList;
}


/*! Retrieve the entwork ID and NetworkInfo by a given name. 
 *	If no network is found by the given name, an empty Network Info is returned 
 */
bool NetworkDao::getNetworkInfo(const QString& name, NetworkInfo& info){

	QSqlQuery query = getQuery("SELECT NetworkID, Name, Description FROM Networks where name ='" + name+ "'");
	executeQuery(query);
	
	auto qs = query.size(); //!amount of found records
	if(qs<=0) 
		return false; 

	query.next(); 
	info.setID(query.value(0).toUInt());
	info.setName(query.value(1).toString());
	info.setDescription(query.value(2).toString());
	
	return true; 
}


/*! Returns information about the neuron groups associated with the specified network */
void NetworkDao::getNeuronGroupsInfo(unsigned int networkID, QList<NeuronGroupInfo>& neurGrpInfoList){
	QSqlQuery query = getQuery("SELECT NeuronGroupID, Name, Description, Parameters, NeuronTypeID FROM NeuronGroups WHERE NetworkID=" + QString::number(networkID));
	executeQuery(query);
	neurGrpInfoList.clear();
	XMLParameterParser parameterParser;
	for(int i=0; i<query.size(); ++i){
		query.next();
		neurGrpInfoList.append(
				NeuronGroupInfo(
						query.value(0).toUInt(),
						query.value(1).toString(),
						query.value(2).toString(),
						parameterParser.getParameterMap(query.value(3).toString()),
						getNeuronType(query.value(4).toUInt())
				)
		);
	}
}


/*! Returns a list of the neuron ids in the specified network */
QList<unsigned int> NetworkDao::getNeuronIDs(unsigned int networkID){
	QList<unsigned int> neurIDList;
#ifndef LN_FIX_PROTOTYPE_IDS
	QSqlQuery query = getQuery("SELECT NeuronID FROM Neurons WHERE NeuronGroupID IN (SELECT NeuronGroupID FROM NeuronGroups WHERE NetworkID=" + QString::number(networkID) + ")");
#else
	QSqlQuery query = getQuery("SELECT NeuronID FROM Neurons, NeuronGroups WHERE Neurons.NeuronGroupID = NeuronGroups.NeuronGroupID AND NeuronGroups.NetworkID=" + QString::number(networkID) + " ORDER BY NeuronId ASC");
#endif
	executeQuery(query);
	for(int i=0; i<query.size(); ++i){
		query.next();
		neurIDList.append(Util::getUInt(query.value(0).toString()));
	}
	return neurIDList;
}


/*! Returns a map containing the parameters for the specified neuron group */
QHash<QString, double> NetworkDao::getNeuronParameters(const NeuronGroupInfo& neurGrpInfo){
	/* Get class describing the type of neuron in this group - this contains the parameter
		table name and info about the parameters */
	NeuronType neuronType = getNeuronType(neurGrpInfo.getNeuronTypeID());

	//Return empty map if there are no parameters
	if(neuronType.getParameterInfoList().isEmpty())
		return QHash<QString, double>();

	//Build query to extract values of parameters for this neuron group
	QString queryStr = "SELECT ";
	QList<ParameterInfo> parameterInfoList = neuronType.getParameterInfoList();
	foreach(ParameterInfo paramInfo, parameterInfoList){
		queryStr += paramInfo.getName() + ",";
	}
	queryStr.truncate(queryStr.length() - 1);
	queryStr += " FROM " + neuronType.getParameterTableName() + " WHERE NeuronGroupID=" + QString::number(neurGrpInfo.getID());
	QSqlQuery query = getQuery(queryStr);
	executeQuery(query);

	//Check only one row exists
	if(query.size() != 1)
		throw SpikeStreamException("Zero or multiple parameter entries for neuron group in table " + neuronType.getParameterTableName() + ": " + QString::number(query.size()));

	//Extract parameters
	query.next();
	QHash<QString, double> paramMap;
	int indx = 0;
	foreach(ParameterInfo paramInfo, parameterInfoList){
		if(paramMap.contains(paramInfo.getName()))
			throw SpikeStreamException("Duplicate entries in parameter map!");

		paramMap[paramInfo.getName()] = Util::getDouble(query.value(indx).toString());
		++indx;
	}

	//Return the finished map
	return paramMap;
}


/*! Returns the neuron type for a specific neuron group */
NeuronType NetworkDao::getNeuronType(unsigned int neuronTypeID){
	//Resuse other method - not efficient, but more likely to be reliable than duplicating code.
	QList<NeuronType> neurTypesList = getNeuronTypes();
	foreach(NeuronType neurType, neurTypesList){
		if(neurType.getID() == neuronTypeID)
			return neurType;
	}
	throw SpikeStreamException("Neuron type with ID " + QString::number(neuronTypeID) + " not found.");
}


/*! Returns the neuron type for a specific neuron type description */
NeuronType NetworkDao::getNeuronType(const QString& neuronTypeDescription){
	//Resuse other method - not efficient, but more likely to be reliable than duplicating code.
	QList<NeuronType> neurTypesList = getNeuronTypes();
	foreach(NeuronType neurType, neurTypesList){
		if(neurType.getDescription().toUpper() == neuronTypeDescription.toUpper())
			return neurType;
	}
	throw SpikeStreamException("Neuron type with description " + neuronTypeDescription + " not found.");
}


/*! Returns the current list of available neuron types.
	This information is stored in the NeuronTypes table. */
QList<NeuronType> NetworkDao::getNeuronTypes(){
	QList<NeuronType> neuronTypesList;
	QSqlQuery query = getQuery("SELECT NeuronTypeID, Description, ParameterTableName, ClassLibrary FROM NeuronTypes ORDER BY NeuronTypeID");
	executeQuery(query);
	for(int i=0; i<query.size(); ++i){
		query.next();
		NeuronType tmpNeurType(
			Util::getUInt(query.value(0).toString()),//ID
			query.value(1).toString(),//Description
			query.value(2).toString(),//ParameterTableName
			query.value(3).toString()//ClassLibrary
		);
		neuronTypesList.append(tmpNeurType);
	}

	//Add the information about parameters for each neuron type
	for(int i=0; i<neuronTypesList.size(); ++i){
		QList<ParameterInfo> paramInfoList = getNeuronParameterInfo(neuronTypesList.at(i));
		neuronTypesList[i].setParameterInfoList(paramInfoList);
	}
	return neuronTypesList;
}


/*! Returns the first neuron ID in the group */
unsigned NetworkDao::getStartNeuronID(unsigned neuronGroupID){
	QSqlQuery query = getQuery("SELECT MIN(NeuronID) FROM Neurons WHERE NeuronGroupID=" + QString::number(neuronGroupID));
	executeQuery(query);
	query.next();
	return Util::getUInt(query.value(0).toString());
}


/*! Returns the synapse type for a specific connection group */
SynapseType NetworkDao::getSynapseType(unsigned int synapseTypeID){
	//Resuse other method - not efficient, but more likely to be reliable than duplicating code.
	QList<SynapseType> synTypesList = getSynapseTypes();
	foreach(SynapseType synType, synTypesList){
		if(synType.getID() == synapseTypeID)
			return synType;
	}
	throw SpikeStreamException("Synapse type with ID " + QString::number(synapseTypeID) + " not found.");
}


/*! Returns the synapse type for a specific description. */
SynapseType NetworkDao::getSynapseType(const QString& synapseTypeDescription){
	//Resuse other method - not efficient, but more likely to be reliable than duplicating code.
	QList<SynapseType> synTypesList = getSynapseTypes();
	foreach(SynapseType synType, synTypesList){
		if(synType.getDescription().toUpper() == synapseTypeDescription.toUpper())
			return synType;
	}
	throw SpikeStreamException("Synapse type with description " + synapseTypeDescription + " not found.");
}


/*! Returns a map containing the parameters for the specified connection group */
QHash<QString, double> NetworkDao::getSynapseParameters(const ConnectionGroupInfo& conGrpInfo){
	/* Get class describing the type of synapse in this group - this contains the parameter
		table name and info about the parameters */
	SynapseType synapseType = getSynapseType(conGrpInfo.getSynapseTypeID());

	//Return empty map if there are no parameters
	if(synapseType.getParameterInfoList().isEmpty())
		return QHash<QString, double>();

	//Build query to extract values of parameters for this connection group
	QString queryStr = "SELECT ";
	QList<ParameterInfo> parameterInfoList = synapseType.getParameterInfoList();
	foreach(ParameterInfo paramInfo, parameterInfoList){
		queryStr += paramInfo.getName() + ",";
	}
	queryStr.truncate(queryStr.length() - 1);
	queryStr += " FROM " + synapseType.getParameterTableName() + " WHERE ConnectionGroupID=" + QString::number(conGrpInfo.getID());
	QSqlQuery query = getQuery(queryStr);
	executeQuery(query);

	//Check only one row exists
	if(query.size() != 1)
		throw SpikeStreamException("Zero or multiple parameter entries for connection group in table " + synapseType.getParameterTableName() + ": " + QString::number(query.size()));

	//Extract parameters
	query.next();
	QHash<QString, double> paramMap;
	int indx = 0;
	foreach(ParameterInfo paramInfo, parameterInfoList){
		if(paramMap.contains(paramInfo.getName()))
			throw SpikeStreamException("Duplicate entries in parameter map!");

		paramMap[paramInfo.getName()] = Util::getDouble(query.value(indx).toString());
		++indx;
	}

	//Return the finished map
	return paramMap;
}


/*! Returns the current list of available synapse types.
	This information is stored in the SynapseTypes table. */
QList<SynapseType> NetworkDao::getSynapseTypes(){
	QList<SynapseType> synapseTypesList;
	QSqlQuery query = getQuery("SELECT SynapseTypeID, Description, ParameterTableName, ClassLibrary FROM SynapseTypes ORDER BY SynapseTypeID");
	executeQuery(query);
	for(int i=0; i<query.size(); ++i){
		query.next();
		SynapseType tmpSynType(
			Util::getUInt(query.value(0).toString()),//ID
			query.value(1).toString(),//Description
			query.value(2).toString(),//ParameterTableName
			query.value(3).toString()//ClassLibrary
		);
		synapseTypesList.append(tmpSynType);
	}

	//Add the information about parameters for each synapse type
	for(int i=0; i<synapseTypesList.size(); ++i){
		QList<ParameterInfo> paramInfoList = getSynapseParameterInfo(synapseTypesList.at(i));
		synapseTypesList[i].setParameterInfoList(paramInfoList);
	}

	return synapseTypesList;
}


/*! Returns the weightless neuron with the specified ID. */
WeightlessNeuron* NetworkDao::getWeightlessNeuron(unsigned int neuronID){
	//Query to select neurons connected to weightless neuron and the pattern index of each neuron
	QSqlQuery query = getQuery("SELECT cons.FromNeuronID, weiCons.PatternIndex FROM Connections cons INNER JOIN WeightlessConnections weiCons ON cons.ConnectionID=weiCons.ConnectionID WHERE cons.ToNeuronID=" + QString::number(neuronID));
	executeQuery(query);

	/* Store the pattern index of each neuron that connects to
	There may be several connections between neurons, so connection map contains a list of connections for each neuron
	instead of a single entry */
	QHash<unsigned int, QList<unsigned int> > tmpConMap;
	unsigned int tmpFromNeurID, tmpPatternIndex;
	for(int i=0; i<query.size(); ++i){
		query.next();
		tmpFromNeurID = Util::getUInt(query.value(0).toString());
		tmpPatternIndex = Util::getUInt(query.value(1).toString());
		tmpConMap[tmpFromNeurID].append(tmpPatternIndex);
	}

	//Create the weightless neuron
	WeightlessNeuron* tmpWeightlessNeuron = new WeightlessNeuron(tmpConMap, neuronID);

	//Query to get the training patterns
	query = getQuery("SELECT Pattern, Output FROM WeightlessNeuronTrainingPatterns WHERE NeuronID = " + QString::number(neuronID));
	executeQuery(query);

	//Add training patterns to neuron
	for(int i=0; i<query.size(); ++i){
		query.next();
		QByteArray tmpByteArray = query.value(0).toByteArray();
		tmpWeightlessNeuron->addTraining(tmpByteArray, Util::getUInt(query.value(1).toString()));
	}

	//Create and return the weigthless neuron
	return tmpWeightlessNeuron;
}


/*! Returns the neuron group containing the specified neuron */
unsigned int NetworkDao::getNeuronGroupID(unsigned int neuronID){
	QSqlQuery query = getQuery("SELECT NeuronGroupID FROM Neurons WHERE NeuronID = " + QString::number(neuronID));
	executeQuery(query);
	query.next();
	return Util::getUInt(query.value(0).toString());
}


/*! Returns true if the neuron is weightless.
	Uses the string "Weightless Neuron" rather than the ID, which may change. */
bool NetworkDao::isWeightlessNeuron(unsigned int neuronID){
	unsigned int tmpNeurGrpID = getNeuronGroupID(neuronID);

	//Query to select neuron type description
	QSqlQuery query = getQuery("SELECT neurType.Description FROM NeuronTypes neurType INNER JOIN NeuronGroups neurGrps ON neurType.NeuronTypeID=neurGrps.NeuronTypeID WHERE neurGrps.NeuronGroupID=" + QString::number(tmpNeurGrpID));
	executeQuery(query);

	//Check to see if it is weightless
	query.next();
	if(query.value(0).toString().toUpper() == "WEIGHTLESS NEURON")
		return true;
	return false;
}


/*! Returns true if all the neurons in the network are weightless.
	Uses the string "Weightless Neuron" rather than the ID, which may change. */
bool NetworkDao::isWeightlessNetwork(unsigned int networkID){

	//Query to select neuron type description
	QSqlQuery query = getQuery("SELECT neurType.Description FROM NeuronTypes neurType INNER JOIN NeuronGroups neurGrps ON neurType.NeuronTypeID=neurGrps.NeuronTypeID WHERE neurGrps.NetworkID=" + QString::number(networkID));
	executeQuery(query);

	//Check to see if it is weightless
	while(query.next()){
		if(query.value(0).toString().toUpper() != "WEIGHTLESS NEURON")
			return false;
	}
	return true;
}


/*! Sets the description of a connection group */
void NetworkDao::setConnectionGroupProperties(unsigned connectionGroupID, const QString& description){
	QString queryStr = "UPDATE ConnectionGroups SET Description='" + description + "' ";
	queryStr += "WHERE ConnectionGroupID=" + QString::number(connectionGroupID);
	QSqlQuery query = getQuery(queryStr);
	executeQuery(query);
	if(query.numRowsAffected() != 1)
		throw SpikeStreamDBException("Error in connection group update. Exactly one row should be affected: " + QString::number(query.numRowsAffected()) + ", ID: " + QString::number(connectionGroupID));
}


/*! Sets the name and description of the specified network.
	Throws an exception if no rows were changed. */
void NetworkDao::setNetworkProperties(unsigned networkID, const QString& name, const QString& description){
	QString queryStr = "UPDATE Networks SET Name='" + name + "', Description='" + description + "' ";
	queryStr += "WHERE NetworkID=" + QString::number(networkID);
	QSqlQuery query = getQuery(queryStr);
	executeQuery(query);
	if(query.numRowsAffected() != 1)
		throw SpikeStreamDBException("Error in network update. Exactly one row should be affected: " + QString::number(query.numRowsAffected()));
}


/*! Sets the name and description of a neuron group */
void NetworkDao::setNeuronGroupProperties(unsigned neuronGroupID, const QString& name, const QString& description){
	QString queryStr = "UPDATE NeuronGroups SET Name='" + name + "', Description='" + description + "' ";
	queryStr += "WHERE NeuronGroupID=" + QString::number(neuronGroupID);
	QSqlQuery query = getQuery(queryStr);
	executeQuery(query);
	if(query.numRowsAffected() != 1)
		throw SpikeStreamDBException("Error in neuron group update. Exactly one row should be affected: " + QString::number(query.numRowsAffected()) + ", ID: " + QString::number(neuronGroupID));
}


/*! Sets the neuron group's parameters */
void NetworkDao::setNeuronParameters(const NeuronGroupInfo& neurGrpInfo, QHash<QString, double>& paramMap){
	/* Get class describing the type of neuron in this group - this contains the parameter
		table name and info about the parameters */
	NeuronType neuronType = getNeuronType(neurGrpInfo.getNeuronTypeID());

	//Do nothing if there are no parameters
	if(neuronType.getParameterCount() == 0)
		return;

	//Build query to update parameters for this neuron group
	QString queryStr = "UPDATE " + neuronType.getParameterTableName() + " SET ";
	foreach(ParameterInfo paramInfo, neuronType.getParameterInfoList()){
		if(!paramMap.contains(paramInfo.getName()))
			throw SpikeStreamException("Parameters in neuron type and parameters in paramter map do not match.");

		queryStr += paramInfo.getName() + "=" + QString::number(paramMap[paramInfo.getName()]) + ",";
	}
	queryStr.truncate(queryStr.length() - 1);//Take off the last trailing comma
	queryStr += " WHERE NeuronGroupID=" + QString::number(neurGrpInfo.getID());
	executeQuery(queryStr);
}


/*! Sets the connection group's parameters */
void NetworkDao::setSynapseParameters(const ConnectionGroupInfo& conGrpInfo, QHash<QString, double>& paramMap){
	/* Get class describing the type of synapse in this group - this contains the parameter
		table name and info about the parameters */
	SynapseType synapseType = getSynapseType(conGrpInfo.getSynapseTypeID());

	//Do nothing if there are no parameters
	if(synapseType.getParameterCount() == 0)
		return;

	//Build query to update parameters for this neuron group
	QString queryStr = "UPDATE " + synapseType.getParameterTableName() + " SET ";
	foreach(ParameterInfo paramInfo, synapseType.getParameterInfoList()){
		if(!paramMap.contains(paramInfo.getName()))
			throw SpikeStreamException("Parameters in synapse type and parameters in paramter map do not match.");

		queryStr += paramInfo.getName() + "=" + QString::number(paramMap[paramInfo.getName()]) + ",";
	}
	queryStr.truncate(queryStr.length() - 1);//Take off the last trailing comma
	queryStr += " WHERE ConnectionGroupID=" + QString::number(conGrpInfo.getID());
	executeQuery(queryStr);
}


/*! Sets the weight between two neurons.  */
void NetworkDao::setWeight(unsigned connectionID, double newWeight){
	QSqlQuery query = getQuery("UPDATE Connections SET Weight=" + QString::number(newWeight) + " WHERE ConnectionID=" + QString::number(connectionID));
	executeQuery(query);
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Returns a list of ParameterInfos describing the parameters available for a
	particular neuron type. The description of the parameters is stored as comments in the table. */
QList<ParameterInfo> NetworkDao::getNeuronParameterInfo(const NeuronType& neuronType){
	QList<ParameterInfo> paramInfoList;

	//Extract variable names and descriptions (as comments) from parameter table
	QSqlQuery query = getQuery("SHOW FULL COLUMNS FROM " + neuronType.getParameterTableName());
	executeQuery(query);
	int variableNameCol = query.record().indexOf("Field");
	int commentCol = query.record().indexOf("Comment");
	int typeCol = query.record().indexOf("Type");
	while(query.next()){
		QString variableName = query.value(variableNameCol).toString();
		if(variableName != "NeuronGroupID"){
			if(query.value(typeCol).toString() == "tinyint(1)")
				paramInfoList.append(ParameterInfo(variableName, query.value(commentCol).toString(), ParameterInfo::BOOLEAN));
			else
				paramInfoList.append(ParameterInfo(variableName, query.value(commentCol).toString(), ParameterInfo::DOUBLE));
		}
	}
	return paramInfoList;
}


/*! Returns a list of ParameterInfos describing the parameters available for a
	particular synapse type. The description of the parameters is stored as comments in the table. */
QList<ParameterInfo> NetworkDao::getSynapseParameterInfo(const SynapseType& synapseType){
	QList<ParameterInfo> paramInfoList;

	//Extract variable names and descriptions (as comments) from parameter table
	QSqlQuery query = getQuery("SHOW FULL COLUMNS FROM " + synapseType.getParameterTableName());
	executeQuery(query);
	int variableNameCol = query.record().indexOf("Field");
	int commentCol = query.record().indexOf("Comment");
	int typeCol = query.record().indexOf("Type");
	while(query.next()){
		QString variableName = query.value(variableNameCol).toString();
		if(variableName != "ConnectionGroupID"){
			if(query.value(typeCol).toString() == "tinyint(1)")
				paramInfoList.append(ParameterInfo(variableName, query.value(commentCol).toString(), ParameterInfo::BOOLEAN));
			else
				paramInfoList.append(ParameterInfo(variableName, query.value(commentCol).toString(), ParameterInfo::DOUBLE));
		}
	}
	return paramInfoList;
}

