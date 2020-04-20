#pragma once

enum {ID = 0, DESCRIPTION, SCRIPT, MODEL_FILE, TRAJ_FILE};
enum {RATIO_LEFT = 3, RATIO_RIGHT, RATIO_BOTTOM, RATIO_TOP, TRACK_ENTITY_ID, TRACK_ENTITY_TYPE};
static const char* Default_TableField[] = { "id", "description", "script" };
static const char* Weapon_TableField[] = { "id", "description", "script", "model_file", "traj_file" };
static const char* Effect_TableField[] = { "id", "description", "script", "model_file" };
static const char* RedArmy_TableField[] = { "id", "description", "script", "model_file", "traj_file" };
static const char* BlueArmy_TableField[] = { "id", "description", "script", "model_file", "traj_file" };
static const char* AllyArmy_TableField[] = { "id", "description", "script", "model_file", "traj_file" };
static const char* Stationary_TableField[] = { "id", "description", "script", "model_file" };
static const char* WarReporter_TableField[] = { "id", "description", "script", "ratio_left", "ratio_right", "ratio_bottom", "ratio_top", "track_entity_id", "track_entity_type" };
