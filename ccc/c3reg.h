#pragma once

BOOL c3reg_get_int(HKEY hkey, LPCSTR subkey, LPCSTR value_name, int* ret_value);
BOOL c3reg_set_int(HKEY hkey, LPCSTR subkey, LPCSTR value_name, int set_value);

BOOL c3reg_get_string(HKEY hkey, LPCSTR subkey, LPCSTR value_name, char* ret_value, size_t size);
BOOL c3reg_set_string(HKEY hkey, LPCSTR subkey, LPCSTR value_name, const char* set_value);

BOOL c3reg_get_float(HKEY hkey, LPCSTR subkey, LPCSTR value_name, float* ret_value);
BOOL c3reg_set_float(HKEY hkey, LPCSTR subkey, LPCSTR value_name, float set_value);