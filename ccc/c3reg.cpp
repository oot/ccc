#include "stdafx.h"
#include "c3reg.h"
#include <stdio.h>

BOOL c3reg_get_int( HKEY hkey, LPCSTR subkey, LPCSTR value_name, int* ret_value )
{
	int ret;
	HKEY key;
	DWORD disposition;
	UINT result;
	DWORD size = sizeof(LONG);

	ret = RegCreateKeyExA(hkey, subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &key, &disposition);
	if ( ret != ERROR_SUCCESS) 
		return FALSE;

	ret = RegQueryValueExA(key, value_name, 0, NULL, (LPBYTE)&result, &size);
	if ( ret != ERROR_SUCCESS)
	{
		RegCloseKey(key);
		return FALSE;
	}

	(*ret_value) = result;
	RegCloseKey(key);

	return TRUE;
}

BOOL c3reg_set_int( HKEY hkey, LPCSTR subkey, LPCSTR value_name, int set_value )
{
	int ret;
	HKEY key;
	DWORD disposition;

	ret = RegCreateKeyExA(hkey, subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_SET_VALUE, NULL, &key, &disposition);
	if ( ret != ERROR_SUCCESS) 
		return FALSE;

	ret = RegSetValueExA(key, value_name, 0, REG_DWORD, (LPBYTE)&set_value, sizeof(UINT));
	if ( ret != ERROR_SUCCESS)
	{
		RegCloseKey(key);
		return FALSE;
	}

	RegCloseKey(key);
	return TRUE;
}

BOOL c3reg_get_string( HKEY hkey, LPCSTR subkey, LPCSTR value_name, char* ret_value, size_t size )
{
	int ret;
	HKEY key;
	DWORD disposition;
	DWORD type;
	DWORD sz = size - 1;

	ret = RegCreateKeyExA(hkey, subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &key, &disposition);
	if ( ret != ERROR_SUCCESS) 
		return FALSE;

	ret = RegQueryValueExA(key, value_name, 0, &type, (BYTE*)ret_value, &sz);
	if ( ret != ERROR_SUCCESS || type != REG_SZ)
	{
		RegCloseKey(key);
		return FALSE;
	}

	RegCloseKey(key);

	return TRUE;
}

BOOL c3reg_set_string( HKEY hkey, LPCSTR subkey, LPCSTR value_name, const char* set_value)
{
	int ret;
	HKEY key;
	DWORD disposition;

	ret = RegCreateKeyExA(hkey, subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_SET_VALUE, NULL, &key, &disposition);
	if ( ret != ERROR_SUCCESS) 
		return FALSE;

	ret = RegSetValueExA(key, value_name, 0, REG_SZ, (BYTE*)set_value, strlen(set_value));
	if ( ret != ERROR_SUCCESS)
	{
		RegCloseKey(key);
		return FALSE;
	}

	RegCloseKey(key);
	return TRUE;
}

BOOL c3reg_get_float( HKEY hkey, LPCSTR subkey, LPCSTR value_name, float* ret_value )
{
	char str[32] = {0};

	if(c3reg_get_string(hkey, subkey, value_name, str, sizeof(str)))
	{
		(*ret_value) = (float)atof(str);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL c3reg_set_float( HKEY hkey, LPCSTR subkey, LPCSTR value_name, float set_value )
{
	char str[32] = {0};
	size_t size = sizeof(str);

	_snprintf(str, sizeof(str), "%f", set_value);

	if(c3reg_set_string(hkey, subkey, value_name, str))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}