#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <tuple>
#include <set>
#include <vector>

using namespace std;

namespace Maze { //Namespace created to have enumerated types for Cell and Point
	struct Cell {
		int coordX;
		int coordY;
		bool visited;

		Cell(int x, int y) {
			coordX = x;
			coordY = y;
		}
	};
	typedef Maze::Cell Cell;
}

using namespace Maze;

//user input
void userInput() {

}

//user interface

//input validation

//initialise grid
char** initGrid(int row, int col) {
	char** mazeGrid = 0;
	mazeGrid = new char* [row]; //initialise the array with number of rows

	//for all rows, add columns
	for (int i = 0; i < row; i++) {

		//add the second array to the current array
		mazeGrid[i] = new char[col];

		//for all columns and rows, fill in values
		for (int j = 0; j < col; j++) {
			//fill the array with X chars
			mazeGrid[i][j] = 'X';
		}
	}

	return mazeGrid;
}

pair<int, int> findStartPoint(int r, int c) {
	//find the center of the maze coordinates
	int centerCoords[2] = { floor(r / 2), floor(c / 2) }; //half the input values of the maze then round down to find the index in the array

	bool isValid = false;
	pair<int, int> startPoint;

	//while starting point is invalid
	while (!isValid) {
		//pick random start point on the maze
		//don't pick anywhere in the center square
		startPoint.first = { (rand() % r) };
		startPoint.second = { (rand() % c) };

		//if the current position is within the center square then it isn't valid
		isValid = startPoint.first != centerCoords[0] && startPoint.second != centerCoords[1] ? true : false;
	}
	return startPoint;
}

//find cell neighbours
vector<Cell> GetNeighbours(const char& maze, const Cell& cell, const bool visited) {
	const auto x = cell.coordX;
	const auto y = cell.coordY;

	vector<Cell> neighbours;

	//north neighbour is x-1, y
	Cell* northN = new Cell(x - 1, y);
	neighbours.push_back(*northN);

	//western neightbour is x, y-1
	Cell* westN = new Cell(x, y - 1);
	neighbours.push_back(*westN);

	//eastern neighbour is x, y+1
	Cell* eastN = new Cell(x, y + 1);
	neighbours.push_back(*eastN);

	//southern neighbour is x+1, y
	Cell* southN = new Cell(x + 1, y);
	neighbours.push_back(*southN);

	for (int i = 0; i < neighbours.size(); i++) {
		if (neighbours[i].coordX < 0 || neighbours[i].coordY < 0) {
			neighbours.erase(neighbours.begin() + i);
		}
	}
}

// maze generation algorithm
void generateMaze(int rows, int cols) {

	//Create the maze
	char** maze = initGrid(rows, cols);

	pair<int, int> startPoint = findStartPoint(rows, cols);

	//from there find adjacent tiles (two blocks away)
	
	//create a cell object to hold the coordinates of the starting point
	Cell* startingCell = new Cell(startPoint.first, startPoint.second);

	//create a set to hold the number of Cells on the path
	vector<Cell> pathSet;
	pathSet.push_back(*startingCell); //add cell to path

	while (!pathSet.empty()) {
		//begin search at start of path set as the current cell
		auto currentCell = pathSet.begin();
		std::advance(currentCell, pathSet.size()); //move forward
		(*currentCell).visited = true; //set the current cell as visited


	}

}



//display maze
void displayMaze(char** maze, int rows, int cols) {
	size_t rowCount = rows;
	size_t colCount = cols;

	for (int i = 0; i < rowCount; i++) {
		for (int j = 0; j < colCount; j ++) {
			cout << maze[i][j];
		}
		cout << endl;
	}
}

//main
int main() {
	int rows, cols, exits;

	cout << "Select number of rows: ";
	cin >> rows;
	cout << "Select number of columns: ";
	cin >> cols;
	cout << "Select number of exits: ";
	cin >> exits;

	//displayMaze(maze, rows, cols);
	generateMaze(rows, cols);
}