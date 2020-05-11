#pragma once

enum {ID = 0, DESCRIPTION, SCRIPT, POSX, POSY, POSZ, HEADING, PITCH, ROLL, SCALEX, SCALEY, SCALEZ, PARENT, VISIBLE, MODEL_FILE, SOUND_FILE, TRAJ_FILE};
enum {RATIO_LEFT = 3, RATIO_RIGHT, RATIO_BOTTOM, RATIO_TOP, TRACK_ENTITY};

static const char* TableField[] = { "id", "description", "script", "x", "y", "z", "heading", "pitch", "roll", "sx", "sy", "sz", "parent",
"visible", "model_file", "sound_file", "traj_file" };

static const char* WarReporter_TableField[] = { "id", "description", "script", "ratio_left", "ratio_right", "ratio_bottom", "ratio_top", "track_entity" };

static const char* Chs_TableField[] = { "编号", "描述", "脚本", "位置X", "位置Y", "位置Z", "旋转H", "旋转P", "旋转R", "缩放X", "缩放Y", "缩放Z", "挂载实体",
"可见性", "模型文件", "音效文件", "轨迹文件" };

static const char* Chs_WarReporter_TableField[] = { "编号", "描述", "脚本", "视口左侧比例", "视口右侧比例", "视口底部比例", "视口顶部比例", "跟踪实体" };
