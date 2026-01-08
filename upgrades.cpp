#include <stdio.h>

#include "helpers.hpp"

struct UpgradeButton {
    Panel panel;
    Upgrade* upgrade;
};

UpgradeButton upgrade_buttons[15];
int upgrade_buttons_count = 0;

// Load the upgrades from upgrades.txt into the global upgrades array
// Only done once at the start of the game
bool parse_upgrades() {
    FILE *file = fopen("upgrades.txt", "r");
    if (file == nullptr) {
        printf("Failed to open upgrades.txt\n");
        return false;
    }

    char line[500];
    int required_upgrade_id;
    char garbage[20];
    upgrades_count = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '/' && line[1] == '/') {
            continue; // Skip comment lines
        }

        Upgrade* upgrade = &upgrades[upgrades_count];

        sscanf(line, "%d%[ ]%d%[ ]%d%[ ]%[^\t]%[\t]%[^\t]%[\t]%d%[ ]%f%[ ]%d%[ ]%d%[ ]%d%[ ]%f%[ ]%f",
               (int*)&upgrade->for_tower,
               garbage,
               &upgrade->upgrade_id,
               garbage,
               &required_upgrade_id,
               garbage,
               upgrade->name,
               garbage,
               upgrade->description,
               garbage,
               &upgrade->price,
               garbage,
               &upgrade->reload_time,
               garbage,
               &upgrade->range,
               garbage,
               &upgrade->damage,
               garbage,
               &upgrade->projectile_area_of_effect,
               garbage,
               &upgrade->slowing_amount,
               garbage,
               &upgrade->slowing_duration
        );

        if (required_upgrade_id != -1) {
            for (int i = 0; i < upgrades_count; i++) {
                if (upgrades[i].upgrade_id == required_upgrade_id && upgrades[i].for_tower == upgrade->for_tower) {
                    upgrade->required_upgrade = &upgrades[i];
                    break;
                }
            }

            upgrade->requires_upgrade = true;
        } else {
            upgrade->required_upgrade = nullptr;
            upgrade->requires_upgrade = false;
        }

        upgrades_count++;
    }

    fclose(file);
    return true;
}

// Checks if a tower can apply a given upgrade
bool can_apply_upgrade(Tower* tower, Upgrade* upgrade) {
    if (tower == nullptr || upgrade == nullptr) {
        printf("Tower or upgrade pointer is null in can_apply_upgrade\n");
        return false;
    }

    if (upgrade->for_tower != tower->type) {
        return false;
    }

    if (upgrade->requires_upgrade) {
        if (upgrade->required_upgrade == nullptr) {
            printf("WARN: required_upgrade is nullptr in can_apply_upgrade\n");
            return false;
        }

        if (upgrade->required_upgrade->upgrade_id != tower->best_upgrade_id) {
            return false;
        }
    } else {
        if (tower->best_upgrade_id != -1) {
            return false;
        }
    }

    return true;
}

// Apply an upgrade to a tower
void apply_upgrade(Tower &tower, Upgrade upgrade) {
    tower.damage += upgrade.damage;
    tower.reload_time -= upgrade.reload_time;
    tower.range += upgrade.range;
    tower.projectile_area_of_effect += upgrade.projectile_area_of_effect;
    tower.slowing_amount += upgrade.slowing_amount;
    tower.slowing_duration += upgrade.slowing_duration;

    tower.best_upgrade_id = upgrade.upgrade_id;
}

// Draw the upgrades available for the selected tower
void draw_upgrades(Vector2i top_left, Vector2i bottom_right) {
    int current_y = top_left.y;
    const int upgrade_height = 50;
    const int upgrade_spacing = 10;

    upgrade_buttons_count = 0;

    for (int i = 0; i < upgrades_count; i++) {
        Upgrade* upgrade = &upgrades[i];
        if (!can_apply_upgrade(card_menu_tower, upgrade)) {
            continue;
        }

        Panel upgrade_panel;

        upgrade_panel.top_left = {top_left.x, current_y};
        upgrade_panel.bottom_right = {bottom_right.x, current_y + upgrade_height};
        upgrade_panel.color = LIGHT_GRAY;
        upgrade_panel.has_border = true;
        upgrade_panel.border_color = BLACK;
        upgrade_panel.border_thickness = 2.0f;
        upgrade_panel.exists = true;
        upgrade_panel.is_button = true;
        snprintf(upgrade_panel.text, sizeof(upgrade_panel.text), "%s - %d coins", upgrade->name, upgrade->price);

        upgrade_panel.has_tooltip = true;
        upgrade_panel.tooltip_lines = 5;
        snprintf(upgrade_panel.tooltip_text[0], sizeof(upgrade_panel.tooltip_text[0]), "%s", upgrade->description);
        snprintf(upgrade_panel.tooltip_text[1], sizeof(upgrade_panel.tooltip_text[1]), "Damage: +%d, Range: +%d", upgrade->damage, upgrade->range);
        snprintf(upgrade_panel.tooltip_text[2], sizeof(upgrade_panel.tooltip_text[2]), "Reload Time: -%.2f s", upgrade->reload_time);
        snprintf(upgrade_panel.tooltip_text[3], sizeof(upgrade_panel.tooltip_text[3]), "Explosion Radius: +%d", upgrade->projectile_area_of_effect);
        snprintf(upgrade_panel.tooltip_text[4], sizeof(upgrade_panel.tooltip_text[4]), "Slowing: +%.2f%% for +%.2f s", upgrade->slowing_amount, upgrade->slowing_duration);

        draw(upgrade_panel);

        upgrade_buttons[upgrade_buttons_count].panel = upgrade_panel;
        upgrade_buttons[upgrade_buttons_count].upgrade = upgrade;
        upgrade_buttons_count++;

        current_y += upgrade_height + upgrade_spacing;
    }
}

// Handle clicks on upgrade buttons
void do_upgrade_buttons(ALLEGRO_EVENT ev) {
    for (int i = 0; i < upgrade_buttons_count; i++) {
        Panel* button = &upgrade_buttons[i].panel;
        Upgrade* upgrade = upgrade_buttons[i].upgrade;

        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1) {
            if (currently_clicking(*button)) {
                if (player_coins >= upgrade->price) {
                    apply_upgrade(*card_menu_tower, *upgrade);
                    player_coins -= upgrade->price;
                    printf("Applied upgrade %s to tower %s\n", upgrade->name, card_menu_tower->name);
                } else {
                    printf("Not enough coins to apply upgrade %s\n", upgrade->name);
                }
            }
        }
    }
}
