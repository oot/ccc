// ccc.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "ccc.h"
#include "GdiplusStarter.h"
#include "c3reg.h"
#include "c3setting.h"
#include "vars.h"
#include "keyboard.h"
#include <stdio.h>

#define MAX_LOADSTRING 100

//Gdiplus initilize

GdiplusStarter  g_gdiplus_starter;


// global vars

HINSTANCE		g_hinst;
HWND			g_hwnd_mainwin;
TCHAR			g_str_title[MAX_LOADSTRING];				
TCHAR			g_str_window_class[MAX_LOADSTRING];

int				g_mouse_over_current = CCC_MOVER_BK;
int				g_mouse_over_prev = CCC_MOVER_BK;
int				g_is_keychange_set = 0;
int				g_is_mouse_down = 0;
RECT			g_button_area[4];
c3setting		g_setting;
char			g_setting_reg_subkey[] = "Software\\xxxix\\ccc";

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK	keyboard_proc(int code, WPARAM wParam, LPARAM lParam);
void OnPaint(HWND hwnd, HDC* hdc);
void PaintFillRoundRect(Gdiplus::Graphics* g, Gdiplus::Brush* brush, float x, float y, float width, float height, float round, int type);
void PaintDrawRoundRect(Gdiplus::Graphics* g, Gdiplus::Pen* pen, float x, float y, float width, float height, int type);
void PaintLabel(Gdiplus::Graphics* g, Gdiplus::Pen* pen, float x, float y, float width, float height, int type);
BOOL DetectMouseOver(RECT* rect, POINT* pt);



int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	if(::FindWindow(_T("ccc"), NULL))
	{
		::MessageBox(NULL, _T("already exists"), _T("ccc"), MB_ICONERROR);
		return EXIT_FAILURE;
	}

#ifdef _DEBUG
	AllocConsole();
	freopen( "CONOUT$",  "wt", stdout); 
#endif

	c3setting_initialize(&g_setting);
	c3setting_load_from_reg(&g_setting, g_setting_reg_subkey);

	g_setting.win_size_width += 2;
	g_setting.win_size_height += 2;

	LoadString(hInstance, IDS_APP_TITLE, g_str_title, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CCC, g_str_window_class, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CCC));

	HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboard_proc, GetModuleHandle(NULL), 0);
	if (!hHook) return -2;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	UnhookWindowsHookEx(hHook);

	c3setting_save_to_reg(&g_setting, g_setting_reg_subkey);
	c3setting_finalize(&g_setting);

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CCC));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//MAKEINTRESOURCE(IDC_CCC);
	wcex.lpszClassName	= g_str_window_class;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_CCC));


	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   g_hinst = hInstance;
   g_hwnd_mainwin = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, g_str_window_class, g_str_title, WS_POPUP,
      CW_USEDEFAULT, 0, g_setting.win_size_width, g_setting.win_size_height, NULL, NULL, hInstance, NULL);

   if (!g_hwnd_mainwin) return 0;

// SetLayeredWindowAttributes(g_hwnd_mainwin, NULL, 111, LWA_ALPHA);

   ShowWindow(g_hwnd_mainwin, nCmdShow);
   UpdateWindow(g_hwnd_mainwin);

   return 1;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	POINT pt;
	HRGN hWndRgn;
	int screenx, screeny;
	WINDOWPOS* winpos = 0;
	RECT rect_win, rect_task;

	static RECT rect_screen;

	switch (message)
	{
	case WM_CREATE:
		hWndRgn=CreateRoundRectRgn(0, 0, 
				g_setting.win_size_width, g_setting.win_size_height, 
				g_setting.win_size_round, g_setting.win_size_round);

		SetWindowRgn(hWnd, hWndRgn, FALSE);

		screenx = GetSystemMetrics(SM_CXSCREEN);
		screeny = GetSystemMetrics(SM_CYSCREEN);

		GetWindowRect(FindWindow(_T("Shell_TrayWnd"), NULL ), &rect_task);

		rect_screen.left = 0; rect_screen.top = 0; 
		rect_screen.right = screenx; rect_screen.bottom = screeny;
		
		if(rect_task.top == 0 && rect_task.left == 0)
		{
			if(rect_task.bottom < screeny/2)
			{ //top
				rect_screen.top = rect_task.bottom;
			}
			else
			{ //left
				rect_screen.left = rect_task.right;
			}
		}
		else
		{
			if(rect_task.top < screeny/2)
			{ //right
				rect_screen.right = rect_task.left;
			}
			else
			{ //bottom
				rect_screen.bottom = rect_task.top;
			}
		}

		g_is_keychange_set = g_setting.is_keychange_set;

		rect_win.left = g_setting.start_pos_x;
		rect_win.top = g_setting.start_pos_y;
		rect_win.right = rect_win.left + g_setting.win_size_width;
		rect_win.bottom = rect_win.top + g_setting.win_size_height;

		if(rect_win.left < rect_screen.left || rect_win.right > rect_screen.right) rect_win.left = rect_screen.left;
		if(rect_win.top < rect_screen.top || rect_win.bottom > rect_screen.bottom) rect_win.top = rect_screen.top;

		SetWindowPos(hWnd, HWND_TOPMOST, rect_win.left, rect_win.top, 0, 0, SWP_NOSIZE);

		return 0;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 메뉴의 선택 영역을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_ABOUT:
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		OnPaint(hWnd, &hdc);
		EndPaint(hWnd, &ps);
		printf("paint:\n");
		break;
	case WM_DESTROY:
		GetWindowRect(hWnd,&rect_win);

		g_setting.start_pos_x = rect_win.left;
		g_setting.start_pos_y = rect_win.top;

		PostQuitMessage(0);
		break;
	case WM_WINDOWPOSCHANGING:
		winpos = (WINDOWPOS*)lParam;

		if(winpos->x <= (rect_screen.left + g_setting.win_distance_docking)) 
			winpos->x = rect_screen.left;

		if(winpos->y <= (rect_screen.top + g_setting.win_distance_docking)) 
			winpos->y = rect_screen.top;

		if(winpos->x >= (rect_screen.right + 1 - winpos->cx - g_setting.win_distance_docking)) 
			winpos->x = rect_screen.right + 1 - winpos->cx;

		if(winpos->y >= (rect_screen.bottom + 1 - winpos->cy - g_setting.win_distance_docking)) 
			winpos->y = rect_screen.bottom + 1 - winpos->cy;

		break;
	case WM_LBUTTONDOWN:
		printf("mouse-ldown:\n");
		g_is_mouse_down = 1;
		break;
	case WM_LBUTTONUP:
		printf("mouse-lup:\n");
		g_is_mouse_down = 0;
		if(g_mouse_over_current == CCC_MOVER_KCHG) g_is_keychange_set = g_is_keychange_set ? 0 : 1;
		else if(g_mouse_over_current == CCC_MOVER_CLOSE) DestroyWindow(hWnd);
		break;
	case WM_MOUSEHOVER:
		printf("mouse-over:\n");
		break;
	case WM_NCHITTEST:
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);

		ScreenToClient(hWnd, &pt);

		if(DetectMouseOver(&g_button_area[CCC_MOVER_CAPS], &pt))
			g_mouse_over_current = CCC_MOVER_CAPS;
		else if(DetectMouseOver(&g_button_area[CCC_MOVER_NUML], &pt))
			g_mouse_over_current = CCC_MOVER_NUML;
		else if(DetectMouseOver(&g_button_area[CCC_MOVER_CLOSE], &pt))
			g_mouse_over_current = CCC_MOVER_CLOSE;
		else if(DetectMouseOver(&g_button_area[CCC_MOVER_KCHG], &pt))
			g_mouse_over_current = CCC_MOVER_KCHG;
		else
			g_mouse_over_current = CCC_MOVER_BK;

		printf("hit-test: current_hover, %d\n", g_mouse_over_current);
		

		if(g_mouse_over_current == CCC_MOVER_KCHG || g_mouse_over_current == CCC_MOVER_CLOSE)
		{
			if(g_mouse_over_prev != g_mouse_over_current)
				InvalidateRect(hWnd, &g_button_area[g_mouse_over_current], FALSE);

			g_mouse_over_prev = g_mouse_over_current;
			return HTCLIENT;
		}
		else
		{
			if(g_mouse_over_prev == CCC_MOVER_KCHG || g_mouse_over_prev == CCC_MOVER_CLOSE)
				InvalidateRect(hWnd, &g_button_area[g_mouse_over_prev], FALSE);
		}

		g_is_mouse_down = FALSE;
		g_mouse_over_prev = g_mouse_over_current;
		return HTCAPTION;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void OnPaint(HWND hWnd, HDC* hdc)
{
	using namespace Gdiplus;
	RECT rect;
	GetClientRect(hWnd, &rect);
	//////////////////////////////////////////////////////////////////////////
	Bitmap bmp(rect.right,rect.bottom);
	Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(&bmp);
	//////////////////////////////////////////////////////////////////////////
// 	Graphics grps(*hdc);
// 	Graphics* g = &grps;
	//////////////////////////////////////////////////////////////////////////

	g->SetSmoothingMode(SmoothingModeHighQuality);

	int clientx = rect.right - rect.left;
	int clienty = rect.bottom - rect.top;
	
	float sizex = (float)g_setting.win_size_width;
	float sizey = (float)g_setting.win_size_height;

	float bnsizex = g_setting.button_size_xy;
	float bnsizey = g_setting.button_size_xy;

	float disx = g_setting.button_distance_near;
	float divDisx = g_setting.button_distance_far;
	float disShadow = g_setting.label_distance_shadow;

	float borderx = g_setting.win_size_border;
	float bordery = g_setting.win_size_border;
	float round = g_setting.button_size_round;
	
	Color color_button, color_label;

	Pen pen_label(color_button);
	SolidBrush brush(color_label);

	pen_label.SetWidth(g_setting.label_size_pen);

	// transparent
	SetBkMode(*hdc,TRANSPARENT);

	// background
	color_button.SetValue(RGB_BACKGROUND);
	brush.SetColor(color_button);
	PaintFillRoundRect(g, &brush, -1, -1, (float)clientx, (float)clienty, (float)(g_setting.win_size_round), CCC_BN_INDEPENDENT);

	float orgx, orgy;

	if(g_is_keychange_set)
	{
		color_button.SetValue(RGB_BN_ACTIVE);

		if(g_mouse_over_current == CCC_MOVER_KCHG) color_label.SetValue(RGB_LABEL_HOVER_ACTIVE);
		else color_label.SetValue(RGB_LABEL_NORMAL_ACTIVE);
	}
	else 
	{
		color_button.SetValue(RGB_BN_NORMAL);

		if(g_mouse_over_current == CCC_MOVER_KCHG) color_label.SetValue(RGB_LABEL_HOVER);
		else color_label.SetValue(RGB_LABEL_NORMAL);
	}

	brush.SetColor(color_button);
	pen_label.SetColor(color_label);
	
	orgx = borderx;
	orgy = bordery;
	PaintFillRoundRect(g, &brush, orgx, orgy, bnsizex, bnsizey, round, CCC_BN_INDEPENDENT);
	PaintLabel(g, &pen_label, orgx, orgy, bnsizex, bnsizey, CCC_MOVER_KCHG);

	g_button_area[CCC_MOVER_KCHG].left = (int)orgx;
	g_button_area[CCC_MOVER_KCHG].top = (int)orgy;
	g_button_area[CCC_MOVER_KCHG].right = (int)(orgx + bnsizex) + 1;
	g_button_area[CCC_MOVER_KCHG].bottom = (int)(orgy + bnsizey) + 1;
	
	if(keyboard_is_on(VK_CAPITAL))
	{
		color_button.SetValue(RGB_BN_ACTIVE);
		color_label.SetValue(RGB_LABEL_NORMAL_ACTIVE);
	}
	else
	{
		color_button.SetValue(RGB_BN_NORMAL);
		color_label.SetValue(RGB_LABEL_NORMAL);
	}
	brush.SetColor(color_button);
	pen_label.SetColor(color_label);

	orgx += bnsizex + divDisx;
	PaintFillRoundRect(g, &brush, orgx, orgy, bnsizex, bnsizey, round, CCC_BN_LEFT);
	PaintLabel(g, &pen_label, orgx, orgy, bnsizex, bnsizey, CCC_MOVER_CAPS);

	g_button_area[CCC_MOVER_CAPS].left = (int)orgx;
	g_button_area[CCC_MOVER_CAPS].top = (int)orgy;
	g_button_area[CCC_MOVER_CAPS].right = (int)(orgx + bnsizex) + 1;
	g_button_area[CCC_MOVER_CAPS].bottom = (int)(orgy + bnsizey) + 1;

	if(keyboard_is_on(VK_NUMLOCK))
	{
		color_button.SetValue(RGB_BN_ACTIVE);
		color_label.SetValue(RGB_LABEL_NORMAL_ACTIVE);
	}
	else
	{
		color_button.SetValue(RGB_BN_NORMAL);
		color_label.SetValue(RGB_LABEL_NORMAL);
	}
	brush.SetColor(color_button);
	pen_label.SetColor(color_label);

	orgx += bnsizex + disx;
	PaintFillRoundRect(g, &brush, orgx, orgy, bnsizex, bnsizey, round, CCC_BN_RIGHT);
	PaintLabel(g, &pen_label, orgx, orgy, bnsizex, bnsizey, CCC_MOVER_NUML);

	g_button_area[CCC_MOVER_NUML].left = (int)orgx;
	g_button_area[CCC_MOVER_NUML].top = (int)orgy;
	g_button_area[CCC_MOVER_NUML].right = (int)(orgx + bnsizex) + 1;
	g_button_area[CCC_MOVER_NUML].bottom = (int)(orgy + bnsizey) + 1;



	color_button.SetValue(RGB_BN_NORMAL);
	if(g_mouse_over_current == CCC_MOVER_CLOSE) 
		color_label.SetValue(RGB_LABEL_HOVER);
	else 
		color_label.SetValue(RGB_LABEL_NORMAL);

	brush.SetColor(color_button);
	pen_label.SetColor(color_label);

	orgx += bnsizex + divDisx;
	PaintFillRoundRect(g, &brush, orgx, orgy, bnsizex, bnsizey, round, CCC_BN_INDEPENDENT);
	PaintLabel(g, &pen_label, orgx, orgy, bnsizex, bnsizey, CCC_MOVER_CLOSE);

	g_button_area[CCC_MOVER_CLOSE].left = (int)orgx;
	g_button_area[CCC_MOVER_CLOSE].top = (int)orgy;
	g_button_area[CCC_MOVER_CLOSE].right = (int)(orgx + bnsizex) + 1;
	g_button_area[CCC_MOVER_CLOSE].bottom = (int)(orgy + bnsizey) + 1;

	//////////////////////////////////////////////////////////////////////////
	Graphics graphics(*hdc);
	graphics.DrawImage(&bmp, rect.left, rect.top, rect.right, rect.bottom);
	if(g) delete g;
}

void PaintFillRoundRect(Gdiplus::Graphics* g, Gdiplus::Brush* brush, float x, float y, float width, float height, float round, int type)
{
	using namespace Gdiplus;

	Gdiplus::GraphicsPath path;
	const float arcoffset = round;
	const float lineoffset = arcoffset/2;

	float orgx = x;
	float orgy = y;

	switch(type)
	{
	default:
	case CCC_BN_INDEPENDENT:
		{
			path.AddLine(orgx + lineoffset, orgy, orgx + width - lineoffset, orgy);
			path.AddArc(orgx + width - arcoffset, orgy, arcoffset, arcoffset, 270, 90);
			path.AddLine(orgx + width, orgy + lineoffset, orgx + width, orgy + height - lineoffset);
			path.AddArc(orgx + width - arcoffset, orgy + height - arcoffset, arcoffset, arcoffset, 360, 90);
			path.AddLine(orgx + width - lineoffset, orgy + height, orgx + lineoffset, orgy + height);
			path.AddArc(orgx, orgy + height - arcoffset, arcoffset, arcoffset, 90, 90);
			path.AddLine(orgx, orgy + height - lineoffset, orgx, orgy + lineoffset);
			path.AddArc(orgx, orgy, arcoffset, arcoffset, 180, 90);

			break;
		}
	case CCC_BN_MIDDLE:
		{
			Rect rect; 
			rect.X = (int)orgx;
			rect.Y = (int)orgy;
			rect.Width = (int)width;
			rect.Height = (int)height;
			path.AddRectangle(rect);
			
			break;
		}
	case CCC_BN_LEFT:
		{
			path.AddLine(orgx + lineoffset, orgy, orgx + width, orgy);
			path.AddLine(orgx + width, orgy, orgx + width, orgy + height);
			path.AddLine(orgx + width, orgy + height, orgx + lineoffset, orgy + height);
			path.AddArc(orgx, orgy + height - arcoffset, arcoffset, arcoffset, 90, 90);
			path.AddLine(orgx, orgy + height - lineoffset, orgx, orgy + lineoffset);
			path.AddArc(orgx, orgy, arcoffset, arcoffset, 180, 90);

			break;
		}
	case CCC_BN_RIGHT:
		{
			path.AddLine(orgx, orgy, orgx + width - lineoffset, orgy);
			path.AddArc(orgx + width - arcoffset, orgy, arcoffset, arcoffset, 270, 90);
			path.AddLine(orgx + width, orgy + lineoffset, orgx + width, orgy + height - lineoffset);
			path.AddArc(orgx + width - arcoffset, orgy + height - arcoffset, arcoffset, arcoffset, 360, 90);
			path.AddLine(orgx + width - lineoffset, orgy + height, orgx, orgy + height);
			path.AddLine(orgx, orgy + height, orgx, orgy);

			break;
		}
	}

	

	g->FillPath(brush, &path);
}

void PaintDrawRoundRect(Gdiplus::Graphics* g, Gdiplus::Pen* pen, float x, float y, float width, float height, int type)
{
	using namespace Gdiplus;

	const float arcoffset = (float)g_setting.win_size_round;
	const float lineoffset = arcoffset / 2;
	float orgx = x;
	float orgy = y;

	switch(type)
	{
	default:
	case CCC_BN_INDEPENDENT:
		{
			g->DrawLine(pen, orgx + lineoffset, orgy, orgx + width - lineoffset, orgy);
			g->DrawArc(pen, orgx + width - arcoffset, orgy, arcoffset, arcoffset, 270, 90);
			g->DrawLine(pen, orgx + width, orgy + lineoffset, orgx + width, orgy + height - lineoffset);
			g->DrawArc(pen, orgx + width - arcoffset, orgy + height - arcoffset, arcoffset, arcoffset, 360, 90);
			g->DrawLine(pen, orgx + width - lineoffset, orgy + height, orgx + lineoffset, orgy + height);
			g->DrawArc(pen, orgx, orgy + height - arcoffset, arcoffset, arcoffset, 90, 90);
			g->DrawLine(pen, orgx, orgy + height - lineoffset, orgx, orgy + lineoffset);
			g->DrawArc(pen, orgx, orgy, arcoffset, arcoffset, 180, 90);
			break;
		}
	case CCC_BN_MIDDLE:
		{
			g->DrawRectangle(pen, orgx, orgy, width, height);
			break;
		}
	case CCC_BN_LEFT:
		{
			g->DrawLine(pen, orgx, orgy, orgx + width, orgy);
			g->DrawLine(pen, orgx + width, orgy, orgx + width, orgy + height);
			g->DrawLine(pen, orgx + width, orgy + height, orgx + lineoffset, orgy + height);
			g->DrawArc(pen, orgx, orgy + height - arcoffset, arcoffset, arcoffset, 90, 90);
			g->DrawLine(pen, orgx, orgy + height - lineoffset, orgx, orgy + lineoffset);
			g->DrawArc(pen, orgx, orgy, arcoffset, arcoffset, 180, 90);
			break;
		}
	case CCC_BN_RIGHT:
		{
			g->DrawLine(pen, orgx, orgy, orgx + width - lineoffset, orgy);
			g->DrawArc(pen, orgx + width - arcoffset, orgy, arcoffset, arcoffset, 270, 90);
			g->DrawLine(pen, orgx + width, orgy + lineoffset, orgx + width, orgy + height - lineoffset);
			g->DrawArc(pen, orgx + width - arcoffset, orgy + height - arcoffset, arcoffset, arcoffset, 360, 90);
			g->DrawLine(pen, orgx + width - lineoffset, orgy + height, orgx, orgy);
			g->DrawLine(pen, orgx, orgy + height, orgx, orgy);
			break;
		}
	}
}
void PaintLabel(Gdiplus::Graphics* g, Gdiplus::Pen* pen, float x, float y, float width, float height, int type)
{
	using namespace Gdiplus;

	float scale = g_setting.label_size_scale;

	float cx = x + (width / 2);
	float cy = y + (height / 2);

	float sizex = width * scale;
	float sizey = height * scale;

	switch(type)
	{
	case CCC_MOVER_KCHG:
		{
			g->DrawArc(pen, cx - sizex/2, cy - sizey/2,  sizex, sizey, 0, 360);
			break;
		}
	case CCC_MOVER_CAPS:
		{
			g->DrawLine(pen, cx - sizex/2, cy + sizey/2, cx, cy - sizey/2);
			g->DrawLine(pen, cx, cy - sizey/2, cx + sizex/2, cy + sizey/2);
			g->DrawLine(pen, cx - sizex/3.5f, cy + sizey/8, cx + sizex/3.5f, cy + sizey/8);
			break;
		}
	case CCC_MOVER_NUML:
		{
			g->DrawLine(pen, cx, cy - sizey/2, cx, cy + sizey/2);
			g->DrawLine(pen, cx - sizex/10, cy + sizey/2, cx + sizex/10, cy + sizey/2);
			g->DrawLine(pen, cx, cy - sizey/2, cx - sizex/10, cy - sizey/2 + sizey/6);
			break;
		}
	case CCC_MOVER_CLOSE:
		{
			g->DrawLine(pen, cx - sizex/2, cy - sizey/2, cx + sizex/2, cy + sizey/2);
			g->DrawLine(pen, cx - sizex/2, cy + sizey/2, cx + sizex/2, cy - sizey/2);
			break;
		}
	default:
		break;
	}

}

BOOL DetectMouseOver(RECT* rect, POINT* pt)
{
	if(pt->x >= rect->left && pt->x <= rect->right)
	{
		if(pt->y >= rect->top && pt->y <= rect->bottom)
			return TRUE;
	}

	return FALSE;
}

LRESULT CALLBACK keyboard_proc(int code, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT *)lParam;
	int keycode = pKeyBoard->vkCode;

	if(!g_is_keychange_set)
	{
		if(keycode == VK_CAPITAL) InvalidateRect(g_hwnd_mainwin, &g_button_area[CCC_MOVER_CAPS], FALSE);
		if(keycode == VK_NUMLOCK) InvalidateRect(g_hwnd_mainwin, &g_button_area[CCC_MOVER_NUML], FALSE);

		return CallNextHookEx (NULL, code, wParam, lParam);
	}

	if ( ((KBDLLHOOKSTRUCT*)lParam)->flags & LLKHF_INJECTED )
	{
		if(keycode == VK_CAPITAL) InvalidateRect(g_hwnd_mainwin, &g_button_area[CCC_MOVER_CAPS], FALSE);
		if(keycode == VK_NUMLOCK) InvalidateRect(g_hwnd_mainwin, &g_button_area[CCC_MOVER_NUML], FALSE);

		return CallNextHookEx (NULL, code, wParam, lParam);
	}

	switch(keycode)
	{
	case VK_CAPITAL:
		{
			switch (wParam)
			{
			case WM_KEYDOWN:
				keybd_event(VK_LCONTROL, 0, 0, 0);
				return 1;
			case WM_KEYUP:
				keybd_event(VK_LCONTROL, 0, KEYEVENTF_KEYUP, 0);
				return 1;
			}
		}

	case VK_LCONTROL:
		{
			switch (wParam)
			{
			case WM_KEYDOWN:
				keybd_event(VK_CAPITAL, 0, 0, 0);
				return 1;
			case WM_KEYUP:
				keybd_event(VK_CAPITAL, 0, KEYEVENTF_KEYUP, 0);
				return 1;
			}
		}
	default:
		if(keycode == VK_NUMLOCK) InvalidateRect(g_hwnd_mainwin, &g_button_area[CCC_MOVER_NUML], FALSE);
		CallNextHookEx( NULL, code, wParam, lParam );
	}

	return 0;
}