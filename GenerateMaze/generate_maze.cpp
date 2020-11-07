#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <tuple>
#include <set>
#include <vector>
#include <time.h>

using namespace std;

namespace Maze { //Namespace created to have enumerated types for Cell and Point
	struct Cell {
		int coordX;
		int coordY;
		bool visited;

		Cell(int x, int y) {
			coordX = x;
			coordY = y;
			visited = false;
		}

		bool operator== (const Cell& rhs) {
			return coordX == rhs.coordX && coordY == rhs.coordY && visited == rhs.visited;
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

bool** makeOpenList(int row, int col) {
	bool** openList = 0;
	openList = new bool* [row]; //initialise the array with number of rows

	//for all rows, add columns
	for (int i = 0; i < row; i++) {

		//add the second array to the current array
		openList[i] = new bool[col];

		//for all columns and rows, fill in values
		for (int j = 0; j < col; j++) {
			//fill the array with 'false' bool values
			openList[i][j] = false;
		}
	}

	return openList;
}

//find a random start point
pair<int, int> findStartPoint(int r, int c) {
	//find the center of the maze coordinates
	int centerCoords[2] = { floor(r / 2), floor(c / 2) }; //half the input values of the maze then round down to find the index in the array

	bool isValid = false;
	pair<int, int> startPoint;

	//while starting point is invalid
	while (!isValid) {
		//pick random start point on the maze
		//don't pick anywhere in the center square
		startPoint.first = { rand() % r };
		startPoint.second = { rand() % c };

		//if the current position is within the center square then it isn't valid
		isValid = (startPoint.first != centerCoords[0] && startPoint.second != centerCoords[1] ? true : false);
	}
	return startPoint;
}

//find cell neighbours and return one at random
vector<pair<Cell, Cell>> GetNeighbours(Cell& cell, int rMax, int cMax, bool** openList) {
	const auto x = cell.coordX;
	const auto y = cell.coordY;

	vector<pair<Cell, Cell>> neighbours;

	//north neighbour is x-1, y
	Cell* parentNorth = new Cell(x - 2, y);
	//if not already visited
	Cell* childNorth = new Cell(x - 1, y);
	if ((parentNorth->coordX > 0 && parentNorth->coordX < rMax) && (parentNorth->coordY > 0 && parentNorth->coordY < cMax)) {
		if (openList[parentNorth->coordX][parentNorth->coordY] == false)
			neighbours.push_back(make_pair(*parentNorth, *childNorth));
	}


	//western neightbour is x, y-1
	Cell* parentWest = new Cell(x, y - 2);
	Cell* childWest = new Cell(x, y - 1);
	if ((parentWest->coordX > 0 && parentWest->coordX < rMax) && (parentWest->coordY > 0 && parentWest->coordY < cMax)) {
		if (openList[parentWest->coordX][parentWest->coordY] == false)
			neighbours.push_back(make_pair(*parentWest, *childWest));
	}

	//eastern neighbour is x, y+2, child is y+1
	Cell* parentEast = new Cell(x, y + 2);
	Cell* childEast = new Cell(x, y + 1);
	if ((parentEast->coordX > 0 && parentEast->coordX < rMax) && (parentEast->coordY > 0 && parentEast->coordY < cMax)) {
		if (openList[parentEast->coordX][parentEast->coordY] == false)
			neighbours.push_back(make_pair(*parentEast, *childEast));
	}

	//southern neighbour is x+1, y
	Cell* parentSouth = new Cell(x + 2, y);
	Cell* childSouth = new Cell(x + 1, y);
	if ((parentSouth->coordX > 0 && parentSouth->coordX < rMax) && (parentSouth->coordY > 0 && parentSouth->coordY < cMax)) {
		if (openList[parentSouth->coordX][parentSouth->coordY] == false)
			neighbours.push_back(make_pair(*parentSouth, *childSouth));
	}

	return neighbours;
}

//display maze
void displayMaze(char** maze, int rows, int cols) {
	size_t rowCount = rows;
	size_t colCount = cols;

	for (int i = 0; i < rowCount; i++) {
		for (int j = 0; j < colCount; j++) {
			cout << maze[i][j];
		}
		cout << endl;
	}
}

void createCenter(pair<int, int> c, char** m) {

	//<<Left column>>
	m[c.first - 1][c.second - 1] = ' '; //top
	m[c.first][c.second - 1] = ' '; //middle
	m[c.first + 1][c.second - 1] = ' '; //bottom

	//<<Middle column>>
	m[c.first - 1][c.second] = ' '; //top
	m[c.first][c.second] = 'S'; //center
	m[c.first + 1][c.second] = ' '; //bottom

	//<<Right column>>
	m[c.first - 1][c.second + 1] = ' '; //top
	m[c.first][c.second + 1] = ' '; //middle
	m[c.first + 1][c.second + 1] = ' '; //bottom
}

void changeChar(char** m, int r, int c) {
	m[r][c] = ' ';
}

void genMaze(int rows, int cols) {
	char** maze = initGrid(rows, cols);
	bool** openList = makeOpenList(rows, cols);

	pair<int, int> startPoint = findStartPoint(rows, cols);
	Cell* startCell = new Cell(startPoint.first, startPoint.second);

	vector<Cell> pathSet;
	pathSet.push_back(*startCell);

	Cell currentCell = pathSet[0];

	//while there are still tiles to check
	while (!pathSet.empty()) {

		openList[currentCell.coordX][currentCell.coordY] = true; //mark as visited
		changeChar(maze, currentCell.coordX, currentCell.coordY); //mark on maze

		vector<pair<Cell, Cell>> neighbourCells = GetNeighbours(currentCell, rows, cols, openList); //find the neighbours

		//if the set of neighbours is not empty
		if (!neighbourCells.empty()) {

			//for all neighbours
			for (pair<Cell, Cell> p : neighbourCells) {
				pathSet.push_back(p.first); //add parent neighbour

				//mark child as visited
				openList[p.second.coordX][p.second.coordY] = true;
				changeChar(maze, p.second.coordX, p.second.coordY);
			}

			auto pos = find(pathSet.begin(), pathSet.end(), currentCell);
			pathSet.erase(pos); //remove current cell from set
			//only cell remaining should be the parent neighbour

			neighbourCells.clear();
		}
		else {
			pathSet.erase(find(pathSet.begin(), pathSet.end(), currentCell));
		}

		if (pathSet.empty())
			break;

		int randIndex = pathSet.size() != 0 ? rand() % pathSet.size() : 0;
		currentCell = pathSet.at(randIndex);
	}

	pair<int, int> centerCoords = make_pair(rows / 2, cols / 2);
	createCenter(centerCoords, maze);
	displayMaze(maze, rows, cols);
}



//main
int main() {
	srand(time(NULL));
	int rows, cols, exits;

	cout << "Select number of rows: ";
	cin >> rows;
	cout << "Select number of columns: ";
	cin >> cols;
	cout << "Select number of exits: ";
	cin >> exits;

	//generateMaze(rows, cols);
	genMaze(rows, cols);
}
