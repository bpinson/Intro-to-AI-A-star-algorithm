/*
Brian Pinson
U91813366
CIS4930.002F17 Intro to AI
Project 1

Program is designed to solve for the Straight Line Distance and Fewest Links
for any given weighted graph whose inputs are taken from a connections text file
and a locations text file.

*/



#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>


class PathFinder
{
public:
	explicit PathFinder(); // constructor
	~PathFinder(); // destructor
	
private:

	void setup(); // responsible for getting receiving user inputs
	void buildConnections(); // adds in cities and their neighbors
	void buildLocations(); // adds in cities and their coordinates
	void alphabetize(); // organizes neighbors alphabetically just like the sample output
	void setStartCity(); // user inputs start city
	void setEndCity(); // user inputs end city
	void setExcluded(); // user inputs excluded cities
	bool isExcluded(int); // checks if city is excluded
	bool isEnding(int); // checks if city leads to end city
	void setPathway(); // sets heuristic used
	void setOutput(); // sets output method
	void setEstDistance(); // estimates how far a city is from the end
	double distance(int, int); // calculate distance of two cities
	void setNeighbors(); // puts neighbors in a list
	void ShortestPath(); // calculates shortest path
	struct path { int loc = 0; double dist = 0; int step = 0; }; // struct holds location, distance traveled, and steps taken in the path
	void addNeighbors(path, double); // adds neighbors into the queue
	bool isCloser(path); // checks if city was found at an earlier point
	void printPath(); // prints path

	int start;
	int end;
	char pathChar;
	char outputChar;
	struct city;
	std::vector<city> cities;
	std::vector<int> excluded;
	std::vector<path> paths;
	std::vector<path> theQueue;

};

int main()
{

	std::cout << "/////////////////////////////////////////////////////////////\n";
	std::cout << "/                                                           /\n";
	std::cout << "/    Introduction to AI      CIS4930.002F17                 /\n";
	std::cout << "/                                                           /\n";
	std::cout << "/                   Project 1                               /\n";
	std::cout << "/                                                           /\n";
	std::cout << "/  Straight Line Distance and Fewest Links Calculator       /\n";
	std::cout << "/  by: Brian Pinson                                         /\n";
	std::cout << "/                                                           /\n";
	std::cout << "/////////////////////////////////////////////////////////////\n" << std::endl;



	PathFinder findPath; // class handles inputs, calculations, and outputs

	return 0;
}



struct PathFinder::city // struct contains city's name, coordinates, estimiated distance, and neighboring cities
{
	std::string name = "null";
	int x = 0;
	int y = 0;
	double est_dist = 1;
	std::vector<std::string> connections;
	std::vector<int> neighbors;
};


PathFinder::PathFinder() // constructor
{
	setup();
	if (pathChar == 's')
	{
		setEstDistance(); // estimates distance from point to end point
		ShortestPath(); // calculates shortest distance using estimated distance as its heuristic
	}
	else
	{
		ShortestPath(); // calculates least number of nodes needed to reach end
	}
	printPath(); // prints final path
	std::cin.ignore();
}


PathFinder::~PathFinder() //destructor
{
}


void PathFinder::setup() // reads in user's inputs and adds information to cities vector
{
	std::cout << "Input file pathway and connections filename" << std::endl;
	buildConnections(); // adds connections.txt to cities vector
	std::cout << "Input file pathway and locations filename" << std::endl;
	buildLocations(); // adds locations.txt to cities vector
	alphabetize(); // alphabetizes neighbors
	setNeighbors(); // takes string of neighboring city's name and gives location of city on vector
	std::cout << "Input start city" << std::endl;
	setStartCity(); //inputs starting city
	std::cout << "Input end city" << std::endl;
	setEndCity(); //inputs ending city
	std::cout << "Input cities (if any) to be excluded from the solution path" << std::endl;
	setExcluded(); // inputs excluded list
	std::cout << "Input 's' for straight line distance or 'f' for fewest links" << std::endl;
	setPathway();
	std::cout << "Input 'o' for optimal path or 's' for step-by-step" << std::endl;
	setOutput();
	std::cout << std::endl;
}


void PathFinder::buildConnections() // reads in the connections file
{
	std::string connections;
	std::getline(std::cin, connections);
	std::fstream textfile;
	textfile.open(connections);
	while (!textfile.is_open()) // checks to see if file can even be opened
	{
		std::cout << "Error connections file not found" << std::endl;
		std::getline(std::cin, connections);
		textfile.open(connections);
	}
	if (textfile.is_open()) // double checks to make sure file is opened
	{
		std::string line;

		for (int i = 0; std::getline(textfile, line) && line != "END"; i++) // scans through all the lines in a text file
		{
			int lineSize = line.size();
			city newCity;
			cities.push_back(newCity);
			int j = 0;
			int k = 0;
			for (; j < lineSize && line[j] != ' '; j++); // scans through the line for the name of the city
			cities[i].name = line.substr(0, j);
			j++;
			for (k = j + 1; k < lineSize && line[k] != ' '; k++); // scans through the line for the number of cities
			int size = stoi(line.substr(j, k));
			j = k + 1;

			cities[i].connections.resize(size);
			for (int n = 0; n < size; n++) // scans through every city connected to the current city
			{
				for (k = j + 1; k < lineSize && line[k] != ' '; k++); // scans through and finds city connected to the current city
				cities[i].connections[n] = line.substr(j, k - j);

				if (n == size - 1)
				{
					cities[i].connections[n] = line.substr(j, k - j ); // newline at the end of a line causes problems
				}


				j = k + 1;

			}

		}
		textfile.close();
	}
}


void PathFinder::buildLocations() // opens locations.txt and checks if file can be opened or even exists
{
	std::string locations;
	std::getline(std::cin, locations);
	std::fstream textfile;
	textfile.open(locations);
	while (!textfile.is_open()) // if file cannot be found
	{
		std::cout << "Error locations file not found" << std::endl;
		std::getline(std::cin, locations);
		textfile.open(locations);
	}
	if (textfile.is_open())
	{
		std::string line;

		for (int i = 0; std::getline(textfile, line) && line != "END"; i++) // scans through all the lines in a text file
		{
			int lineSize = line.size();

			int j = 0;
			int k = 0;
			for (; j < lineSize && line[j] != ' '; j++); // scans through the line for the name of the city
			std::string cityName = line.substr(0, j);
			j++;

			int citySize = cities.size();
			int n = 0; // location of city in the array of cities
			for (; n < citySize && !(cityName == cities[n].name); n++); // scans through list of cities for a match
			if (!(cityName == cities[n].name)) // city wasn't found so a new one will be added
			{
				city newCity;
				newCity.name = cityName;
				cities.push_back(newCity);
				citySize++;
			}


			for (k = j + 1; k < lineSize && line[k] != ' '; k++); // scans through the line for the number of cities
			cities[n].x = stoi(line.substr(j, k - j));
			j = k + 1;
			for (k = j + 1; k < lineSize && line[k] != ' '; k++); // scans through the line for the number of cities
			cities[n].y = stoi(line.substr(j, k - j));

		}
		textfile.close();
	}
}

void PathFinder::alphabetize() // alphabetizes the list of connected cities
{
	int citySize = cities.size();
	for (int i = 0; i < citySize; i++)
	{
		std::sort(cities[i].connections.begin(), cities[i].connections.end());
	}
}



void PathFinder::setStartCity() // sets starting city
{
	std::string starting;
	while (1) // checks to see if start city exists
	{
		std::cin >> starting;
		int citiesSize = cities.size();

		for (int i = 0; i < citiesSize; i++) //scans through every city
		{
			if (starting == cities[i].name)
			{
				start = i;
				return;
			}
		}
		std::cout << "Error starting city not found" << std::endl;
	}

}


void PathFinder::setEndCity() // finds the last city
{
	std::string ending;
	while (1) // checks to see if last city actually exists
	{
		std::cin >> ending;
		int citiesSize = cities.size();

		for (int i = 0; i < citiesSize; i++) // scans through every city
		{
			if (ending == cities[i].name)
			{
				end = i;
				return;
			}
		}
		std::cout << "Error ending city not found" << std::endl;
	}
}

void PathFinder::setEstDistance() // calculates distance between a point and the end point
{
	int size = cities.size();
	for (int i = 0; i < size; i++) // sets estimated distance for all points
	{
		cities[i].est_dist = distance(i, end);
	}
}



void PathFinder::setExcluded() // lets user set cities to be excluded
{

	int citySize = cities.size();
	while (1) // lets user input as many excluded cities as they need
	{
		std::cout << "Type 'done' when finished" << std::endl;
		std::string exclude;
		std::cin >> exclude;
		if (exclude == "done") // when 'done' is input the function finishes
		{
			return;
		}

		bool found = false;
		for (int i = 0; i < citySize; i++) // makes sure the excluded city exist
		{
			if (exclude == cities[i].name)
			{
				found = true;
				excluded.push_back(i);
			}
		}
		if (!found)
		{
			std::cout << "Error city not found" << std::endl;
		}
		std::cout << "Input any other cities to be excluded" << std::endl;
	}
}


bool PathFinder::isExcluded(int input) // bool function checks if city is on the excluded list
{
	int size = excluded.size();
	for (int i = 0; i < size; i++) // scans through list of excluded cities
	{
		if (input == excluded[i])
		{
			return true;
		}
	}
	return false;
}

bool PathFinder::isEnding(int input) // checks if point connects to ending point
{
	int size = cities[input].neighbors.size();
	for (int i = 0; i < size; i++) // scans through list of neighbors to see if one is the ending city
	{
		if (cities[input].neighbors[i] == end)
		{
			return true;
		}
	}
	return false;
}


void PathFinder::setPathway() // lets user input desired calculation: straight line distance or fewest links
{
	char path;
	std::cin >> path;
	while (path != 's' && path != 'f') // checks to make sure the input is a 's' or 'f'
	{
		std::cout << "Error, input 's' for straight line distance or 'f' for fewest links" << std::endl;
		std::cin >> path;
	}
	pathChar = path;
}


void PathFinder::setOutput() // user inputs desired output: optimal path or step-by-step
{
	char output;
	std::cin >> output;
	while (output != 'o' && output != 's') // checks if input is an 'o' or a 's'
	{
		std::cout << "Error, input 'o' for optimal path or 's' for step-by-step" << std::endl;
		std::cin >> output;
	}
	outputChar = output;
}


double PathFinder::distance(int a, int b) // function takes in two cities and returns distance
{
	return sqrt((double)(cities[a].x - cities[b].x) * (cities[a].x - cities[b].x) + (cities[a].y - cities[b].y) * (cities[a].y - cities[b].y));
}

void PathFinder::setNeighbors() // program reads in neighboring cities name as a string, function represents cities as the int from their index in the vector cities
{
	int citiesSize = cities.size();
	for (int i = 0; i < citiesSize; i++) // scans through all cities
	{
		int size = cities[i].connections.size();
		for (int j = 0; j < size; j++) // scans through all connections in the city
		{
			for (int k = 0; k < citiesSize; k++) // scans through all connections to find connected city index
			{
				if (cities[i].connections[j] == cities[k].name) // saves connected city as int representing index
				{
					cities[i].neighbors.push_back(k);
					break;
				}
			}
		}
	}
}



void PathFinder::printPath() // prints out final output
{
	if (pathChar == 's') // decies which solution to print
	{
		std::cout << "Heuristic: Straight Line distance:\nStarting city: " << cities[start].name << "\nTarget city: " << cities[end].name << std::endl;
	}
	else
	{
		std::cout << "Heuristic: Fewest Links:\nStarting city: " << cities[start].name << "\nTarget city: " << cities[end].name << std::endl;
	}

	if (outputChar == 'o') // prints optimal path
	{
		std::cout << "\nOptimal Path: " << std::endl;
		int size = paths.size();
		std::cout << cities[paths[0].loc].name;
		for (int i = 1; i < paths[size - 1].step; i++) // scans through all paths taken
		{
			for (int j = size - 1; j >= 0; j--) // decides if path was part of the final solution
			{
				if (paths[j].step == i) // path is part of final solution
				{
					std::cout << "->" << cities[paths[i].loc].name;
					break;
				}
			}
		}
		std::cout << "->" << cities[paths[size - 1].loc].name << "\nDistance Traveled: " << paths[size - 1].dist << std::endl;
	}
	else // prints out step-by-step
	{
		int size = paths.size();
		std::cin.ignore();
		int i = 0;
		for (i = 0; i < size; i++) // number of paths to be printed
		{

			if (i == size - 1) // checks if path is the final path
			{
				std::cout << "\nFinal Path: " << std::endl;
			}
			else
			{
				std::cout << "\nOptimal Path: " << std::endl;
			}
			std::cout << cities[paths[0].loc].name;
			std::vector<path> temp;
			temp.push_back(paths[i]);
			for (int k = 0; k < size; k++) // scans through all paths taken
			{
				int tempSize = temp.size();
				if (temp.back().loc == start || tempSize >= paths[i].step)
					break;
				int sizeN = cities[paths[k].loc].neighbors.size();
				for (int n = 0; n < sizeN; n++) // scans through all neighbors in paths taken
				{
					if (cities[paths[k].loc].neighbors[n] == temp.back().loc && paths[k].loc != temp.back().loc)
					{
						temp.push_back(paths[k]); // path leads up to city[i] so it is added to a stack
						k = 0;
						break;
					}

				}

			}
			while (!temp.empty() && temp.back().loc != start) // while stack is not empty it prints paths
			{
				std::cout << "->" << cities[temp.back().loc].name;
				temp.pop_back();
			}
			std::cout << "\nDistance Traveled: " << paths[i].dist << std::endl;
			std::cin.ignore();
		}
	}
	std::cin.ignore();
}


void PathFinder::ShortestPath() // finds solution and creates the paths vector which holds all paths taken until end is found
{
	path a{ start, 0 , 0 };
	excluded.push_back(start); // no reason to ever return to starting location;
	theQueue.push_back(a);
	paths.push_back(a);
	double traveled = 0; // total distance traveled

	while (theQueue.front().loc != end) // while the end has not been found
	{
		excluded.push_back(theQueue.front().loc); // adds previous locations visted to excluded list

		traveled = theQueue.front().dist;
		addNeighbors(theQueue.front(), traveled);
		if (theQueue.empty())
		{
			std::cout << "Error solution exist for reaching ending city!" << std::endl;
			return;
		}
		paths.push_back(theQueue.front()); // adds the next path taken
	}
}

void PathFinder::addNeighbors(path current, double traveled) // adds all neighboring cities to the queue
{
	theQueue.erase(theQueue.begin()); // pops the first element in the queue
	path a{ 0, 0, 0 };

	if (pathChar == 'f' && isEnding(current.loc)) // if the next city is the end
	{
		a = { end, current.dist + 1,  current.step + 1 };
		theQueue.insert(theQueue.begin(), a); // skip the remaining steps and add ending to front of the queue

		return;
	}


	int size = cities[current.loc].neighbors.size();
	for (int i = 0; i < size; i++) // scans through all neighbors of the city
	{
		if (!isExcluded(cities[current.loc].neighbors[i]) && current.loc != cities[current.loc].neighbors[i]) // checks to makes sure path is not excluded
		{
			if (pathChar == 's') // adds paths for straight line distance solutino
			{
				a = { cities[current.loc].neighbors[i], traveled + distance(current.loc, cities[current.loc].neighbors[i]),  current.step + 1 };
			}
			else // adds paths for fewest links solution
			{
				a = { cities[current.loc].neighbors[i], traveled + 1,  current.step + 1 };
			}
			if (isCloser(a)) // checks if the path is the shortest path to point 'a'
			{
				theQueue.push_back(a); // pont 'a' is added to solution
			}
		}
	}


	std::vector<path> temp;


	size = theQueue.size();

	for (int i = 0; i < size; i++) // organizes the queue from least to greatest traveled distance + estimated distance
	{
		for (int j = i + 1; j < size; j++) // compares i to j
		{
			if (theQueue[i].dist + cities[theQueue[i].loc].est_dist > theQueue[j].dist + cities[theQueue[j].loc].est_dist)
			{
				path temp = theQueue[i];
				theQueue[i] = theQueue[j];
				theQueue[j] = temp;
			}
		}
	}


}

bool PathFinder::isCloser(path input) // checks if point is connected by the shortest possible path
{
	int size = theQueue.size();

	for (int i = 0; i < size; i++) // scans through queue
	{
		if (theQueue[i].loc == input.loc) // if point is found on queue
		{
			if (theQueue[i].dist + cities[theQueue[i].loc].est_dist > input.dist + cities[input.loc].est_dist) // if point on queue has a longer path
			{
				theQueue.erase(theQueue.begin() + i); // delete the point on queue
				return true; // affirms point is now connected to shortest path
			}
			else
				return false; // point was already on shorter path
		}
	}
	return true; // point was never on path
}




