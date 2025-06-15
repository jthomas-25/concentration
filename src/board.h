/*
* Contains information about the 5 x 5 game board, which is used by various drawing functions in graphics.cpp.
* Game logic, such as the board pattern and which boxes have been played, is handled in logic.cpp.
*/
class board {
public:
    // constructor
	board();
    // returns the number of rows/columns this board has
    int get_size();
    // returns the width of the board in pixels
    int get_width();
    // returns the height of the board in pixels
    int get_height();
    // returns the width of a section of the board in pixels
    int get_box_width();
    // returns the height of a section of the board in pixels
    int get_box_height();
private:
    int size; // n x n boxes
    int width, height; // board dimensions in pixels
    int box_width, box_height; // box dimensions in pixels
};