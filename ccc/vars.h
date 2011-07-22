#pragma once

#define RGB_BLACK	Color::MakeARGB(255, 0,0,0)
#define RGB_ORGNGE	Color::MakeARGB(255, 255,0,0)
#define RGB_BLUE	Color::MakeARGB(255, 0,0,255)
#define RGB_GRAY_LT	Color::MakeARGB(255, 122,122,122)
#define RGB_GRAY	Color::MakeARGB(255, 66,66,66)
#define RGB_WHITE	Color::MakeARGB(255, 255, 255, 255)

#define RGB_BACKGROUND			RGB_GRAY
#define RGB_BN_ACTIVE			RGB_ORGNGE
#define RGB_BN_NORMAL			RGB_BLACK

#define RGB_LABEL_NORMAL		RGB_WHITE
#define RGB_LABEL_NORMAL_ACTIVE	RGB_WHITE
#define RGB_LABEL_HOVER			RGB_ORGNGE
#define RGB_LABEL_HOVER_ACTIVE	RGB_BLACK

#define RGB_LABEL_SHA			RGB_WHITE
#define RGB_LABEL_SHA_ACTIVE	RGB_WHITE
#define RGB_LABEL_SHA_DN		RGB_WHITE
#define RGB_LABEL_SHA_DN_ACTIVE	RGB_WHITE


const int CCC_MOVER_KCHG	= 0;
const int CCC_MOVER_CAPS	= 1;
const int CCC_MOVER_NUML	= 2;
const int CCC_MOVER_CLOSE	= 3;
const int CCC_MOVER_BK		= 4;

const int CCC_BN_INDEPENDENT	= 0;
const int CCC_BN_LEFT			= 1;
const int CCC_BN_RIGHT			= 2;
const int CCC_BN_MIDDLE			= 3;

