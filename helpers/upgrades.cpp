#include <stdio.h>

#include "headers/helpers.hpp"

// Load the upgrades from upgrades.txt into the global upgrades array
// Only done once at the start of the game
bool parse_upgrades() {
    FILE *file = fopen("upgrades.txt", "r");
    if (file == nullptr) {
        printf("Failed to open upgrades.txt\n");
        return false;
    }

    char line[300];
    int required_upgrade_id;
    upgrades_count = 0;

    fgets(line, sizeof(line), file); // Skip comment line

    while (fgets(line, sizeof(line), file)) {
        Upgrade* upgrade = &upgrades[upgrades_count];

        sscanf(line, "%d %d %d %[^\t]\t%[^\t]\t%d %f %d %d %d %f %f",
               (int*)&upgrade->for_tower,
               &upgrade->upgrade_id,
               &required_upgrade_id,
               upgrade->name,
               upgrade->description,
               &upgrade->price,
               &upgrade->reload_time,
               &upgrade->range,
               &upgrade->damage,
               &upgrade->projectile_area_of_effect,
               &upgrade->slowing_amount,
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

    return true;
}


// Checks if a tower can apply a given upgrade
bool can_apply_upgrade(Tower tower, Upgrade upgrade) {
    if (upgrade.for_tower != tower.type) {
        return false;
    }

    if (upgrade.requires_upgrade) {
        if (upgrade.required_upgrade->upgrade_id != tower.best_upgrade_id) {
            return false;
        }
    } else {
        if (tower.best_upgrade_id != -1) {
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
