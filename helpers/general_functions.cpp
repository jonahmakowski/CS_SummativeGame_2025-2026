#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <math.h>
#include <stdio.h>

#include "headers/helpers.hpp"

// Removes newline characters from a string
void remove_newline_from_string(char str[]) {
    str[strcspn(str, "\n")] = 0;
    str[strcspn(str, "\r")] = 0;
}

// Using a keybind struct, checks if the key pressed in the event is one of the keycodes in the keybind
bool pressing_keybind(Keybind keybind, ALLEGRO_EVENT ev) {
    for (int i = 0; i < 20; i++) {
        if (keybind.keycodes[i] == -1) {
            break;
        } else if (keybind.keycodes[i] == ev.keyboard.keycode) {
            return true;
        }
    }
    return false;
}

// Gets the position adjusted for the camera
Vector2i camera_fixed_position(Vector2i position) {
    Vector2i camera_fixed_pos = {position.x + camera.position.x, position.y + camera.position.y};
    return camera_fixed_pos;
}

// Draws an image adjusted for the camera position and DELETE_ME
void draw(ALLEGRO_BITMAP *image, Vector2i position, Vector2 scale, int rotation) {
    Vector2i fixed_pos = camera_fixed_position(position);
    draw_scaled_image(image, fixed_pos, scale, rotation);
}

// Uses the draw function to draw an Object struct
void draw(Object obj) {
    draw(obj.image, obj.position, obj.scale, obj.rotation_degrees);
}

// Draws a panel and its children
void draw(Panel panel) {
    draw_rectangle_rounded(panel.top_left, panel.bottom_right, PANEL_ROUNDING, panel.color);
    if (panel.has_border) {
        draw_rectangle_rounded_outline(panel.top_left, panel.bottom_right, PANEL_ROUNDING, panel.border_color, panel.border_thickness);
    }
    if (currently_clicking(panel) && panel.is_button) {
        draw_rectangle_rounded(panel.top_left, panel.bottom_right, PANEL_ROUNDING, BUTTON_HOVER_COLOR);
    }
    draw_text(*panel.font, panel.text_color, 
        {panel.top_left.x + (panel.bottom_right.x - panel.top_left.x) / 2, panel.top_left.y + (panel.bottom_right.y - panel.top_left.y) / 2}, 
        panel.text);
    
    if (panel.has_tooltip && currently_clicking(panel)) {
        tooltip_panel.top_left = {mouse_pos.x + 15, mouse_pos.y + 15};

        int longest_index = 0;
        for (int i = 1; i < panel.tooltip_lines; i++) {
            if (strlen(panel.tooltip_text[i]) > strlen(panel.tooltip_text[longest_index])) {
                longest_index = i;
            }
        }

        int text_width = al_get_text_width(panel.font->font, panel.tooltip_text[longest_index]);
        int text_height = panel.font->size;
        int padding = 10;

        tooltip_panel.bottom_right.x = mouse_pos.x + 15 + text_width + 2 * padding;
        tooltip_panel.bottom_right.y = mouse_pos.y + 15 + (text_height * panel.tooltip_lines) + 2 * padding;
        
        tooltip_panel.color = LIGHT_GRAY;
        tooltip_panel.exists = true;

        tooltip_panel.has_border = true;
        tooltip_panel.border_color = BLACK;
        tooltip_panel.border_thickness = 2.0f;

        strcpy(tooltip_panel.text, "");

        tooltip_text_panels_count = panel.tooltip_lines;

        for (int i = 0; i < panel.tooltip_lines; i++) {
            Panel* text_panel = &tooltip_text_panels[i];
            text_panel->top_left = {tooltip_panel.top_left.x + padding, tooltip_panel.top_left.y + padding + (i * text_height) - text_height};
            text_panel->bottom_right = {tooltip_panel.bottom_right.x - padding, tooltip_panel.top_left.y + padding + ((i + 1) * text_height) - text_height};
            text_panel->color = TRANSPARENT;
            text_panel->exists = true;
            snprintf(text_panel->text, sizeof(text_panel->text), "%s", panel.tooltip_text[i]);
            text_panel->top_left.y += text_height;
            text_panel->bottom_right.y += text_height;
        }
    }
}

// Draws the tooltip if it exists
void draw_tooltip() {
    if (tooltip_panel.exists) {

        if (tooltip_panel.bottom_right.x > get_display_width()) {
            int overflow = tooltip_panel.bottom_right.x - get_display_width();
            tooltip_panel.top_left.x -= overflow;
            tooltip_panel.bottom_right.x -= overflow;

            for (int i = 0; i < tooltip_text_panels_count; i++) {
                tooltip_text_panels[i].top_left.x -= overflow;
                tooltip_text_panels[i].bottom_right.x -= overflow;
            }
        }

        draw(tooltip_panel);
        for (int i = 0; i < tooltip_text_panels_count; i++) {
            draw(tooltip_text_panels[i]);
        }
    }
}

// Draws a tower using its object
void draw(Tower tower) {
    draw(tower.object);
}

// Draws an enemy using its object
void draw(Enemy enemy) {
    draw(enemy.object);
}

// Draws a projectile using its object
void draw(Projectile projectile) {
    draw(projectile.object);
}

// Draws a housespawn using its object
void draw(HouseSpawn housespawn) {
    draw(housespawn.object);
}

// Gets the mouse position adjusted for the camera
Vector2i get_mouse_pos() {
    Vector2i cam_mouse_pos = {mouse_pos.x - camera.position.x, mouse_pos.y - camera.position.y};
    return cam_mouse_pos;
}

// Updates an object's position based on its velocity
void update_position(Object &obj) {
    obj.position.x += obj.velocity.x;
    obj.position.y += obj.velocity.y;
}

// Updates a projectile's position based on its velocity
void update_position(Projectile &projectile) {
    update_position(projectile.object);
}

// Normalizes a Vector2 to have a maximum absolute value of 1
Vector2 normalize(Vector2 vec) {
    float max = fmax(fabs(vec.x), fabs(vec.y));
    Vector2 normalized = {vec.x / max, vec.y / max};
    return normalized;
}

// Gets the direction vector from one point to another, normalized
Vector2 get_direction_to(Vector2i from, Vector2i to) {
    Vector2 direction;
    direction.x = to.x - from.x;
    direction.y = to.y - from.y;
    return normalize(direction);
}

// Updates the camera's position based on its velocity
void update_camera_position(bool limit, Vector2i limit_top_left, Vector2i limit_bottom_right) {
    camera.position.x += camera.velocity.x;
    camera.position.y += camera.velocity.y;
    
    if (!limit) {
        return;
    }

    camera.position.x = fmin(limit_top_left.x, camera.position.x);
    camera.position.y = fmin(limit_top_left.y, camera.position.y);

    camera.position.y = fmax(limit_bottom_right.y, camera.position.y);
    camera.position.x = fmax(limit_bottom_right.x, camera.position.x);
}

// Gets the distance between two Vector2i points using the Pythagorean theorem
float distance_between(Vector2i a, Vector2i b) {
    return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

// Gets the size of an object adjusted for its scale and camera DELETE_ME
Vector2 get_object_size(Object obj) {
    Vector2 size;
    size.x = al_get_bitmap_width(obj.image) * obj.scale.x;
    size.y = al_get_bitmap_height(obj.image) * obj.scale.y;
    return size;
}

// Converts a Vector2 to a Vector2i by casting the float values to integers
Vector2i vector2_to_vector2i(Vector2 vec) {
    Vector2i veci;
    veci.x = (int)vec.x;
    veci.y = (int)vec.y;
    return veci;
}

// Checks if two objects are colliding
bool is_colliding(Object a, Object b) {
    Vector2i a_size = vector2_to_vector2i(get_object_size(a));
    Vector2i b_size = vector2_to_vector2i(get_object_size(b));
    
    Vector2i a_position_upper_left = {a.position.x - a_size.x / 2, a.position.y - a_size.y / 2};
    Vector2i b_position_upper_left = {b.position.x - b_size.x / 2, b.position.y - b_size.y / 2};

    return (a_position_upper_left.x < b_position_upper_left.x + b_size.x &&
            a_position_upper_left.x + a_size.x > b_position_upper_left.x &&
            a_position_upper_left.y < b_position_upper_left.y + b_size.y &&
            a_position_upper_left.y + a_size.y > b_position_upper_left.y);
}

// Checks if a point is within an object's boundaries
bool is_within(Object obj, Vector2i point) {
    Vector2i size = vector2_to_vector2i(get_object_size(obj));

    Vector2i obj_position_upper_left = camera_fixed_position({obj.position.x - size.x / 2, obj.position.y - size.y / 2});

    return (point.x >= obj_position_upper_left.x && point.x <= obj_position_upper_left.x + size.x &&
            point.y >= obj_position_upper_left.y && point.y <= obj_position_upper_left.y + size.y);
}

// Checks if a point is within a panel's boundaries
bool is_within(Panel panel, Vector2i point) {
    return (point.x >= panel.top_left.x && point.x <= panel.bottom_right.x &&
            point.y >= panel.top_left.y && point.y <= panel.bottom_right.y);
}

// Checks if a point is within a tower's boundaries
bool is_within(Tower tower, Vector2i point) {
    return is_within(tower.object, point);
}

// Checks if a point is within an enemy's boundaries
bool is_within(Enemy enemy, Vector2i point) {
    return is_within(enemy.object, point);
}

// Checks if the mouse is currently within a panel
bool currently_clicking(Panel panel) {
    return is_within(panel, mouse_pos);
}

// Checks if the mouse is currently within an object
bool currently_clicking(Object obj) {
    return is_within(obj, get_mouse_pos());
}

// Checks if the mouse is currently within a tower
bool currently_clicking(Tower tower) {
    return is_within(tower.object, get_mouse_pos());
}

// Checks if the mouse is currently within an enemy
bool currently_clicking(Enemy enemy) {
    return is_within(enemy.object, get_mouse_pos());
}

// Removes an Upgrade from an array of Upgrades by index and shifts the rest down
void remove_object_from_array(Upgrade arr[], int &count, int index) {
    if (index < 0 || index >= count) {
        printf("Error: Index out of bounds in remove_object_from_array\n");
        return;
    }
    for (int i = index; i < count - 1; i++) {
        arr[i] = arr[i + 1];
    }
    count--;
}

// Removes a Projectile from an array of Projectiles by index and shifts the rest down
void remove_object_from_array(Projectile arr[], int &count, int index) {
    if (index < 0 || index >= count) {
        printf("Error: Index out of bounds in remove_object_from_array\n");
        return;
    }
    for (int i = index; i < count - 1; i++) {
        arr[i] = arr[i + 1];
    }
    count--;
}

// Removes an Enemy from an array of Enemies by index and shifts the rest down
void remove_object_from_array(Enemy arr[], int &count, int index) {
    if (index < 0 || index >= count) {
        printf("Error: Index out of bounds in remove_object_from_array\n");
        return;
    }
    for (int i = index; i < count - 1; i++) {
        arr[i] = arr[i + 1];
    }
    count--;
}

// Multiplies a Vector2i by a float multiplier
Vector2i multiply_vector(Vector2i vec, float multiplier) {
    Vector2i result;
    result.x = (int)(vec.x * multiplier);
    result.y = (int)(vec.y * multiplier);
    return result;
}

// Multiplies a Vector2 by a float multiplier
Vector2 multiply_vector(Vector2 vec, float multiplier) {
    Vector2 result;
    result.x = vec.x * multiplier;
    result.y = vec.y * multiplier;
    return result;
}

// Checks if a MapTile is in an array of MapTiles
int index_of_in_array(MapTile tile, MapTile arr[], int count) {
    for (int i = 0; i < count; i++) {
        if (arr[i].position.x == tile.position.x && arr[i].position.y == tile.position.y) {
            return i;
        }
    }
    return -1;
}

// Checks if a Vector2i point is in an array of MapTiles
int index_of_in_array(Vector2i point, MapTile arr[], int count) {
    for (int i = 0; i < count; i++) {
        if (arr[i].position.x == point.x && arr[i].position.y == point.y) {
            return i;
        }
    }
    return -1;
}

// Checks if a Vector2i point is in an array of Vector2i
bool is_in_array(Vector2i point, Vector2i arr[], int count) {
    for (int i = 0; i < count; i++) {
        if (arr[i].x == point.x && arr[i].y == point.y) {
            return true;
        }
    }
    return false;
}

// Checks if a Vector2i point is in an array of Vector2i and returns its index
int index_of_in_array(Vector2i point, Vector2i arr[], int count) {
    for (int i = 0; i < count; i++) {
        if (arr[i].x == point.x && arr[i].y == point.y) {
            return i;
        }
    }
    return -1;
}

// Subtracts one Vector2i from another
Vector2i subtract_vector(Vector2i a, Vector2i b) {
    Vector2i result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

// Gets a text representation of a Keybind (What keys will trigger the keybind)
void keybind_text(char buffer[], Keybind keybind) {
    buffer[0] = '\0';
    for (int i = 0; i < 20; i++) {
        if (keybind.keycodes[i] == -1) {
            break;
        }
        const char* key_name = al_keycode_to_name(keybind.keycodes[i]);
        strcat(buffer, key_name);
        if (keybind.keycodes[i + 1] != -1) {
            strcat(buffer, "/");
        }
    }
}

// Resets the globals related to the game state
void reset_game_globals() {
    player_coins = INITIAL_MONEY;
    player_health = 100;
    active_enemies_count = 0;
    active_projectiles_count = 0;
    active_towers_count = 0;
    active_housespawn_count = 0;
    camera.position = {0, 0};
    camera.velocity = {0, 0};
}
