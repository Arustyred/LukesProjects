/*
Lukas Shiley
2-2-2021

Nineteen.cpp

Solves the nineteen tile game using A* or dfs
IndexMinPQ and search methods adapted from Simon

*/


#include <iostream>
#include <cstdint>
#include <string>
#include <stack>
#include <vector>
#include <map>
#include <chrono>
#include <ctime>
#include <thread>
#include <mutex>

#include "Node.h"
#include "State.h"
#include "IndexMinPQ.h"

using namespace std;

#define WIDTH 6
#define HEIGHT 6
#define ARR_SIZE 36
#define NUM_TILES 20
#define BLANK_VAL 0
#define LIMIT 30

const int State::SIZE = ARR_SIZE;
const int State::BLANK_VALUE = BLANK_VAL;
int* State::opposite_move = new int[4]{ 1, 0, 3, 2 };

//input
int prob_num = -1;
int algorithm = 0;
uint8_t* custom_problem;

//array used in heuristic to keep track of x, y, target x, target y   for each tile
int** heuristic_states = new int* [NUM_TILES];
//other heuristic variables
int hx, hy, hk, dx, dy;
int num, estimated_moves;
uint8_t spot_val = 0;
vector<uint8_t> conflict_tiles;

//time
long long start_time;
long long end_time;
unsigned long long ram_usage;  //not accurate, didnt have time to finish this
unsigned long long ram_limit;

//Method Declarations
void astar(State initial, State& goal);
int h(State& current);
int square_conflict(int x, int y, int x2, int y2, int dx, int dy, State& current);
//int center_square_check(State& current);  //removed
int linear_conflicts(State& current);
void iterative_deepening_search(State initial, State& goal);
int breadth_first_search(State current, State& goal, stack<State>& path, int limit);
void print_result(stack<State>& path, int moves);
void print_result(Node n, int nodes_searched);
uint8_t** create_problems();
void init();
void read_input();
void cleanup(uint8_t**& test_problems);

//stuff with threads
int threads = 2;
int threads_active;
bool goal_reached = false;
void threaded_command(IndexMinPQ<Node>& frontier, map<State, Node>& explored, State& goal, int& minH, int thread_num, mutex& frontier_mutex, mutex& explored_mutex);


int main()
{
	State start, goal;

	//read user input to determine problem and parameters
	read_input();

	//create default problem states and goal state
	uint8_t** tests = create_problems();
	if (prob_num != -1)
		start = *new State(tests[prob_num]);
	else
		start = *new State(custom_problem);

	goal = *new State(tests[7]);
	init();

	//begin search
	if (algorithm == 0)
		astar(start, goal);
	else
		iterative_deepening_search(start, goal);

	//clear memory
	cleanup(tests);

	//added this so that the exe doesnt immediately close after finishing
	cin.get();
	return 0;
}

void astar(State initial, State& goal) {
	State state;
	Node z = *new Node(initial, NULL, 0, h(initial));
	IndexMinPQ<Node> frontier = *new IndexMinPQ<Node>();
	map<State, Node> explored = *new map<State, Node>();

	cout << "\nStarting A* search for state\n\n" << initial.to_string();

	//min heuristic evaluated
	int minH = INT32_MAX;
	frontier.add(z);
	explored.emplace(z.state, z);
	cout << "|  h(n)  |  Frontier  |  Explored  |     Time     |    Memory    |\n";

	vector<thread> the_threads;
	mutex frontier_mutex; mutex explored_mutex;

	//create threads, pass parameters by reference, and then sleep for a sec to initialize
	for (int i = 0; i < threads; i++)
		the_threads.push_back(thread(threaded_command, ref(frontier), ref(explored), ref(goal), ref(minH), i, ref(frontier_mutex), ref(explored_mutex)));
	this_thread::sleep_for(chrono::milliseconds(100));

	//sleep until all threads have finished, then resume main thread
	while (threads_active > 0)
		this_thread::sleep_for(chrono::milliseconds(100));
	for (int i = 0; i < threads; i++)
		the_threads[i].join();
}

//heuristic evaluation function using manhattan distance, linear conflicts, and square conflicts
int h(State& current) {
	num = 0;
	estimated_moves = 0;
	for (hy = 0; hy < HEIGHT; hy++)
	{
		for (hx = 0; hx < WIDTH; hx++)
		{
			spot_val = current.tiles[WIDTH * hy + hx];
			if (spot_val == 255)
				continue;
			heuristic_states[spot_val][0] = hx;  //current position for spot_val
			heuristic_states[spot_val][1] = hy;
			heuristic_states[num][2] = hx;  //target position for num
			heuristic_states[num][3] = hy;
			num++;
		}
	}

	//heuristic_states[t][0] x position for square with value t
	//heuristic_states[t][1] y position for square with value t
	//heuristic_states[t][2] target x position for square with value t
	//heuristic_states[t][3] target y position for square with value t

	for (hk = 1; hk < NUM_TILES; hk++)
	{
		//also save dx, dy, and manhattan distance for each tile
		heuristic_states[hk][4] = dx = heuristic_states[hk][2] - heuristic_states[hk][0];
		heuristic_states[hk][5] = dy = heuristic_states[hk][3] - heuristic_states[hk][1];
		heuristic_states[hk][6] = abs(dx) + abs(dy);

		//check if this number is in the correct position
		if (heuristic_states[hk][6] == 0)
			continue;

		estimated_moves += abs(dx) + abs(dy);    //manhattan
	}

	//this next part is easier to explain with an example
	//the first call checks if the tiles at (2,1) and (3,1) are in the correct positions
	//if they both are AND either tile above them is out of place, add 2 to estimated moves
	//because at least one will need to be moved out of place and then back

	//the only time this wont apply is if the blank and the tile beside the blank are swapped AND no other tiles are out of place
	//in that case and no other would estimated moves be 1 after manhattan, so add the if statement to make sure estimated moves is greater than 1
	if (estimated_moves > 1) {
		estimated_moves += square_conflict(2, 1, 3, 1, 0, -1, current);
		estimated_moves += square_conflict(2, 4, 3, 4, 0, 1, current);
		estimated_moves += square_conflict(1, 2, 1, 3, -1, 0, current);
		estimated_moves += square_conflict(4, 2, 4, 3, 1, 0, current);
		//estimated_moves += center_square_check(current);  //this did not work
	}

	return estimated_moves + linear_conflicts(current);
}

//A* method being run by each thread
void threaded_command(IndexMinPQ<Node>& frontier, map<State, Node>& explored, State& goal, int& minH, int thread_num, mutex& frontier_mutex, mutex& explored_mutex) {
	threads_active++;

	State state;
	while (true) {
		unique_lock<mutex> guard_frontier(frontier_mutex);
		if (frontier.empty()) {
			guard_frontier.unlock();
			break;
		}

		//get lowest priority from frontier
		Node current = frontier.remove();
		guard_frontier.unlock();
		current.in_frontier = false;

		//check if another thread reached the goal, this state is the goal, or this state has the new lowest h value
		if (goal_reached)
			break;
		if (current.h < minH) {
			minH = current.h;
			long long current_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();

			//i tried
			ram_usage = (sizeof(State) + ARR_SIZE) * (frontier.size + explored.size()) + (sizeof(State) + ARR_SIZE + sizeof(Node)) * explored.size() + (explored.size() + frontier.size) * sizeof(Node);

			printf("|%*u |%*u |%*llu |%*llu ms |%*llu |\n", 7, minH, 11, frontier.size, 11, (long long unsigned int) explored.size(), 10, current_time - start_time, 13, ram_usage);
		}
		if (current.state == goal) {
			end_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
			print_result(current, explored.size());
			goal_reached = true;
			break;
		}

		state = *new State(current.state.tiles, current.state.blank_pos);
		//loop through all of this states moves
		for (int i = 0; i < 4; i++)
		{
			if (state.Move(i)) {
				//if the state is not explored, create a new node and state
				unique_lock<mutex> guard_explored(explored_mutex);
				if (explored.find(state) == explored.end()) {
					Node next = *new Node(state, &(explored.find(current.state)->second), current.g + 1, h(state));
					//&(explored.find(current.state)->second)    gets the pointer to the parent node that is stored in the map
					explored.emplace(next.state, next);
					guard_explored.unlock();
					unique_lock<mutex> guard_frontier(frontier_mutex);
					frontier.add(next);
					guard_frontier.unlock();
				}
				else {
					//if this path cost is less than the cost of the other path to this state, update the node's parent so it follows the current path
					Node next = explored.find(state)->second;
					if (next.in_frontier && current.g + 1 < next.g) {
						next.parent = &(explored.find(current.state)->second);
						guard_explored.unlock();
						next.g = current.g + 1;
						unique_lock<mutex> guard_frontier(frontier_mutex);
						frontier.update(next);
						guard_frontier.unlock();
					}
					else
						guard_explored.unlock();
				}
				state.Move(State::opposite_move[i]);
			}
		}
	}
	threads_active--;
}

//explained in heuristic method
int square_conflict(int x, int y, int x2, int y2, int dx, int dy, State& current) {
	if (heuristic_states[current.tiles[WIDTH * y + x]][6] == 0 && heuristic_states[current.tiles[WIDTH * y2 + x2]][6] == 0)
		if (heuristic_states[current.tiles[WIDTH * (y + dy) + x + dx]][6] > 0 || heuristic_states[current.tiles[WIDTH * (y2 + dy) + x2 + dx]][6] > 0)
			return 2;
	return 0;
}

int linear_conflicts(State& current) {
	int conflict_moves = 0;;
	for (int i = 1; i < NUM_TILES; i++)
	{
		bool continue_loop = false;
		for (int k = 0; k < conflict_tiles.size(); k++)   //check if this tile is already involved in a conflict
			if (conflict_tiles[k] == i) {
				continue_loop = true;
				break;
			}
		if (continue_loop)  //if it is, break first loop and continue second
			continue;

		//if it is not already involved in a conflict, loop through all of the tiles in the same row
		for (int k = 0; k < WIDTH; k++)
		{
			uint8_t other = current.tiles[WIDTH * heuristic_states[i][1] + k];
			if (other == i || other == 255 || other == BLANK_VAL)  //dont compare to self or tiles not on board or blank pos
				continue;

			//other tile target x is less than our x and our target x is greater than other tile x, then we are in linear conflict
			if (heuristic_states[other][2] < heuristic_states[i][0] && heuristic_states[i][2] < heuristic_states[other][0]) {
				conflict_moves += 2;
				continue_loop = true;
				conflict_tiles.push_back(i);
				conflict_tiles.push_back(other);
				break;
			}

			//other way around
			if (heuristic_states[i][2] < heuristic_states[other][0] && heuristic_states[other][2] < heuristic_states[i][0]) {
				conflict_moves += 2;
				continue_loop = true;
				conflict_tiles.push_back(i);
				conflict_tiles.push_back(other);
				break;
			}
		}
		if (continue_loop)
			continue;

		//if not in a row conflict, check for column conflicts
		for (int k = 0; k < HEIGHT; k++)
		{
			uint8_t other = current.tiles[WIDTH * k + heuristic_states[i][0]];
			if (other == i || other == 255 || other == BLANK_VAL)  //dont compare to self
				continue;

			//other tile target y is less than our y and our target y is greater than other tile y, then we are in linear conflict
			if (heuristic_states[other][3] < heuristic_states[i][1] && heuristic_states[i][3] < heuristic_states[other][1]) {
				conflict_moves += 2;
				conflict_tiles.push_back(i);
				conflict_tiles.push_back(other);
				break;
			}

			//other way around
			if (heuristic_states[i][3] < heuristic_states[other][1] && heuristic_states[other][3] < heuristic_states[i][1]) {
				conflict_moves += 2;
				conflict_tiles.push_back(i);
				conflict_tiles.push_back(other);
				break;
			}
		}
	}
	conflict_tiles.clear();
	return conflict_moves;
}

void iterative_deepening_search(State initial, State& goal) {
	stack<State> path = *new stack<State>();
	for (int i = 1; i <= LIMIT; i++)
	{
		cout << "Height: " << i << endl;
		int result = breadth_first_search(initial, goal, path, i);
		if (result == 2) {
			print_result(path, i);
			return;
		}
	}
	cout << "Failed";
}

int breadth_first_search(State current, State& goal, stack<State>& path, int limit) {
	if (current == goal) {
		path.push(*new State(current.tiles));
		end_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
		return 2;
	}
	else if (limit == 0) {
		return 1;
	}
	else {
		bool cutoff = false;
		for (int i = 0; i < 4; i++) {
			if (current.Move(i)) {
				switch (breadth_first_search(current, goal, path, limit - 1))
				{
				case 1:
					cutoff = true;
					break;
				case 2:
					current.Move(State::opposite_move[i]);
					path.push(*new State(current.tiles));
					return 2;
				}
				current.Move(State::opposite_move[i]);
			}
		}
		return (cutoff ? 1 : 0);
	}
}

void print_result(stack<State>& path, int moves) {
	//print stack
	cout << "\nSolved\n\n";
	for (int i = 0; i <= moves; i++) {
		cout << path.top().to_string();
		path.pop();
	}
	cout << "\nMinimum moves to solve: " << moves << endl;
	cout << "Solved in " << end_time - start_time << "ms" << endl << endl;
}

void print_result(Node n, int nodes_searched) {
	int moves = n.g;
	stack<Node> path = *new stack<Node>();
	path.push(n);
	while (path.top().parent != NULL) {
		n = *n.parent;
		path.push(n);
	}

	cout << "\nSolved\n\n";
	while (!path.empty()) {
		cout << path.top().state.to_string();
		path.pop();
	}

	cout << "\nMinimum moves to solve: " << moves << endl;
	cout << "Solved in " << end_time - start_time << "ms" << endl;
	cout << "Searched " << nodes_searched << " nodes" << endl << endl;
}

void init() {
	ram_usage = 0; ram_limit = 4000000000; //this is in no way shape or form accurate
	num = dx = dy = estimated_moves = 0;
	conflict_tiles = *new vector<uint8_t>();
	for (int i = 0; i < NUM_TILES; i++)
		heuristic_states[i] = new int[7];
	start_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
}

//uh this is also overly complicated because I added commands
void read_input() {
	cout << "19 Game!\n\n";

	//need to loop in case the user enters an incorrect command
	while (true) {
		cout << "Commands (make sure to enter any commands before beginning search):\n\n";
		cout << "                    {x}    - Just enter number from 1 to 6 for corresponding assignment problem. (starts search)\n";
		cout << "{a} {b} {c} ... {s} {t}    - Enter 20 numbers to create a board. Numbers must be 0-19. (starts search)\n";
		cout << "          algorithm {a}    - Specify search algorithm where a is |a*| or |dfs| without the |\n";
		cout << "            threads {n}    - Specify number of threads to use for search (Only applies to A* and default is 2)\n";
		cout << endl;

		//first read a single line and see if it is a command or a problem number
		string s;
		getline(cin, s);

		int num;
		if (s.size() == 1) {   //if string size is 1 then user problably just entered a problem number
			num = s[0] - '0';
			if (num >= 0 && num < 7) {
				prob_num = num;
				return;
			}
		}
		if (s.size() >= 10) {   //the string size must be at least 10 if they entered this command
			if (s.substr(0, 9) == "algorithm") {  //check if the first 9 characters match the command
				s = s.substr(10, s.size() - 10);

				//now check the arguments
				if (s.size() == 2)
					if (s.substr(0, 2) == "a*") {
						algorithm = 0;
						cout << "\nAlgorithm set to A*\n\n";
						continue;
					}
				if (s.size() == 3)
					if (s.substr(0, 3) == "dfs") {
						algorithm = 1;
						cout << "\nAlgorithm set to Depth First Search\n\n";
						continue;
					}
			}
		}
		//similar to last command
		if (s.size() >= 7) {
			if (s.substr(0, 7) == "threads") {
				s = s.substr(7, s.size() - 7);

				try {
					int num = stoi(s);
					if (num > 0 && num < 1001) {  //limit number of threads
						threads = num;
						cout << "\nNumber of threads set to " << threads << " (Recommended: 2)\n\n";
						continue;
					}
					else {
						cout << "\n\nInvalid number of threads, keep the number between 1 and 1000 (2 is probably best)\n\n\n";
						continue;
					}
					throw 1;
				}
				catch (exception e) {
				}
			}
		}

		//if the user did not enter any of the prior commands then try to make a problem from the input
		try {
			custom_problem = new uint8_t[ARR_SIZE];
			int char_index = 0;
			int string_arr_index = 0;
			string* s_split = new string[NUM_TILES];

			//please forgive me for this
			custom_problem[0] = 255; custom_problem[1] = 255; custom_problem[4] = 255; custom_problem[5] = 255;
			custom_problem[6] = 255; custom_problem[7] = 255; custom_problem[10] = 255; custom_problem[11] = 255;
			custom_problem[24] = 255; custom_problem[25] = 255; custom_problem[28] = 255; custom_problem[29] = 255;
			custom_problem[30] = 255; custom_problem[31] = 255; custom_problem[34] = 255; custom_problem[35] = 255;
			//that just sets the corners of the puzzle (outside the cross) to the flag value 255


			//need to start another loop and keep reading lines until enough numbers have been read to create a puzzle
			while (true) {
				//loop through and split the string entered (manually because there is no split function for strings in c++ for some reason)
				for (int i = 0; i < s.length(); i++)
				{
					if (s[i] == ' ') {  //use spaces as delimeter
						if (i == char_index) {
							char_index++;
							continue;
						}
						s_split[string_arr_index] = s.substr(char_index, i - char_index);
						int test = stoi(s_split[string_arr_index]);  //test to make sure integers are being entered
						string_arr_index++;
						if (string_arr_index >= NUM_TILES)
							break;
						char_index = i + 1;
					}
				}
				if (string_arr_index >= NUM_TILES)
					break;

				s_split[string_arr_index] = s.substr(char_index, s.length() - char_index);
				int test = stoi(s_split[string_arr_index]);  //test to make sure integers are being entered
				string_arr_index++;
				char_index = 0;

				if (string_arr_index >= NUM_TILES)
					break;

				//if there havent been any errors and not enough numbers have been entered to create a puzzle, read another line
				getline(cin, s);
			}

			if (string_arr_index > NUM_TILES) {
				cout << "Too many numbers entered\n";
				continue;
			}

			//now convert the string array into the tile array of uint8_t that will be used for the State class
			int problem_index = 0;
			for (int i = 0; i < string_arr_index; i++) {
				num = stoi(s_split[i]);
				while (custom_problem[problem_index] == 255)  //skip the corner squares with the flag value from earlier
					problem_index++;

				custom_problem[problem_index] = num;
				problem_index++;
			}
			return;
		}
		catch (exception e) {
			cout << "\nUnrecognized command or error reading data: " << e.what() << endl;
			continue;
		}

		cout << "\n\nUnrecognized command!\n\n\n";
	}
}

uint8_t** create_problems() {
	uint8_t* sample = new uint8_t[ARR_SIZE]{  //10 moves
		255, 255, 2, 1, 255, 255,
		255, 255, 6, 3, 255, 255,
		4, 11, 5, 7, BLANK_VAL, 8,
		10, 12, 13, 14, 15, 9,
		255, 255, 16, 17, 255, 255,
		255, 255, 18, 19, 255, 255 };
	uint8_t* hw1 = new uint8_t[ARR_SIZE]{  //6 moves
		255, 255, 2, 1, 255, 255,
		255, 255, 6, 3, 255, 255,
		4, 5, 12, 7, 8, 9,
		10, 11, 13, 17, 14, 15,
		255, 255, BLANK_VAL, 16, 255, 255,
		255, 255, 18, 19, 255, 255 };
	uint8_t* hw2 = new uint8_t[ARR_SIZE]{  //24 moves
		255, 255, 2, 1, 255, 255,
		255, 255, 6, 7, 255, 255,
		5, 3, BLANK_VAL, 17, 8, 9,
		4, 10, 12, 11, 14, 15,
		255, 255, 13, 16, 255, 255,
		255, 255, 18, 19, 255, 255 };
	uint8_t* hw3 = new uint8_t[ARR_SIZE]{  //30 moves
		255, 255, 2, 1, 255, 255,
		255, 255, 6, 7, 255, 255,
		BLANK_VAL, 4, 12, 17, 8, 9,
		3, 5, 10, 11, 14, 15,
		255, 255, 13, 16, 255, 255,
		255, 255, 18, 19, 255, 255 };
	uint8_t* hw4 = new uint8_t[ARR_SIZE]{  //44 moves
		255, 255, 1, 6, 255, 255,
		255, 255, 2, BLANK_VAL, 255, 255,
		5, 3, 12, 7, 17, 15,
		4, 10, 13, 16, 9, 8,
		255, 255, 18, 14, 255, 255,
		255, 255, 19, 11, 255, 255 };
	uint8_t* hw5 = new uint8_t[ARR_SIZE]{  //62 moves
		255, 255, 1, 6, 255, 255,
		255, 255, 2, 7, 255, 255,
		4, 13, 10, 15, 9, 12,
		3, BLANK_VAL, 5, 16, 17, 8,
		255, 255, 18, 19, 255, 255,
		255, 255, 11, 14, 255, 255 };
	uint8_t* hw6 = new uint8_t[ARR_SIZE]{  //80 moves (uses too much memory to solve)
		255, 255, 9, 1, 255, 255,
		255, 255, 7, 6, 255, 255,
		4, 18, BLANK_VAL, 2, 17, 12,
		3, 13, 5, 19, 10, 8,
		255, 255, 11, 14, 255, 255,
		255, 255, 16, 15, 255, 255 };

	uint8_t* goal = new uint8_t[ARR_SIZE]{
		255, 255, BLANK_VAL, 1, 255, 255,
		255, 255, 2, 3, 255, 255,
		4, 5, 6, 7, 8, 9,
		10, 11, 12, 13, 14, 15,
		255, 255, 16, 17, 255, 255,
		255, 255, 18, 19, 255, 255 };

	return new uint8_t * [8]{ sample, hw1, hw2, hw3, hw4, hw5, hw6, goal };
}

void cleanup(uint8_t**& test_problems) {
	for (int i = 0; i < 8; i++)
		delete test_problems[i];
	delete[] test_problems;

	for (int i = 0; i < NUM_TILES; i++)
		delete heuristic_states[i];
	delete[] heuristic_states;
	delete[] custom_problem;
}


//This was an attempt at making the heuristic more accurate
//the theory was that if two adjacent tiles in the center 2x2 that cut off a side of the cross were both in their correct final position
//AND any of the tiles in the section that was cut off were out of place
//then in theory the current state should require one of these tiles to be moved out of place and then back into place
//so add 2 to estimated_moves
//it also checked to make sure the blank was not in the section that was cut off

//the code seemed correct for this but when implemented, i was no longer getting accurate solutions
//so this must not be correct

/*int center_square_check(State& current) {
	int x = 2;
	int y = 2;

	//heuristic_states[current.tiles[WIDTH * y + x]][6]  this checks the manhattan distance for tile at coordinates x,y

	//if the blank is not in the cut off section and the manhattan distance is 0 for the two center tiles that cut off this section
	//then check if the manhattan distance is greater than 0 for any of the cut off tiles
	if (heuristic_states[0][1] > 1 && heuristic_states[current.tiles[WIDTH * y + x]][6] == 0 && heuristic_states[current.tiles[WIDTH * y + x + 1]][6] == 0)
		if (heuristic_states[current.tiles[WIDTH * (y - 1) + x]][6] > 0 || heuristic_states[current.tiles[WIDTH * (y - 1) + x + 1]][6] > 0 ||
			heuristic_states[current.tiles[WIDTH * (y - 2) + x]][6] > 0 || heuristic_states[current.tiles[WIDTH * (y - 2) + x + 1]][6] > 0)
			return 2;

	y = 3;
	if (heuristic_states[0][1] < 4 && heuristic_states[current.tiles[WIDTH * y + x]][6] == 0 && heuristic_states[current.tiles[WIDTH * y + x + 1]][6] == 0)
		if (heuristic_states[current.tiles[WIDTH * (y + 1) + x]][6] > 0 || heuristic_states[current.tiles[WIDTH * (y + 1) + x + 1]][6] > 0 ||
			heuristic_states[current.tiles[WIDTH * (y + 2) + x]][6] > 0 || heuristic_states[current.tiles[WIDTH * (y + 2) + x + 1]][6] > 0)
			return 2;

	y = 2;
	if (heuristic_states[0][0] > 1 && heuristic_states[current.tiles[WIDTH * y + x]][6] == 0 && heuristic_states[current.tiles[WIDTH * (y + 1) + x]][6] == 0)
		if (heuristic_states[current.tiles[WIDTH * y + x - 1]][6] > 0 || heuristic_states[current.tiles[WIDTH * (y + 1) + x - 1]][6] > 0 ||
			heuristic_states[current.tiles[WIDTH * y + x - 2]][6] > 0 || heuristic_states[current.tiles[WIDTH * (y + 1) + x - 2]][6] > 0)
			return 2;

	x = 3;
	if (heuristic_states[0][0] < 4 && heuristic_states[current.tiles[WIDTH * y + x]][6] == 0 && heuristic_states[current.tiles[WIDTH * (y + 1) + x]][6] == 0)
		if (heuristic_states[current.tiles[WIDTH * y + x + 1]][6] > 0 || heuristic_states[current.tiles[WIDTH * (y + 1) + x + 1]][6] > 0 ||
			heuristic_states[current.tiles[WIDTH * y + x + 2]][6] > 0 || heuristic_states[current.tiles[WIDTH * (y + 1) + x + 2]][6] > 0)
			return 2;

	return 0;
}*/