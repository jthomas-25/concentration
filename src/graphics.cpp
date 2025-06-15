#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include "logic.h"
#include "board.h"
#include <iostream>

// mouse position
int mx, my;

// sets up the logic and graphics of the game
void setup_game(logic &game_logic, board &board, ALLEGRO_FONT *font, int time_played, int pairs_matched, ALLEGRO_TIMER *timer);

// draws the given 5 x 5 board
void draw_board(board &board);

/*
* reveals shapes as the player clicks on the board
* figures out which box was clicked based on mouse position, then calls draw_objects to draw the corresponding shape
* keeps track of two shapes at a time as well as their board positions
* calls logic::compare to see if the shapes match
* shape_pair_pos is a pointer to an array whose elements are: [first_shape_boardx, first_shape_boardy, second_shape_boardx, second_shape_boardy]
*/
void get_mouse_input(board &board, logic &game_logic, int *shape_pair_pos, bool &shapes_match, ALLEGRO_TIMER *show_shapes_timer, bool &show_shapes);

// finds the center of a box in pixels, given its board index
void get_box_center(int boardx, int boardy, board &board, int &box_centerx, int &box_centery);

/*
* draws the appropriate shape, given the board index of the box it's in
* calls logic::get_shape to determine which shape to draw and then calls one of the following draw functions:
* - draw_octagon
* - draw_triangle
* - draw_diamond
* - draw_rectangle
* - draw_oval
* - draw_circle
*/
void draw_objects(int boardx, int boardy, board &board, logic &game_logic);

// draws an octagon centered at the given location
void draw_octagon(int box_centerx, int box_centery);

// draws a triangle centered at the given location
void draw_triangle(int box_centerx, int box_centery);

// draws a diamond centered at the given location
void draw_diamond(int box_centerx, int box_centery);

// draws a rectangle centered at the given location
void draw_rectangle(int box_centerx, int box_centery);

// draws an oval centered at the given location
void draw_oval(int box_centerx, int box_centery);

// draws a circle centered at the given location
void draw_circle(int box_centerx, int box_centery);

// erases a pair of shapes given their board indexes and draws an 'X' over each of their locations
// shape_pair_pos is a pointer to an array whose elements are: [first_shape_boardx, first_shape_boardy, second_shape_boardx, second_shape_boardy]
void x_out_shape_pair(int *shape_pair_pos, board &board, logic &game_logic);

// erases a pair of shapes given their board indexes and resets their locations to be playable
// shape_pair_pos is a pointer to an array whose elements are: [first_shape_boardx, first_shape_boardy, second_shape_boardx, second_shape_boardy]
void hide_shape_pair(int *shape_pair_pos, board &board, logic &game_logic);

// displays "CONCENTRATION" with the given font
void draw_game_title(ALLEGRO_FONT *font);

// displays the time spent playing in seconds with the given font
void draw_timer(ALLEGRO_FONT *font, int time_played);

// displays the number of matched and unmatched shape pairs with the given font
void draw_status(ALLEGRO_FONT *font, int &pairs_matched, logic &game_logic);

// displays a "game over" message with the given font when the player wins
void game_message(bool &game_over, logic &game_logic, int pairs_matched, ALLEGRO_TIMER *timer, ALLEGRO_FONT *font);

// destroys all Allegro objects
void clean_up(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue, ALLEGRO_TIMER *timer, ALLEGRO_TIMER *show_shapes_timer, ALLEGRO_FONT *font);

int main(int argc, char **argv)
{
    logic game_logic;
    board board; // the 5 x 5 board

    // screen variables
    int width = 640;
    int height = 480;

    // gameplay variables
    bool done = false; // controls when to quit the program
    bool game_over = false; // controls when the game ends (i.e. the player wins)
    bool shapes_match = false;
    bool show_shapes = false; // works together with show_shapes_timer, "disables" mouse input while true
    int pairs_matched = 0;
    int shape_pair_pos[4]; // [first_shape_boardx, first_shape_boardy, second_shape_boardx, second_shape_boardy]
    int time_played = 0; // counts seconds played

    // Allegro variables
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL; // works together with time_played
    ALLEGRO_TIMER *show_shapes_timer = NULL; // controls how long two shapes appear before they are hidden again
    ALLEGRO_FONT *font = NULL;

    // check if Allegro can be initialized
    if (!al_init()) {
        al_show_native_message_box(NULL, "Error!", "Allegro has failed to initialize.", 0, 0, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

    // check if the primitives addon failed to be initialized
    if (!al_init_primitives_addon()) {
        al_show_native_message_box(NULL, "Error!", "Failed to initialize the primitives addon.", 0, 0, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

    // check if the mouse failed to be installed
    if (!al_install_mouse()) {
        al_show_native_message_box(NULL, "Error!", "Failed to install the mouse.", 0, 0, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

    // check if the keyboard failed to be installed
    if (!al_install_keyboard()) {
        al_show_native_message_box(NULL, "Error!", "Failed to install the keyboard.", 0, 0, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

    // allow fonts
    al_init_font_addon();
    al_init_ttf_addon();

    // create a graphics window with the given width and height
    display = al_create_display(width, height);
    // check if display creation failed
    if (!display) {
        al_show_native_message_box(NULL, "Error!", "Failed to create the display.", 0, 0, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

    event_queue = al_create_event_queue();
    // check if event queue creation failed
    if (!event_queue) {
        al_show_native_message_box(display, "Error!", "Failed to create the event queue.", 0, 0, ALLEGRO_MESSAGEBOX_ERROR);
        clean_up(display, event_queue, timer, show_shapes_timer, font);
        return -1;
    }

    timer = al_create_timer(1.0);
    // check if timer creation failed
    if (!timer) {
        al_show_native_message_box(display, "Error!", "Failed to create timer.", 0, 0, ALLEGRO_MESSAGEBOX_ERROR);
        clean_up(display, event_queue, timer, show_shapes_timer, font);
        return -1;
    }

    show_shapes_timer = al_create_timer(0.5);
    // check if timer creation failed
    if (!show_shapes_timer) {
        al_show_native_message_box(display, "Error!", "Failed to create timer.", 0, 0, ALLEGRO_MESSAGEBOX_ERROR);
        clean_up(display, event_queue, timer, show_shapes_timer, font);
        return -1;
    }

    font = al_load_font("fonts/GROBOLD.ttf", 24, 0);
    // check if font failed to load
    if (!font) {
        al_show_native_message_box(display, "Error!", "Failed to load font.", 0, 0, ALLEGRO_MESSAGEBOX_ERROR);
        clean_up(display, event_queue, timer, show_shapes_timer, font);
        return -1;
    }

    // tell Allegro to look for mouse events and send them to the queue
    al_register_event_source(event_queue, al_get_mouse_event_source());
    // tell Allegro to look for keyboard events and send them to the queue
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    // tell Allegro to look for timer events and send them to the queue
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_timer_event_source(show_shapes_timer));
    // tell Allegro to look for display events and send them to the queue
    al_register_event_source(event_queue, al_get_display_event_source(display));

    srand(time(NULL)); // init RNG

    try {
        setup_game(game_logic, board, font, time_played, pairs_matched, timer);
        while (!done) {
            ALLEGRO_EVENT ev;
            al_wait_for_event(event_queue, &ev);

            // check if the close button of the window was clicked
            if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                done = true;
            }
            // check if a mouse button was pressed
            else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                // if left mouse button was clicked
                if (ev.mouse.button & 1) {
                    // the player can't reveal more shapes if show_shapes == true
                    if (show_shapes == false) {
                        // get mouse position
                        mx = ev.mouse.x;
                        my = ev.mouse.y;
                        get_mouse_input(board, game_logic, shape_pair_pos, shapes_match, show_shapes_timer, show_shapes);
                    }
                }
            }
            // check if a key was pressed
            if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch (ev.keyboard.keycode) {
                case ALLEGRO_KEY_ESCAPE:
                    done = true;
                    break;
                }
            }
            if (ev.type == ALLEGRO_EVENT_TIMER) {
                if (ev.timer.source == timer) {
                    // 1 second has passed, increment the counter
                    time_played++;
                }
                if (ev.timer.source == show_shapes_timer) {
                    // 0.5 seconds have passed, x out the shapes if they match or hide them if they don't
                    al_stop_timer(show_shapes_timer);
                    if (shapes_match) {
                        pairs_matched++;
                        x_out_shape_pair(shape_pair_pos, board, game_logic);
                    }
                    else {
                        hide_shape_pair(shape_pair_pos, board, game_logic);
                    }
                    show_shapes = false; // "enable" mouse input
                }
            }

            draw_board(board);
            draw_timer(font, time_played);
            draw_status(font, pairs_matched, game_logic);
            game_message(game_over, game_logic, pairs_matched, timer, font);
            al_flip_display();

            // wait for the player to decide whether to play again or really end the game
            while (game_over && !done) {
                al_wait_for_event(event_queue, &ev);
                if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    done = true;
                }
                else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                    switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_Y:
                        // reset the game
                        game_over = false;
                        pairs_matched = 0;
                        time_played = 0;
                        setup_game(game_logic, board, font, time_played, pairs_matched, timer);
                        break;
                    case ALLEGRO_KEY_N:
                    case ALLEGRO_KEY_ESCAPE:
                        // end the game and quit
                        done = true;
                        break;
                    }
                }
            }
        }
    }
    catch (std::exception &e) {
        al_show_native_message_box(display, "Exception!", e.what(), 0, 0, ALLEGRO_MESSAGEBOX_ERROR);
    }

    // destroy all Allegro objects
    clean_up(display, event_queue, timer, show_shapes_timer, font);

    return 0;
}

void setup_game(logic &game_logic, board &board, ALLEGRO_FONT *font, int time_played, int pairs_matched, ALLEGRO_TIMER *timer) {
    try {
        // logic setup
        game_logic.reset();
        game_logic.random_create(12);
        // graphics setup
        al_clear_to_color(al_map_rgb(0, 0, 0));
        draw_board(board);
        draw_game_title(font);
        draw_timer(font, time_played);
        draw_status(font, pairs_matched, game_logic);
        al_flip_display();
        al_start_timer(timer);
    }
    catch (std::exception &e) {
        throw e;
    }
}

void draw_board(board &board) {
    int x = 1;
    int y = 1;
    int box_width = board.get_box_width();
    int box_height = board.get_box_height();
    ALLEGRO_COLOR color = al_map_rgb(255, 255, 255);
    // draw vertical lines
    for (int i = 0; i < board.get_size() + 1; i++) {
        al_draw_line(x + i * box_width, y, x + i * box_width, box_height * board.get_size(), color, 1);
    }
    // draw horizontal lines
    for (int i = 0; i < board.get_size() + 1; i++) {
        al_draw_line(x, y + i * box_height, box_width * board.get_size(), y + i * box_height, color, 1);
    }
}

void get_mouse_input(board &board, logic &game_logic, int *shape_pair_pos, bool &shapes_match, ALLEGRO_TIMER *show_shapes_timer, bool &show_shapes) {
    // if mouse is inside the board
    if (mx < board.get_width() && my < board.get_height()) {
        // figure out which box was clicked
        int boardx = mx / board.get_box_width();
        int boardy = my / board.get_box_height();
        try {
            // if this location is playable
            if (game_logic.is_playable(boardx, boardy)) {
                game_logic.set_played(boardx, boardy, true);
                Shape shape = game_logic.get_shape(boardx, boardy); // get the shape in this box
                // if there's a shape in this box, reveal it
                if (shape != Shape::null) {
                    // when comparing two shapes, keep track of what the first shape is and where it's located
                    static Shape first_shape = Shape::null;
                    // first shape was selected
                    if (first_shape == Shape::null) {
                        first_shape = shape;
                        shape_pair_pos[0] = boardx;
                        shape_pair_pos[1] = boardy;
                        draw_objects(boardx, boardy, board, game_logic);
                    }
                    // second shape was selected
                    else {
                        shape_pair_pos[2] = boardx;
                        shape_pair_pos[3] = boardy;
                        draw_objects(boardx, boardy, board, game_logic);
                        shapes_match = game_logic.compare(boardx, boardy, first_shape); // compare the two shapes
                        first_shape = Shape::null; // reset now that two shapes have been checked
                        // show the shapes for 0.5 seconds
                        if (!al_get_timer_started(show_shapes_timer)) {
                            al_start_timer(show_shapes_timer);
                            show_shapes = true;
                        }
                    }
                }
            }
        }
        catch (std::exception &e) {
            throw e;
        }
    }
}

void get_box_center(int boardx, int boardy, board &board, int &box_centerx, int &box_centery) {
    int box_width = board.get_box_width();
    int box_height = board.get_box_height();
    try {
        box_centerx = (box_width / 2) + (boardx * box_width);
        box_centery = (box_height / 2) + (boardy * box_height);
    }
    catch (std::exception &e) {
        throw e;
    }
}

void draw_objects(int boardx, int boardy, board &board, logic &game_logic) {
    try {
        // find the center of this box
        int box_centerx, box_centery;
        get_box_center(boardx, boardy, board, box_centerx, box_centery);
        // get the shape in this box
        Shape shape = game_logic.get_shape(boardx, boardy);
        // draw the shape
        switch (shape) {
        case Shape::octagon:
            draw_octagon(box_centerx, box_centery);
            break;
        case Shape::triangle:
            draw_triangle(box_centerx, box_centery);
            break;
        case Shape::diamond:
            draw_diamond(box_centerx, box_centery);
            break;
        case Shape::rectangle:
            draw_rectangle(box_centerx, box_centery);
            break;
        case Shape::oval:
            draw_oval(box_centerx, box_centery);
            break;
        case Shape::circle:
            draw_circle(box_centerx, box_centery);
            break;
        }
    }
    catch (std::exception &e) {
        throw e;
    }
}

void draw_octagon(int box_centerx, int box_centery) {
    // vertex positions relative to the center of the box
    int vertex_posx[8] = {0, -14, -20, -14, 0, 14, 20, 14};
    int vertex_posy[8] = {-20, -14, 0, 14, 20, 14, 0, -14};
    ALLEGRO_COLOR color = al_map_rgb(255, 0, 0);
    for (int i = 0; i < 8; i++) {
        // connect each vertex to the next
        // when i == 7, connect the first and last vertices
        int x1 = box_centerx + vertex_posx[i];
        int y1 = box_centery + vertex_posy[i];
        int x2 = box_centerx + vertex_posx[(i + 1) % 8];
        int y2 = box_centery + vertex_posy[(i + 1) % 8];
        al_draw_line(x1, y1, x2, y2, color, 1);
    }
}

void draw_triangle(int box_centerx, int box_centery) {
    int radius = 20;
    ALLEGRO_COLOR color = al_map_rgb(255, 255, 0);
    al_draw_filled_triangle(box_centerx, box_centery - radius, box_centerx - radius, box_centery + radius, box_centerx + radius, box_centery + radius, color);
}

void draw_diamond(int box_centerx, int box_centery) {
    int base = 18;
    int height = 24;
    ALLEGRO_COLOR color = al_map_rgb(255, 0, 255);
    al_draw_filled_triangle(box_centerx, box_centery - height, box_centerx - base, box_centery, box_centerx, box_centery, color);
    al_draw_filled_triangle(box_centerx, box_centery - height, box_centerx + base, box_centery, box_centerx, box_centery, color);
    al_draw_filled_triangle(box_centerx, box_centery + height, box_centerx - base, box_centery, box_centerx, box_centery, color);
    al_draw_filled_triangle(box_centerx, box_centery + height, box_centerx + base, box_centery, box_centerx, box_centery, color);
}

void draw_rectangle(int box_centerx, int box_centery) {
    int width = 30;
    int height = 20;
    ALLEGRO_COLOR color = al_map_rgb(0, 255, 0);
    al_draw_filled_rectangle(box_centerx - width, box_centery - height, box_centerx + width, box_centery + height, color);
}

void draw_oval(int box_centerx, int box_centery) {
    int rx = 30;
    int ry = 20;
    ALLEGRO_COLOR color = al_map_rgb(0, 255, 255);
    al_draw_filled_ellipse(box_centerx, box_centery, rx, ry, color);
}

void draw_circle(int box_centerx, int box_centery) {
    int radius = 20;
    ALLEGRO_COLOR color = al_map_rgb(0, 0, 255);
    al_draw_filled_circle(box_centerx, box_centery, radius, color);
}

void x_out_shape_pair(int *shape_pair_pos, board &board, logic &game_logic) {
    try {
        int box_width = board.get_box_width();
        int box_height = board.get_box_height();
        int box_centerx, box_centery;
        for (int i = 0; i < 4; i += 2) {
            get_box_center(shape_pair_pos[i], shape_pair_pos[i + 1], board, box_centerx, box_centery);
            al_draw_filled_rectangle(box_centerx - box_width / 2, box_centery - box_height / 2, box_centerx + box_width / 2, box_centery + box_height / 2, al_map_rgb(0, 0, 0));
            al_draw_line(box_centerx - box_width / 2, box_centery - box_height / 2, box_centerx + box_width / 2, box_centery + box_height / 2, al_map_rgb(255, 255, 255), 1);
            al_draw_line(box_centerx + box_width / 2, box_centery - box_height / 2, box_centerx - box_width / 2, box_centery + box_height / 2, al_map_rgb(255, 255, 255), 1);
        }
    }
    catch (std::exception &e) {
        throw e;
    }
}

void hide_shape_pair(int *shape_pair_pos, board &board, logic &game_logic) {
    try {
        int box_width = board.get_box_width();
        int box_height = board.get_box_height();
        int box_centerx, box_centery;
        for (int i = 0; i < 4; i += 2) {
            get_box_center(shape_pair_pos[i], shape_pair_pos[i + 1], board, box_centerx, box_centery);
            al_draw_filled_rectangle(box_centerx - box_width / 2, box_centery - box_height / 2, box_centerx + box_width / 2, box_centery + box_height / 2, al_map_rgb(0, 0, 0));
            game_logic.set_played(shape_pair_pos[i], shape_pair_pos[i + 1], false);
        }
    }
    catch (std::exception &e) {
        throw e;
    }
}

void draw_game_title(ALLEGRO_FONT *font) {
    int x = 100;
    int y = 430;
    ALLEGRO_COLOR color = al_map_rgb(0, 0, 0);
    al_draw_filled_rectangle(0, 401, 401, 480, al_map_rgb(255, 255, 0));
    al_draw_text(font, color, x, y, ALLEGRO_ALIGN_LEFT, "CONCENTRATION");
}

void draw_timer(ALLEGRO_FONT *font, int time_played) {
    int x = 440;
    int y = 60;
    ALLEGRO_COLOR color = al_map_rgb(255, 255, 255);
    al_draw_filled_rectangle(401, 0, 640, 401, al_map_rgb(0, 0, 255));
    al_draw_textf(font, color, x, y, ALLEGRO_ALIGN_LEFT, "Time: %i", time_played);
}

void draw_status(ALLEGRO_FONT *font, int &pairs_matched, logic &game_logic) {
    al_draw_filled_rectangle(401, 401, 640, 480, al_map_rgb(255, 0, 0));
    al_draw_textf(font, al_map_rgb(255, 255, 255), 440, 415, ALLEGRO_ALIGN_LEFT, "Score: % i", pairs_matched);
    al_draw_textf(font, al_map_rgb(255, 255, 255), 440, 445, ALLEGRO_ALIGN_LEFT, "Remaining: % i", game_logic.get_total_pairs() - pairs_matched);
}

void game_message(bool &game_over, logic &game_logic, int pairs_matched, ALLEGRO_TIMER *timer, ALLEGRO_FONT *font) {
    try {
        game_over = game_logic.done(pairs_matched);
        if (game_over) {
            al_stop_timer(timer);
            ALLEGRO_COLOR color = al_map_rgb(0, 255, 0);
            al_draw_text(font, color, 460, 120, ALLEGRO_ALIGN_LEFT, "You win!");
            al_draw_text(font, color, 420, 150, ALLEGRO_ALIGN_LEFT, "Play again? (y/n)");
        }
    }
    catch (std::exception &e) {
        throw e;
    }
}

void clean_up(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue, ALLEGRO_TIMER *timer, ALLEGRO_TIMER *show_shapes_timer, ALLEGRO_FONT *font) {
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
    al_destroy_timer(show_shapes_timer);
    al_destroy_font(font);
}
