#include "headers/helpers.hpp"

float time_since_launch = 0; // for the help popup

void do_help_popup() {
    if (!moved) {
        time_since_launch += 1.0/FPS;
    }

    if (time_since_launch >= HELP_POPUP_TIME && !moved) {
        Panel help_popup;
        help_popup.top_left = {get_display_width()/2 - 400, get_display_height()/2 - 50};
        help_popup.bottom_right = {get_display_width()/2 + 400, get_display_height()/2 + 50};
        help_popup.color = LIGHT_GRAY;
        strcpy(help_popup.text, "Use WASD or Arrow Keys to Move the Camera");
        draw(help_popup);
    }
}
