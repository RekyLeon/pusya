/*
DIESER VERTRAG DARF AUS KEINER WEITERENTWICKELTEN VERSION DIESES PLUGINS ENTFERNT WERDEN!

DarkVis Beta 1.5 Code - init.cpp

Copyright (C) 2001 by DarKnight (ICQ: 76468699, eMail: darknight@deltaeagle.net)

Teile des Codes basieren auf dem Initierungscode von NeHe (nehe.gamedev.net)
oder dem VIS SDK fьr Winamp von Nullsoft (www.winamp.com).

Sie sind berechtigt diesen Code beliebig zu verдndern und auch diesen
Code weiterzugeben. Allerdings ist es ihnen nicht gestattet, Elemente dieses
Codes in kommerziellen Produkten zu verwenden. Ausserdem wьrde ich mich als
Entwickler des Ursprungscodes darьber freuen ьber alle Verдnderungen aufgeklдrt
zu werden.

---

Hier finden sie den OpenGL Initialisierungscode sowie verschiedene Funktionen,
die etwas mit Initialisierung der Komponenten zu tun haben.

*/

//------------------------------------------------------------------------------

#include "init.h"

extern HDC			hDC;
extern HWND			hwnd;
extern HGLRC		hRC;

extern bool        keys[256];
extern bool        active;
extern bool        fullscreen;

extern GLuint	base;
extern GLYPHMETRICSFLOAT gmf[256];
extern GLfloat fogColor[4];

extern int config_x;
extern int config_y;

extern int framerate;

extern bool fog;						//Ist der Nebel aktiviert?
extern bool randomc;					//Sind Zufallsfarben aktiviert?
extern bool spectrum;					//Sollen die Graphen als Spektrum oder als Wavedata angezeigt werden?
extern bool connections;				//Sind Verbindungen zwischen den Blur-Graphen aktiviert?
extern bool solid;						//Soll "Solid" gerendert werden (nicht richtig implementiert)?
extern bool landscape;					//Ist der "Landscape" Modus aktiviert?
extern bool anti;						//Ist OpenGL-Line-Anti Aliasing aktiviert?
extern bool actpeaks;					//Sind die Peaks aktiviert?
extern bool scientific;					//Ist der Scientific Mode aktiviert?

extern int r, g, b;						//Farben des Wurmlochs
extern int r2, g2, b2;

extern int modeset;

//------------------------------------------------------------------------------

//+++++++++++++++++++++++++++
//OpenGL initiation Functions
//+++++++++++++++++++++++++++

GLvoid resizeglscene(GLsizei width, GLsizei height)		// resize and initialize the gl window

{
	if (height == 0)										// prevent a divide by zero by
	{
		height = 1;										// making height equal one
	}

	glViewport(0, 0, width, height);					// reset the current viewport

	glMatrixMode(GL_PROJECTION);						// select the projection matrix
	glLoadIdentity();									// reset the projection matrix

	// calculate the aspect ratio of the window
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);							// select the modelview matrix
	glLoadIdentity();									// reset the modelview matrix
}

//------------------------------------------------------------------------------

int initgl(GLvoid)											// all setup for opengl goes here

{
	glShadeModel(GL_SMOOTH);								// enables smooth shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);					// black background
	glClearDepth(1.0);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);					// depth buffer setup
	glEnable(GL_DEPTH_TEST);								// enables depth testing
	glDepthFunc(GL_LEQUAL);									// the type of depth test to do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);		// really nice perspective calculations

	return true;											// initialization went ok
}

//------------------------------------------------------------------------------

GLvoid killglwindow(GLvoid)									// properly kill the window

{

	if (fullscreen)											// Are We In Fullscreen Mode?

	{
		ChangeDisplaySettings(NULL, 0);						// If So Switch Back To The Desktop
		ShowCursor(TRUE);									// Show Mouse Pointer

	}

	if (hRC)												// Do We Have A Rendering Context?
	{

		if (!wglMakeCurrent(NULL, NULL))						// Are We Able To Release The DC And RC Contexts?

		{
			MessageBox(NULL, L"Release Of DC And RC Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);

		}

		if (!wglDeleteContext(hRC))							// Are We Able To Delete The RC?

		{

			MessageBox(NULL, L"Release Rendering Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);

		}
		hRC = NULL;											// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hwnd, hDC))						// Are We Able To Release The DC

	{
		//		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC = NULL;											// Set DC To NULL
	}

	if (hwnd && !DestroyWindow(hwnd))						// Are We Able To Destroy The Window?

	{
		//		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hwnd = NULL;											// Set hWnd To NULL

	}



}

//------------------------------------------------------------------------------

bool createglwindow(char* title, int width, int height, int bits, bool fullscreenflag, winampVisModule* this_mod)

{
	GLuint					pixelformat;                                // holds the results after searching for a match
	HINSTANCE				hinstance;									// holds the instance of the application
	WNDCLASS				wc;											// windows class structure
	DWORD					dwexstyle;									// window extended style
	DWORD					dwstyle;									// window style

	fullscreen = fullscreenflag;											// set the global fullscreen flag

	hinstance = GetModuleHandle(NULL);						// grab an instance for our window
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;			// redraw on move, and own dc for window
	wc.lpfnWndProc = (WNDPROC)WndProc;							// wndproc handles messages
	wc.cbClsExtra = 0;											// no extra window data
	wc.cbWndExtra = 0;											// no extra window data
	wc.hInstance = this_mod->hDllInstance; //hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);					// load the default icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);					// load the arrow pointer
	wc.hbrBackground = NULL;											// no background required for gl
	wc.lpszMenuName = NULL;											// we don't want a menu
	wc.lpszClassName = L"OpenGL";										// set the class name

	if (!RegisterClass(&wc))                                        // attempt to register the window class

	{
		MessageBox(NULL, L"failed to register the window class.", L"error", MB_OK | MB_ICONEXCLAMATION);
		return false;												// exit and return false
	}

	if (fullscreen)													// attempt fullscreen mode?

	{

		DEVMODE dmscreensettings;										// device mode
		memset(&dmscreensettings, 0, sizeof(dmscreensettings));			// makes sure memory's cleared
		dmscreensettings.dmSize = sizeof(dmscreensettings);				// size of the devmode structure
		dmscreensettings.dmPelsWidth = width;					// selected screen width
		dmscreensettings.dmPelsHeight = height;					// selected screen height
		dmscreensettings.dmBitsPerPel = bits;					// selected bits per pixel
		dmscreensettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// try to set selected mode and get results.  note: cds_fullscreen gets rid of start bar.
		if (ChangeDisplaySettings(&dmscreensettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)

		{

			// if the mode fails, offer two options.  quit or run in a window.
			if (MessageBox(NULL, L"the requested fullscreen mode is not supported by\nyour video card. use windowed mode instead?", L"opengl test", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)

			{

				fullscreen = false;										// select windowed mode (fullscreen=false)

			}

			else

			{


				return false;											// exit and return false

			}

		}
	}


	if (fullscreen)														// are we still in fullscreen mode?

	{

		dwexstyle = WS_EX_APPWINDOW;										// window extended style
		dwstyle = WS_POPUP;												// windows style
		ShowCursor(false);												// hide mouse pointer
	}

	else
	{
		dwexstyle = WS_EX_APPWINDOW | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE;					// window extended style
		dwstyle = /*WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;*/WS_OVERLAPPEDWINDOW;									// windows style
	}

    // Преобразуем char* title в wchar_t* для CreateWindowEx
    wchar_t w_title[256];
    MultiByteToWideChar(CP_ACP, 0, title, -1, w_title, 256);

	if (!(hwnd = CreateWindowEx(dwexstyle,					// extended style for the window
		L"OpenGL",								// class name
		w_title,								// window title (исправлено)
		WS_CLIPSIBLINGS |						// required window style
		WS_CLIPCHILDREN |						// required window style
		dwstyle,								// selected window style
		0, 0,									// window position
		width, height,							// selected width and height
		this_mod->hwndParent,					// no parent window
		NULL,									// no menu
		this_mod->hDllInstance,					//hinstance,								// instance
		NULL)))									// don't pass anything to wm_create


	{
		killglwindow();													// reset the display
		MessageBox(NULL, L"window creation error.", L"error", MB_OK | MB_ICONEXCLAMATION);
		return false;													// return false
	}


	static        PIXELFORMATDESCRIPTOR pfd =							// pfd tells windows how we want things to be

	{
		sizeof(PIXELFORMATDESCRIPTOR),									// size of this pixel format descriptor
			1,															// version number
			PFD_DRAW_TO_WINDOW |                                        // FORMAT MUST SUPPORT WINDOW
			PFD_SUPPORT_OPENGL |                                        // FORMAT MUST SUPPORT OPENGL
			PFD_DOUBLEBUFFER,											// MUST SUPPORT DOUBLE BUFFERING
			PFD_TYPE_RGBA,												// request an rgba format
			bits,														// select our color depth
			0, 0, 0, 0, 0, 0,											// color bits ignored
			0,															// no alpha buffer
			0,															// shift bit ignored
			0,															// no accumulation buffer
			0, 0, 0, 0,													// accumulation bits ignored
			16,															// 16bit z-buffer (depth buffer)
			0,															// no stencil buffer
			0,															// no auxiliary buffer
			PFD_MAIN_PLANE,                                                // main drawing layer
			0,															// reserved
			0, 0, 0														// layer masks ignored
	};
	if (!(hDC = GetDC(hwnd)))												// did we get a device context?

	{
		killglwindow();													// reset the display
		MessageBox(NULL, L"can't create a gl device context.", L"error", MB_OK | MB_ICONEXCLAMATION);
		return false;													// return false
	}


	if (!(pixelformat = ChoosePixelFormat(hDC, &pfd)))						// did windows find a matching pixel format?

	{
		killglwindow();													// reset the display
		MessageBox(NULL, L"can't find a suitable pixelformat.", L"error", MB_OK | MB_ICONEXCLAMATION);
		return false;													// return false
	}

	if (!SetPixelFormat(hDC, pixelformat, &pfd))							// are we able to set the pixel format?

	{
		killglwindow();													// reset the display
		MessageBox(NULL, L"can't set the pixelformat.", L"error", MB_OK | MB_ICONEXCLAMATION);
		return false;													// return false
	}

	if (!(hRC = wglCreateContext(hDC)))									// are we able to get a rendering context?

	{
		killglwindow();													// reset the display
		MessageBox(NULL, L"can't create a gl rendering context.", L"error", MB_OK | MB_ICONEXCLAMATION);
		return false;													// return false
	}

	if (!wglMakeCurrent(hDC, hRC))										// try to activate the rendering context

	{
		killglwindow();													// reset the display
		MessageBox(NULL, L"can't activate the gl rendering context.", L"error", MB_OK | MB_ICONEXCLAMATION);
		return false;													// return false
	}

	ShowWindow(hwnd, SW_SHOW);											// show the window
	SetForegroundWindow(hwnd);											// slightly higher priority
	SetFocus(hwnd);														// sets keyboard focus to the window
	resizeglscene(width, height);										// set up our perspective gl screen

	if (!initgl())														// initialize our newly created gl window

	{
		killglwindow();                                                // reset the display
		MessageBox(NULL, L"initialization failed.", L"error", MB_OK | MB_ICONEXCLAMATION);
		return false;													// return false
	}

	return true;														// success

}

//------------------------------------------------------------------------------

GLvoid BuildFont(GLvoid)								// Build Our Bitmap Font
{
	HFONT	font;										// Windows Font ID

	base = glGenLists(256);								// Storage For 256 Characters

	font = CreateFont(8,								// Height Of Font
		8,						// Width Of Font
		0,								// Angle Of Escapement
		0,								// Orientation Angle
		100,						// Font Weight
		FALSE,							// Italic
		FALSE,							// Underline
		FALSE,							// Strikeout
		ANSI_CHARSET,					// Character Set Identifier
		OUT_TT_PRECIS,					// Output Precision
		CLIP_DEFAULT_PRECIS,			// Clipping Precision
		ANTIALIASED_QUALITY,			// Output Quality
		FF_DONTCARE | DEFAULT_PITCH,		// Family And Pitch
		L"Verdana");						// Font Name

	SelectObject(hDC, font);							// Selects The Font We Created

	wglUseFontOutlines(hDC,							// Select The Current DC
		0,								// Starting Character
		255,							// Number Of Display Lists To Build
		base,							// Starting Display Lists
		0.0f,							// Deviation From The True Outlines
		0.001f,							// Font Thickness In The Z Direction
		WGL_FONT_LINES,				// Use Polygons, Not Lines
		gmf);							// Address Of Buffer To Recieve Data

}

//------------------------------------------------------------------------------

GLvoid KillFont(GLvoid)									// Delete The Font
{
	glDeleteLists(base, 96);							// Delete All 96 Characters
}

//------------------------------------------------------------------------------

GLvoid glPrint(float zpos, const char* fmt, ...)					// Custom GL "Print" Routine
{
	float		length = 0;								// Used To Find The Length Of The Text
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	for (unsigned int loop = 0; loop < (strlen(text)); loop++)	// Loop To Find Text Length
	{
		length += gmf[text[loop]].gmfCellIncX;			// Increase Length By Each Characters Width
	}

	glTranslatef(-length / 2, 0, zpos);					// Center Our Text On The Screen

	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(base);									// Sets The Base Character to 0
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
	glPopAttrib();										// Pops The Display List Bits
}

//------------------------------------------------------------------------------

//Winamp Init Funktionen

//------------------------------------------------------------------------------

//Winamp Vis Modul

char mod1_description[] = "DarkVis OpenGL Beta 1.5";

winampVisModule mod1 =
{
	mod1_description,
	NULL,	// hwndparent
	NULL,	// hdllinstance
	0,		// srate
	2,		// nch
	0,		// latencyms
	framerate,		// delayms
	2,		// spectrumnch
	2,		// waveformnch
	{ 0, },	// spectrumdata
	{ 0, },	// waveformdata
	config,
	init,
	render,
	quit
};

// getmodule routine from the main header. returns null if an invalid module was requested,
// otherwise returns either mod1, mod2 or mod3 depending on 'which'.
winampVisModule* getModule(int which)
{
	switch (which)
	{
	case 0:		return &mod1;
	default:	return NULL;
	}
}

// configuration. passed this_mod, as a "this" parameter. allows you to make one configuration
// function that shares code for all your modules (you don't have to use it though, you can make
// config1(), config2(), etc...)
void config(struct winampVisModule* this_mod)
{
	MessageBox(this_mod->hwndParent, L"DarkVis OpenGL Beta 1.5 for Winamp\n"
		"Copyright (C) 2001 by DarKnight (darknight@deltaeagle.net)\n\n"
		"Diese Winamp-Erweiterung nutzt OpenGL. Alle OpenGL-Initialisierungsfunktionen sind von Jeff Molofee (NeHe) erstellt. Danke daran!\n\n"
		"Wie man die Einstellungen ändert:\n\n"

		//"H: Show this info during running the plugin\n"

		"Nach Oben / Unten: Bewege dich in den Bildschirm hinein und hinaus\n"
		"Links / Rechts: Erhöhe oder verringere die Hintergrundgeschwindigkeit\n"
		"Leertaste: Setze die Hintergrundgeschwindigkeit auf Standard zurück\n"
		"Numpad 4: Titel Zurück\n"
		"Numpad 6: Titel Vor\n"
		"A: Aktiviere / Deaktiviere Anti-Aliasing\n"
		"N: Aktiviere / Deaktiviere Peaks\n"
		"Numpad 2 / Numpad 8: Erhöhe / Verringere die Fallgeschwindigkeit der Peaks\n"
		"B: Modus wechseln\n"
		"Y: Spektraldaten-Graph (Standard) / Wellenform-Graph\n"
		"X: Zeige Linien zwischen den Graphen / Deaktiviere Linien zwischen den Graphen\n"
		"Bild Hoch / Runter: Erhöhe / Verringere die Anzahl dieser Linien\n"
		"C: Standard Graphfarbe / Zufällige Graphfarbe\n"
		"V: Ändere die Farben des 'Wurmlochs'\n"
		"T: Zeige den Namen des gespielten Titels an \n\n"
		"Viel Spaß mit Ihrer Musik!\n\n"
		"Die neueste Version finden Sie unter http://sourceforge.net/projects/darkvis/", L"Info", MB_OK);
}

//------------------------------------------------------------------------------

//Winamp Config Funktionen

//------------------------------------------------------------------------------

void config_getinifn(struct winampVisModule* this_mod, char* ini_file)
{	// makes a .ini file in the winamp directory named "DarkVis.ini"
	char* p;
#ifdef UNICODE
	wchar_t temp[MAX_PATH];
	GetModuleFileName(this_mod->hDllInstance, temp, MAX_PATH);
	// Преобразуем wchar_t* в char*
	size_t converted = 0;
	wcstombs_s(&converted, ini_file, MAX_PATH, temp, _TRUNCATE);
#else
	GetModuleFileName(this_mod->hDllInstance, ini_file, MAX_PATH);
#endif
	p = ini_file + strlen(ini_file);
	while (p >= ini_file && *p != '\\') p--;
	if (++p >= ini_file) *p = 0;
	strcat(ini_file, "DarkVis.ini");
}

//Лесен der Einstellungen

void config_read(struct winampVisModule* this_mod)
{
	char ini_file[MAX_PATH];
	wchar_t w_ini_file[MAX_PATH];
	wchar_t w_section[256];

	config_getinifn(this_mod, ini_file);

	// Преобразуем ini_file и this_mod->description в wchar_t*
	CharToWChar(ini_file, w_ini_file, MAX_PATH);
	CharToWChar(this_mod->description, w_section, 256);

	config_x = GetPrivateProfileIntW(w_section, L"Screen_x", config_x, w_ini_file);
	config_y = GetPrivateProfileIntW(w_section, L"Screen_y", config_y, w_ini_file);

	int i_fog = GetPrivateProfileIntW(w_section, L"Fog", 0, w_ini_file);
	fog = (i_fog == 1);

	int i_randomc = GetPrivateProfileIntW(w_section, L"Randomcolors", 1, w_ini_file);
	randomc = (i_randomc == 1);

	int i_spectrum = GetPrivateProfileIntW(w_section, L"Spectrumgraph", 1, w_ini_file);
	spectrum = (i_spectrum == 1);

	int i_connections = GetPrivateProfileIntW(w_section, L"Connections", 1, w_ini_file);
	connections = (i_connections == 1);

	int i_solid = GetPrivateProfileIntW(w_section, L"Solidrendering", 0, w_ini_file);
	solid = (i_solid == 1);

	int i_landscape = GetPrivateProfileIntW(w_section, L"Landscape", 0, w_ini_file);
	if (i_landscape == 1)
	{
		landscape = true;
		setlandscape(1);
	}
	else
		landscape = false;

	int i_scientific = GetPrivateProfileIntW(w_section, L"Scientific", 1, w_ini_file);
	if (i_scientific == 1)
	{
		setscientific(1);
		scientific = true;
	}
	else
		scientific = false;

	int i_anti = GetPrivateProfileIntW(w_section, L"Antialiasing", 0, w_ini_file);
	anti = (i_anti == 1);

	int i_peaks = GetPrivateProfileIntW(w_section, L"Peaks", 1, w_ini_file);
	actpeaks = (i_peaks == 1);

	r = GetPrivateProfileIntW(w_section, L"R", r, w_ini_file);
	g = GetPrivateProfileIntW(w_section, L"G", g, w_ini_file);
	b = GetPrivateProfileIntW(w_section, L"B", b, w_ini_file);

	r2 = GetPrivateProfileIntW(w_section, L"R2", r2, w_ini_file);
	g2 = GetPrivateProfileIntW(w_section, L"G2", g2, w_ini_file);
	b2 = GetPrivateProfileIntW(w_section, L"B2", b2, w_ini_file);
}

//Schreiben der Einstellungen

void config_write(struct winampVisModule* this_mod)
{
    char string[32];
    char ini_file[MAX_PATH];

    config_getinifn(this_mod, ini_file);

    // Используем sprintf вместо wsprintf для char*
    sprintf(string, "%d", config_x);
    WritePrivateProfileStringA(this_mod->description, "Screen_x", string, ini_file);
    sprintf(string, "%d", config_y);
    WritePrivateProfileStringA(this_mod->description, "Screen_y", string, ini_file);

    //---------

    sprintf(string, "%d", fog ? 1 : 0);
    WritePrivateProfileStringA(this_mod->description, "Fog", string, ini_file);

    sprintf(string, "%d", randomc ? 1 : 0);
    WritePrivateProfileStringA(this_mod->description, "Randomcolors", string, ini_file);

    sprintf(string, "%d", spectrum ? 1 : 0);
    WritePrivateProfileStringA(this_mod->description, "Spectrumgraph", string, ini_file);

    sprintf(string, "%d", connections ? 1 : 0);
    WritePrivateProfileStringA(this_mod->description, "Connections", string, ini_file);

    sprintf(string, "%d", solid ? 1 : 0);
    WritePrivateProfileStringA(this_mod->description, "Solidrendering", string, ini_file);

    sprintf(string, "%d", landscape ? 1 : 0);
    WritePrivateProfileStringA(this_mod->description, "Landscape", string, ini_file);

    sprintf(string, "%d", scientific ? 1 : 0);
    WritePrivateProfileStringA(this_mod->description, "Scientific", string, ini_file);

    sprintf(string, "%d", anti ? 1 : 0);
    WritePrivateProfileStringA(this_mod->description, "Antialiasing", string, ini_file);

    sprintf(string, "%d", actpeaks ? 1 : 0);
    WritePrivateProfileStringA(this_mod->description, "Peaks", string, ini_file);

    sprintf(string, "%d", r);
    WritePrivateProfileStringA(this_mod->description, "R", string, ini_file);
    sprintf(string, "%d", g);
    WritePrivateProfileStringA(this_mod->description, "G", string, ini_file);
    sprintf(string, "%d", b);
    WritePrivateProfileStringA(this_mod->description, "B", string, ini_file);

    sprintf(string, "%d", r2);
    WritePrivateProfileStringA(this_mod->description, "R2", string, ini_file);
    sprintf(string, "%d", g2);
    WritePrivateProfileStringA(this_mod->description, "G2", string, ini_file);
    sprintf(string, "%d", b2);
    WritePrivateProfileStringA(this_mod->description, "B2", string, ini_file);
}

//Вспомогательная функция для преобразования строки
//Рисунок 14 из 14

void CharToWChar(const char* src, wchar_t* dst, int dstSize)
{
    MultiByteToWideChar(CP_ACP, 0, src, -1, dst, dstSize);
}

//Ende von init.cpp =)
