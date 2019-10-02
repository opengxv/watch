#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "tesseract/baseapi.h"
#include "leptonica/allheaders.h"
#include <iconv.h>
#include <getopt.h>
#include <map>
#include <string>

HWND hWindow;

#define BIT_COUNT 32
#define MARK1 40320540000
#define MARK2 40320550000

const char* vk_strs[] = {
	"CTRL-1",
	"CTRL-2",
	"CTRL-3",
	"CTRL-4",
	"CTRL-5",
	"CTRL-6",
	"CTRL-7",
	"CTRL-8",
	"CTRL-9",
	"CTRL-0",
	"ALT-1",
	"ALT-2",
	"ALT-3",
	"ALT-4",
	"ALT-5",
	"ALT-6",
	"ALT-7",
	"ALT-8",
	"ALT-9",
	"ALT-0",
	"ALT-F1",
	"ALT-F2",
	"ALT-F3",
	"ALT-F5",
	"ALT-F6",
	"ALT-F7",
	"ALT-F8",
	"ALT-F9",
	"ALT-F10",
	"ALT-F11",
	"ALT-F12",
	"CTRL-F1",
	"CTRL-F2",
	"CTRL-F3",
	"CTRL-F4",
	"CTRL-F5",
	"CTRL-F6",
	"CTRL-F7",
	"CTRL-F8",
	"CTRL-F9",
	"CTRL-F10",
	"CTRL-F11",
	"CTRL-F12",
	"ALT-CTRL-1",
	"ALT-CTRL-2",
	"ALT-CTRL-3",
	"ALT-CTRL-4",
	"ALT-CTRL-5",
	"ALT-CTRL-6",
	"ALT-CTRL-7",
	"ALT-CTRL-8",
	"ALT-CTRL-9",
	"ALT-CTRL-0",
	"ALT-CTRL-F1",
	"ALT-CTRL-F2",
	"ALT-CTRL-F3",
	"ALT-CTRL-F5",
	"ALT-CTRL-F6",
	"ALT-CTRL-F7",
	"ALT-CTRL-F8",
	"ALT-CTRL-F9",
	"ALT-CTRL-F10",
	"ALT-CTRL-F11",
	"ALT-CTRL-F12",
	"SHIFT-CTRL-F1",
	"SHIFT-CTRL-F2",
	"SHIFT-CTRL-F3",
	"SHIFT-CTRL-F5",
	"SHIFT-CTRL-F6",
	"SHIFT-CTRL-F7",
	"SHIFT-CTRL-F8",
	"SHIFT-CTRL-F9",
	"SHIFT-CTRL-F10",
	"SHIFT-CTRL-F11",
	"SHIFT-CTRL-F12",
	"SHIFT-ALT-F1",
	"SHIFT-ALT-F2",
	"SHIFT-ALT-F3",
	"SHIFT-ALT-F5",
	"SHIFT-ALT-F6",
	"SHIFT-ALT-F7",
	"SHIFT-ALT-F8",
	"SHIFT-ALT-F9",
	"SHIFT-ALT-F10",
	"SHIFT-ALT-F11",
	"SHIFT-ALT-F12",
	"SHIFT-CTRL-1",
	"SHIFT-CTRL-2",
	"SHIFT-CTRL-3",
	"SHIFT-CTRL-4",
	"SHIFT-CTRL-5",
	"SHIFT-CTRL-6",
	"SHIFT-CTRL-7",
	"SHIFT-CTRL-8",
	"SHIFT-CTRL-9",
	"SHIFT-CTRL-0"
};

std::map<std::string, int> vk_map = {
	{"CTRL",	VK_CONTROL},
	{"ALT",		VK_MENU},
	{"SHIFT",	VK_SHIFT},
	{"0",		0x30},
	{"1",		0x31},
	{"2",		0x32},
	{"3",		0x33},
	{"4",		0x34},
	{"5",		0x35},
	{"6",		0x36},
	{"7",		0x37},
	{"8",		0x38},
	{"9",		0x39},
	{"F1",		VK_F1},
	{"F2",		VK_F2},
	{"F3",		VK_F3},
	{"F4",		VK_F4},
	{"F5",		VK_F5},
	{"F6",		VK_F6},
	{"F7",		VK_F7},
	{"F8",		VK_F8},
	{"F9",		VK_F9},
	{"F10",		VK_F10},
	{"F11",		VK_F11},
	{"F12",		VK_F12},
};
#define KEY_COUNT (sizeof(vk_strs) / sizeof(char*))
struct keymap_t {
	INPUT inputs[4];
	unsigned count;
};
keymap_t keymaps[KEY_COUNT];

void init_keymaps() {
	char buf[255];
	keymap_t *key = keymaps;
	int ki;
	for (int i = 0; i < KEY_COUNT; ++i, ++key) {
		const char* str = vk_strs[i];
		const char* p;
		const char* org = str;
		size_t n;

		memset(key, 0, sizeof(keymap_t));
		while (*org) {
			p = strchr(org, '-');
			if (p) {
				n = p - org;
				strncpy(buf, org, n);
				org = p + 1;
			}
			else {
				n = strlen(org);
				strncpy(buf, org, n);
				org += n;
			}

			if (n) {
				buf[n] = '\0';
				INPUT* input = &key->inputs[key->count++];
				input->type = INPUT_KEYBOARD;
				input->ki.dwFlags = (DWORD)0;
				input->ki.wVk = (WORD)vk_map[buf];
				input->ki.wScan = MapVirtualKey(input->ki.wVk, 0);
			}
		}
	}
}

void ShowLastError() {
	// Retrieve the system error message for the last-error code
	DWORD ERROR_ID = GetLastError();
	void* MsgBuffer = nullptr;
	//LCID lcid;
	//GetLocaleInfoEx(L"en-US", LOCALE_RETURN_NUMBER | LOCALE_ILANGUAGE, (wchar_t*)& lcid, sizeof(lcid));

	//get error message and attach it to Msgbuffer
	FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, ERROR_ID, 0, (wchar_t*)& MsgBuffer, 0, NULL);
	//concatonate string to DisplayBuffer
	const std::wstring DisplayBuffer = L" failed with error " + std::to_wstring(ERROR_ID) + L": " + static_cast<wchar_t*>(MsgBuffer);

	// Display the error message and exit the process
	MessageBoxExW(NULL, DisplayBuffer.c_str(), L"Error", MB_ICONERROR | MB_OK, 0);
}

#define assertWin32(v) if (!(v)) { ShowLastError(); exit(1); }

class Screen {
private:
	HWND m_hwnd;
	HDC m_hdcWnd;
	HBITMAP m_hbmWnd;
	HDC m_hdcMem;
	BITMAP m_bmpWnd;
	unsigned char* m_bmp;
	unsigned char* m_buffer;
	unsigned m_size;
	unsigned m_buffersize;
	int m_x;
	int m_y;
	unsigned m_width;
	unsigned m_height;
	BITMAPFILEHEADER* m_header;
	BITMAPINFOHEADER *m_bi;
public:
	Screen(HWND hwnd, int x, int y, unsigned width, unsigned height) 
	: m_hwnd(hwnd), m_hdcWnd(), m_hbmWnd(), m_hdcMem(), m_bmpWnd(), m_buffer(),
	  m_x(x), m_y(y), m_width(width), m_height(height) {
		assertWin32(m_hdcWnd = ::GetDC(hwnd));
		assertWin32(m_hbmWnd = ::CreateCompatibleBitmap(m_hdcWnd, m_width, m_height));
		assertWin32(m_hdcMem = ::CreateCompatibleDC(m_hdcWnd));
		assertWin32(::SelectObject(m_hdcMem, m_hbmWnd));

		m_size = ((m_width * BIT_COUNT + 31) / 32) * 4 * m_height;
		m_buffersize = m_size + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		m_buffer = (unsigned char*)malloc(m_buffersize);
		memset(m_buffer, 0, m_buffersize);

		m_header = (BITMAPFILEHEADER*)m_buffer;
		m_bi = (BITMAPINFOHEADER*)(m_buffer + sizeof(BITMAPFILEHEADER));
		m_bmp = m_buffer + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		BITMAP bmpWnd;
		assertWin32(::GetObject(m_hbmWnd, sizeof(BITMAP), &bmpWnd));

		m_bi->biSize = sizeof(BITMAPINFOHEADER);
		m_bi->biWidth = bmpWnd.bmWidth;
		m_bi->biHeight = bmpWnd.bmHeight;
		m_bi->biPlanes = 1;
		m_bi->biBitCount = BIT_COUNT;
		m_bi->biCompression = BI_RGB;
		m_bi->biSizeImage = 0;
		m_bi->biXPelsPerMeter = 0;
		m_bi->biYPelsPerMeter = 0;
		m_bi->biClrUsed = 0;
		m_bi->biClrImportant = 0;

		m_header->bfType = 0x4d42;
		m_header->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		m_header->bfSize = m_buffersize;
	}

	~Screen() {
		if (m_hdcMem) {
			::DeleteDC(m_hdcMem);
		}
		if (m_hbmWnd) {
			::DeleteObject(m_hbmWnd);
		}
		if (m_hdcWnd) {
			::ReleaseDC(m_hwnd, m_hdcWnd);
		}
		if (m_buffer) {
			free(m_buffer);
		}
	}

	PIX* get() {
		if (!::BitBlt(m_hdcMem, 0, 0, m_width, m_height, m_hdcWnd, m_x, m_y, SRCCOPY)) {
			return NULL;
		}
		if (!::GetDIBits(m_hdcMem, m_hbmWnd, 0, m_height, m_bmp, (BITMAPINFO*)m_bi, DIB_RGB_COLORS)) {
			return NULL;
		}
		return pixReadMemBmp(m_buffer, m_buffersize);
	}

	void save() {
		FILE* fp = fopen("capture.bmp", "w");
		fwrite(m_buffer, m_buffersize, 1, fp); 
		fclose(fp);
	}
};

unsigned long long get_time() {
	struct timeval tv;		
	time_t clock;		
	struct tm tm;		
	SYSTEMTIME wtm; 		
	GetLocalTime(&wtm);		
	tm.tm_year = wtm.wYear - 1900;		
	tm.tm_mon = wtm.wMonth - 1;		
	tm.tm_mday = wtm.wDay;		
	tm.tm_hour = wtm.wHour;		
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm.tm_isdst = -1;
	clock = mktime(&tm);
	tv.tv_sec = clock;
	tv.tv_usec = wtm.wMilliseconds * 1000;	
	return ((unsigned long long)tv.tv_sec * 1000 + (unsigned long long)tv.tv_usec / 1000);
}

void send_input(int32_t keyvalue, bool is_up) {
	INPUT input;
	memset(&input, 0, sizeof(INPUT));
	input.type = INPUT_KEYBOARD;
	input.ki.dwFlags = is_up ? KEYEVENTF_KEYUP : 0;
	input.ki.wVk = (WORD)keyvalue;
	input.ki.wScan = MapVirtualKey(keyvalue, 0);
	SendInput(1, &input, sizeof(INPUT));
}

void send_key(unsigned event) {
	if (event < 1) {
		return;
	}
	event--;
	if (event >= KEY_COUNT) {
		return;
	}

	keymap_t* key = &keymaps[event];
	for (int i = 0; i < key->count; i++) {
		INPUT* input = &key->inputs[i];
		input->ki.dwFlags = (DWORD)0;
	}
	SendInput(key->count, key->inputs, sizeof(INPUT));
	Sleep(10);
	for (int i = 0; i < key->count; i++) {
		INPUT* input = &key->inputs[i];
		input->ki.dwFlags = (DWORD)KEYEVENTF_KEYUP;
	}
	SendInput(key->count, key->inputs, sizeof(INPUT));
	printf("%s\n", vk_strs[event]);
}

void handle_event(unsigned long long event) {
	event -= MARK1;
	event %= 100;

	send_key(event);
}

int main(int argc, char **argv)
{
	int x = 10;
	int y = -116;
	unsigned width = 150;
	unsigned height = 25;
	bool storage = false;
	bool capture = false;

	FILE *f = fopen("watch.cfg", "r");
	if (f) {
		fscanf(f, "%d %d %u %u", &x, &y, &width, &height);
		fclose(f);
	}

	int opt;
	while ((opt = getopt(argc, argv, "csx:y:w:h:")) != -1) {
		switch (opt) {
		case 's':
			storage = true;
			break;
		case 'c':
			capture = true;
			break;
		case 'x':
			x = atoi(optarg);
			break;

		case 'y':
			y = atoi(optarg);
			break;
		case 'w':
			width = atoi(optarg);
			break;
		case 'h':
			height = atoi(optarg);
			break;
		default:
			fprintf(stderr, "param error.\n");
			return 0;
		}
	}

	printf("x = %d, y = %d, w = %d, h = %d\n", x, y, width, height);
	if (storage) {
		f = fopen("watch.cfg", "w+");
		if (f) {
			fprintf(f, "%d %d %u %u\n", x, y, width, height);
			fclose(f);
		}
		return 0;
	}

	init_keymaps();

	int screen_height = GetSystemMetrics(SM_CYSCREEN);
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	printf("screen_width = %d, screen_height = %d\n", screen_width, screen_height);
	y += screen_height;

again:
	while (1) {
		Screen screen(0, x, y, width, height);
		if (capture) {
			Pix* pix = screen.get();
			screen.save();
			pixDestroy(&pix);
			return 0;
		}


		char* out;
		tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
		if (api->Init(NULL, "eng")) {
			fprintf(stderr, "Could not initialize tesseract.\n");
			exit(1);
		}

		unsigned long long old = 0;
		while (1) {
			unsigned long long t = get_time();
			Pix* pix = screen.get();
			if (!pix) {
				Sleep(1000);
				goto again;
			}
			pix->xres = 70;
			pix->yres = 70;
			api->SetImage(pix);	// Get OCR result	
			out = api->GetUTF8Text();
			unsigned long long val = atoll(out);
			if (val >= MARK1 && val <= MARK2) {
				if (old != val) {
					old = val;
					handle_event(old);
					printf("%llu %llu\n", old, get_time() - t);
				}
			}

			delete[] out;
			pixDestroy(&pix);
			Sleep(20);
		}
		api->End();
		delete api;
	}

	return 0;
}

