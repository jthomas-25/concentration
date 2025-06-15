#include "board.h"

board::board() {
    size = 5;
    width = 400;
    height = width;
    box_width = width / size;
    box_height = box_width;
}

int board::get_size() {
    return size;
}

int board::get_width() {
    return width;
}

int board::get_height() {
    return height;
}

int board::get_box_width() {
    return box_width;
}

int board::get_box_height() {
    return box_height;
}