#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <math.h>

#include "headers/helpers.hpp"

Panel card_buttons[5];

// Tower card drawing function
void draw_card() {
    int card_type = rand() % avalible_deck_count;
    new_tower(current_hand[current_hand_count], avalible_deck[card_type]);
    current_hand_count++;
}

// Draws a card to the screen
void display_card(int index, int width, int upper_y) {
    Vector2i upper_left;
    Vector2i bottom_right;
    int screen_width = get_display_width();
    
    // getting the x of Vectori of the first card (hopefully)
    int first_card_x = ((float)screen_width/2) - (((float)current_hand_count / 2) * (width + 10));
    
    upper_left.x = first_card_x + index * (width + 10);
    upper_left.y = upper_y;

    bottom_right.x = width + upper_left.x;
    bottom_right.y = get_display_height();

    // Drawing the actual card

    /*
    What's included:
        Main panel -- Background
        Tower Name
        Picture of the tower
        Stats
        Price
        Button to purchase
    */

    // Background panel
    Panel card_panel;
    card_panel.top_left = upper_left;
    card_panel.bottom_right = bottom_right;
    card_panel.color = BLUE_GREY;
    strcpy(card_panel.text, "");

    // Tower Name
    Panel tower_name;
    tower_name.top_left = {upper_left.x + 10, upper_left.y + 10};
    tower_name.bottom_right = {bottom_right.x - 10, upper_left.y + 60};
    tower_name.color = card_panel.color;
    tower_name.text_color = BLACK;
    strcpy(tower_name.text, current_hand[index].name);

    // Tower Image
    current_hand[index].object.position = {(upper_left.x + bottom_right.x) / 2, upper_left.y + 140};

    // Stats Panel
    Panel stats_panel;
    stats_panel.top_left = {upper_left.x + 10, upper_left.y + 220};
    stats_panel.bottom_right = {bottom_right.x - 10, bottom_right.y - 125};
    stats_panel.color = LIGHT_GRAY;
    strcpy(stats_panel.text, "");

    Panel damage_stat;
    damage_stat.top_left = {stats_panel.top_left.x, stats_panel.top_left.y};
    damage_stat.bottom_right = {stats_panel.bottom_right.x, stats_panel.bottom_right.y - 100};
    damage_stat.color = TRANSPARENT;
    snprintf(damage_stat.text, sizeof(damage_stat.text), "Damage: %.2f", current_hand[index].damage);

    Panel reload_stat;
    reload_stat.top_left = {stats_panel.top_left.x, stats_panel.top_left.y + 50};
    reload_stat.bottom_right = {stats_panel.bottom_right.x, stats_panel.bottom_right.y - 60};
    reload_stat.color = TRANSPARENT;
    snprintf(reload_stat.text, sizeof(reload_stat.text), "Reload Time: %.2f s", current_hand[index].reload_time);

    Panel range_stat;
    range_stat.top_left = {stats_panel.top_left.x, stats_panel.top_left.y + 100};
    range_stat.bottom_right = {stats_panel.bottom_right.x, stats_panel.bottom_right.y - 20};
    range_stat.color = TRANSPARENT;
    snprintf(range_stat.text, sizeof(range_stat.text), "Range: %d", current_hand[index].range);

    // Price Panel
    Panel price_panel;
    price_panel.top_left = {upper_left.x + 10, bottom_right.y - 115};
    price_panel.bottom_right = {bottom_right.x - 10, bottom_right.y - 60};
    price_panel.color = YELLOW;
    snprintf(price_panel.text, sizeof(price_panel.text), "Price: %d", current_hand[index].price);

    // Buy Button
    Panel buy_button;
    buy_button.top_left = {upper_left.x + 10, bottom_right.y - 50};
    buy_button.bottom_right = {bottom_right.x - 10, bottom_right.y - 10};
    if (player_coins < current_hand[index].price) {
        buy_button.color = RED;
    } else {
        buy_button.color = GREEN;
        buy_button.is_button = true;
    }
    snprintf(buy_button.text, sizeof(buy_button.text), "Buy");
    card_buttons[index] = buy_button;

    // Draw all components
    draw(card_panel);
    draw(tower_name);
    draw_scaled_image(current_hand[index].object.image, current_hand[index].object.position, multiply_vector(current_hand[index].object.scale, 0.75f), current_hand[index].object.rotation_degrees);
    draw(stats_panel);
    draw(damage_stat);
    draw(reload_stat);
    draw(price_panel);
    draw(range_stat);
    draw(card_buttons[index]);
}

// Draws the entire hand of cards
void display_hand() {
    if (!show_ui || ui_force_hidden) {
        return;
    }

    int card_width = 300;

    for (int i = 0; i < current_hand_count; i++) {
        display_card(i, card_width, UPPER_CARD_Y);
    }

    for (int i = 0; i < current_hand_count; i++) {
        Panel tooltip_card_panel;
        int first_card_x = ((float)get_display_width() / 2) - (((float)current_hand_count / 2) * (card_width + 10));
    
        tooltip_card_panel.top_left.x = first_card_x + i * (card_width + 10);
        tooltip_card_panel.top_left.y = UPPER_CARD_Y;

        tooltip_card_panel.bottom_right.x = card_width + tooltip_card_panel.top_left.x;
        tooltip_card_panel.bottom_right.y = get_display_height();

        strcpy(tooltip_card_panel.text, "");

        tooltip_card_panel.color = TRANSPARENT;
        tooltip_card_panel.exists = true;
        tooltip_card_panel.has_tooltip = true;
        tooltip_card_panel.tooltip_lines = 1;
        snprintf(tooltip_card_panel.tooltip_text[0], sizeof(tooltip_card_panel.tooltip_text[0]), "%s", current_hand[i].description);

        draw(tooltip_card_panel);
    }

    // If a card has been purchased, display a message to place the tower
    if (to_place.object.exists) {
        Panel place_panel;
        place_panel.top_left = {get_display_width() / 2 - 300, 0};
        place_panel.bottom_right = {get_display_width() / 2 + 300, 80};
        place_panel.color = ORANGE;
        snprintf(place_panel.text, sizeof(place_panel.text), "Place your %s tower!", to_place.name);
        draw(place_panel);
    }
}

// Handles buying a card when clicked
void handle_buy_card(ALLEGRO_EVENT ev) {
    if (!show_ui || ui_force_hidden) {
        return;
    }
    
    for (int i = 0; i < current_hand_count; i++) {
        if (currently_clicking(card_buttons[i])) {
            if (player_coins >= current_hand[i].price) {
                if (to_place.object.exists) {
                    printf("You are already placing a tower!\n");
                    break;
                }

                player_coins -= current_hand[i].price;
                
                to_place = current_hand[i];
                to_place.object.exists = true;
                
                // Remove the purchased card from the hand
                for (int j = i; j < current_hand_count - 1; j++) {
                    current_hand[j] = current_hand[j + 1];
                }
                current_hand_count--;

                draw_card();
            } else {
                printf("Not enough coins to purchase %s\n", current_hand[i].name);
            }
            break;
        }
    }
}
