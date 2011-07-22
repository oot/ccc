#include "stdafx.h"
#include "c3setting.h"
#include "c3reg.h"

struct _c3setting_private
{
	float win_size_scale;
	float win_size_border;

	int win_size_width;
	int win_size_height;
	int win_size_round;

	float button_size_round;
	float button_size_xy;
	float button_distance_far;
	float button_distance_near;

	float label_size_pen;
	float label_distance_shadow;
};

void c3setting_apply_scale(c3setting* setting)
{
	setting->win_size_border = setting->pvars->win_size_scale * setting->pvars->win_size_border;

	setting->win_size_width = (int)(setting->pvars->win_size_scale * setting->pvars->win_size_width);
	setting->win_size_height = (int)(setting->pvars->win_size_scale * setting->pvars->win_size_height);
	setting->win_size_round = (int)(setting->pvars->win_size_scale * setting->pvars->win_size_round);

	setting->button_size_round = setting->pvars->win_size_scale * setting->pvars->button_size_round;
	setting->button_size_xy = setting->pvars->win_size_scale * setting->pvars->button_size_xy;
	setting->button_distance_far = setting->pvars->win_size_scale * setting->pvars->button_distance_far;
	setting->button_distance_near = setting->pvars->win_size_scale * setting->pvars->button_distance_near;

	setting->label_size_pen = setting->label_size_scale * setting->pvars->label_size_pen;
	setting->label_distance_shadow = setting->label_size_scale * setting->pvars->label_distance_shadow;
}

void c3setting_initialize( c3setting* setting )
{
	setting->pvars = (_c3setting_private*)calloc(1, sizeof(_c3setting_private));

	setting->start_pos_x = 0;
	setting->start_pos_y = 0;

	setting->is_keychange_set = 0;
	setting->win_distance_docking = 0;
	setting->label_size_scale = 0.5;

	setting->pvars->win_size_scale = 1;
	setting->pvars->win_size_border = 2;

	setting->pvars->win_size_width = 84;
	setting->pvars->win_size_height = 22;
	setting->pvars->win_size_round = 4;

	setting->pvars->button_size_round = 4;
	setting->pvars->button_size_xy = 18;
	setting->pvars->button_distance_far = 4;
	setting->pvars->button_distance_near = 0;

	setting->pvars->label_size_pen = 3;
	setting->pvars->label_distance_shadow = 0.2f;


	c3setting_apply_scale(setting);
}

void c3setting_finalize( c3setting* setting )
{
	free(setting->pvars);
}

void c3setting_load_from_reg( c3setting* setting, const char* reg_subkey )
{
	// public vars
	c3reg_get_int(HKEY_CURRENT_USER, reg_subkey, "start-pos-x", &setting->start_pos_x);
	c3reg_get_int(HKEY_CURRENT_USER, reg_subkey, "start-pos-y", &setting->start_pos_y);
	c3reg_get_int(HKEY_CURRENT_USER, reg_subkey, "is-keychange-set", &setting->is_keychange_set);
	c3reg_get_int(HKEY_CURRENT_USER, reg_subkey, "win-distance-docking", &setting->win_distance_docking);
	setting->is_keychange_set = setting->is_keychange_set ? 1 : 0;

	// private vars
	c3reg_get_float(HKEY_CURRENT_USER, reg_subkey, "win-size-scale", &setting->pvars->win_size_scale);
	c3reg_get_float(HKEY_CURRENT_USER, reg_subkey, "win-size-border", &setting->pvars->win_size_border);

	c3reg_get_int(HKEY_CURRENT_USER, reg_subkey, "win-size-width", &setting->pvars->win_size_width);
	c3reg_get_int(HKEY_CURRENT_USER, reg_subkey, "win-size-height", &setting->pvars->win_size_height);
	c3reg_get_int(HKEY_CURRENT_USER, reg_subkey, "win-size-round", &setting->pvars->win_size_round);

	c3reg_get_float(HKEY_CURRENT_USER, reg_subkey, "button-size-round", &setting->pvars->button_size_round);
	c3reg_get_float(HKEY_CURRENT_USER, reg_subkey, "button-size-xy", &setting->pvars->button_size_xy);
	c3reg_get_float(HKEY_CURRENT_USER, reg_subkey, "button-distance-far", &setting->pvars->button_distance_far);
	c3reg_get_float(HKEY_CURRENT_USER, reg_subkey, "button-distance-near", &setting->pvars->button_distance_near);

	c3reg_get_float(HKEY_CURRENT_USER, reg_subkey, "label-size-pen", &setting->pvars->label_size_pen);
	c3reg_get_float(HKEY_CURRENT_USER, reg_subkey, "label-size-scale", &setting->label_size_scale);
	c3reg_get_float(HKEY_CURRENT_USER, reg_subkey, "label-distance-shadow", &setting->pvars->label_distance_shadow);

//	setting->pvars->win_size_scale = 10;

	c3setting_apply_scale(setting);
}


void c3setting_save_to_reg( const c3setting* setting, const char* reg_subkey )
{
	c3reg_set_int(HKEY_CURRENT_USER, reg_subkey, "start-pos-x", setting->start_pos_x);
	c3reg_set_int(HKEY_CURRENT_USER, reg_subkey, "start-pos-y", setting->start_pos_y);

	c3reg_set_int(HKEY_CURRENT_USER, reg_subkey, "is-keychange-set", setting->is_keychange_set);

	c3reg_set_int(HKEY_CURRENT_USER, reg_subkey, "win-distance-docking", setting->win_distance_docking);

	c3reg_set_float(HKEY_CURRENT_USER, reg_subkey, "win-size-scale", setting->pvars->win_size_scale);
	c3reg_set_float(HKEY_CURRENT_USER, reg_subkey, "win-size-border", setting->pvars->win_size_border);

	c3reg_set_int(HKEY_CURRENT_USER, reg_subkey, "win-size-width", setting->pvars->win_size_width);
	c3reg_set_int(HKEY_CURRENT_USER, reg_subkey, "win-size-height", setting->pvars->win_size_height);
	c3reg_set_int(HKEY_CURRENT_USER, reg_subkey, "win-size-round", setting->pvars->win_size_round);

	c3reg_set_float(HKEY_CURRENT_USER, reg_subkey, "button-size-round", setting->pvars->button_size_round);
	c3reg_set_float(HKEY_CURRENT_USER, reg_subkey, "button-size-xy", setting->pvars->button_size_xy);
	c3reg_set_float(HKEY_CURRENT_USER, reg_subkey, "button-distance-far", setting->pvars->button_distance_far);
	c3reg_set_float(HKEY_CURRENT_USER, reg_subkey, "button-distance-near", setting->pvars->button_distance_near);

	c3reg_set_float(HKEY_CURRENT_USER, reg_subkey, "label-size-pen", setting->pvars->label_size_pen);
	c3reg_set_float(HKEY_CURRENT_USER, reg_subkey, "label-size-scale", setting->label_size_scale);
	c3reg_set_float(HKEY_CURRENT_USER, reg_subkey, "label-distance-shadow", setting->pvars->label_distance_shadow);
}

