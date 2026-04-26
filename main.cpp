/*
DIESER VERTRAG DARF AUS KEINER WEITERENTWICKELTEN VERSION DIESES PLUGINS ENTFERNT WERDEN!

DarkVis Beta 1.5 Code - main.cpp

Copyright (C) 2001 by DarKnight (ICQ: 76468699, eMail: darknight@deltaeagle.net)

Teile des Codes basieren auf dem Initierungscode von NeHe (nehe.gamedev.net)
oder dem VIS SDK fьr Winamp von Nullsoft (www.winamp.com).

Sie sind berechtigt diesen Code beliebig zu verдndern und auch diesen
Code weiterzugeben. Allerdings ist es ihnen nicht gestattet, Elemente dieses
Codes in kommerziellen Produkten zu verwenden. Ausserdem wьrde ich mich als
Entwickler des Ursprungscodes darьber freuen ьber alle Verдnderungen aufgeklдrt
zu werden.

---

Hier befindet sich der Hauptcode von DarkVis mit allen Funktionen
Die Kommentare von mir sind zwar sehr dьrftig sollten aber
trotzdem einen kleinen Ьberblick ьber den Code verschaffen. Die meissten Kommentare
sind in Deutsch, einige auch Englisch. Die Englischen Kommentare stammen
grцsstenteils aus NeHe's OpenGL Initierungscode. Die restlichen sind
ьbriggebliebene Kommentare aus dem Winamp VIS SDK.
*/

//------------------------------------------------------------------------------

#include "init.h"

HDC			hDC = NULL;					// permanent rendering context
HWND        hwnd = NULL;				// private gdi device context
HGLRC		hRC = NULL;					// holds our window handle

bool        keys[256];					// array used for the keyboard routine
bool        active = true;				// window active flag set to true by default
bool        fullscreen = true;			// fullscreen flag set to fullscreen mode by default

GLuint	base;
GLYPHMETRICSFLOAT gmf[256];
GLfloat fogColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };

int config_x = 50;
int config_y = 50;

int framerate = 10;						//Anzahl der Millisekunden nach denen ein neuer Frame gerendert wird

int halfblur = int(blurcount / 2);

//Einrichtung der Arrays

Graph graph[2];

Graph* blur = new Graph[blurcount];
Point* point = new Point[planetcount];
Point* worm = new Point[planetcount];
Point* peaks = new Point[peakcount];
Star* stars = new Star[starcount];
int* ticks = new int[halfblur];

extern int beats[2][4];

//Speicherbare Einstellungen

bool fog = false;						//Ist der Nebel aktiviert?
bool randomc = true;					//Sind Zufallsfarben aktiviert?
bool texteffekt = false;				//Ist ein Texteffekt im Moment aktiv?
bool spectrum = true;					//Sollen die Graphen als Spektrum oder als Wavedata angezeigt werden?
bool connections = true;				//Sind Verbindungen zwischen den Blur-Graphen aktiviert?
bool solid = false;						//Soll "Solid" gerendert werden (nicht richtig implementiert)?
bool landscape = false;					//Ist der "Landscape" Modus aktiviert?
bool anti = false;						//Ist OpenGL-Line-Anti Aliasing aktiviert?
bool songdisplayed = true;				//Wurde der Name des aktuell abgespielten Titels schon angezeigt?
bool actpeaks = true;					//Sind die Peaks aktiviert?
bool scientific = false;				//"Wissenschaftlicher" Modus aktiviert?
bool whipping = true;					//Ist der "wippende" Effekt aktiviert?
bool beatdetection = false;				//Beatdetection ist aus

int r, g, b;							//Farben des Wurmlochs
int r2, g2, b2;

float fallspeed = 0.0001f;

int planetcount = 30;					//Anzahl der "Striche" aus denen das Wurmloch besteht
int blurcount = 15;						//Anzahl der Graphen, die im Hintergrund sind
int starcount = 1000;					//Anzahl der Sterne im Hintergrund
int peakcount = 30;						//Anzahl der Peaks
int connectioncount = 10;				//Anzahl der Verbindungen zwischen den Blur-Graphen

//------------------------------------------------------------------------------

int pointcount = pointc;

int x;

int tmpradius, tmpangle, tmpspeed;

int frames;
int ticker;

int modeset = 0;

int beatsecond = 0;
float bpm = 0;
int tmp_ticks;

float zadd;
int cadd = 1;

float zturn = 0;
float zturnadd = 0.5f;

float zmove = 0.0f;
float yturn = 0.0f;

float textz;

float frameratio;
float floatticks;

char title[256];
char soundtitle[256];
char cframes[256];
char textstring[256];

//------------------------------------------------------------------------------

HWND hwndwinamp = NULL;					//Handle des Winamp Fensters

// returns a winampvismodule when requested. used in hdr, below
winampVisModule* getModule(int which);

// "member" functions
void config(struct winampVisModule* this_mod); // configuration dialog
int init(struct winampVisModule* this_mod);	   // initialization for module
int render(struct winampVisModule* this_mod);
void quit(struct winampVisModule* this_mod);   // deinitialization for module

void renderblur(int index);
void TextEffekt(char* string);

//------------------------------------------------------------------------------

//++++++++++++++++
//Drawing Function
//++++++++++++++++

int drawglscene(GLvoid)

{
	float r_, g_, b_, r2_, g2_, b2_;
	int i, k;
	float temp_z, temp_y1, temp_y2;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (whipping)
		glRotatef(zturn, 0, 0, 1.0);	//um die z Achse drehen                                  

	glRotatef(yturn, 0, 1.0, 0);	//um die y Achse drehen

	if (!landscape || scientific)
		glTranslatef(0, 0, zmove);

	else
		glTranslatef(0, -0.025f, 0.095f);


	if (anti)
		glEnable(GL_LINE_SMOOTH);

	//	glLineWidth(2.0); 

	//----------------- Zeichnen des Scientific Modes ------------------------

	glColor3f(float(graph[0].r) / 255, float(graph[0].g) / 255, float(graph[0].b) / 255);

	if (scientific)

	{
		glBegin(GL_LINES);
		glVertex3f(-0.052f, -0.03f, graph[0].z);
		glVertex3f(-0.052f, 0.03f, graph[0].z);

		glVertex3f(0.052f, -0.03f, graph[0].z);
		glVertex3f(0.052f, 0.03f, graph[0].z);

		glVertex3f(-0.052f, -0, graph[0].z);
		glVertex3f(0.052f, 0, graph[0].z);

		glEnd();

	}

	//----------------- Zeichnen beider Haupt-Graphen ------------------------

	for (int j = 0; j < 2; j++)

	{
		if (graph[j].active)

		{

			for (i = 1; i < pointcount; i++)

			{
				glColor3f(float(graph[j].r) / 255, float(graph[j].g) / 255, float(graph[j].b) / 255);


				glBegin(GL_LINES);

				if (!spectrum)

				{
					glVertex3f(graph[j].x[i - 1] / (pointcount * 10), graph[j].y[i - 1] / 40000, graph[j].z);
					glVertex3f(graph[j].x[i] / (pointcount * 10), graph[j].y[i] / 40000, graph[j].z);
				}

				else

				{
					if (j == 1)
					{
						glVertex3f(graph[j].x[i - 1] / (pointcount * 10), graph[j].y[i - 1] / 40000, graph[j].z);
						glVertex3f(graph[j].x[i] / (pointcount * 10), graph[j].y[i] / 40000, graph[j].z);
					}

					else
					{
						glVertex3f(graph[j].x[i - 1] / (pointcount * 10), graph[j].y[i - 1] / 10000, graph[j].z);
						glVertex3f(graph[j].x[i] / (pointcount * 10), graph[j].y[i] / 10000, graph[j].z);
					}

				}

				glEnd();

			}
		}
	}

	//----------------- Zeichnen der Peaks ------------------------

	float temp_y;
	int j = 0;

	if (actpeaks)

	{
		glLineWidth(2.0);

		for (i = 0; i < pointcount; i += pointcount / peakcount)

		{
			if (i < pointcount && j < peakcount)

			{
				glColor3f(float(graph[0].r) / 255, float(graph[0].g) / 255, float(graph[0].b) / 255);

				glBegin(GL_LINES);

				if (!spectrum)

				{
					if (peaks[j].y <= float(graph[0].y[i] / 40000))
					{
						temp_y = float(graph[0].y[i] / 40000);
						peaks[j].y = temp_y;

						if (beatdetection)

						{
							if (j == 0)
							{
								beats[0][beatsecond]++;
							}

							if (j == 1)
							{
								beats[1][beatsecond]++;
							}
						}
					}

					else
						temp_y = peaks[j].y;

					glVertex3f((graph[0].x[i] / (peakcount * int((90 / (peakcount / 10))))) - 0.0007f, temp_y, float(graph[0].z));
					glVertex3f((graph[0].x[i] / (peakcount * int((90 / (peakcount / 10))))) + 0.0007f, temp_y, float(graph[0].z));
				}

				else

				{

					if (peaks[j].y <= float(graph[0].y[i] / 10000))
					{
						temp_y = float(graph[0].y[i] / 10000);
						peaks[j].y = temp_y;

						if (beatdetection)

						{
							if (j == 0)
							{
								beats[0][beatsecond]++;
							}

							if (j == 1)
							{
								beats[1][beatsecond]++;
							}
						}
					}

					else
					 temp_y = peaks[j].y;

					glVertex3f((graph[0].x[i] / (peakcount * int((90 / (peakcount / 10))))) - 0.0007f, temp_y, float(graph[0].z));
					glVertex3f((graph[0].x[i] / (peakcount * int((90 / (peakcount / 10))))) + 0.0007f, temp_y, float(graph[0].z));
				}

				glEnd();

				j++;
			}
		}
	}

	//----------------- Zeichnen der Blur-Graphen ------------------------

	glLineWidth(1.0);

for (j = 1; j < blurcount; j++)

	{

		for (i = 1; i < pointcount; i++)

		{
			glColor3f(float(blur[j].r) / 255, float(blur[j].g) / 255, float(blur[j].b) / 255);

			glBegin(GL_LINES);

			if (!spectrum)
				k = 40000;
			else
				k = 10000;

			temp_z = float(graph[0].z - (j * 0.1f));

			if (!scientific)
			{
				temp_y1 = float(blur[j].y[i - 1] / k + (j * 0.015));
				temp_y2 = float(blur[j].y[i] / k + (j * 0.015));
			}
			else
			{
				temp_y1 = float(blur[j].y[i - 1] / k + (j * 0.03));
				temp_y2 = float(blur[j].y[i] / k + (j * 0.03));
			}

			glVertex3f(blur[j].x[i - 1] / (pointcount * 10), temp_y1, temp_z);
			glVertex3f(blur[j].x[i] / (pointcount * 10), temp_y2, temp_z);

			glEnd();

		}
	}

	//----------------- Zeichnen der Verbindungen zwischen den Graphen ------------------------

	if (connections)

	{
		if (!solid)

		{

			for (j = 0; j < blurcount; j++)

			{

				for (i = 1; i < pointcount; i += pointcount / connectioncount - 1)

				{
					glColor3f(float(blur[j].r) / 255, float(blur[j].g) / 255, float(blur[j].b) / 255);

					if (j != pointcount && j != 0 && j != blurcount - 1)

					{

						if (!spectrum)
							k = 40000;
						else
							k = 10000;

						temp_z = float(graph[0].z - (j * 0.1f));

						if (!scientific)
						{
							temp_y1 = float(blur[j].y[i - 1] / k + (j * 0.015));
							temp_y2 = float(blur[j + 1].y[i] / k + ((j + 1) * 0.015));
						}
						else
						{
							temp_y1 = float(blur[j].y[i - 1] / k + (j * 0.03));
							temp_y2 = float(blur[j + 1].y[i] / 40000 + ((j + 1) * 0.03));
						}

						glBegin(GL_LINES);

						glVertex3f(blur[j].x[i] / (pointcount * 10), temp_y1, float(graph[0].z - (j * 0.1)));
						glVertex3f(blur[j + 1].x[i] / (pointcount * 10), temp_y2, float(graph[0].z - ((j + 1) * 0.1)));

						glEnd();

					}

				}
			}

		}

		//Solid sollte jeden Punkt der Linien mit QUADS verbinden. 
		//Allerdings ist mir das noch nicht ganz gelungen. Versucht euch dran, 
		//dьrfte einfach sein hab aber keinen Bock gehabt. =)

		else

		{

			for (j = 0; j < blurcount; j++)

			{

				for (i = 1; i < pointcount; i += pointcount / connectioncount - 1)

				{
					glColor3f(float(blur[j].r) / 255, float(blur[j].g) / 255, float(blur[j].b) / 255);

					if (j != pointcount && j != 0 && j != blurcount - 1)

					{
						glBegin(GL_QUADS);

						if (!spectrum)
						{

							glVertex3f(blur[j + 1].x[i] / (pointcount * 10), float(blur[j + 1].y[i] / 40000 + ((j + 1) * 0.015)), float(graph[0].z - ((j + 1) * 0.1)));

							glVertex3f(blur[j + 1].x[i + pointcount / connectioncount - 1] / (pointcount * 10), float(blur[j + 1].y[i + pointcount / connectioncount - 1] / 40000 + ((j + 1) * 0.015)), float(graph[0].z - ((j + 1) * 0.1)));

							glVertex3f(blur[j].x[i + pointcount / connectioncount - 1] / (pointcount * 10), float(blur[j].y[i + pointcount / connectioncount - 1] / 40000 + (j * 0.015)), float(graph[0].z - (j * 0.1)));

							glVertex3f(blur[j].x[i] / (pointcount * 10), float(blur[j].y[i] / 40000 + (j * 0.015)), float(graph[0].z - (j * 0.1)));

						}
						else

						{
							glVertex3f(blur[j + 1].x[i] / (pointcount * 10), float(blur[j + 1].y[i] / 10000 + ((j + 1) * 0.015)), float(graph[0].z - ((j + 1) * 0.1)));

							glVertex3f(blur[j + 1].x[i + pointcount / connectioncount - 1] / (pointcount * 10), float(blur[j + 1].y[i + pointcount / connectioncount - 1] / 10000 + ((j + 1) * 0.015)), float(graph[0].z - ((j + 1) * 0.1)));

							glVertex3f(blur[j].x[i + pointcount / connectioncount - 1] / (pointcount * 10), float(blur[j].y[i + pointcount / connectioncount - 1] / 10000 + (j * 0.015)), float(graph[0].z - (j * 0.1)));

							glVertex3f(blur[j].x[i] / (pointcount * 10), float(blur[j].y[i] / 10000 + (j * 0.015)), float(graph[0].z - (j * 0.1)));
						}

						glEnd();

					}

				}
			}
		}
	}

	if (!landscape)

	{

		//----------------- Zeichnen aller Elemente des Wurmlochs ------------------------

		//glLineWidth(1.0f); 

		for (i = 0; i < planetcount; i++)

		{

			r_ = float(point[i].r) / 255;
			g_ = float(point[i].g) / 255;
			b_ = float(point[i].b) / 255;

			r2_ = float(point[i].r2) / 255;
			g2_ = float(point[i].g2) / 255;
			b2_ = float(point[i].b2) / 255;

			glBegin(GL_LINES);
			glColor3f(r_, g_, b_);
			glVertex3f(point[i].x, point[i].y, graph[0].z);
			glColor3f(r2_, g2_, b2_);
			glVertex3f(point[i].x, point[i].y, -6.0f);
			glEnd();

			if (i < planetcount - 1)

			{
				glBegin(GL_LINES);

				glColor3f(r_, g_, b_);
				glVertex3f(point[i].x, point[i].y, graph[0].z);
				glColor3f(r2_, g2_, b2_);
				glVertex3f(point[i + 1].x, point[i + 1].y, graph[0].z);
				glColor3f(r_, g_, b_);
				glVertex3f(point[i].x, point[i].y, -6.0f);
				glColor3f(r2_, g2_, b2_);
				glVertex3f(point[i + 1].x, point[i + 1].y, -6.0f);
				glEnd();
			}

			else

			{
				glBegin(GL_LINES);
				glColor3f(r_, g_, b_);
				glVertex3f(point[planetcount - 1].x, point[planetcount - 1].y, graph[0].z);
				glColor3f(r2_, g2_, b2_);
				glVertex3f(point[0].x, point[0].y, graph[0].z);
				glColor3f(r2_, g2_, b2_);
				glVertex3f(point[planetcount - 1].x, point[planetcount - 1].y, -6.0f);
				glColor3f(r2_, g2_, b2_);
				glVertex3f(point[0].x, point[0].y, -6.0f);
				glEnd();
			}


			if (graph[1].active)

			{
				glColor3f(float(worm[i].r) / 255, float(worm[i].g) / 255, float(worm[i].b) / 255);

				if (i < planetcount - 1)

				{

					glBegin(GL_LINES);
					glVertex3f(worm[i].x, worm[i].y, graph[1].z);
					glVertex3f(worm[i + 1].x, worm[i + 1].y, graph[1].z);
					glEnd();

				}

				else

				{

					glBegin(GL_LINES);
					glVertex3f(worm[planetcount - 1].x, worm[planetcount - 1].y, graph[1].z);
					glVertex3f(worm[0].x, worm[0].y, graph[1].z);
					glEnd();

				}
			}
		}

	}

	//----------------- Zeichnen der Sterne ------------------------

	glPointSize(1.0f);
	glColor3f(1.0f, 1.0f, 1.0f);

	for (i = 0; i < starcount; i++)

	{
		glBegin(GL_POINTS);
		glVertex3f(stars[i].x, stars[i].y, -1.0f - zmove);
		glEnd();
	}

	if (!scientific)
		glColor3f(1.0f, 1.0f, 1.0f);
	else
		glColor3f(100.0f / 255, 100.0f / 255, 100.0f / 255);

	glTranslatef(0, 0, -12.0f);

	//----------------- Zeichnen des Texteffekts ------------------------

	if (texteffekt)

	{
		textz -= 0.2f;
		glTranslatef(0, 0, 0);
		glRotatef(textz, 1.0, 0, 0);
		glPrint(textz, textstring);

	}

	if (textz < -50.0f)
		texteffekt = false;

	//----------------- Zeichnen des Nebels ------------------------

	if (fog)

	{

		//	glClearColor(0.2f,0.2f,0.2f,1.0f);		// We'll Clear To The Color Of The Fog ( Modified )

		glFogi(GL_FOG_MODE, GL_LINEAR);				// Fog Mode
		glFogfv(GL_FOG_COLOR, fogColor);			// Set Fog Color
		glFogf(GL_FOG_DENSITY, 0.35f);				// How Dense Will The Fog Be
		glHint(GL_FOG_HINT, GL_DONT_CARE);			// Fog Hint Value
		glFogf(GL_FOG_START, 1.0f);					// Fog Start Depth
		glFogf(GL_FOG_END, 7.0f);					// Fog End Depth
		glEnable(GL_FOG);							// Enables GL_FOG

	}

	return true;
}

//------------------------------------------------------------------------------

//++++++++++++++++++
//Rendering Function
//++++++++++++++++++

void renderscene(struct winampVisModule* this_mod)

{
	int i;

	frameratio = 50 / float(framerate);

	//Behandlung der Ticks (Einheit fьr die Geschwindigkeit einiger Elemente (Framerateunabhдngig)

	floatticks++;

	if (floatticks >= 10 * (float(framerate) / 50))
	{
		for (i = 0; i < halfblur; i++)

		{
			ticks[i]++;
		}

		floatticks = 0;
	}

	for (i = 0; i < halfblur; i++)

	{
		if (ticks[i] >= blurcount)
			ticks[i] = 0;
	}

	for (i = 0; i < halfblur; i++)

	{
		renderblur(ticks[i]);
	}

	frames++;

	//Verwaltung der Winamp Daten

	hwndwinamp = FindWindow(L"Winamp v1.x", NULL);
	int aktplspos = SendMessage(hwndwinamp, WM_USER, 0, 125);
	int soundtl = SendMessage(hwndwinamp, WM_USER, aktplspos, 212);
	sprintf(soundtitle, "%s", soundtl);

	int songread = SendMessage(hwndwinamp, WM_USER, 0, 105);

	if (songread > 100 && songread < 200 && !songdisplayed)
	{
		songdisplayed = true;
		char temp[256];
		sprintf(temp, soundtitle);
		if (soundtitle != "")
			TextEffekt(soundtitle);
		else
		{
			strcpy(temp, "Sry, Function Not Available!");
			TextEffekt(temp);
		}
	}

	if (songread > 600)
		songdisplayed = false;

	/*	sprintf(title, "DarkVis OpenGL - BPM: %f", bpm);
		SetWindowText(hwnd, (LPCTSTR)title); */

		//----------------- Rendering der Peaks ------------------------

	for (i = 0; i < peakcount; i++)

	{
		peaks[i].y -= fallspeed;
	}

	//----------------- Rendering des "weglaufenden" Graphen ------------------------

	if (graph[1].z <= -6.0f)

	{

		for (int x = -(pointcount / 2); x < pointcount / 2; x++)
		{
			graph[1].x[x + pointcount / 2] = float(x);
			graph[1].y[x + pointcount / 2] = graph[0].y[x + pointcount / 2];
		}

		graph[1].z = graph[0].z;


		//----------------- Rendering des "weglaufenden" Kreises ------------------------

		for (i = 0; i < planetcount; i++)
		{
			worm[i].z = graph[1].z;
			worm[i].r = rand() % 255;
			worm[i].g = rand() % 255;
			worm[i].b = rand() % 255;
		}

		graph[1].r = rand() % 255;
		graph[1].g = rand() % 255;
		graph[1].b = rand() % 255;
	}

	if (graph[1].active)

	{
		graph[1].z -= 0.15f / frameratio;

		for (x = -(pointcount / 2); x < pointcount / 2; x++)
		{
			//graph[1].x[x + pointcount / 2] -= 2.0f * pointcount * 10;
			graph[1].y[x + pointcount / 2] -= (0.02f * pointcount * 10) / frameratio;
		}


		for (int i = 0; i < planetcount; i++)
		{

			worm[i].z = graph[1].z;

		}
	}

	//----------------- Rendering des aktuellen Graphen ------------------------

	for (x = -(pointcount / 2); x < pointcount / 2; x++)
	{
		graph[0].x[x + pointcount / 2] = float(x);
		if (spectrum)
			graph[0].y[x + pointcount / 2] = float(this_mod->spectrumData[1][int((x + pointcount / 2) * (576 / pointcount))]);
		else
			graph[0].y[x + pointcount / 2] = float(this_mod->waveformData[1][int(x * (576 / pointcount))]);
	}

	if (!landscape)

	{

		if (graph[0].z <= -0.2)
			zadd = 0.001f;

		if (graph[0].z >= -0.1)
			zadd = -0.001f;

		graph[0].z += zadd / frameratio;

	}

	if (!randomc)

	{

		if (graph[0].r >= 254 || graph[0].g >= 254 || graph[0].b >= 254)
			cadd = -1;

		if (graph[0].r <= 10 || graph[0].g <= 10 || graph[0].b <= 10)
			cadd = 1;

		graph[0].r += cadd;
		graph[0].g += cadd;
		graph[0].b += cadd;

	}

	else

	{

		graph[0].r = rand() % 255;
		graph[0].g = rand() % 255;
		graph[0].b = rand() % 255;

	}

	//----------------- Rendering der drehenden Linien (Wurmloch) ------------------------

	for (int i = 0; i < planetcount; i++)
	{

		if (point[i].angle >= 360)
			point[i].angle = 0.0f;

		if (worm[i].angle >= 360)
			worm[i].angle = 0.0f;


		point[i].angle += 0.25f / frameratio;

		if (zturnadd == -0.5f)
			point[i].angle += 0.75f / frameratio;


		worm[i].angle += 0.25f / frameratio;

		if (zturnadd == -0.5f)
			worm[i].angle += 0.75f / frameratio;


		point[i].x = float(point[i].radius * cos(point[i].angle * 0.017453));
		point[i].y = float(point[i].radius * sin(point[i].angle * 0.017453));

		worm[i].x = float(worm[i].radius * cos(worm[i].angle * 0.017453));
		worm[i].y = float(worm[i].radius * sin(worm[i].angle * 0.017453));

	}

	//----------------- Turn-Rendering (Schaukeleffekt) ------------------------

	if (zturn >= 50)
		zturnadd = -0.5f;

	if (zturn <= -50)
		zturnadd = 0.5f;

	zturn += zturnadd / frameratio;
}

//------------------------------------------------------------------------------

//++++++++++++++++++++++++
//Funktion fьr jeden Frame
//++++++++++++++++++++++++

int render(struct winampVisModule* this_mod)
{
	//----------------- Beats berechnen ------------------------

	if (beatdetection)
	{
		tmp_ticks++;
		getbeat();
	}

	//----------------- Alles Zeichnen ------------------------

	renderscene(this_mod);		//Rendert die Scene
	SwapBuffers(hDC);			//Buffer Austausch
	drawglscene();				//Stellt die Scene da

	return 0;
}

//------------------------------------------------------------------------------

//++++++++++++++++++++++
//Winamp Init Funktionen
//++++++++++++++++++++++

// module header, includes version, description, and address of the module retriever function
winampVisHeader hdr = { VIS_HDRVER, (char*)"DarkVis OpenGL for Winamp Beta 1.5", getModule };

// this is the only exported symbol. returns our main header.
// if you are compiling c++, the extern "c" { is necessary, so we just #ifdef it
#ifdef __cplusplus
extern "C" {
#endif
	__declspec(dllexport) winampVisHeader* winampVisGetHeader()
	{
		return &hdr;
	}
#ifdef __cplusplus
}
#endif

// initialization. registers our window class, creates our window, etc. again, this one works for
// both modules, but you could make init1() and init2()...
// returns 0 on success, 1 on failure.
int init(struct winampVisModule* this_mod)
{
	SetWindowLong(hwnd, GWL_USERDATA, (long)this_mod); // set our user data to a "this" pointer

	if (MessageBox(NULL, L"Fullscreen (1024 x 768)?", L"DarkVis OpenGL Fullscreen", MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		fullscreen = false;												// windowed mode

		if (!createglwindow((char*)"DarkVis OpenGL for Winamp Beta 1.5 - by DarKnight (C) 2001", 800, 600, 16, fullscreen, this_mod))
		{
			return 0;														// quit if window was not created
		}
	}

	else

	{
		if (!createglwindow((char*)"DarkVis OpenGL for Winamp Beta 1.5 - by DarKnight (C) 2001", 1024, 768, 16, fullscreen, this_mod))
		{
			return 0;														// quit if window was not created
		}
	}

	srand((unsigned)time(NULL));

	//Komplette Grafik Initierung

	BuildFont();				//Anlegen der Schrift

	r = rand() % 250;		//Farben des Wurmlochs
	g = rand() % 250;
	b = rand() % 250;

	r2 = rand() % 250;
	g2 = rand() % 250;
	b2 = rand() % 250;

	graph[0].z = -0.15f;		//Setze den ersten Graphen
	zadd = -0.001f;				//Geschwindigkeit mit der sich die Scene verschiebt

	for (int i = 1; i < blurcount; i++)	//Blur-Graphen Farben einrichten

	{
		blur[i].r = int(255 - i * (255 / blurcount));
		blur[i].g = int(255 - i * (255 / blurcount));
		blur[i].b = int(255 - i * (255 / blurcount));
	}

	graph[1].r = 60;			//Farben fьr den zweiten Graphen
	graph[1].g = 40;
	graph[1].b = 240;

	graph[0].active = true;		//Beide Graphen aktiven 
	graph[1].active = true;

	int i;
	for (i = 0; i < starcount; i++)	//Die Positionen der Sterne einrichten (Zufall)

	{
		stars[i].x = float(rand() % 300) / 100 - 1.5f;
		stars[i].y = float(rand() % 150) / 100 - 0.75f;
	}

	for (i = 0; i < halfblur; i++)

	{
		ticks[i] = i * 3;
	}

	config_read(this_mod);

	for (i = 0; i < planetcount; i++)	//Richte das Wurmloch ein

	{
		tmpradius = rand() % 11;
		tmpangle = (360 / planetcount) * i;
		tmpspeed = rand() % 21;

		point[i].radius = 0.1f;				//float(tmpradius) / 10;
		point[i].angle = float(tmpangle);
		point[i].speed = 0.5f;				//float(tmpspeed) / 10;

		point[i].r = r;
		point[i].g = g;
		point[i].b = b;

		point[i].r2 = r2;
		point[i].g2 = g2;
		point[i].b2 = b2;

		worm[i].radius = 0.1f;				//float(tmpradius) / 10;
		worm[i].angle = float(tmpangle);
		worm[i].speed = 0.5f;				//float(tmpspeed) / 10;

		worm[i].r = 60;
		worm[i].g = 40;
		worm[i].b = 240;

		if (point[i].radius == 0.00f)
			point[i].radius = 0.1f;


		if (point[i].speed == 0.00f)
			point[i].speed = 0.5f;
	}

	return 0;
}

// cleanup (opposite of init()). Destroys the window, unregisters the window class
void quit(struct winampVisModule* this_mod)
{
	KillFont();		//Schriftart auflцsen

	//Speicherreservierung der Arrays freigeben

	delete[] blur;
	delete[] point;
	delete[] worm;
	delete[] stars;
	delete[] peaks;

	config_write(this_mod);		// write configuration

	killglwindow();														//Fenster auflцsen														// Kill The Window
	UnregisterClass(L"OpenGL", this_mod->hDllInstance);					// unregister window class
}

/*
=============================
WndProc Nachrichtenbehandlung
=============================
*/

LRESULT CALLBACK WndProc(HWND        hWnd,								// Handle For This Window
	UINT        uMsg,								// Message For This Window
	WPARAM        wParam,							// Additional Message Information
	LPARAM        lParam)							// Additional Message Information
{
	switch (uMsg)														// Check For Windows Messages

	{

	case WM_DESTROY: PostQuitMessage(0); return 0;
	case WM_CREATE:		return 0;

	case WM_ACTIVATE:													// Watch For Window Activate Message
	{
		if (!HIWORD(wParam))										// Check Minimization State
		{
			active = TRUE;											// Program Is Active
		}
		else
		{
			active = FALSE;											// Program Is No Longer Active
		}

		return 0;													// Return To The Message Loop
	}

	case WM_SYSCOMMAND:													// Intercept System Commands
	{
		switch (wParam)												// Check System Calls
		{
		case SC_SCREENSAVE:											// Screensaver Trying To Start?
		case SC_MONITORPOWER:										// Monitor Trying To Enter Powersave?
			return 0;												// Prevent From Happening
		}
		break;														// Exit
	}

	//Behandlung der Key Messages

	case WM_KEYDOWN:													// Is A Key Being Held Down?
	{

		keys[wParam] = TRUE;


		if (keys[VK_ESCAPE])

		{
			PostQuitMessage(0);
		}

		if (keys[VK_UP] && zmove <= 3.0f)

		{
			if (!landscape || scientific)
				zmove += 0.02f;
		}

		if (keys[VK_DOWN] && zmove >= -0.3f)

		{
			if (!landscape || scientific)
				zmove -= 0.02f;
		}

		if (keys[VK_LEFT] && framerate >= 1)

		{
			framerate--;
		}

		if (keys[VK_RIGHT])

		{
			framerate++;
		}

		if (keys[VK_SPACE])

		{
			framerate = 10;
			for (int i = 0; i < planetcount; i++)	//Richte das Wurmloch ein

			{
				tmpradius = rand() % 11;
				tmpangle = (360 / planetcount) * i;
				tmpspeed = rand() % 21;

				point[i].radius = 0.1f;				//float(tmpradius) / 10;
				point[i].angle = float(tmpangle);
				point[i].speed = 0.5f;				//float(tmpspeed) / 10;

				worm[i].radius = 0.1f;				//float(tmpradius) / 10;
				worm[i].angle = float(tmpangle);
				worm[i].speed = 0.5f;				//float(tmpspeed) / 10;
			}

			TextEffekt("Background Moverate Reset");
		}

		if (keys['A'])

		{
			if (!anti)
			{
				anti = true;
				TextEffekt("Anti Aliasing Enabled");
			}

			else
			{
				anti = false;
				glDisable(GL_LINE_SMOOTH);
				TextEffekt("Anti Aliasing Disabled");
			}
		}

		if (keys['B'])

		{
			modeset++;

			if (modeset == 3)
				modeset = 0;

			if (modeset == 0)
				setlandscape(1);

			if (modeset == 1)
			{
				setlandscape(0);
				setscientific(1);
			}

			if (modeset == 2)
			{
				setscientific(0);
				setdefault(1);
			}

		}

		if (keys[VK_NEXT] && connectioncount > 5)

		{
			connectioncount--;
		}

		if (keys[VK_PRIOR] && connectioncount < 30)

		{
			connectioncount++;
		}

		if (keys['N'])

		{
			if (actpeaks)
			{
				actpeaks = false;
				TextEffekt("Peaks Disabled");
			}
			else
			{
				actpeaks = true;
				TextEffekt("Peaks Enabled");
			}
		}

		if (keys['X'])

		{
			if (!connections)
			{
				connections = true;
				TextEffekt("Enabled Connections");
			}
			else
			{
				connections = false;
				TextEffekt("Disabled Connections");
			}
		}

		if (keys['Y'])

		{
			if (!spectrum)
			{
				spectrum = true;
				actpeaks = true;
				TextEffekt("Toggled Spectrumdata Graph");
			}
			else
			{
				spectrum = false;
				actpeaks = false;
				TextEffekt("Toggled Wavedata Graph");
			}
		}


		if (keys['C'])

		{
			if (!randomc)
			{
				randomc = true;
				TextEffekt("Toggled Random Graph Color");
			}
			else
			{
				randomc = false;
				graph[0].r = 100;
				graph[0].g = 100;
				graph[0].b = 100;

				for (int i = 1; i < blurcount; i++)

				{

					blur[i].r = int(255 - i * (255 / blurcount));
					blur[i].g = int(255 - i * (255 / blurcount));
					blur[i].b = int(255 - i * (255 / blurcount));

				}

				TextEffekt("Toggled Default Graph Color");
			}
		}

		if (keys['T'])

		{
			char temp[256];
			sprintf(temp, soundtitle);
			if (soundtitle != "")
				TextEffekt(soundtitle);

			else
			{
				strcpy(temp, "Sry, Function Not Available!");
				TextEffekt(temp);
			}

		}

		if (keys[VK_NUMPAD4])
		{
			TextEffekt("Previous Track");
			SendMessage(hwndwinamp, WM_COMMAND, 40044, 0);
		}
		if (keys[VK_NUMPAD6])
		{
			TextEffekt("Next Track");
			SendMessage(hwndwinamp, WM_COMMAND, 40048, 0);
		}
		if (keys[VK_NUMPAD2])
		{
			if (fallspeed >= 0)
				fallspeed -= 0.00005f;
		}

		if (keys[VK_NUMPAD8])
			fallspeed += 0.00005f;

		if (keys['V'])

		{
			r = rand() % 250;
			g = rand() % 250;
			b = rand() % 250;
			r2 = rand() % 250;
			g2 = rand() % 250;
			b2 = rand() % 250;

			for (int i = 0; i < planetcount; i++)

			{

				point[i].r = r;
				point[i].g = g;
				point[i].b = b;

				point[i].r2 = r2;
				point[i].g2 = g2;
				point[i].b2 = b2;

			}

			TextEffekt("Changed Wormhole Colors");
		}

		/*if(keys['N'])

{
	if(!solid)
	{
		solid = true;
		TextEffekt("Enabled Solid Rendering");
	}
	else
	{
		solid = false;
		TextEffekt("Disabled Solid Rendering");
	}
}


if(keys['M'])
{
	Beat();
}
*/

		return 0;													// Jump Back 
	}

	case WM_KEYUP:														// Has A Key Been Released?
	{
		keys[wParam] = FALSE;										// If So, Mark It As FALSE
		return 0;													// Jump Back
	}

	case WM_SIZE:														// Resize The OpenGL Window
	{
		resizeglscene(LOWORD(lParam), HIWORD(lParam));				// LoWord=Width, HiWord=Height
		drawglscene();
		return 0;													// Jump Back
	}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//Ende von main.cpp =)
