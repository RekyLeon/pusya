/*
DIESER VERTRAG DARF AUS KEINER WEITERENTWICKELTEN VERSION DIESES PLUGINS ENTFERNT WERDEN!

DarkVis Beta 1.5 Code - init.cpp

Copyright (C) 2001 by DarKnight (ICQ: 76468699, eMail: darknight@deltaeagle.net)

Teile des Codes basieren auf dem Initierungscode von NeHe (nehe.gamedev.net)
oder dem VIS SDK für Winamp von Nullsoft (www.winamp.com).

Sie sind berechtigt diesen Code beliebig zu verändern und auch diesen
Code weiterzugeben. Allerdings ist es ihnen nicht gestattet, Elemente dieses
Codes in kommerziellen Produkten zu verwenden. Ausserdem würde ich mich als
Entwickler des Ursprungscodes darüber freuen über alle Veränderungen aufgeklärt
zu werden.

---

Alle externen Effekte befinden sich hier

*/

//------------------------------------------------------------------------------

#include "init.h"

extern int beatsecond;
extern int pointcount;
extern int cadd;
extern int tmp_ticks;

extern float textz;
extern float bpm;
extern float zmove;
extern float yturn;

extern bool randomc;
extern bool texteffekt;
extern bool connections;
extern bool landscape;
extern bool scientific;
extern bool whipping;

extern Graph* blur;
extern Point* point;
extern Point* worm;
extern Point* peaks;
extern Star* stars;

extern Graph graph[2];

extern char textstring[256];

//------------------------------------------------------------------------------

int bstopcount = 4;
int _ticks;
int beats[2][4];
int tmp_beats[2][4];
int beatsum[2];

float bpm_temp;

//------------------------------------------------------------------------------

//+++++++++++++
//Beatdetection
//+++++++++++++

void getbeat()

{
	if (tmp_ticks * 10 >= 500)

	{
		tmp_ticks = 0;
		beatsecond++;

		if (beatsecond > bstopcount - 1)
		{
			for (int j = 0; j < 2; j++)

			{
				for (int i = 0; i < bstopcount; i++)
				{
					tmp_beats[j][i] = beats[j][i];
					beats[j][i] = 0;
				}

				for (int i = 0; i < bstopcount; i++)
				{
					beatsum[j] = beatsum[j] + tmp_beats[j][i];
				}
			}

			bpm_temp = (float(beatsum[0] + beatsum[1])) / 2;
			bpm_temp = bpm_temp / bstopcount;
			bpm = bpm_temp * 30;

			if (bpm > 400)
				bpm = 400;
			if (bpm < 0)
				bpm = 0;

			beatsecond = 0;

			for (int j = 0; j < 2; j++)
			{
				beatsum[j] = 0;
			}
		}
	}
}

//------------------------------------------------------------------------------

//+++++++++++++++++++++++++++++++++++
//Renderfunktion für die Blur-Graphen
//+++++++++++++++++++++++++++++++++++

void renderblur(int index)

{
	int b = index;

	if (b == 0)

	{
		for (int x = -(pointcount / 2); x < pointcount / 2; x++)
		{
			blur[0].x[x + pointcount / 2] = graph[0].x[x + pointcount / 2];
			blur[0].y[x + pointcount / 2] = graph[0].y[x + pointcount / 2];

			if (randomc)

			{
				blur[0].r = graph[0].r;
				blur[0].g = graph[0].g;
				blur[0].b = graph[0].b;
			}

		}
	}

	else

	{

		for (int x = -(pointcount / 2); x < pointcount / 2; x++)
		{
			blur[b].x[x + pointcount / 2] = blur[b - 1].x[x + pointcount / 2];
			blur[b].y[x + pointcount / 2] = blur[b - 1].y[x + pointcount / 2];
		}

		if (randomc)

		{
			blur[b].r = blur[b - 1].r;
			blur[b].g = blur[b - 1].g;
			blur[b].b = blur[b - 1].b;
		}
	}
}

//------------------------------------------------------------------------------

//+++++++++++++++++++++++++++++++++++
//Funktion für den Text Effekt
//+++++++++++++++++++++++++++++++++++

void TextEffekt(char* string)

{
	sprintf(textstring, "");
	textz = -4.0f;
	sprintf(textstring, string);
	texteffekt = true;
}

//------------------------------------------------------------------------------


//+++++++++++++++++++++++++++++++++++
//Beat Funktion
//+++++++++++++++++++++++++++++++++++

void Beat()

{
	for (int x = -(pointcount / 2); x < pointcount / 2; x++)
	{
		graph[1].x[x + pointcount / 2] = float(x);
		graph[1].y[x + pointcount / 2] = graph[0].y[x + pointcount / 2];
	}

	graph[1].z = graph[0].z;


	//----------------- Rendering des "weglaufenden" Kreises ------------------------

	for (int i = 0; i < planetcount; i++)
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

/*
Mode Set Methoden
*/

//==================================================

//Landscape Mode

void setlandscape(int _switch)

{
	if (_switch == 1)

	{
		graph[1].active = false;
		graph[0].z = -0.1f;
		connections = true;
		randomc = true;
		landscape = true;
		TextEffekt(const_cast<char*>("Landscape Mode"));
	}
	else
	{

		graph[1].active = true;
		graph[0].z = -0.2f;
		landscape = false;
	}
}

//Scientific Mode

void setscientific(int _switch)

{
	if (_switch == 1)

	{
		scientific = true;
		whipping = false;
		landscape = true;
		connections = false;
		graph[1].active = false;
		randomc = false;

		yturn = 0;
		cadd = 0;
		zmove = 0.05f;

		graph[0].z = -0.2f;

		graph[0].r = 100;
		graph[0].g = 100;
		graph[0].b = 100;

		for (int i = 1; i < blurcount; i++)

		{
			blur[i].r = int(100 + i * (255 / blurcount));
			blur[i].g = int(100 + i * (255 / blurcount));
			blur[i].b = int(100 + i * (255 / blurcount));

		}

		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

		TextEffekt(const_cast<char*>("Scientific Mode"));
	}

	else

	{
		scientific = false;
		whipping = true;
		landscape = false;
		connections = true;
		graph[1].active = true;
		randomc = true;

		yturn = 0;
		cadd = 0;
		zmove = 0;

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	}
}

//Default Mode

void setdefault(int _switch)

{
	if (_switch == 1)

	{
		graph[1].active = true;
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		TextEffekt(const_cast<char*>("Default Mode"));
	}
}

//Ende von effects.cpp =)