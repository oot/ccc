#include "StdAfx.h"
#include "Keyboard.h"

int keyboard_is_on(int vk)
{
	int on = (GetKeyState(vk) & 0x0001) != 0 ? 1 : 0;
	
	return on;
}

int keyboard_is_change()
{
	static int is_caps;
	static int is_numlock;

	if(is_caps != keyboard_is_on(VK_CAPITAL))
	{
		is_caps = is_caps ? 0 : 1;
		return VK_CAPITAL;
	}

	if(is_caps != keyboard_is_on(VK_NUMLOCK))
	{
		is_numlock = is_numlock ? 0 : 1;
		return VK_NUMLOCK;
	}
	
	return 0;
}