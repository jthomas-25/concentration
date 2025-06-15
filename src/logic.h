#include "shape.h"
#include <stdlib.h>
#include <iostream>

// Handles the game logic
class logic {
public:
	// constructor
	logic();

	// returns the number of shape pairs the board has
	int get_total_pairs();

	// returns the shape at the given (x, y) location
	// throws an exception if either index is out of range (index < 0 || index > 4)
	Shape get_shape(int x, int y);

	// sets the given shape at the given (x, y) location
	// throws an exception if either index is out of range (index < 0 || index > 4)
	void set_shape(int x, int y, Shape shape);

	// returns true if the given (x, y) location is playable and false if not
	// throws an exception if either index is out of range (index < 0 || index > 4)
	bool is_playable(int x, int y);

	// sets the given (x, y) location to a unplayable/playable state
	// true means the location is unplayable, false means it's playable
	// throws an exception if either index is out of range (index < 0 || index > 4)
	void set_played(int x, int y, bool state);
	
	// compares the shape at the given (x, y) location to the given shape
	// returns true if the shapes match, false if not
	// throws an exception if either index is out of range (index < 0 || index > 4)
	bool compare(int x, int y, Shape shape);
	
	// determines if the game is over (i.e. all shape pairs matched) with the given number of matched pairs
	// throws an exception if the number of matched pairs is greater than the total number of pairs
	bool done(int pairs_matched);
	
	// clears the board and resets its status
	void reset();

	// randomly generates the given number of shape pairs and randomly places them on the board
	// throws an exception if the given number of pairs is out of range (num_pairs < 1 || num_pairs > max_pairs)
	void random_create(int num_pairs);

	// debug methods
	void print_shape(int x, int y);
	void print_pattern();
	void print_board();
private:
	Shape pattern[5][5]; // board layout of shapes
	bool already_played[5][5]; // board state
	int total_pairs; // number of shape pairs the board has
	int max_pairs; // the maximum number of shape pairs the board can have
};