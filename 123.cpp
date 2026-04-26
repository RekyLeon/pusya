// XMPlay Waveform visual plugin

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "C:\Users\Леонид\Desktop\xmp-sdk\xmpvis.h"

XMPFUNC_REGISTRY* xmpfreg;

HINSTANCE dllinst;
HWND confwin;

DWORD colors[2] = { 0x00ff00, 0xf00f00 };

void WINAPI VIS_Config(HWND win);
BOOL WINAPI VIS_Open(DWORD colors[3]);
void WINAPI VIS_Close();
BOOL WINAPI VIS_Render(DWORD* buf, SIZE size, XMPVIS_DATA* data, DWORD flags);
void WINAPI VIS_Button(DWORD x, DWORD y);

XMPVIS xmpvis = {
	XMPVIS_FLAG_PCMDATA,
	"Waveform",
	256,
	0,
	VIS_Config,
	VIS_Open,
	VIS_Close,
	VIS_Render,
	NULL,
	VIS_Button
};

BOOL CALLBACK ConfigDialogProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	switch (m) {
	case WM_COMMAND:
		switch (LOWORD(w)) {
		case IDCANCEL:
			EndDialog(h, 0);
			break;
		case 10:
		case 11:
		{
			char buf[32];

			GetDlgItemText(h, LOWORD(w), buf, sizeof(buf));
			if (strlen(buf) == 6) colors[LOWORD(w) - 10] = strtol(buf, 0, 16);
		}
		break;
		case 12:
		{
			int n = GetDlgItemInt(h, LOWORD(w), NULL, FALSE);
			if (n > 0) xmpvis.samples = n;
		}
		break;
		case 13:
			if (SendMessage((HWND)l, BM_GETCHECK, 0, 0))
				xmpvis.flags |= XMPVIS_FLAG_MONODATA;
			else
				xmpvis.flags &= ~XMPVIS_FLAG_MONODATA;
			break;
		}
		return 1;

	case WM_HSCROLL:
		if (l) xmpvis.fadetime = SendMessage((HWND)l, TBM_GETPOS, 0, 0) / 10.f;
		return 1;

	case WM_INITDIALOG:
		confwin = h;
		{
			POINT p;
			GetCursorPos(&p);
			SetWindowPos(h, 0, p.x - 120, p.y - 10, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			for (int a = 0; a < 2; a++) {
				char buf[32];
				sprintf(buf, "%06X", colors[a]);
				SetDlgItemText(h, 10 + a, buf);
			}
			SetDlgItemInt(h, 12, xmpvis.samples, FALSE);
			SendDlgItemMessage(h, 13, BM_SETCHECK, xmpvis.flags & XMPVIS_FLAG_MONODATA ? 1 : 0, 0);
			SendDlgItemMessage(h, 14, TBM_SETRANGE, FALSE, MAKELONG(0, 20));
			SendDlgItemMessage(h, 14, TBM_SETPOS, TRUE, xmpvis.fadetime * 10 + 0.5f);
		}
		return 1;

	case WM_DESTROY:
		confwin = 0;
		break;
	}
	return 0;
}

void WINAPI VIS_Config(HWND win)
{
	if (confwin)
		SetForegroundWindow(confwin);
	else
		DialogBox(dllinst, MAKEINTRESOURCE(1000), win, ConfigDialogProc);
}

BOOL WINAPI VIS_Open(DWORD colors[3])
{
	return TRUE;
}

void WINAPI VIS_Close()
{
}

BOOL WINAPI VIS_Render(DWORD* buf, SIZE size, XMPVIS_DATA* data, DWORD flags)
{
	static bool blank; // last render was blank

	if (!data->samples) { // no data (not playing)
		if (blank && !(flags & XMPVIS_RENDER_INIT)) return FALSE; // nothing to do
		blank = true;
	}
	else {
		blank = false;
		xmpvis.samples = data->samples; // update config in case requested number was invalid
	}

	if (!xmpvis.fadetime) memset(buf, 0, size.cy * size.cx * 4); // clear the display first if not fading

	float step = (float)(data->samples - 1) / size.cx; // samples per pixel

	for (int a = 0; a < min(data->chans, 2); a++) {
		float* pcm = data->pcm[a];
		DWORD color = colors[a];
		float wavepos = 0;
		for (int x = 0, y; x < size.cx; x++, wavepos += step) {
			int waveposi = wavepos;
			float sample = pcm[waveposi] + (pcm[waveposi + 1] - pcm[waveposi]) * (wavepos - waveposi);
			int v = (size.cy / 2) * (1 - sample);
			if (v < 0) v = 0;
			else if (v >= size.cy) v = size.cy - 1;
			if (!x) y = v;
			do {
				if (y < v) y++;
				else if (y > v) y--;
				buf[y * size.cx + x] = color;
			} while (y != v);
		}
	}

	return TRUE;
}

void WINAPI VIS_Button(DWORD x, DWORD y)
{
	// toggle mono/stereo
	xmpvis.flags ^= XMPVIS_FLAG_MONODATA;
}

XMPVIS* WINAPI XMPVIS_GetInterface(DWORD face, InterfaceProc faceproc)
{
	if (face != XMPVIS_FACE) return NULL;
	xmpfreg = (XMPFUNC_REGISTRY*)faceproc(XMPFUNC_REGISTRY_FACE);
	// get config
	char buf[32];
	if (xmpfreg->GetString("Waveform vis", "ColorLeft", buf, sizeof(buf)))
		colors[0] = strtol(buf, 0, 16);
	if (xmpfreg->GetString("Waveform vis", "ColorRight", buf, sizeof(buf)))
		colors[1] = strtol(buf, 0, 16);
	xmpfreg->GetInt("Waveform vis", "Samples", (int*)&xmpvis.samples);
	int val;
	xmpfreg->GetInt("Waveform vis", "Mono", &val);
	if (val) xmpvis.flags |= XMPVIS_FLAG_MONODATA;
	xmpfreg->GetInt("Waveform vis", "FadeTime", &val);
	xmpvis.fadetime = val / 1000.f;

	return &xmpvis;
}

BOOL WINAPI DllMain(HINSTANCE hDLL, DWORD reason, LPVOID reserved)
{
	switch (reason) {
	case DLL_PROCESS_ATTACH:
		dllinst = hDLL;
		DisableThreadLibraryCalls(hDLL);
		break;

	case DLL_PROCESS_DETACH:
		// store config
		if (xmpfreg) {
			char buf[32];
			sprintf(buf, "%06X", colors[0]);
			xmpfreg->SetString("Waveform vis", "ColorLeft", buf);
			sprintf(buf, "%06X", colors[1]);
			xmpfreg->SetString("Waveform vis", "ColorRight", buf);
			xmpfreg->SetInt("Waveform vis", "Samples", (int*)&xmpvis.samples);
			int val = xmpvis.flags & XMPVIS_FLAG_MONODATA ? 1 : 0;
			xmpfreg->SetInt("Waveform vis", "Mono", &val);
			val = xmpvis.fadetime * 1000 + 0.5f;
			xmpfreg->SetInt("Waveform vis", "FadeTime", &val);
		}
		break;
	}
	return TRUE;
}
