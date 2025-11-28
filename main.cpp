// to run:
// clang++ main.cpp -o ./target/main $(pkg-config allegro-5 allegro_main-5 allegro_font-5 allegro_primitives-5 allegro_image-5 allegro_ttf-5 --libs --cflags); ./target/main

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "helpers/structs.cpp"
#include "helpers/globals.cpp"
#include "helpers/rewritten_allegro_crap.cpp"
#include "helpers/colors.cpp"
#include "helpers/functions.cpp"
#include "helpers/keybinds.cpp"
#include "helpers/specific.cpp"
#include "helpers/enemies_and_towers.cpp"

Tower test_tower;
Enemy test_enemy;

// Run every frame
void frame_logic() {
    fill_screen(WHITE);

    draw(test_tower);
    draw(test_enemy);

    draw_range_circle(test_tower);
}

// Handling the keyboard input ev is the allegro event
void handle_keyboard_input_down(ALLEGRO_EVENT ev) {
    
}

// Run when a key is released
void handle_keyboard_input_up(ALLEGRO_EVENT ev) {
    
}

// Run when mouse input is detected
void handle_mouse_input(ALLEGRO_EVENT ev) {
    
}

// Run to setup the game before the main loop
// Does things like defining sprites for game objects
void setup_game() {

}

int main(int argc, char *argv[]) {    
    if (!init_allegro()) {
        return -1;
    }

    srand(time(0));

    setup_game();

    new_snowman(test_tower);
    test_tower.object.scale = {0.5f, 0.5f};
    test_tower.object.position = {get_display_width() / 2, get_display_height() / 2};

    new_penguin(test_enemy);
    test_enemy.object.scale = {0.15f, 0.15f};
    test_enemy.object.position = {(get_display_width() / 2) + 100, (get_display_height() / 2) + 100};

    al_start_timer(timer);
    al_get_mouse_cursor_position(&mouse_pos.x, &mouse_pos.y);
    while(true) {
        ALLEGRO_EVENT ev;

        al_wait_for_event(event_queue, &ev);

        // Handling Input
        if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouse_pos = {ev.mouse.x, ev.mouse.y};
        }

        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            handle_mouse_input(ev);
        }

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            handle_keyboard_input_down(ev);
        }

        if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            handle_keyboard_input_up(ev);
        }

        // Run frame logic
        if(ev.type == ALLEGRO_EVENT_TIMER) {
            frame_logic();
            update();
        }

        check_for_exit(ev);
    }

    al_destroy_display(display);
    return 0;
}

