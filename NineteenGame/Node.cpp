#include <string>
#include "Node.h"
using namespace std;

Node::Node() {}

Node::Node(State _state, Node* _parent, int _g, int _h) {
	state = *new State(_state.tiles);
	parent = _parent;
	g = _g;
	h = _h;
	in_frontier = true;
	index = 0;
}

int Node::get_index() {
	return index;
}

void Node::set_index(int i) {
	index = i;
}

string Node::to_string() {
	return state.to_string();
}

//compare node priorities
int Node::operator>(const Node& other_node) const {
	return other_node.g + other_node.h < g + h;
}

int Node::operator<(const Node& other_node) const {
	return g + h < other_node.g + other_node.h;
}

int Node::operator==(const Node& other_node) const {
	return g + h == other_node.g + other_node.h;
}