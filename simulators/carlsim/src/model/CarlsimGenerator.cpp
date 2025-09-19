
#include "CarlsimGenerator.h"

#include "CarlsimWrapper.h"
#include "ConnectionGroup.h"

#include "carlsim.h"

#include <bitset>
#include <vector>
#include <cassert>
#include <tuple>

//#define DEBUG_CONGRP

CarlsimGenerator::CarlsimGenerator(ConnectionGroup* group): 
	group(group), weightFactor(1.0f) {		
}

CarlsimGenerator::~CarlsimGenerator() {
	wrapper = NULL; 
	group = NULL; 
}

void CarlsimGenerator::setWrapper(spikestream::CarlsimWrapper* wrapper_) {
	wrapper = wrapper_;

	NeuronGroup* from = wrapper->persistentNeurGrpMap[group->getFromNeuronGroupID()]; 
	NeuronGroup* to = wrapper->persistentNeurGrpMap[group->getToNeuronGroupID()]; 

#ifdef DEBUG_CONGRP
	printf("Connection Group: %s (%u)\n", 
		group->getInfo().getDescription().toStdString().c_str(), group->getID()); 
	printf("Pre-Neuron-Grp: %s (%u),  Post-Neuron-Grp: %s (%u)\n", 
		from->getInfo().getName().toStdString().c_str(), from->getID(), 
		to->getInfo().getName().toStdString().c_str(), to->getID()); 
#endif
	nfrom = from->size();
	nto = to->size();
	connections.resize(nfrom*nto); 

	auto fromStart = from->getStartNeuronID(); 
	auto toStart = to->getStartNeuronID();

	for(auto iter=group->begin(); iter<group->end(); iter++) {			
		unsigned pre = iter->getFromNeuronID()-fromStart; 
		unsigned post = iter->getToNeuronID()-toStart; 
		float w = iter->getWeight();  
		float d = iter->getDelay(); 
#ifdef DEBUG_CONGRP
		printf("pre[%u](%u) --> post[%u](%u)  w:%f  u:%f \n", 
			pre, pre+fromStart, post, post+toStart, w, d); 
#endif
		connections[idx(pre,post)] = true; 
		synapses[QPair<unsigned,unsigned>(pre,post)] = QPair<float,float>(w,d);

	}
}

int CarlsimGenerator::idx(unsigned pre, unsigned post) {

	assert(pre >= 0 && pre < nfrom);
	assert(post >= 0 && post < nto);
	return pre*nto +post; 
}

bool CarlsimGenerator::isConnected(unsigned pre, unsigned post) {
	return connections[idx(pre,post)]; 
}




void CarlsimGenerator::connect(CARLsim* sim, int srcGrp, int i, int destGrp, 
			   int j, float& weight, float& maxWt,
			   float& delay, bool& connected)
{
	connected = isConnected(i, j);

	if(connected) {
		auto synapse = synapses[QPair<unsigned,unsigned>(i,j)]; 
		weight = synapse.first * weightFactor; 
		delay = synapse.second;
#ifdef DEBUG_CONGRP
		printf("srcGrp[%d].pre[%d] --> destGrp[%d].post[%d] w:%f  d:%f \n", 
			srcGrp, i, destGrp, j, weight, delay); 
#endif
	} else {
		weight = .0f;
		delay = 0.f;
	}

	maxWt = 4.f * weightFactor; // get from Connection Group parameter and store in class
}


// header from CARLsim 

void CarlsimGenerator::writeTo(QString path, unsigned connId, unsigned gIdPre, unsigned gIdPost, bool learning) {

	// path: Directory 
	// filename is derived by pre/post group id
	// .dat as it is binary
	// maybe text: json, xaml for debugging and export


	char filename[256];
	sprintf(filename, "%sconngrpgen_%d_%d_%d.dat", path.toStdString().c_str(), connId, gIdPre, gIdPost);     // connection group generator
	FILE* file = fopen(filename, "wb"); // binary write

	// header -> struct  -> sizeof
	// count connections as check-sum
	size_t content = synapses.size(); 
	size_t connected = 0; 
	for (unsigned pre = 0; pre < nfrom; pre++)
		for (unsigned post = 0; post < nto; post++) {
			connected += isConnected(pre, post) ? 1 : 0;
			// reserved: checksum (order dependent)
		}
	assert(connected == content);
	// write 
	// header_version (unsigned)
	// header_size (unsigned)
	// pre_group_gid(unsigned), 
	// post_group_gid (unsigned), 
	// connected(unsigned), 
	// content(unsigned)
	// weight_factor (float32)
	// nTo (unsigned)
	// nFrom (unsigned)
	// total size (u64)
	// checksum (u64)

	// -> CARLsim  tool.  loader, 
	struct conn_gen_header_t {
		unsigned version; // 01.00.000   Major, Minor, Patch
		size_t header_size;
		size_t payload_size;
		int gConnId;
		int gIdPre; 
		int gIdPost;
		unsigned connected;
		unsigned content; // records
		double weight_factor;
		unsigned nfrom;
		unsigned nto;
		unsigned long long checksum; // reserved
	};

	conn_gen_header_t conn_gen_header;

	conn_gen_header.version = 100000;
	conn_gen_header.header_size = sizeof(conn_gen_header_t);
	conn_gen_header.payload_size = (sizeof(unsigned)*2+ sizeof(float)*2) * content;
	conn_gen_header.gConnId = connId;
	conn_gen_header.gIdPre = gIdPre;
	conn_gen_header.gIdPost = gIdPost;
	conn_gen_header.connected = connected;
	conn_gen_header.content = content;
	conn_gen_header.weight_factor = weightFactor;
	conn_gen_header.nfrom = nfrom;
	conn_gen_header.nto = nto;
	conn_gen_header.checksum = 0; // reserved

	assert(fwrite(&conn_gen_header, sizeof(conn_gen_header_t), 1, file));

	// payload
	unsigned n = 0;
	for (auto iter = synapses.begin(); iter != synapses.end(); iter++) {
		
		auto &k = iter.key(); //QPair<unsigned, unsigned>(i,j) 
		auto &v = iter.value(); // QPair<float,float>(w,d);

		// file stream write 
		// unsigned, unsigned, float32, float32 
		// pre, post, w, d 

		assert(fwrite(&k, sizeof(unsigned), 2, file));
		assert(fwrite(&v, sizeof(float), 2, file));

		n++;
	}
	assert(n == connected);

	fclose(file);
}