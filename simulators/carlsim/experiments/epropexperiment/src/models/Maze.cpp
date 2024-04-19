
#include "Maze.h"

#include <stdio.h>
//#include <assert.h>			// assert

#include <algorithm>		// std::find
#include <sstream>
#include <iomanip>
#include <regex>
using namespace std;


Maze::Maze(int rows, int columns, QString costs, int border, int barrier) : rows(rows), columns(columns) {

	// C++11 solution 
	maze.resize(rows);
	for (auto iter = maze.begin(); iter < maze.end(); iter++)
		*iter = std::vector<int>(columns, -1);


	//TODO Maze methods  
// readCostsFromString
// writeCostsToString 

	vector<vector<unsigned>> maze_;

	if (!costs.isEmpty()) {
		std::istringstream stream(costs.toStdString());
		stream.setf(std::ios_base::skipws);

		//define the line buffer
		std::string line;

		//read first line and validate begin patter "{.*"
		regex re_begin("^\\{.*$");   // Line that begins with {
		std::getline(stream, line);
		std::smatch base_match;
		if (!regex_match(line, base_match, re_begin))
			std::printf("first line must beginn with {\n");

		//read expected lines
		std::vector<unsigned> line_values;
		for (int r = 0; r < rows; r++) {

			std::getline(stream, line);
			regex re_content("^\\s*\\{([\\,\\d\\s]+)\\}.*$");   // Line that begins with {
			std::smatch base_match;
			if (!regex_match(line, base_match, re_content))
				std::printf("line has wrong format\n");

			auto content = base_match[1];

			//maze_.push_back(std::vector<unsigned>());
			//auto& row = maze_.end();

			istringstream line_stream(content);
			int cost;
			char chr;
			while (!line_stream.eof()) {
				line_stream >> cost;
				std::printf(" %2d", cost);
				line_values.push_back(cost);
				if (line_stream.peek() == ',')
				{
					line_stream >> chr;
					//printf("%c ", chr);
				}
			}
			//check if expected amount of colmn was read
			std::printf("\n");

			maze_.push_back(line_values);
			line_values.clear(); // keep the internal size
			// std::move would have no benefit as line_values implements the cache already

		}

		//read last line and validate end pattern "}.*"
		regex re_end("^\\}.*$");   // Last line needs to begin with }
		std::getline(stream, line);
		if (!regex_match(line, base_match, re_begin))
			std::printf("last line must beginn with }\n");

		for (int r = 0; r < rows; r++)
			for (int c = 0; c < columns; c++) {
				auto cost = maze_[r][c];
				// overwrite 0 with parameterized value for border
				if (cost == 0 && border > -1)
					cost = border;
				maze[r][c] = cost;
			}

	}

	//Set barrier
	if (barrier > -1) {
		auto r_barrier = row0(barrier);
		auto c_barrier = column0(barrier);
		maze[r_barrier][c_barrier] = border;

	}

	initialize();
}


#define LN_DISCOUNT_ON_ENTER

/*! the maze was initialized in the constructure (inline in .h).
* initialize the cost map with traversal costs from pre to post
*/
void Maze::initialize() {

	n = rows * columns;

	int pre, post; 
	uint8_t d; 

//FIXME  parameter discount
//FIXME Morris has 8 not 4 -> rewrite to cardinals
#ifdef LN_DISCOUNT_ON_ENTER

	// forward column wise, starting at (0,0)
	for (unsigned r = 0; r < rows; r++)
		for (unsigned c = 1; c < columns; c++) {
			d = maze[r][c]; // traverse costs to
			if (d > 20) d = 20;
			pre = index0(r, c - 1 );
			post = index0(r, c);
			map.emplace(std::tuple<int, int>(pre, post), d);
		}

	// forward row wise
	for (unsigned r = 1; r < rows; r++) {
		for (unsigned c = 0; c < columns; c++) {
			d = maze[r][c]; // traverse costs to
			if (d > 20) d = 20;
			pre = index0(r - 1, c);
			post = index0(r, c);
			map.emplace(std::tuple<int, int>(pre, post), d);
		}
	}

	// backward colunn whise
	for (unsigned r = 0; r < rows; r++)
		for (unsigned c = 1; c < columns; c++) {
			d = maze[r][c - 1]; // traverse costs to
			if (d > 20) d = 20;
			pre = index0(r, c );
			post = index0(r, c - 1);
			map.emplace(std::tuple<int, int>(pre, post), d);
		}

	// backward row wise
	for (unsigned r = 1; r < rows; r++)
		for (unsigned c = 0; c < columns; c++) {
			d = maze[r - 1][c]; // traverse costs to
			if (d > 20) d = 20;
			pre = index0(r, c);
			post = index0(r - 1, c);
			map.emplace(std::tuple<int, int>(pre, post), d);
		}

#else
	// forward column wise, starting at (0,0)
	for (unsigned r = 1; r <= rows; r++)
		for (unsigned c = 1; c <= columns - 1; c++) {
			d = maze[r-1][c-1]; // traverse costs
			pre = index(r, c); 
			post = index(r, c+1);
			map.emplace(std::tuple<int, int>(pre, post), d);
		}

	// forward row wise
	for (unsigned r = 1; r <= rows - 1; r++) {
		for (unsigned c = 1; c <= columns; c++) {
			d = maze[r-1][c-1]; // traverse costs
			pre = index(r, c);
			post = index(r + 1, c);
			map.emplace(std::tuple<int, int>(pre, post), d);
		}
	}

	// backward colunn whise
	for (unsigned r = 1; r <= rows; r++)
		for (unsigned c = 1; c <= columns - 1; c++) {
			d = maze[r-1][c-1 + 1]; // traverse costs
			pre = index(r, c + 1);
			post = index(r, c);
			map.emplace(std::tuple<int, int>(pre, post), d);
		}

	// backward row wise
	for (unsigned r = 1; r <= rows - 1; r++)
		for (unsigned c = 1; c <= columns; c++) {
			d = maze[r-1 + 1][c-1]; // traverse costs
			pre = index(r + 1, c);
			post = index(r, c);
			map.emplace(std::tuple<int, int>(pre, post), d);
		}
#endif

}

// fix base cost for EProp
void Maze::initializeConnDelays(ConnDelays_t& connDelays, uint8_t cost) {
	connDelays.resize(0);  
	connDelays.clear();
	for (unsigned r = 0; r < rows; r++)
		for (unsigned c = 0; c < columns; c++) {
			connDelays.push_back(std::tuple<int, int, uint8_t>(r, c, cost));
		}
}

// optimal values
void Maze::initializeConnDelays(ConnDelays_t& connDelays) {
	connDelays.resize(0);
	for (unsigned r = 0; r < rows; r++)
		for (unsigned c = 0; c < columns; c++) {
			connDelays.push_back(std::tuple<int, int, uint8_t>(r, c, maze[r][c]));
			printf("connDelays[%d] = (%d, %d) --> %d\n", (int)connDelays.size() - 1, r,  c, maze[r][c]);
		}
}


/* converts the maze into delays and Point3Ds
	*/
//void Maze::initialize() {

	//// convert matrix structure to neuron Ids and store as Point3D
	//for (int i = 0; i < n; i++) {
	//	int r_i = row(i);
	//	int c_i = column(i);
	//	neurons3D.push_back(Point3D(c_i, r_i, 0));
	//}

	//for (int i = 0; i < n; i++) { // from
	//	int r_i = row(i);
	//	int c_i = column(i);
	//	assert(neurons3D[i].x == c_i);
	//	assert(neurons3D[i].y == r_i);
	//	for (int j = 0; j < n; j++) {  // to 
	//		int r_j = row(j);
	//		int c_j = column(j);

	//		assert(neurons3D[j].x == c_j);
	//		assert(neurons3D[j].y == r_j);

	//		int delay;
	//		bool connected = false;
	//		//if (abs(r_i - r_j) == 1 && c_i == c_j) {	// bidirectional 
	//		//if (r_i - r_j == 1 && c_i == c_j) {  // from j to i 
	//		if (r_j - r_i == 1 && c_i == c_j) {  // from i to j 
	//			delay = maze[r_j - 1][c_j - 1];
	//			connected = delay > 0;
	//			//int d = *maze[index(r_j, c_j)];
	//			//EXPECT_EQ(delay, d);
	//			if (ll >= DEV) printf("(%d,%d) -> (%d,%d) = %d (vertical)\n", r_i, c_i, r_j, c_j, delay);
	//		}
	//		else
	//			//if (r_i == r_j && abs(c_i - c_j) == 1) {  // bidectional
	//			//if (r_i == r_j && c_i - c_j == 1) {		// from j to i
	//			if (r_i == r_j && c_j - c_i == 1) {		// from i to j
	//				delay = maze[r_j - 1][c_j - 1];
	//				connected = delay > 0;
	//				//int d = *maze[index(r_j, c_j)];
	//				//EXPECT_EQ(delay, d);
	//				if (ll >= DEV) printf("(%d,%d) -> (%d,%d) = %d (horizontal)\n", r_i, c_i, r_j, c_j, delay);
	//			}
	//			else {
	//				connected = false;
	//				//printf("diagonal (%d,%d) -> (%d, %d) = INVALID\n", r_i, c_i, r_j, c_j);
	//			}
	//		// update map with traversal costs
	//		if (connected) {
	//			int pre = index(r_i, c_i);
	//			int post = index(r_j, c_j);
	//			map.emplace(std::tuple<int, int>(pre, post), delay);
	//			connDelays.push_back(std::tuple<int, int, uint8_t>(pre, post, delay));
	//			if (ll >= DEV) printf("map[%d](%d,%d) -> [%d](%d,%d) = %d \n", pre, r_i, c_i, post, r_j, c_j, delay);
	//		}
	//	}
	//}
//}





