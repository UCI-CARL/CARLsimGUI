
#include "WmExpCarlsimWrapper.h"

#include "CarlsimLib.h"
#include "Globals.h"

//#include "CarlsimOatSpikeMonitor.h"

//#include <stdio.h>
#include <algorithm>		// std::find
//#include <assert.h>			// assert


//#include <carlsim.h>   // ambigous Point3D due linear algebrar.h --> fix this in Cs6.1 as there is no real funct
#include <spike_monitor.h>


WmExpCarlsimWrapper::WmExpCarlsimWrapper(CarlsimWrapper* wrapper, carlsim_monitors::OatSpikeMonitor* monitor) :
	wrapper(wrapper), monitor(monitor) {

	//auto neuronGroups = Globals::getNetwork()->getNeuronGroupsMap();
	//auto neuronGroup = neuronGroups["CA1 Place Cells"];
	//unsigned gGrpId = neuronGroup->getVID();
	//grpSize = neuronGroup->size();

	//connect(wrapper, SIGNAL(carlsimSetupState()), this, assignOatMonitor(), Qt::DirectConnection);

	//Delegate search of OAT SpikeMonitor out of the widgets to the wrapper
//monitor = wrapper->getOatMonitor("Spike-Monitor", "CA1 Place Cells");
//OatSpikeMonitor* oatMonitor = nullptr; // FIXME
//NeuronGroup* group;  // must be set explictly 
//SpikeMonitor* monitor
//monitor = wrapper->carlsim->getSpikeMonitor(group->getVID(), path.toStdString());

}

WmExpCarlsimWrapper::~WmExpCarlsimWrapper() {
	// reset pointers, but do not fee them 
	//wrapper = nullptr; 
	//maze = nullptr; 
}

void WmExpCarlsimWrapper::setMonitor(carlsim_monitors::OatSpikeMonitor* monitor) {

	// assert type == "Spike-Monitor"   tartet== "CA1 Place Cells"
	this->monitor = monitor;

	// ISSUE: which one to store?  check status before usage -> oatMonitor
	 
	//SpikeMonitor* spkMon = oatMonitor->getSpikeMonitor(); 

}



// FIXME 4x4
void WmExpCarlsimWrapper::printSpikes(int offset) {

	SpikeMonitor* spkMon = monitor->monitor; 

	//Do not print empty spike tables
	if (spkMon->getPopNumSpikes() < 1)
		return;

	spikes_t spikes = spkMon->getSpikeVector2D();

	int n = spikes.size(); // neuron ids
	for (int i = 0; i < n; i++) {
		printf("%2d: ", i);
		auto aer = spikes[i]; 
		int m = aer.size(); 
		for (int j = 0; j < m; j++) {
			int t = aer[j];
			printf("%6d", t);
		}
		printf("\n");
	}
	printf("\n");


	////printf(" |  1  2  3  4  |\n");
	//printf("  |");
	//for (int x = 1; x <= maze->columns; x++)
	//	printf(" %6d", x);
	//printf("  |\n");

	////printf("-+--------------+\n");
	//printf("--+");
	//for (int x = 1; x <= maze->columns; x++)
	//	printf("------", x);
	//printf("--+\n");

	//for (int y = 1; y <= maze->rows; y++) {
	//	printf("%2d|", y);
	//	for (int x = 1; x <= maze->columns; x++) {
	//		int i = maze->indexXY(x, y);
	//		int t = -1;
	//		if (!spikes[i].empty()) {
	//			t = spikes[i][0];
	//			printf("%6d", t);
	//		}
	//		else {
	//			printf("   ");
	//		}
	//	}
	//	printf("  |\n");
	//}
	////printf("-+--------------+\n");
	//printf("--+");
	//for (int x = 1; x <= maze->columns; x++)
	//	printf("------", x);
	//printf("--+\n");

};

//
//bool WmExpCarlsimWrapper::getSpike(int r, int c, int &t) {
//	//SpikeMonitor* spkMon = nullptr; // monitor->getSpikeMonitor(); // todo  monitor->getSpikeMonitor();
//	SpikeMonitor* spkMon = monitor->monitor;
//	spikes_t &spikes = spkMon->getSpikeVector2D();
//	//int i = maze->index(r, c);
//	//if(spikes[i].empty())
//	//	return false;
//	//t = spikes[i][0];
//	return true;
//};


//Check if target neuron has fired at current time
bool WmExpCarlsimWrapper::checkTargetFired (int targetNId, unsigned int currentTime) {
	SpikeMonitor* spkMon = monitor->monitor;
	spikes_t& spikes  = spkMon->getSpikeVector2D();
	if (!spikes[targetNId].empty()) {
		auto& firings = spikes[targetNId];
		auto fired = firings.back();
		return currentTime - fired <= 1;  // SpikeMon has 1ms latency
	}
	return false;
};


//Check if target neuron has fired at current time
bool WmExpCarlsimWrapper::checkNeuronFiring (int nId, double f_expected, double eps, int ms, int spikes_min) {
	SpikeMonitor* spkMon = monitor->monitor;
	spikes_t& spikes = spkMon->getSpikeVector2D();
	if (!spikes[nId].empty()) {
		auto& firings = spikes[nId];
		int n_spikes = firings.size();
		if (n_spikes >= spikes_min) {
			double f = ((double) n_spikes) / ms * 1000.0;
			double err = std::abs(f - f_expected)/f_expected;
			auto ok = err < eps;
			if (!ok)
				printf("checkNeuronFiring nok:  nId:%d f:%f (f_expected:%f),  err: %f (eps %f),  %d ms, n_spikes: %d (spikes_min: %d)\n",
					nId, f, f_expected, err, eps, ms, n_spikes, spikes_min);
			return ok;
		}
		else {
			printf("checkNeuronFiring nok: nId:%d, %d ms, n_spikes: %d (spikes_min: %d)\n", 
				nId, ms, n_spikes, spikes_min);
		}
	}
	return false; 
};

//bool WmExpCarlsimWrapper::findWaveFront(int startNId, int endNId) {
//
//
//	//eligibility.resize(grpSize, 0.0f);  // clear(0.0f)
//	eligibility.reserve(grpSize);
//	eligibility.clear();
//	eligibility.resize(grpSize, 0.0f);
//
//	path.resize(0); // reset path
//
//
//	//FIXME grpIds are global, see getDelays,  netId must not be passed but derived
//	//FIXME path must be restricted to neurons of the group only, 
//	//		elsewise it got lost in the interneurons...
//	//FIXME pass logging level for better debugging (V2: use central log manager ala Log4J)
//
//	wrapper->carlsim->findWavefrontPath(path, eligibility, 8, grpId, startNId, endNId);
//
//	if (grpSize < 100) {
//		printf("\neligibility\n");
//		for (int nId = 0; nId < grpSize; nId++) {  // FIXME
//			printf("%d %.3f\n", nId, eligibility[nId]);
//		}
//	}
//
//	printf("\npath\n");
//	for (int i = 0; i < path.size(); i++) {
//		printf("path[%d] = %d\n", i, path[i]);
//	}
//
//	int length = path.size(); 
//
//	// 0, 15 & grpSize==16 => path length = 7, however, start/end are arbitrary
//	printf("found path of length %d\n", length);
//
//	return length > 1;  
//}
//
//void WmExpCarlsimWrapper::writePathString(std::string & pathString) {
//
//	//std::ostringstream string_stream(pathString); //FIXME pathString is not written
//	std::ostringstream string_stream;
//
//	for (auto iter = path.rbegin(); iter < path.rend(); iter++) {
//		if (iter != path.rbegin())
//			string_stream << ",";
//		string_stream << *iter;
//	}
//	//string_stream.flush();
//	pathString = string_stream.str();
//}
//
//
//void WmExpCarlsimWrapper::writePath(std::string filename) {
//	
//	const int buffer_len = 100;
//	char buffer[buffer_len];
//	std::FILE* file = std::fopen(filename.c_str(), "w");
//
//	//char* sep = "";
//	for (auto iter = path.rbegin(); iter < path.rend(); iter++) {		
//		//if (iter != path.rbegin())
//		//	sep = " ";
//		sprintf_s(buffer, buffer_len, "%d ", *iter);
//		fputs(buffer, file);
//
//	}
//
//	std::fclose(file);
//}
//
//
//void WmExpCarlsimWrapper::writePath(std::unique_ptr<ResultsCursor> &fileResults) {
//	std::string path;  // instanciate local variable
//	fileResults->getFilePath(path);
//	writePath(path);
//}
//
//
//
//
//void WmExpCarlsimWrapper::writeEligibility(std::string filename) {
//	const int buffer_len = 100;
//	char buffer[buffer_len];
//	std::FILE* file = std::fopen(filename.c_str(), "w");
//	for (int nId = 0; nId < maze->n; nId++) {
//		sprintf_s(buffer, buffer_len, "%d %.3f\n", nId, eligibility[nId]);  
//		fputs(buffer, file);
//	}
//	std::fclose(file);
//}
//
//
//void WmExpCarlsimWrapper::writeEligibility(std::unique_ptr<ResultsCursor> &fileResults) {
//	std::string path;  // instanciate local variable
//	fileResults->getFilePath(path);
//	writeEligibility(path);
//}
//
//
//void WmExpCarlsimWrapper::writeCosts(std::string filename) 
//{
//	const int buffer_len = 100;
//	char buffer[buffer_len];
//	std::FILE* file = std::fopen(filename.c_str(), "w");
//	for (int row = 0; row < maze->rows; row++)
//		for (int column = 0; column < maze->columns; column++) {
//			int nId = maze->index(row+1, column+1); // FIXME cleanup Indizes (Matlab 1based, array 0)
//			int cost = maze->maze[row][column];
//			sprintf_s(buffer, buffer_len, "%d %d\n", nId, cost);
//			fputs(buffer, file);
//		}
//	std::fclose(file);
//}
//
//void WmExpCarlsimWrapper::writeCosts(std::unique_ptr<ResultsCursor> &fileResults) {
//	std::string path;  // instanciate local variable
//	fileResults->getFilePath(path);
//	writeCosts(path);
//}
//
//
//
//void WmExpCarlsimWrapper::getDelays() {
//
//	// fix 2022-11-25
//	connDelays.clear();
//
//	int gGrpId = grpId; // FIXME local / global
//	int gStartN = wrapper->carlsim->getGroupStartNeuronId(gGrpId);
//	int gEndN = wrapper->carlsim->getGroupEndNeuronId(gGrpId);
//	//int start = grp->getStartNeuronID();
//	//int offset = 0;
//
//	int numPreN, numPostN;
//	uint8_t* delays = wrapper->carlsim->getDelays(gGrpId, gGrpId, numPreN, numPostN);
//
//	//FIXME eliminate special case check 		
//	assert(numPreN == grpSize);
//	assert(numPostN == grpSize);
//	// INV count numbers != 0 expected: 48
//	int m = 0;
//	uint8_t* p = delays;
//	for (int n = 0; n < numPreN * numPostN; n++)
//		if (*p++ > 0)
//			m++;
//	// generic rectangular place cells (column wise + row wize in both directions):
//	// e.g. special case for 4x4: (4 * 3 + 3 * 4 ) * 2 = 48 
//	int regularPlaceCellConnections = 2 * (sqrt(numPreN) * (sqrt(numPostN) - 1) + (sqrt(numPreN) - 1) * (sqrt(numPostN)));
//	if (m != regularPlaceCellConnections)
//		printf("WARNING inconsistent delays returned %d instead of %d\n", m, regularPlaceCellConnections);
//	// FIXME: eliminate special case check
//	//assert(m == 48);
//
//	//Print diagonal matrix for small networks
//	if (numPostN < 50) {
//		printf("  \\ post\n");
//		printf("pre");
//		for (int j = 0; j < numPostN; j++)
//			printf(" %02d", j);
//		printf("\n");
//
//		// 2 x 12 of 16x(16-1)
//		for (int i = 0; i < numPreN; i++) {
//			printf(" %02d", i);
//			for (int j = 0; j < numPostN; j++) {
//				int d = delays[j * numPostN + i];
//				//int d = delays[j + i * numPreN];
//				if (d > 0) {
//					// TODO if DEBUG_AXONPLAST
//					printf(" %2d", d);
//				}
//				else
//					printf("   ");
//			}
//			printf("\n");
//		}
//	}
//
//	//Initialize connection delays with the loaded
//	for (int i = 0; i < numPreN; i++) {
//		for (int j = 0; j < numPostN; j++) {
//			int d = delays[j * numPostN + i];
//			int row_i = maze->row(i);
//			int column_i = maze->column(i);
//			int row_j = maze->row(j);
//			int column_j = maze->column(j);
//			if (d > 0) {
//				if (numPostN < 50)
//					printf("[%d](%d,%d)->[%d](%d,%d): %d\n", i, row_i, column_i, j, row_j, column_j, d);
//				connDelays.push_back(std::tuple<int, int, uint8_t>(i, j, d));
//			}
//		}
//	}
//
//
//
//	////TODO 
//	//for (ConnectionIterator iter = cGrp->begin(); iter != cGrp->end(); iter++) {
//
//	//	unsigned preSynN = iter->getFromNeuronID() - start;
//	//	unsigned postSynN = iter->getToNeuronID() - start;
//
//	//	float d_old = iter->getTempDelay();
//	//	int d_index = preSynN + numPostN * postSynN;
//	//	float d_new = delays[d_index];
//
//	//	printf(" %02d -> %02d = %f (%f)\n", preSynN, postSynN, d_new, d_old);
//
//	//	//assert(d_index >= 0 && d_index < numPreN);
//
//	//	if (d_new != 0) {
//	//		//d_new = d_new > 1 ? d_new - 1 : d_old;  // demo
//	//		iter->setTempDelay(d_new);  // does not work, seems to be a copy
//	//	}
//	//	else {
//	//		printf("Warning: CARLsim returned a zero delay for pre %d and post %d\n", preSynN, postSynN);
//	//	}
//
//
//	//}
//
//	printf("\n");
//	delete delays; //FIXME rework legacy CARLsim API
//
//
//}
//
//
////void WmExpCarlsimWrapper::setBarrier(int barrier, int border) {
////
////	auto r_barrier = maze->row0(barrier);
////	auto c_barrier = maze->column0(barrier);
////	maze->maze[r_barrier][c_barrier] = border;
////
////	// N E S W  -> caution N is here from higher rows to lower, 
////	maze->map.emplace(std::tuple<int, int>(barrier + maze->columns, barrier), border); // N
////	maze->map.emplace(std::tuple<int, int>(barrier, barrier + maze->columns), border); // E
////	maze->map.emplace(std::tuple<int, int>(barrier - maze->columns, barrier), border); // S
////	maze->map.emplace(std::tuple<int, int>(barrier, barrier - maze->columns), border); // W
////}
//
//
//void WmExpCarlsimWrapper::eprop() {
//
//	// the whole matrix is updated 
//	// 
//	//// initialize valid steps that made up the path
//	//QSet<std::tuple<int, int>> steps; 
//	//for (int i = 1; i < path.size(); i++) {
//	//	int pre = path[i]; 
//	//	int post = path[i - 1]; 
//	//	steps.insert(std::tuple<int, int>(pre, post));
//	//	printf("adding ");
//	//}
//
//	loss = 0;
//	activity = 0;
//	connDelays2.clear();
//
//	//sim.updateDelays(8, CA3_Pryramidal, eligibility, map);
//	float delta = 0.5f;  // FIXME use parameter
//	for (auto iter = connDelays.begin(); iter != connDelays.end(); iter++) {
//		int pre = std::get<0>(*iter);
//		int post = std::get<1>(*iter);
//		uint8_t d = std::get<2>(*iter);
//		float e_i = eligibility[post];
//		//try {
//			auto found = maze->map.find(std::tuple<int, int>(pre, post));
//			if (found != maze->map.end()) {
//				//uint8_t cost = maze->map.at(std::tuple<int, int>(pre, post));
//				uint8_t cost = found->second; // value
////// patch
////if (pre == 58+13 && post == 58)  // N
////cost = 120;
//
//				if (e_i > .0f) {
//					uint8_t D = d + delta * e_i * (cost - d); // best convergence
//					//uint8_t D = d + std::round(delta * e_i * (cost - d));  // see cladde 05.03.2022
//					//uint8_t D = d + std::round(delta * (e_i * (cost - d)));  // see cladde 05.03.2022
//
//					//Mark weights projecting from along the path
//					bool alongPathPre = find(begin(path), end(path), pre) != end(path);
//					bool alongPathPost = find(begin(path), end(path), post) != end(path);
//					//*iter = std::tuple<int, int, uint8_t>(pre, post, D);  // update delay matrix or by get delays?
//					//Patch 2022-11-04 Update the error only along the path; not the whole network
//					if (alongPathPre || alongPathPost) {
//
//						// adjust values 
//						if (D > 20) { // D might be raisen be border 120 
//							printf("adjust E-Prop D overflow %d --> 20\n", D);
//							D = 20; // cap to DELAY_MAX
//						}
//						else if (D < 1) {
//							printf("adjust E-Prop D underflow %d --> 1\n", D); 
//							D = 1;  // 
//						}
//
//						printf("pre:%2d  post:%2d  e[%2d]:%.3f map:%2d d:%2d --> D:%2d %s\n", pre, post, post, e_i, (int)cost, (int)d, (int)D, alongPathPre ? "(pre)" : "(post)");
//						loss += std::abs(d - std::min<int>(cost, 20));
//						// push only weights along the path for update
//						connDelays2.push_back(std::tuple<int, int, uint8_t>(pre, post, D));
//					}
//
//					// eligibility greater 0 is sufficient for neural activity 
//					// (regardless if it is along the path)
//					activity++; 
//				}
//				//if (e_i > 0.01f && cost != d) // count on path  
//				//	errors += std::abs(d - cost);
//			}
//		//}
//		//catch (...) {
//		//	printf("map.at throw an exception for (%d, %d)\n", pre, post);
//		//}
//
//	}
//
//	iteration++;
//
//	printf("training results of iteration %d: loss=%d, activity=%d ( sum|D-map| )  \n\n", iteration, loss, activity);
//
//	//print_D();
//}
//
//
//void WmExpCarlsimWrapper::updateDelays() {
//
//	//Maze::ConnDelays_t connDelays2;
//
//	//connDelays2.push_back(std::tuple<int, int, uint8_t>(0, 1, 1));  
//	//connDelays2.push_back(std::tuple<int, int, uint8_t>(1, 2, 2));
//	//connDelays2.push_back(std::tuple<int, int, uint8_t>(2, 3, 3)); // FIXME  1
//
//	//int limit = 999;
//
//	////Apply E-Prop to weights projecting from along the path
//	//for (auto iter = connDelays.begin(); iter != connDelays.end(); iter++) {
//	//	int pre = std::get<0>(*iter);
//	//	auto result = find(begin(path), end(path), pre);  // FIXME: initialize 2 boolean matrix from path:  pre, post
//	//	if (result !=end(path)) {
//	//		int post = std::get<1>(*iter);
//	//		uint8_t d = std::get<2>(*iter);
//	//		printf("update delays:  pre: %2d  post: %2d   --> d: %2d\n", pre, post, (int) d);
//	//		connDelays2.push_back(tuple<int, int, uint8_t>(pre, post, d));
//
//	//		if (limit-- <= 0) {
//	//			//FIXME
//	//			//WARNING: skipping setDelay (offset != n) : pre = 15, post = 11 delay = 3
//	//			//[ERROR C : \cockroach - ut3\src\CARLsim6\carlsim\kernel\src\snn_cpu_module.cpp:1255] Post - syn
//	//			break;
//	//		}
//	//			
//	//	}
//	//}
//
//	// connDelays2 is filled in eprop
//	// see ::eprop
//	
//	wrapper->carlsim->updateDelays(grpId, grpId, connDelays2);
//
//
//	//wrapper->carlsim->updateDelays(grpId, grpId, connDelays);
//
///*
//* 
//* eligibility
//0 0.001
//1 0.002
//2 0.009
//3 0.035
//4 0.002
//5 0.007
//6 0.044
//7 0.134
//8 0.009
//9 0.044
//10 0.107
//11 0.410
//12 0.035
//13 0.134
//14 0.410
//15 1.000
//
//path
//path[0] = 15
//path[1] = 11
//path[2] = 10
//path[3] = 6
//path[4] = 5
//path[5] = 1
//path[6] = 0
//found path of length 7
//pre:0 post:1  e[1]:0.002 map:1 d:4 --> D:3
//pre:0 post:4  e[4]:0.002 map:1 d:4 --> D:3
//pre:1 post:0  e[0]:0.001 map:1 d:4 --> D:3
//pre:1 post:2  e[2]:0.009 map:1 d:4 --> D:3
//pre:1 post:5  e[5]:0.007 map:1 d:4 --> D:3
//pre:2 post:1  e[1]:0.002 map:1 d:4 --> D:3
//pre:2 post:3  e[3]:0.035 map:1 d:4 --> D:3 *
//pre:2 post:6  e[6]:0.044 map:1 d:4 --> D:3 *
//pre:3 post:2  e[2]:0.009 map:1 d:4 --> D:3
//pre:3 post:7  e[7]:0.134 map:1 d:4 --> D:3 *
//pre:4 post:0  e[0]:0.001 map:1 d:4 --> D:3
//pre:4 post:5  e[5]:0.007 map:1 d:4 --> D:3
//pre:4 post:8  e[8]:0.009 map:1 d:4 --> D:3
//pre:5 post:1  e[1]:0.002 map:2 d:4 --> D:3
//pre:5 post:4  e[4]:0.002 map:2 d:4 --> D:3
//pre:5 post:6  e[6]:0.044 map:2 d:4 --> D:3 *
//pre:5 post:9  e[9]:0.044 map:2 d:4 --> D:3 *
//pre:6 post:2  e[2]:0.009 map:2 d:4 --> D:3
//pre:6 post:5  e[5]:0.007 map:2 d:4 --> D:3
//pre:6 post:7  e[7]:0.134 map:2 d:4 --> D:3 *
//pre:6 post:10  e[10]:0.107 map:2 d:4 --> D:3 *
//pre:7 post:3  e[3]:0.035 map:1 d:4 --> D:3 *
//pre:7 post:6  e[6]:0.044 map:1 d:4 --> D:3 *
//pre:7 post:11  e[11]:0.410 map:1 d:4 --> D:3 *
//pre:8 post:4  e[4]:0.002 map:1 d:4 --> D:3
//pre:8 post:9  e[9]:0.044 map:1 d:4 --> D:3 *
//pre:8 post:12  e[12]:0.035 map:1 d:4 --> D:3 *
//pre:9 post:5  e[5]:0.007 map:2 d:4 --> D:3
//pre:9 post:8  e[8]:0.009 map:2 d:4 --> D:3
//pre:9 post:10  e[10]:0.107 map:2 d:4 --> D:3 *
//pre:9 post:13  e[13]:0.134 map:2 d:4 --> D:3 *
//pre:10 post:6  e[6]:0.044 map:2 d:4 --> D:3 *
//pre:10 post:9  e[9]:0.044 map:2 d:4 --> D:3 *
//pre:10 post:11  e[11]:0.410 map:2 d:4 --> D:3 *
//pre:10 post:14  e[14]:0.410 map:2 d:4 --> D:3 *
//pre:11 post:7  e[7]:0.134 map:1 d:4 --> D:3 *
//pre:11 post:10  e[10]:0.107 map:1 d:4 --> D:3 *
//pre:11 post:15  e[15]:1.000 map:1 d:4 --> D:2 *
//pre:12 post:8  e[8]:0.009 map:1 d:4 --> D:3
//pre:12 post:13  e[13]:0.134 map:1 d:4 --> D:3 *
//pre:13 post:9  e[9]:0.044 map:2 d:4 --> D:3 *
//pre:13 post:12  e[12]:0.035 map:2 d:4 --> D:3 *
//pre:13 post:14  e[14]:0.410 map:2 d:4 --> D:3 *
//pre:14 post:10  e[10]:0.107 map:1 d:4 --> D:3 *
//pre:14 post:13  e[13]:0.134 map:1 d:4 --> D:3 *
//pre:14 post:15  e[15]:1.000 map:1 d:4 --> D:2 *
//pre:15 post:11  e[11]:0.410 map:1 d:4 --> D:3 *
//pre:15 post:14  e[14]:0.410 map:1 d:4 --> D:3 *
//errors in iteration 1: 77 ( sum|D-map| )
//WARNING: skipping setDelay (offset!=n): pre=15, post=11 delay=3
//[ERROR C:\cockroach-ut3\src\CARLsim6\carlsim\kernel\src\snn_cpu_module.cpp:1255] Post-synaptic delay was not sorted correctly pre_id=15, offset=61
//WARNING: skipping setDelay (offset!=n): pre=15, post=14 delay=3
//Read delays from CARLsim for forward at 0.500 ms
//WARNING inconsistent delays returned 50 instead of 48
//Assertion failed: m == 48, file C:\cockroach-ut3\src\SpikeStream\simulators\carlsim\src\model\CarlsimWrapper.cpp, line 2408
//*/
//
//}
//
////#include "gtest/gtest.h"  // this is not allowed as it requires gtest in the executable
////#ifdef EXPECT_EQ  does not bring a benfit, as a lib must work in both contexts
////
////
/////* converts the maze into delays and Point3Ds
////	*/
////void Maze4x4ConnGen::initialize() {
////
////	// convert matrix structure to neuron Ids and store as Point3D
////	for (int i = 0; i < n; i++) {
////		int r_i = row(i);
////		int c_i = column(i);
////		neurons3D.push_back(Point3D(c_i, r_i, 0));
////	}
////
////	for (int i = 0; i < n; i++) { // from
////		int r_i = row(i);
////		int c_i = column(i);
////		assert(neurons3D[i].x == c_i);
////		assert(neurons3D[i].y == r_i);
////		for (int j = 0; j < n; j++) {  // to 
////			int r_j = row(j);
////			int c_j = column(j);
////
////			assert(neurons3D[j].x == c_j);
////			assert(neurons3D[j].y == r_j);
////
////			int delay;
////			bool connected = false;
////			//if (abs(r_i - r_j) == 1 && c_i == c_j) {	// bidirectional 
////			//if (r_i - r_j == 1 && c_i == c_j) {  // from j to i 
////			if (r_j - r_i == 1 && c_i == c_j) {  // from i to j 
////				delay = maze[r_j - 1][c_j - 1];
////				connected = delay > 0;
////				//int d = *maze[index(r_j, c_j)];
////				//EXPECT_EQ(delay, d);
////				if (ll >= DEV) printf("(%d,%d) -> (%d,%d) = %d (vertical)\n", r_i, c_i, r_j, c_j, delay);
////			}
////			else
////				//if (r_i == r_j && abs(c_i - c_j) == 1) {  // bidectional
////				//if (r_i == r_j && c_i - c_j == 1) {		// from j to i
////				if (r_i == r_j && c_j - c_i == 1) {		// from i to j
////					delay = maze[r_j - 1][c_j - 1];
////					connected = delay > 0;
////					//int d = *maze[index(r_j, c_j)];
////					//EXPECT_EQ(delay, d);
////					if (ll >= DEV) printf("(%d,%d) -> (%d,%d) = %d (horizontal)\n", r_i, c_i, r_j, c_j, delay);
////				}
////				else {
////					connected = false;
////					//printf("diagonal (%d,%d) -> (%d, %d) = INVALID\n", r_i, c_i, r_j, c_j);
////				}
////			// update map with traversal costs
////			if (connected) {
////				int pre = index(r_i, c_i);
////				int post = index(r_j, c_j);
////				map.emplace(std::tuple<int, int>(pre, post), delay);
////				connDelays.push_back(std::tuple<int, int, uint8_t>(pre, post, delay));
////				if (ll >= DEV) printf("map[%d](%d,%d) -> [%d](%d,%d) = %d \n", pre, r_i, c_i, post, r_j, c_j, delay);
////			}
////		}
////	}
////}
////
////void Maze4x4ConnGen::connect(CARLsim* s,
////	int srcGrpId, int i,	// from
////	int destGrpId, int j,	// to
////	float& weight,
////	float& maxWt,
////	float& delay,
////	bool& connected) {
////
////	weight = this->weight;
////
////	try {
////		delay = map.at(std::tuple<int, int>(i, j));  // with bounds checking [] inserts "missing" elements
////		connected = true;
////		Point3D& from = neurons3D[i], & to = neurons3D[j];
////		if (ll >= DEV) printf("connect [%d](%d,%d) -> [%d](%d,%d) = %.0f \n", i, int(from.y), int(from.x), j, int(to.y), int(to.x), delay);  // row, column, Fig.1.B
////	}
////	catch (std::out_of_range) {
////		delay = 0.f;
////		connected = false;
////	}
////}
////
//
////
////
////
////
////
