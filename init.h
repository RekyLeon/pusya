/*
DIESER VERTRAG DARF AUS KEINER WEITERENTWICKELTEN VERSION DIESES PLUGINS ENTFERNT WERDEN!

DarkVis Beta 1.5 Code - init.h

Copyright (C) 2001 by DarKnight (ICQ: 76468699, eMail: darknight@deltaeagle.net)

Teile des Codes basieren auf dem Initierungscode von NeHe (nehe.gamedev.net)
oder dem VIS SDK für Winamp von Nullsoft (www.winamp.com).

Sie sind berechtigt diesen Code beliebig zu verändern und auch diesen
Code weiterzugeben. Allerdings ist es ihnen nicht gestattet, Elemente dieses
Codes in kommerziellen Produkten zu verwenden. Ausserdem würde ich mich als
Entwickler des Ursprungscodes darüber freuen über alle Veränderungen aufgeklärt
zu werden.

---

Die Hauptinclude für geteilte Funktionen, Strukturen und Variablen

*/

//------------------------------------------------------------------------------

#include <windows.h>
#include <gl\gl.h>						// header file for the opengl32 library
#include <gl\glu.h>						// header file for the glu32 library
#include <gl\glaux.h>					// header file for the glaux library

#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "Vis.h"

const int pointc = 90;					//Anzahl der Punkte in einem Graph

extern int blurcount;
extern int planetcount;
extern int starcount;
extern int peakcount;

//Struktur eines Graphs

typedef struct {
	float x[pointc];
	float y[pointc];
	float z;

	int r;
	int g;
	int b;

	int count;
	bool active;
}Graph;

//Struktur eines Punktes

typedef struct {
	float x;
	float y;
	float z;
	float angle;
	float radius;
	float speed;
	int r;
	int g;
	int b;
	int r2;
	int g2;
	int b2;

}Point;

//Struktur eines Sterns

typedef struct {
	float x;
	float y;
}Star;

//------------------------------------------------------------------------------

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);			// declaration for wndproc
GLvoid resizeglscene(GLsizei width, GLsizei height);
int initgl(GLvoid);
GLvoid killglwindow(GLvoid);
bool createglwindow(char* title, int width, int height, int bits, bool fullscreenflag, winampVisModule* this_mod);

//------------------------------------------------------------------------------

GLvoid BuildFont(GLvoid);
GLvoid KillFont(GLvoid);
GLvoid glPrint(float zpos, const char* fmt, ...);

//------------------------------------------------------------------------------

void renderblur(int index);
void TextEffekt(char* string);
void Beat();
void getbeat();
void setscientific(int _switch);
void setlandscape(int _switch);
void setdefault(int _switch);

//------------------------------------------------------------------------------

int render(struct winampVisModule* this_mod);
int init(struct winampVisModule* this_mod);
void quit(struct winampVisModule* this_mod);

//------------------------------------------------------------------------------

winampVisModule* getModule(int which);
void config(struct winampVisModule* this_mod);

//------------------------------------------------------------------------------

void config_getinifn(struct winampVisModule* this_mod, char* ini_file);
void config_read(struct winampVisModule* this_mod);
void config_write(struct winampVisModule* this_mod);

//Ende von init.h =)