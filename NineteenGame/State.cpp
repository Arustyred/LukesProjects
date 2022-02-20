#include "State.h"
#include <cstdint>
#include <ostream>
#include <string>
#include <cstring>
#include <iostream>

using namespace std;

State::State() {}

State::State(uint8_t* _tiles) {
	tiles = new uint8_t[SIZE];
	memcpy(tiles, _tiles, SIZE * sizeof(_tiles[0]));

	//create a copy of the tiles and find the position of the blank
	for (int i = 0; i < SIZE; i++)
		if (_tiles[i] == BLANK_VALUE) {
			blank_pos = i;
			return;
		}
}

State::State(uint8_t* _tiles, uint8_t _blank_pos) : State(_tiles)
{
	blank_pos = _blank_pos;
}

bool State::CanMove(int direction)
{
	switch (direction) {
	case 0: //up
		return blank_pos - 6 > 0 && tiles[blank_pos - 6] != 255;
	case 1: //down
		return blank_pos + 6 < SIZE && tiles[blank_pos + 6] != 255;
	case 2: //left
		return tiles[blank_pos - 1] != 255 && blank_pos != 18;
	case 3: //right
		return tiles[blank_pos + 1] != 255 && blank_pos != 17;
	}
	return false;
}

bool State::Move(int direction)
{
	if (CanMove(direction)) {
		uint8_t new_pos = 0;
		switch (direction) {
		case 0: //up
			new_pos = blank_pos - 6;
			break;
		case 1: //down
			new_pos = blank_pos + 6;
			break;
		case 2: //left
			new_pos = blank_pos - 1;
			break;
		case 3: //right
			new_pos = blank_pos + 1;
			break;
		}


		//swap
		swap(tiles[blank_pos], tiles[new_pos]);
		blank_pos = new_pos;
		return true;
	}

	return false;
}

//overly complicated to_string method that manually formats the string
string State::to_string()
{
	string s;
	for (int i = 0; i < SIZE; i++)
	{
		if (tiles[i] == 255)
			continue;

		//this is probably a very slow way of printing since each value in tiles array is converted to a string
		//but its the only way i could think of printing uint8_t
		string tostr = tiles[i] == BLANK_VALUE ? " 0" : std::to_string(tiles[i]);

		if (i % 6 == 2 && i != 14 && i != 20)  //manual formatting because the format documentation is complicated in c++ for some reason
			s += "      ";

		s += tostr.length() == 2 ? tostr : " " + tostr;
		if (i % 6 == 5 || (i % 6 == 3 && i != 15 && i != 21)) //more formatting :(
			s += "\n";
		else
			s += " ";
	}
	//s += "blank position: " + std::to_string(blank_pos);
	s += "\n\n";
	return s;
}

const string State::to_string(const State state)
{
	string s;
	for (int i = 0; i < SIZE; i++)
	{
		if (state.tiles[i] == 255)
			continue;

		//this is probably a very slow way of printing since each value in tiles array is converted to a string
		//but its the only way i could think of printing uint8_t
		string tostr = state.tiles[i] == BLANK_VALUE ? " 0" : std::to_string(state.tiles[i]);

		if (i % 6 == 2 && i != 14 && i != 20)  //manual formatting because the format documentation is complicated in c++ for some reason
			s += "      ";

		s += tostr.length() == 2 ? tostr : " " + tostr;
		if (i % 6 == 5 || (i % 6 == 3 && i != 15 && i != 21)) //more formatting :(
			s += "\n";
		else
			s += " ";
	}
	//s += "blank position: " + std::to_string(state.blank_pos);
	s += "\n\n";
	return s;
}

int State::operator<(const State& other_state) const
{
	for (int i = 0; i < SIZE; i++)
		if (tiles[i] != other_state.tiles[i])
			return tiles[i] < other_state.tiles[i];
	return 0;
}

//compare two states
int State::operator==(const State& other_state) const
{
	if (other_state.blank_pos != blank_pos)
		return 0;
	int total = 0;
	for (int i = 0; i < SIZE; i++)  //compare arrays
		total += abs(other_state.tiles[i] - tiles[i]);
	return total == 0;
}

State::~State() {
	//I cant delete this because i think the const states as keys in the map still refer to the tiles array of deleted states
	//so if i delete the tiles array then things break

	//delete[] tiles;
}