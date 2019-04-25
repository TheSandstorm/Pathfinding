#include <iostream>
#include <vector>
#include <iomanip>

int cols = 0;
int rows = 0;
int numOfObstacle = 0;
std::vector<std::vector<char>> Board(cols, std::vector<char>(rows, ' '));
static bool isValidNode(int x, int y);
void ShowBoard();
void WallPlacement();
struct Node
{
	int y;
	int x;
	int parentX;
	int parentY;
	float gCost;
	float hCost;
	float fCost;
};
bool isOnClosedListCheck(int x, int y);
bool isOnOpenListCheck(int x, int y);
bool isCuttingCorners(Node currentPos, Node currentOrigin);
bool isWall(int x, int y);
static float Hcalcuate(Node Current, Node Destination);
static float Fcalcuate(Node current);
static float Gcalcuate(Node pos, Node currentLocation);
static bool AStar(Node start, Node dest);
static bool isAtDestination(Node Current, Node Dest);
std::vector<Node> closedList;
std::vector<Node> openList;

// 24/04/2019
// John Strange
// Input: x and y of the location a placement
// Checks if location is a vaild one.
static bool isValidPlacement(int x, int y)
{
	if (x >= rows || y >= cols || y < 0 || x < 0)
		return false;
	else if (Board[y][x] != NULL)
		return false;
	else
		return true;
}

// 23/04/2019
// John Strange
// Input: x and y of the location a node is wanted to be created
// Checks if location is out of bounds.
static bool isValidNode(int x, int y)
{
	if (x >= rows || y >= cols || y < 0 || x < 0)
		return false;
	else
		return true;
}

// 22/04/2019
// John Strange
// Input: Current Location x and y and Destination x and y.
// Checks if at the destination.
static bool isAtDestination(Node Current, Node Destination)
{
	if (Current.x == Destination.x && Current.y == Destination.y)
		return true;
	return false;
}



// 22/04/2019
// John Strange
// Input: needs the current x and y and the destination x and y
// H is the current location to the destination distance calc.
static float Hcalcuate(Node Current, Node Destination)
{
	float H = (((abs(Current.x - Destination.x)) + (abs(Current.y - Destination.y)))*10.0f);
	return H;
}

int main()
{
	int x;
	int y;
	bool Reset = false;
	bool Success = false;
	while(Reset == false)
	{
		std::cout << "Set size of board. Cannot be less than 2" << std::endl;
		std::cin >> rows;
		std::cin >> cols;
		//prevents negative or 1x1 boards
		if (cols > 1 && rows > 1)
			Reset = true;
		else
			std::cout << "Input was not vaild" << std::endl;
	}
	Reset = false;
	Board.resize((cols));
	for (int i = 0; i < cols; ++i)
		Board[i].resize((rows));
	//Print Board
	ShowBoard();
	while (Reset == false)
	{
		std::cout << "Place start point" << std::endl;
		std::cin >> x;
		std::cin >> y;
		x--, y--;
		if (isValidPlacement(x, y) == true)
			Reset = true;
		else
			std::cout << "Input was not vaild" << std::endl;
	}
	Reset = false;
	Node start;
	start.x = x;
	start.y = y;
	Board[y][x] = 'S';
	ShowBoard();
	while (Reset == false)
	{
		std::cout << "Place end point" << std::endl;
		std::cin >> x;
		std::cin >> y;
		x--, y--;
		if (isValidPlacement(x, y) == true)
			Reset = true;
		else
			std::cout << "Input was not vaild" << std::endl;
	}
	Reset = false;
	Node dest;
	dest.x = x;
	dest.y = y;
	Board[y][x] = 'D';
	WallPlacement();
	Success = AStar(start, dest);
	if (Success == true)
		std::cout << "Path was found" << std::endl;
	else
		std::cout << "Path was not found" << std::endl;
	system("pause");

	return 0;
}		

// 12/04/2019
// John Strange
// Input: None
// Shows the current board layout
void ShowBoard()
{
	system("cls");
	std::cout << "Key: S = Start, E = Explored, D = Destination, P = Path to destination, W = Walls, ' ' = Not Explored" << std::endl;
	for (int j = 0; j < rows; j++)
		std::cout << "------";
	for (int i = 0; i < cols; i++) {
		std::cout << '\n' << "|";
		for (int j = 0; j < rows; j++) {
			std::cout << std::setw(3) << Board[i][j] << std::setw(3) << " |";
		}
	}
	std::cout << '\n';
	for (int j = 0; j < rows; j++)
		std::cout << "------";
	std::cout << '\n';
}

// 20/04/2019
// John Strange
// Input: None
// Places the amount of wall based on the size of the board - the start and end location.
void WallPlacement()
{
	int x;
	int y;
	bool Reset = false;
	int max_Obstacle = cols * rows - 2;
	while (Reset == false)
	{
		std::cout << "Set the number of obstacles you want. Cannot be more than: " << max_Obstacle << std::endl;
		std::cin >> numOfObstacle;
		if (numOfObstacle <= max_Obstacle || numOfObstacle >= 0)
			Reset = true;
	}
	for (int i = 0; i < numOfObstacle; i++) 
	{
		ShowBoard();
		std::cout << "Place the obstacles. Num Remaining: " << numOfObstacle - i << std::endl;
		std::cin >> x;
		std::cin >> y;
		x--, y--;
		if (isValidPlacement(x, y) == true)
		{
			Board[y][x] = 'W';
			i++;
		}
		else
			std::cout << "Input was not vaild" << std::endl;
		i--;
	}

}

// 20/04/2019 - 25/04/2019
// John Strange
// Input: Start node and end node
// Completes the AStar Algorithm 
static bool AStar(Node start, Node dest) 
{
	bool bpathFound = false;
	Node currentPos;
	Node currentOrigin;
	start.gCost = 0.0f;
	start.hCost = Hcalcuate(start, dest);
	start.fCost = Fcalcuate(start);
	openList.push_back(start);
	//This runs until either a path is found or until no path is found due to walls.
	while (!bpathFound)
	{
		float bestF =99999999.0f;
		for (unsigned int i = 0; i < openList.size(); i++)
		{
			if (openList.at(i).fCost < bestF)
			{
				currentOrigin = openList.at(i);
				bestF = currentOrigin.fCost;
			}
		}
		//If no path is found
		if (openList.size() <= 0)
		{
			bpathFound = true;
		}
		//If the current location is also the end then the path is found
		if (isAtDestination(currentOrigin,dest) == true)
		{
			bpathFound = true;
		}
		//Add current origin to the closed list
		closedList.push_back(currentOrigin);

		//Removes current origin from the open list
		for (unsigned i = 0; i < openList.size(); i++)
		{
			if (openList.at(i).x == currentOrigin.x && openList.at(i).y == currentOrigin.y)
			{
				openList.erase(openList.begin() + i);
			}
		}

		//To add new nodes around the current origin to the open list
		for (int y = 0; y < 3; y++) //y
		{
			for (int x = 0; x < 3; x++) //x
			{
				//New node for the openlist if vaild
				currentPos.y = ((currentOrigin.y - 1) + y);
				currentPos.x = ((currentOrigin.x - 1) + x);
				if (isValidNode(currentPos.x, currentPos.y) == true)
				{ 
					//Need it's g cost for cutting corners
					currentPos.gCost = Gcalcuate(currentPos, currentOrigin);
					//If it's a wall or its on the closed list ignore or it would be cutting corners
					if (isWall(currentPos.x,currentPos.y) == false && 
						isOnClosedListCheck(currentPos.x, currentPos.y) == false && 
						isCuttingCorners(currentPos,currentOrigin) == false) 
					{
						//If it's not explored then E to say it's found
						if (isValidPlacement(currentPos.x, currentPos.y) == true)
						{
							Board[currentPos.y][currentPos.x] = 'E';
						}
						//Add the rest of the info to the Node
						currentPos.hCost = Hcalcuate(currentPos, dest);
						currentPos.fCost = Fcalcuate(currentPos);
						//If it's not on the open list. Add it.
						if (isOnOpenListCheck(currentPos.x, currentPos.y) == false)
						{
							currentPos.parentX = currentOrigin.x;
							currentPos.parentY = currentOrigin.y;
							openList.push_back(currentPos);
						}
						else
						{
							//If it's already in the open list, check to see if it's better, Unless it's cutting corners.
							if (currentPos.gCost < currentOrigin.gCost&&
								isCuttingCorners(currentPos, currentOrigin) == false)
							{
								currentPos.parentX = currentOrigin.x;
								currentPos.parentY = currentOrigin.y;
								//Only the G would change if it's a better path
								currentPos.gCost = Gcalcuate(currentPos, currentOrigin);
								currentPos.fCost = Fcalcuate(currentPos);
							}
						}

					}

				}

			}
			
		}
		ShowBoard();
	}
	//Failed to find a path
	if (openList.size() <= 0) 
	{
		return false;
	}
	else 
	{
		// Using a new node it traces back from the end of the closed list back to the start
		bool tracked = false;
		Node BackTrack = closedList.at(closedList.size() - 1);
		while (!tracked)
		{
			if (BackTrack.x == closedList.at(0).x && BackTrack.y == closedList.at(0).y) 
			{
				tracked = true;
				break;
			}
			if (BackTrack.y != dest.y ||
				BackTrack.x != dest.x)
				Board[BackTrack.y][BackTrack.x] = 'P';
			BackTrack.x = BackTrack.parentX;
			BackTrack.y = BackTrack.parentY;
			for (unsigned i = 0; i < closedList.size(); i++)
			{
				if (closedList.at(i).x == BackTrack.x && closedList.at(i).y == BackTrack.y)
				{
					BackTrack = closedList.at(i);
				}
			}
			ShowBoard();
		}
		ShowBoard();
	}
	return true;
}
// 20/04/2019 - 25/04/2019
// John Strange
// Input: Current node Position and currentOrigin node
// Finds the G value of the Current position

static float Gcalcuate(Node pos, Node currentOrigin)
{
	float G = currentOrigin.gCost;
	if (currentOrigin.x != pos.x && currentOrigin.y != pos.y)
		G += 14.0f;
	else
		G += 10.0f;
	return G;
}
// 20/04/2019 - 25/04/2019
// John Strange
// Input: The node that need to combined it's f value
// Finds the F value of the node
static float Fcalcuate(Node Pos)
{
	float F = Pos.gCost + Pos.hCost;
	return F;
}
// 20/04/2019 - 25/04/2019
// John Strange
// Input: The x and y of the location it's checking
// Checks to see if that location has a node on the closed list
bool isOnClosedListCheck(int x, int y)
{
	for (unsigned i = 0; i < closedList.size(); i++)
	{
		if (closedList.at(i).x == x && closedList.at(i).y == y) {
			return true;
		}
	}
	return false;
}
// 20/04/2019 - 25/04/2019
// John Strange
// Input: The x and y of the location it's checking
// Checks to see if that location has a node on the open list
bool isOnOpenListCheck(int x, int y)
{
	for (unsigned i = 0; i < openList.size(); i++)
	{
		if (openList.at(i).x == x && openList.at(i).y == y) {
			return true;
		}
	}
	return false;
}
// 25/04/2019
// John Strange
// Input: Current node Position and currentOrigin node
// Checks to see if the current node position is cutting a corner with a wall.
bool isCuttingCorners(Node currentPos, Node currentOrigin)
{
	// if it wasn't 10 then it must be diagonal 
	if (currentPos.gCost == 10.0f)
		return false;
	else
	{
		//Need to figure out which two spots are needed to be checked for walls. When 
		int Locx = currentPos.x - currentOrigin.x;
		int Locy = currentPos.y - currentOrigin.y;
		if (Locx == 1 && Locy == 1)
		{
			if (isWall(currentOrigin.x + 1, currentOrigin.y) == true || isWall(currentOrigin.x, currentOrigin.y + 1) == true)
			return true;
		}
		if (Locx == 1 && Locy == -1)
		{
			if (isWall(currentOrigin.x + 1, currentOrigin.y) == true || isWall(currentOrigin.x, currentOrigin.y - 1) == true)
			return true;
		}
		if (Locx == -1 && Locy == -1)
		{
			if (isWall(currentOrigin.x - 1, currentOrigin.y) == true || isWall(currentOrigin.x, currentOrigin.y - 1) == true)
			return true;
		}
		if (Locx == -1 && Locy == 1)
		{
			if (isWall(currentOrigin.x - 1, currentOrigin.y) == true || isWall(currentOrigin.x, currentOrigin.y + 1) == true)
			return true;
		}
	}
	return false;
}
// 25/04/2019
// John Strange
// Input: The x and y of the location it's checking
// Checks to see if there's a wall in the location. Note: this function only should be used when vaild node check is complete
bool isWall(int x, int y)
{
	if (Board[y][x] == 'W')
		return true;
	return false;
}