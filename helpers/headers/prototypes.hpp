#include "globals.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

// enemies_and_towers.cpp
void draw_card();
void new_tower(Tower &tower, TowerType type);
void new_enemy(Enemy &enemy, EnemyType type);

// rewritten_allegro_crap.cpp
void draw_rectangle(Vector2i top_left, Vector2i bottom_right, ALLEGRO_COLOR color);
void draw_rectangle_rounded(Vector2i top_left, Vector2i bottom_right, float radius, ALLEGRO_COLOR color);
void draw_rectangle_rounded_outline(Vector2i top_left, Vector2i bottom_right, float radius, ALLEGRO_COLOR color, float thickness);
void draw_circle(Vector2i center, float radius, ALLEGRO_COLOR color);
void draw_triangle(Vector2i point1, Vector2i point2, Vector2i point3, ALLEGRO_COLOR color);
void draw_line(Vector2i start, Vector2i end, ALLEGRO_COLOR color, float thickness);
void draw_circle_outline(Vector2i center, float radius, ALLEGRO_COLOR color, float thickness);
void draw_image(ALLEGRO_BITMAP *image, Vector2i position);
void draw_text(Font font, ALLEGRO_COLOR color, Vector2i position, const char *text);
Vector2i get_window_size();
void draw_scaled_image(ALLEGRO_BITMAP *image, Vector2i position, Vector2 scale, int rotation = 0);
bool init_allegro();

// general_functions.cpp
void remove_newline_from_string(char str[]);
bool pressing_keybind(Keybind keybind, ALLEGRO_EVENT ev);
Vector2i camera_fixed_position(Vector2i position);
void draw(ALLEGRO_BITMAP *image, Vector2i position, Vector2 scale, int rotation);
void draw(Object obj);
void draw_child_panel(Panel panel, Vector2i parent_top_left);
void draw(Panel panel);
int add_child_panel(Panel &parent, Panel *child);
void draw(Tower tower);
void draw(Enemy enemy);
void draw(Projectile projectile);
Vector2i get_mouse_pos();
void update_position(Object &obj);
void update_position(Projectile &projectile);
Vector2 normalize(Vector2 vec);
Vector2 get_direction_to(Vector2i from, Vector2i to);
void update_camera_position(bool limit, Vector2i limit_top_left, Vector2i limit_bottom_right);
float distance_between(Vector2i a, Vector2i b);
Vector2 get_object_size(Object obj);
Vector2i vector2_to_vector2i(Vector2 vec);
bool is_colliding(Object a, Object b);
bool is_within(Object obj, Vector2i point);
bool is_within(Panel panel, Vector2i point);
bool is_within(Tower tower, Vector2i point);
bool is_within(Enemy enemy, Vector2i point);
bool currently_clicking(Panel panel);
bool currently_clicking(Object obj);
bool currently_clicking(Tower tower);
bool currently_clicking(Enemy enemy);
void remove_object_from_array(Upgrade arr[], int &count, int index);
void remove_object_from_array(Projectile arr[], int &count, int index);
void remove_object_from_array(Enemy arr[], int &count, int index);
int index_of_in_array(MapTile tile, MapTile arr[], int count);
int index_of_in_array(Vector2i point, MapTile arr[], int count);
int index_of_in_array(Vector2i point, Vector2i arr[], int count);
Vector2i subtract_vector(Vector2i a, Vector2i b);

// functions.cpp
Vector2i tile_pos_to_pixel_pos(Vector2i tile_pos);
void apply_upgrade(Tower &tower, int upgrade_index);
void draw_range_circle(Tower tower);
void current_shots();
Vector2i multiply_vector(Vector2i vec, float multiplier);
Vector2 multiply_vector(Vector2 vec, float multiplier);
void shoot_projectile(Tower tower, Enemy* target_enemy);
void recalculate_projectiles();
void draw_all_projectiles();
void draw_all_towers();
void draw_all_enemies();
void check_projectiles();
int load_tile_images();
void display_map();
void print_map();
bool is_in_array(Vector2i point, Vector2i arr[], int count);
void add_path_points_to_map(Map &map);
Map load_map(const char* file_path);
void run_enemies();
void do_ui();
void handle_button_clicks(ALLEGRO_EVENT ev);
void build_tower_on_click(ALLEGRO_EVENT ev);
void display_card(int index, int width, int upper_y);
void display_hand();

// main_menu.cpp
bool load_map_list();
void draw_main_menu();
void do_main_menu_buttons();