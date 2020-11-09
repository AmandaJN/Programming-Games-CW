#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>
#include <map>

using namespace std;

namespace Maze { //Namespace created to have enumerated types for Cell and Point
	struct Cell {
		int coordX;
		int coordY;
		int g;
		int h;
		int previousX;
		int previousY;

		//no parameters makes an empty cell object
		Cell() {
			coordX = 0;
			coordY = 0;
			g = 0;
			h = 0;
			previousX = 0;
			previousY = 0;
		}

		//overload constructor with two parameters
		Cell(int x, int y) {
			coordX = x;
			coordY = y;
			g = 0;
			h = 0;
			previousX = 0;
			previousY = 0;
		}

		bool operator== (const Cell& rhs) {
			return coordX == rhs.coordX && coordY == rhs.coordY;
		}

		bool operator< (const Cell& rhs) {
			return g + h < rhs.g + rhs.h;
		}

	};

	struct Player {
		int x;
		int y;
		int at;
		vector<pair<int, int>> path;
		bool moved;
		bool atEnd;

		Player() {
			this->x = 0;
			this->y = 0;
			this->at = 0;
			this->path = {};
			this->moved = false;
			this->atEnd = false;
		}

		Player(int x, int y, vector<pair<int, int>> path) {
			this->x = x;
			this->y = y;
			this->at = 0;
			this->path = path;
			this->moved = false;
			this->atEnd = false;
		}

		bool operator== (const Player& rhs) {
			return x == rhs.x && y == rhs.y;
		}
	};
}

using namespace Maze;

char** maze;
vector<Player*> players;
pair<int, int> centerCoords;
int rows;
int cols;

//input validation for the start menu of the program
//returns true of the input is valid
int validateMenu() {
	int choice;
	cin >> choice;

	while (1) {
		if (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input incorrect. Select a number." << endl;
			cin >> choice;
		}
		else if (choice <= 0 || choice >= 3) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input incorrect. Please select from the menu." << endl;
			cin >> choice;
		}
		else {
			break;
		}
	}

	return choice;
}

//input validation for number of rows
int validateRows() {
	int choice;
	cin >> choice;

	while (1) {
		if (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input incorrect. Select a number." << endl;
			cin >> choice;
		}
		else if (choice <= 4) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input incorrect. Must be greater than 4." << endl;
			cin >> choice;
		}
		else {
			break;
		}
	}

	return choice;
}

//input validation for number of columns
int validateColumns() {
	int choice;
	cin >> choice;

	while (1) {
		if (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input incorrect. Select a number." << endl;
			cin >> choice;
		}
		else if (choice <= 4) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input incorrect. Must be greater than 4." << endl;
			cin >> choice;
		}
		else {
			break;
		}
	}

	return choice;
}

//input validation for number of entrances
int validateentrances() {
	int choice;
	cin >> choice;

	while (1) {
		if (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input incorrect. Select a number." << endl;
			cin >> choice;
		}
		else if (choice <= 0) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input incorrect. Must have at least one entrance." << endl;
			cin >> choice;
		}
		else if (choice > 4) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input incorrect. No more than 4 entrances." << endl;
			cin >> choice;
		}
		else {
			break;
		}
	}

	return choice;
}

//initialise grid
void initGrid(int row, int col) {
	maze = new char* [row]; //initialise the array with number of rows

	//for all rows, add columns
	for (int i = 0; i < row; i++) {

		//add the second array to the current array
		maze[i] = new char[col];

		//for all columns and rows, fill in values
		for (int j = 0; j < col; j++) {
			//fill the array with X chars
			maze[i][j] = 'X';
		}
	}
}

bool isValid(int row, int col, int maxR, int maxC) {
	return (row >= 0) && (row < maxR) &&
		(col >= 0) && (col < maxC);
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
	if (isValid(parentWest->coordX, parentWest->coordY, rMax, cMax)) {
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

//find cell neighbours and return one at random
vector<Cell> GetPathNeighbours(Cell& cell, int rMax, int cMax, bool** openList) {
	const auto x = cell.coordX;
	const auto y = cell.coordY;

	vector<Cell> neighbours;

	//north neighbour is x-1, y
	Cell* north = new Cell(x - 1, y);
	if ((north->coordX > 0 && north->coordX < rMax) && (north->coordY > 0 && north->coordY < cMax)) {
		if (openList[north->coordX][north->coordY] == false)
			neighbours.push_back(*north);
	}


	//western neightbour is x, y-1
	Cell* west = new Cell(x, y - 1);
	if (isValid(west->coordX, west->coordY, rMax, cMax)) {
		if (openList[west->coordX][west->coordY] == false)
			neighbours.push_back(*west);
	}

	//eastern neighbour is x, y+2, child is y+1
	Cell* east = new Cell(x, y + 1);
	if ((east->coordX > 0 && east->coordX < rMax) && (east->coordY > 0 && east->coordY < cMax)) {
		if (openList[east->coordX][east->coordY] == false)
			neighbours.push_back(*east);
	}

	//southern neighbour is x+1, y
	Cell* south = new Cell(x + 1, y);
	if ((south->coordX > 0 && south->coordX < rMax) && (south->coordY > 0 && south->coordY < cMax)) {
		if (openList[south->coordX][south->coordY] == false)
			neighbours.push_back(*south);
	}

	return neighbours;
}

//display maze
void displayMaze() {
	size_t rowCount = rows;
	size_t colCount = cols;

	for (int i = 0; i < rowCount; i++) {
		for (int j = 0; j < colCount; j++) {
			bool playerFound = false;
			for (Player* p : players) {
				if (p->x == i && p->y == j) {
					playerFound = true;
				}
			}
			if (playerFound) {
				cout << 'P';
			}
			else { cout << maze[i][j]; }
		}
		cout << endl;
	}
	cout << endl;
}

void createCenter(pair<int, int> c) {

	//<<Left column>>
	maze[c.first - 1][c.second - 1] = ' '; //top
	maze[c.first][c.second - 1] = ' '; //middle
	maze[c.first + 1][c.second - 1] = ' '; //bottom

	//<<Middle column>>
	maze[c.first - 1][c.second] = ' '; //top
	maze[c.first][c.second] = 'F'; //center
	maze[c.first + 1][c.second] = ' '; //bottom

	//<<Right column>>
	maze[c.first - 1][c.second + 1] = ' '; //top
	maze[c.first][c.second + 1] = ' '; //middle
	maze[c.first + 1][c.second + 1] = ' '; //bottom
}

void changeChar(int r, int c) {
	maze[r][c] = ' ';
}

//place random entrances at any side in the maze
vector<Cell> placeEntrances(int r, int c, int entrances) {
	vector<Cell> entranceCells;
	Cell* entrance;
	for (int i = 0; i < entrances; i++) {
		int randSide = rand() % 4;
		if (randSide == 0 || randSide == 2) { //left, right
			int x = (rand() % (r - 2))+1;
			int y = randSide == 0 ? 1 : (c - 2)+1;
			maze[x][y] = 'E';
			entrance = new Cell(x, y);
		}
		else {
			int y = (rand() % (c - 2))+1; //top, bottom
			int x = randSide == 1 ? 1 : (r - 2)+1;
			maze[x][y] = 'E';
			entrance = new Cell(x, y);
		}
		entranceCells.push_back(*entrance);
	}

	return entranceCells;
}

void genMaze(int rows, int cols) {
	initGrid(rows, cols);
	bool** openList = makeOpenList(rows, cols);

	pair<int, int> startPoint = findStartPoint(rows, cols);
	Cell* startCell = new Cell(startPoint.first, startPoint.second);

	vector<Cell> pathSet;
	pathSet.push_back(*startCell);

	Cell currentCell = pathSet[0];

	//while there are still tiles to check
	while (!pathSet.empty()) {

		openList[currentCell.coordX][currentCell.coordY] = true; //mark as visited
		changeChar(currentCell.coordX, currentCell.coordY); //mark on maze

		auto neighbourCells = GetNeighbours(currentCell, rows, cols, openList); //find the neighbours, returns a vector of pairs

		//if the set of neighbours is not empty
		if (!neighbourCells.empty()) {

			//for all pairs of neighbours
			for (auto p : neighbourCells) {
				pathSet.push_back(p.first); //add parent neighbour

				//mark child as visited
				openList[p.second.coordX][p.second.coordY] = true;
				changeChar(p.second.coordX, p.second.coordY);
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
}

int calcHeuristic(Cell src, Cell dest) {
	int h = (abs(src.coordX - dest.coordX) + abs(src.coordY - dest.coordY));
	return h;
}

Cell** createCellMap(int rows, int cols) {
	Cell** cellGrid;
	cellGrid = new Cell * [rows]; //initialise the array with number of rows

	//for all rows, add columns
	for (int i = 0; i < rows; i++) {

		//add the second array to the current array
		cellGrid[i] = new Cell[cols];

		//for all columns and rows, fill in values
		for (int j = 0; j < cols; j++) {
			//fill the array with X chars
			cellGrid[i][j].coordX = i;
			cellGrid[i][j].coordY = j;
			cellGrid[i][j].g = FLT_MAX;
			cellGrid[i][j].h = FLT_MAX;
			cellGrid[i][j].previousX = -1;
			cellGrid[i][j].previousY = -1;
		}
	}

	return cellGrid;
}

//A* Search algorithm to find the shortest path from start to entrance
//returns the path
vector<pair<int,int>> aStarSearch(pair<int, int> start, pair<int, int> goal, int rows, int cols) {

	bool** closedList = makeOpenList(rows, cols); //make closedlist
	Cell** cellMap = createCellMap(rows, cols);

	Cell* startCell = &cellMap[start.first][start.second];
	Cell* destCell = &cellMap[goal.first][goal.second];

	calcHeuristic(*startCell, *destCell); //set start cell h value

	vector<Cell*> openSet; //set which will hold all cells visited on path
	openSet.push_back(startCell);

	while (!openSet.empty()) { //while not empty
		Cell* current = openSet[0];
		int bestF = (openSet[0]->g + openSet[0]->h);

		for (auto& c : openSet) { //for each cell in the open set
			int cF = c->g + c->h; //check the F value for a better value
			if (cF < bestF) {
				bestF = cF;
				current = c;
			}
		}

		if (*current == *destCell) { //if reached goal cell then break
			break;
		}

		//for number of neighbours (4)
		for (int i = 0; i < 4; i++) {
			int cellX = current->coordX + (i % 2 == 0 ? i - 1 : 0);
			int cellY = current->coordY + (i % 2 == 0 ? 0 : i - 2);

			//validate the neighbour is on the maze grid
			if (cellX < 1 || cellY < 1 || cellX > rows - 1 || cellY > cols - 1)
				continue;

			Cell* cellChecking = &cellMap[cellX][cellY]; //temporary cell being checked set to cell in map

			//if the cell has already been visited or is a wall
			if (closedList[cellChecking->coordX][cellChecking->coordY] == true || maze[cellChecking->coordX][cellChecking->coordY] == 'X')
				continue;

			int tempG = current->g + 1; //temporary g value is the current g + 1 (adjacent cell)
			int tempH = calcHeuristic(*(cellChecking), *destCell); //temporary h value is the value calculated for the cell being checked
			int tempF = tempG + tempH;

			//if the cell being checked is in the openset or the tempf value is greater than cell f value then continue
			if ((find(openSet.begin(), openSet.end(), cellChecking) != end(openSet)) && tempF > (cellChecking->g + cellChecking->h)) {
				continue;
			}
			else {
				//set the values of the cell ebing checked
				cellChecking->g = tempG;
				cellChecking->h = tempH;
				cellChecking->previousX = current->coordX;
				cellChecking->previousY = current->coordY;

				if (find(openSet.begin(), openSet.end(), cellChecking) == end(openSet))
					openSet.emplace_back(cellChecking); //add the cell being checked to the openset
			}
		}
		if (find(begin(openSet), end(openSet), current) != end(openSet)) {
			openSet.erase(find(openSet.begin(), openSet.end(), current)); //remove current from openset
			closedList[current->coordX][current->coordY] = true;
		}
	}
	vector<pair<int, int>> path;
	//get the optimum path from the nodes visited and add them to the path vector
	if (destCell->previousX && destCell->previousY) {
		Cell* pathCell = destCell;
		while (pathCell != startCell) {
			pair<int, int> p (pathCell->coordX, pathCell->coordY);
			path.emplace_back(p);
			pathCell = &cellMap[pathCell->previousX][pathCell->previousY];
		}
	}

	return path;

}

//Opens and saves a new maze file
void createMazeFile(int rows, int cols) {
	string fileName;
	cout << "Name of file: " << endl;
	cin >> fileName;

	ofstream outFile;
	outFile.open((fileName + ".txt"), ios::out); //open file using input name

	size_t rowCount = rows;
	size_t colCount = cols;

	//for each row and column, output the maze to the file
	for (int i = 0; i < rowCount; i++) {
		for (int j = 0; j < colCount; j++) {
			outFile << maze[i][j];
		}
		outFile << endl;
	}

	outFile.close();
}

//Save a maze generated into file format
void saveMaze(int rows, int cols) {
	char choice;
	cout << "Would you like to save this maze? (Y/N)" << endl; //input prompt, need to validate input
	cin >> choice;

	if (choice == 'Y' || choice == 'y') {
		createMazeFile(rows, cols);
	}
}

void setPlayerPath(Player* player, pair<int, int> center, pair<int, int> entrance, int rows, int cols) {
	vector<pair<int, int>> path = aStarSearch(center, entrance, rows, cols);
	player->path = path;
}

void play() {
	bool end = true;
	bool playersStuck = false;
	for (Player* p : players) {
		if (!p->atEnd) {
			int nextX = p->path.at(p->at).first;
			int nextY = p->path.at(p->at).second;

			if (p->path.back().first == nextX && p->path.back().second == nextY) {
				p->atEnd = true;
				p->moved = true;
				p->x = nextX;
				p->y = nextY;
				continue;
			}
			bool canMove = true;
			//check if another player in cell
			for (Player* e : players) {
				if (*p == *e) continue; //if enemy player is in the cell, avoid deadlock by waiting
				canMove &= !(e->x == nextX && e->y == nextY);
			}

			p->moved = !canMove;
			p->x = (nextX * canMove) + (p->x * !canMove);
			p->y = (nextY * canMove) + (p->y * !canMove);
			p->at += canMove;

			playersStuck &= canMove;
			
		}
	}

	for (Player* p : players) {
		end &= p->atEnd;
	}

	if (!end) {
		displayMaze();
		saveMaze(rows, cols);
		play();
	}
}

//Create a new maze using user input
void newMaze() {
	int entrances;

	cout << "Select number of rows: ";
	rows = validateRows();
	cout << "Select number of columns: ";
	cols = validateColumns();
	cout << "Select number of entrances: ";
	entrances = validateentrances();

	genMaze(rows, cols);
	centerCoords = make_pair(rows / 2, cols / 2);
	createCenter(centerCoords);

	auto entranceCells = placeEntrances(rows, cols, entrances);
	//number of entrances = number of players

	for (int i = 0; i < entranceCells.size(); i++) {
		//cout << "Player " << i+1 << " tracking..." << endl; //statement for UI showing player movement
		pair<int, int> entranceCell = make_pair(entranceCells[i].coordX, entranceCells[i].coordY);
		Player* p = new Player;
		p->x = entranceCell.first;
		p->y = entranceCell.second;
		setPlayerPath(p, centerCoords, entranceCell, rows, cols);
		players.emplace_back(p);
	}

	play();
	//saveMaze(rows, cols);
}

//load a maze file from user input
void loadMaze() {
	cout << "What file would you like to load?" << endl;
	string choice;
	cin >> choice;

	ifstream infile;
	infile.open(choice + ".txt");

	if (infile) { //if file exists
		string line;
		while (std::getline(infile, line)) {
			cout << line << endl;
		}
		infile.close();
	}
	else { //else throw error (fstream does not throw exceptions)
		cout << "File does not exist. Try again." << endl;
		loadMaze();
	}

}

//Initial menu interface
void startMenu() {
	cout << "Would you like to: " << endl;

	cout << "\t 1. Start a new maze" << endl;
	cout << "\t 2. Load an existing maze" << endl;

	int validInput = validateMenu();

	if (validInput == 1) {
		newMaze();
	}
	else {
		loadMaze();
	}
}

bool executeAgain() {
	char choice;
	cout << "Would you like to execute again? (Y/N)" << endl;
	cin >> choice;

	if (choice == 'Y' || choice == 'y') {
		return true;
	}
	else {
		return false;
	}
}

//main
int main() {
	srand(time(NULL));
	do {
		startMenu();
	} while (executeAgain());

}
