#include <stdio.h>

#include "helpers.hpp"

float time_since_launch = 0; // for the help popup

bool help_menu_active = false;

// If the player hasn't moved the camera for a certain amount of time, show a help popup
// The popup tells the player how to move the camera, and how to open the help menu
void do_help_popup() {
    if (!moved) {
        time_since_launch += 1.0/FPS;
    }

    if (time_since_launch >= HELP_POPUP_TIME && !moved) {
        Panel help_popup;
        help_popup.top_left = {get_display_width()/2 - 500, get_display_height()/2 - 50};
        help_popup.bottom_right = {get_display_width()/2 + 500, get_display_height()/2 + 50};
        help_popup.color = LIGHT_GRAY;
        strcpy(help_popup.text, "Use WASD or arrow keys to move the camera (Press H to show the help menu)");
        draw(help_popup);
    }
}

// Function to toggle the help menu on and off (required because we can't modify the help_menu_active variable directly in main.cpp)
void toggle_help_menu() {
    help_menu_active = !help_menu_active;
    ui_force_hidden = !ui_force_hidden;
}

// Shows all the keybinds to the player in a help menu
void help_menu_draw() {
    if (help_menu_active) {
        int keybind_count = 8;
        Keybind *keybinds[] = {
            &help_menu,
            &move_up,
            &move_down,
            &move_left,
            &move_right,
            &range_circle_toggle,
            &next_wave,
            &toggle_ui
        };
        const char *keybind_names[] = {
            "Help Menu (This menu)",
            "Move Up",
            "Move Down",
            "Move Left",
            "Move Right",
            "Toggle Range Circles",
            "Start Next Wave",
            "Toggle UI (and close upgrade menu)"
        };

        const int extra_size = 425;

        Panel help_menu_panel;
        help_menu_panel.top_left = {get_display_width()/2 - 500, get_display_height()/2 - (keybind_count * 30)/2 - 40 - extra_size/2};
        help_menu_panel.bottom_right = {get_display_width()/2 + 500, get_display_height()/2 + (keybind_count * 30)/2 + 40 + extra_size/2};
        help_menu_panel.color = LIGHT_GRAY;

        draw(help_menu_panel);

        Panel title_panel;
        title_panel.top_left = {help_menu_panel.top_left.x + 10, help_menu_panel.top_left.y + 10};
        title_panel.bottom_right = {help_menu_panel.bottom_right.x - 10, help_menu_panel.top_left.y + 50};
        title_panel.color = YELLOW;
        strcpy(title_panel.text, "Help Menu - Keybinds");
        draw(title_panel);

        for (int i = 0; i < keybind_count; i++) {
            Panel panel;
            panel.top_left = {help_menu_panel.top_left.x + 10, help_menu_panel.top_left.y + 50 + i * 30};
            panel.bottom_right = {help_menu_panel.bottom_right.x - 10, help_menu_panel.top_left.y + 80 + i * 30};
            panel.color = TRANSPARENT;
            strcpy(panel.text, keybind_names[i]);
            strcat(panel.text, ": ");
            char keybind_text_buffer[200] = "";
            keybind_text(keybind_text_buffer, *keybinds[i]);
            strcat(panel.text, keybind_text_buffer);
            draw(panel);
        }

        Panel instructions;
        instructions.top_left = {help_menu_panel.top_left.x + 10, help_menu_panel.top_left.y + 70 + keybind_count * 30};
        instructions.bottom_right = {help_menu_panel.bottom_right.x - 10, help_menu_panel.top_left.y + 120 + keybind_count * 30};
        instructions.color = YELLOW;
        strcpy(instructions.text, "Game Instructions");
        draw(instructions);

        al_draw_multiline_text(default_font.font, BLACK, instructions.top_left.x + 10, instructions.bottom_right.y + 20, help_menu_panel.bottom_right.x - help_menu_panel.top_left.x - 20, default_font.size + 5,
            ALLEGRO_ALIGN_LEFT,
            "Welcome to Tower Defence Game!\n"
            "As the title suggests, the goal of the game is to outlast the waves of enemies by attacking them as they travel down a path towards your base by using towers that attack them. If you run out of health, game over. You gain coins gained by defeating enemies, which you can use to buy towers and upgrades.\n"
            "A typical game goes like this: \n"
            "Wave prep: Use your starting coins to buy towers on the tiles with “Sale” signs. You have the choice of upgrading them, or saving up your money for the next round.\n"
            "Wave: Sit back and watch the chaos unfold. If your towers need some assistance, use your coins that you earned by defeating enemies to buy upgrades, which you can open the menu by clicking on the tower you want to upgrade. \n"
            "Repeat these two steps until you have run out of waves.\n"
        );
    }
}
