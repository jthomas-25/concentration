#include "logic.h"

logic::logic() {
	total_pairs = 0;
	max_pairs = 12;
}

int logic::get_total_pairs() {
	return total_pairs;
}

Shape logic::get_shape(int x, int y) {
	if (x < 0 || x > 4 || y < 0 || y > 4) {
		throw std::invalid_argument("Array index out of bounds!");
	}

	return pattern[y][x];
}

void logic::set_shape(int x, int y, Shape shape) {
	if (x < 0 || x > 4 || y < 0 || y > 4) {
		throw std::invalid_argument("Array index out of bounds!");
	}

	pattern[y][x] = shape;
}

bool logic::is_playable(int x, int y) {
	if (x < 0 || x > 4 || y < 0 || y > 4) {
		throw std::invalid_argument("Array index out of bounds!");
	}

	return !already_played[y][x];
}

void logic::set_played(int x, int y, bool state) {
	if (x < 0 || x > 4 || y < 0 || y > 4) {
		throw std::invalid_argument("Array index out of bounds!");
	}

	already_played[y][x] = state;
}

bool logic::compare(int x, int y, Shape guess) {
	if (x < 0 || x > 4 || y < 0 || y > 4) {
		throw std::invalid_argument("Array index out of bounds!");
	}

	return pattern[y][x] == guess;
}

bool logic::done(int pairs_matched) {
	if (pairs_matched > total_pairs) {
		throw std::invalid_argument("The number of matched pairs is greater than the total number of pairs.");
	}

	return pairs_matched == total_pairs;
}

void logic::reset() {
	for (int y = 0; y < 5; y++) {
		for (int x = 0; x < 5; x++) {
			pattern[y][x] = Shape::null;
			already_played[y][x] = false;
		}
	}
}

void logic::random_create(int num_pairs) {
	if (num_pairs < 1 || num_pairs > max_pairs) {
		throw std::invalid_argument("The given number of pairs must be between 1 and 12.");
	}
	total_pairs = num_pairs;
	for (int i = 0; i < total_pairs; i++) {
		// get a random shape
		Shape shape = static_cast<Shape>(rand() % 6 + 1);
		// place a pair of this shape
		for (int j = 0; j < 2; j++) {
			bool placed = false;
			// randomly generate an (x, y) board location and
			// keep looping until we find an empty/available spot
			do {
				int x = rand() % 5;
				int y = rand() % 5;
				if (pattern[y][x] == Shape::null) {
					pattern[y][x] = shape;
					placed = true;
				}
			} while (!placed);
		}
	}
}

void logic::print_shape(int x, int y) {
	if (x < 0 || x > 4 || y < 0 || y > 4) {
		throw std::invalid_argument("Array index out of bounds!");
	}

	std::cout << static_cast<int>(pattern[y][x]);
}

void logic::print_pattern() {
	for (int y = 0; y < 5; y++) {
		for (int x = 0; x < 5; x++) {
			try {
				print_shape(x, y);
				std::cout << " ";
			}
			catch (std::invalid_argument &e) {
				throw e;
			}
		}
		std::cout << "\n";
	}
}

void logic::print_board() {
	for (int y = 0; y < 5; y++) {
		for (int x = 0; x < 5; x++) {
			if (already_played[y][x]) {
				std::cout << 1 << " ";
			}
			else {
				std::cout << 0 << " ";
			}
		}
		std::cout << "\n";
	}
}