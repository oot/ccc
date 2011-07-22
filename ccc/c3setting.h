#pragma once

struct _c3setting_private;

typedef struct _c3setting
{
	int start_pos_x;
	int start_pos_y;

	int is_keychange_set;

	int win_distance_docking;
	
	float win_size_border;

	int win_size_width;
	int win_size_height;
	int win_size_round;

	float button_size_round;
	float button_size_xy;
	float button_distance_far;
	float button_distance_near;

	float label_size_scale;
	float label_size_pen;
	float label_distance_shadow;

	_c3setting_private* pvars;

} c3setting;

void c3setting_initialize(c3setting* setting);
void c3setting_finalize(c3setting* setting);

void c3setting_load_from_reg(c3setting* setting, const char* reg_subkey);
void c3setting_save_to_reg(const c3setting* setting, const char* reg_subkey);

void c3setting_load_from_file(c3setting* setting, const char* path);
void c3setting_save_to_file(const c3setting* setting, const char* path);