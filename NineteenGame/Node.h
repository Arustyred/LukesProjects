#pragma once

#include <string>
#include "State.h"
using namespace std;

class Node
{
public:
	Node();
	Node(State _state, Node* _parent, int _g, int _h);

	State state;
	Node* parent;
	int g;      //cost to get to this node
	int h;      //heuristic estimating cost to get to goal
	int index;  //current index in priority queue
	bool in_frontier;

	int get_index();
	void set_index(int i);
	string to_string();


	int operator>(const Node& other_node) const;
	int operator<(const Node& other_node) const;
	int operator==(const Node& other_node) const;
};