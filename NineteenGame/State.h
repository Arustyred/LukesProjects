#pragma once
#include <cstdint>
#include <ostream>

using namespace std;

class State
{
public:
	static const int SIZE;
	static const int BLANK_VALUE;
	static int* opposite_move;
	uint8_t* tiles;  //i thought using uint8_t instead of int would be clever and save memory but idk if it actually helped much
	uint8_t blank_pos;

	State();
	State(uint8_t* _tiles);
	State(uint8_t* _tiles, uint8_t _blank_pos);
	bool CanMove(int direction);
	bool Move(int direction);

	string to_string();
	const static string to_string(const State state);

	~State();

	int operator<(const State& other_state) const;
	int operator==(const State& other_state) const;
};