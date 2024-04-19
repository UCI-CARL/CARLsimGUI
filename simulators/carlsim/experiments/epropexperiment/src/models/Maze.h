
#ifndef MAZE4X4_H
#define MAZE4X4_H

#if defined(WIN32) || defined(WIN64)
	#include <Windows.h>
#else
	#include <sys/time.h>
	#include <ctime>
#endif

#include <QString>

#include <stdint.h>
#include <string>	
#include <map>
#include <vector>
#include <random>


// CARLsim Point3D
// SpikeStream Point3D


/*
* modify spacial map
*/


class Maze
{
public:

	//const static int rows = 4;		// fix me
	//const static int columns = 4;
	//unsigned maze[rows][columns];

	// C++11 solution 
	int rows, columns;
	std::vector<std::vector<int>> maze;


	typedef std::map<std::tuple<int, int>, uint8_t> delay_map_t;

	typedef std::vector<std::tuple<int, int, uint8_t>> ConnDelays_t;

	delay_map_t map;


	//float weight = 85.f;  // no reliable firing of RS neuron, sometimes with 1ms delay
	float weight = 100.f;   // reliable firing of RS neuron 

	int n;

	Maze(unsigned _maze[4][4]) {
		rows = 4; 
		columns = 4;
		n = rows * columns;

		// C++11 solution 
		maze.resize(rows);
		for (auto iter = maze.begin(); iter < maze.end(); iter++)
			*iter = std::vector<int>(columns, -1);

		for (int r = 0; r < rows; r++)
			for (int c = 0; c < columns; c++)
				maze[r][c] = _maze[r][c];

		initialize();
	}

	Maze(int rows, int columns, unsigned cost): rows(rows), columns(columns) {

		// C++11 solution 
		maze.resize(rows);
		for (auto iter = maze.begin(); iter < maze.end(); iter++)
			*iter = std::vector<int>(columns, -1);

		for (int r = 0; r < rows; r++)
			for (int c = 0; c < columns; c++)
				maze[r][c] = cost;

		initialize();
	}


	//uniform
	Maze(int rows, int columns, int d_min, int d_max): rows(rows), columns(columns) {

		// C++11 solution 
		maze.resize(rows);
		for (auto iter = maze.begin(); iter < maze.end(); iter++)
			*iter = std::vector<int>(columns, -1);

		// Choose a random between 1 and 10
		std::default_random_engine e1(42);
		std::uniform_int_distribution<int> uniform_dist(d_min, d_max);

		for (int row = 0; row < rows; row++)
			for (int column = 0; column < columns; column++)
				maze[row][column] = uniform_dist(e1);

		initialize();
	}

	Maze(int rows, int columns, QString costs, int borderCost = -1, int barrierNId = -1);
	
	virtual ~Maze() {};



	//	std::vector<Point3D> neurons3D;
	// 
	// QList .. see SpikeStream

	// check Grid3D of group 

	// FIXME -> adopt index handling from axonplast2 / 3

	//! row, column start with 1,  -1 invalid, 0 undefined?
	int index(int row, int column) {
		// index(r, c) = > (r * 4) + c
		return (row - 1) * columns + column - 1;   // starting at 1
		//return (row) * columns + column;   // starting at 0
	}

	//! row, column starting at 0
	int index0(int row, int column) {
		return (row) * columns + column;   
	}

	int row(int index) {
		return trunc(index / columns) + 1;  // starting at 1
		//return trunc(index / columns);	 // starting at 0
	}

	int column(int index) {
		return trunc(index % rows) + 1;	 // starting at 1
		//return trunc(index % rows);		// starting at 0
	}

	int row0(int index) {
		return trunc(index / columns);	 // starting at 0
	}

	int column0(int index) {
		return trunc(index % rows);		// starting at 0
	}

	// Matlab coordiates
	int y(int index) {
		return trunc(index / columns) + 1;  // starting at 1
	}

	int x(int index) {
		return trunc(index % rows) + 1;	 // starting at 1
	}

	int indexXY(int x, int y) { return index(y, x); }


	void initializeConnDelays(ConnDelays_t& connDelays, uint8_t cost); // fix base cost for EProp

	void initializeConnDelays(ConnDelays_t& connDelays);  // optimal values



private:

	/* converts the maze into delays and Point3Ds
	*/
	void initialize();

};

#endif // MAZE_H