#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <math.h>

#include "helpers.hpp"

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

// Draws an image adjusted for the camera position and zoom
void draw(ALLEGRO_BITMAP *image, Vector2i position, Vector2 scale) {
    Vector2i fixed_pos = camera_fixed_position(position);
    draw_scaled_image(image, fixed_pos, {scale.x * camera.zoom, scale.y * camera.zoom});
}

// Uses the draw function to draw an Object struct
void draw(Object obj) {
    draw(obj.image, obj.position, obj.scale);
}

// Draws a child of a panel relative to its parent panel
void draw_child_panel(Panel panel, Vector2i parent_top_left) {
    Vector2i adjusted_top_left = {panel.top_left.x + parent_top_left.x, panel.top_left.y + parent_top_left.y};
    Vector2i adjusted_bottom_right = {panel.bottom_right.x + parent_top_left.x, panel.bottom_right.y + parent_top_left.y};
    draw_rectangle_rounded(adjusted_top_left, adjusted_bottom_right, PANEL_ROUNDING, panel.color);
    draw_text(panel.font, panel.text_color, 
        {adjusted_top_left.x + (adjusted_bottom_right.x - adjusted_top_left.x) / 2, adjusted_top_left.y + (adjusted_bottom_right.y - adjusted_top_left.y) / 2}, 
        panel.text);
}

// Draws a panel and its children
void draw(Panel panel) {
    draw_rectangle_rounded(panel.top_left, panel.bottom_right, PANEL_ROUNDING, panel.color);
    draw_text(panel.font, panel.text_color, 
        {panel.top_left.x + (panel.bottom_right.x - panel.top_left.x) / 2, panel.top_left.y + (panel.bottom_right.y - panel.top_left.y) / 2}, 
        panel.text);

    for (int i = 0; i < panel.child_count; i++) {
        draw_child_panel(*panel.children[i], panel.top_left);
    }
}

// Adds a child panel to a parent panel
int add_child_panel(Panel &parent, Panel *child) {
    if (parent.child_count < 20) {
        parent.children[parent.child_count] = child;
        parent.child_count++;
        return 0;
    }
    printf("Error: Maximum child panels reached\n");
    return -1;
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
void update_camera_position() {
    camera.position.x -= camera.velocity.x;
    camera.position.y -= camera.velocity.y;
}

// Gets the distance between two Vector2i points using the Pythagorean theorem
float distance_between(Vector2i a, Vector2i b) {
    return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

// Gets the size of an object adjusted for its scale and camera zoom
Vector2 get_object_size(Object obj) {
    Vector2 size;
    size.x = al_get_bitmap_width(obj.image) * obj.scale.x * camera.zoom;
    size.y = al_get_bitmap_height(obj.image) * obj.scale.y * camera.zoom;
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
    return is_within(panel, get_mouse_pos());
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

// Applies an upgrade to a tower and removes it from the possible upgrades
void apply_upgrade(Tower &tower, int upgrade_index) {
    tower.damage = (int)(tower.damage * tower.possible_upgrades[upgrade_index].damage_multiplier);
    tower.fire_rate = (int)(tower.fire_rate * tower.possible_upgrades[upgrade_index].fire_rate_multiplier);
    tower.range = (int)(tower.range * tower.possible_upgrades[upgrade_index].range_multiplier);

    remove_object_from_array(tower.possible_upgrades, tower.possible_upgrades_count, upgrade_index);
}

// Draws the range circle of a tower
void draw_range_circle(Tower tower) {
    Vector2i tower_pos = camera_fixed_position(tower.object.position);
    draw_circle_outline(tower_pos, tower.range * camera.zoom, BLUE, 2.0f);
}

// Gets the enemies that towers can shoot at
void current_shots() {
    for (int i = 0; i < active_towers_count; i++) {
        for (int j = 0; j < active_enemies_count; j++) {
            if (distance_between(active_towers[i]->object.position, active_enemies[j]->object.position) <= active_towers[i]->range) {
                printf("Tower %s can hit a target\n", active_towers[i]->name);
            }
        }
    }
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


void shoot_projectile(Tower tower, Enemy* target_enemy) {
    Projectile* new_projectile = new Projectile();
    new_projectile->object.image = load_image("images/sun.png");
    new_projectile->object.scale = {0.2f, 0.2f};
    new_projectile->object.position = tower.object.position;
    new_projectile->object.exists = true;

    new_projectile->speed = PROJECTILE_SPEED;
    new_projectile->damage = tower.damage;

    new_projectile->target = target_enemy;

    Vector2 direction = get_direction_to(tower.object.position, target_enemy->object.position);
    Vector2i velocity = vector2_to_vector2i(multiply_vector(direction, new_projectile->speed));
    new_projectile->object.velocity = velocity;

    add_projectile(new_projectile);
}

// Recalculates all projectiles' velocities towards their targets
void recalculate_projectiles() {
    for (int i = 0; i < active_projectiles_count; i++) {
        Projectile* proj = active_projectiles[i];
        if (proj == nullptr) {
            printf("Projectile pointer is null, skipping\n");
            continue;
        }
        Vector2 direction = get_direction_to(proj->object.position, proj->target->object.position);
        Vector2i velocity = vector2_to_vector2i(multiply_vector(direction, proj->speed));
        proj->object.velocity = velocity;
        update_position(proj->object);
    }
}
