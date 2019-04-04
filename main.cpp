//
// This is loosely based on this:
// https://docs.microsoft.com/en-us/cpp/windows/walkthrough-creating-windows-desktop-applications-cpp?view=vs-2019
//
// Pete Goodwin 04/04/19

#include <windows.h>
#include <tchar.h>
#include <math.h>

#define PI 3.1415926538

static const TCHAR * windowClassName = _T("SampleDesktop");
static const TCHAR * windowTitle = _T("Sample Desktop Application");
static const TCHAR * greeting = _T("This is a text string");

static HFONT hFont;
static LOGFONT logFont;

void ObliqueTextInitialisation()
{
	hFont = CreateFont(48, 0, 0, 0, FW_DONTCARE, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, _T("Impact"));

	GetObject(hFont, sizeof(LOGFONT), &logFont);
}

void ObliqueTextOut(HDC dc, int oblique, int x, int y, const TCHAR * text)
{
	double txtRotate = (double)logFont.lfEscapement * PI / 1800.0;  // from 1/10th degrees to radian.
	double txtOblique = oblique * PI / 1800.0;  // from 1/10th degrees to radian.

	TEXTMETRIC tm;
	GetTextMetrics(dc, &tm);
	SIZE size;
	GetTextExtentPoint(dc, text, -1, &size);
	size.cy = tm.tmHeight;

	XFORM xForm;
	xForm.eDx = x;
	xForm.eDy = y;
	xForm.eM11 = cosf(txtRotate);
	xForm.eM21 = sinf(txtRotate) - tanf(txtOblique) * cosf(txtRotate);
	xForm.eM22 = cosf(txtRotate) + tanf(txtOblique) * sinf(txtRotate);

	SetWorldTransform(dc, &xForm);

	LOGFONT horLogFont;
	CopyMemory(&horLogFont, &logFont, sizeof(LOGFONT));

	horLogFont.lfOrientation -= logFont.lfEscapement;
	horLogFont.lfEscapement = 0;
	HFONT horFont = CreateFontIndirect(&horLogFont);

	HFONT oldFont = (HFONT)SelectObject(dc, horFont);

	TextOut(dc, 0, 0, text, _tcsclen(text));

	ModifyWorldTransform(dc, &xForm, MWT_IDENTITY);

	SelectObject(dc, oldFont);
}

void TransformAndDraw(HWND hWnd, HDC dc) 
{ 
    XFORM xForm; 
    RECT rect; 
 
	auto oldFont = SelectObject(dc, hFont);
	TextOut(dc, 10, 10, greeting, _tcsclen(greeting));
	SelectObject(dc, oldFont);
 
    // Set the mapping mode to LOENGLISH. This moves the  
    // client area origin from the upper left corner of the  
    // window to the lower left corner (this also reorients  
    // the y-axis so that drawing operations occur in a true  
    // Cartesian space). It guarantees portability so that  
    // the object drawn retains its dimensions on any display.  

    SetGraphicsMode(dc, GM_ADVANCED);
    SetMapMode(dc, MM_LOENGLISH); 
 
    // Set the appropriate world transformation (ROTATE)
 
    xForm.eM11 = (FLOAT) 0.8660; 
    xForm.eM12 = (FLOAT) 0.5000; 
    xForm.eM21 = (FLOAT) -0.5000; 
    xForm.eM22 = (FLOAT) 0.8660; 
    xForm.eDx  = (FLOAT) 0.0; 
    xForm.eDy  = (FLOAT) 0.0; 
    SetWorldTransform(dc, &xForm); 
 
	oldFont = SelectObject(dc, hFont);
	TextOut(dc, 0, 0, greeting, _tcsclen(greeting));
	SelectObject(dc, oldFont);
	
	// Find the midpoint of the client area.  
 
    GetClientRect(hWnd, (LPRECT) &rect); 
    DPtoLP(dc, (LPPOINT) &rect, 2); 
 
    // Select a hollow brush.  
 
    SelectObject(dc, GetStockObject(HOLLOW_BRUSH)); 
 
    // Draw the exterior circle.  
 
    Ellipse(dc, (rect.right / 2 - 100), (rect.bottom / 2 + 100), 
        (rect.right / 2 + 100), (rect.bottom / 2 - 100)); 
 
    // Draw the interior circle.  
 
    Ellipse(dc, (rect.right / 2 -94), (rect.bottom / 2 + 94), 
        (rect.right / 2 + 94), (rect.bottom / 2 - 94)); 
 
    // Draw the key.  
 
    Rectangle(dc, (rect.right / 2 - 13), (rect.bottom / 2 + 113), 
        (rect.right / 2 + 13), (rect.bottom / 2 + 50)); 
    Rectangle(dc, (rect.right / 2 - 13), (rect.bottom / 2 + 96), 
        (rect.right / 2 + 13), (rect.bottom / 2 + 50)); 
 
    // Draw the horizontal lines.  
 
    MoveToEx(dc, (rect.right/2 - 150), (rect.bottom / 2 + 0), nullptr); 
    LineTo(dc, (rect.right / 2 - 16), (rect.bottom / 2 + 0)); 
 
    MoveToEx(dc, (rect.right / 2 - 13), (rect.bottom / 2 + 0), nullptr); 
    LineTo(dc, (rect.right / 2 + 13), (rect.bottom / 2 + 0)); 
 
    MoveToEx(dc, (rect.right / 2 + 16), (rect.bottom / 2 + 0), nullptr); 
    LineTo(dc, (rect.right / 2 + 150), (rect.bottom / 2 + 0)); 
 
    // Draw the vertical lines.  
 
    MoveToEx(dc, (rect.right/2 + 0), (rect.bottom / 2 - 150), nullptr); 
    LineTo(dc, (rect.right / 2 + 0), (rect.bottom / 2 - 16)); 
 
    MoveToEx(dc, (rect.right / 2 + 0), (rect.bottom / 2 - 13), nullptr); 
    LineTo(dc, (rect.right / 2 + 0), (rect.bottom / 2 + 13)); 
 
    MoveToEx(dc, (rect.right / 2 + 0), (rect.bottom / 2 + 16), nullptr); 
    LineTo(dc, (rect.right / 2 + 0), (rect.bottom / 2 + 150));

	ObliqueTextOut(dc, 30, 10, 10, greeting);
}

void TransformText(HDC dc, const TCHAR* text, bool transform)
{
	if (transform)
	{
		XFORM xForm;

		SetGraphicsMode(dc, GM_ADVANCED);
		SetMapMode(dc, MM_ISOTROPIC);

		// Set the appropriate world transformation (ROTATE)

		xForm.eM11 = (FLOAT) 0.8660 * 4.0;
		xForm.eM12 = (FLOAT) 0.5000;
		xForm.eM21 = (FLOAT)-0.5000;
		xForm.eM22 = (FLOAT) 0.8660 * 4.0;
		xForm.eDx = (FLOAT) 100.0;
		xForm.eDy = (FLOAT)-500.0;
		SetWorldTransform(dc, &xForm);
	}

	auto oldFont = SelectObject(dc, hFont);
	//TextOut(dc, 0, 0, text, _tcsclen(text));
	RECT rect;
	rect.left = 0;
	rect.right = 1000;
	rect.top = 0;
	rect.bottom = 60;
	DrawText(dc, text, -1, &rect, DT_TOP | DT_CENTER);
	SelectObject(dc, oldFont);
}

void DrawTextRotated(HDC dc, const TCHAR* text, int x, int y, int rotation)
{
	XFORM xForm;

	SetGraphicsMode(dc, GM_ADVANCED);
	SetMapMode(dc, MM_ISOTROPIC);
	SetWindowExtEx(dc, 1000, 1000, nullptr);
	SetViewportExtEx(dc, 500, 500, nullptr);

	auto radians = rotation * PI / 180.0f;

	// Set the appropriate world transformation (ROTATE)

	xForm.eM11 = cosf(radians);
	xForm.eM12 = sinf(radians);
	xForm.eM21 = -xForm.eM12;
	xForm.eM22 = xForm.eM11;
	xForm.eDx = (FLOAT)x;
	xForm.eDy = (FLOAT)y;
	SetWorldTransform(dc, &xForm);

	MoveToEx(dc, x - 100, y, nullptr);
	LineTo(dc, x + 100, y);
	MoveToEx(dc, x, y - 100, nullptr);
	LineTo(dc, x, y + 100);

	RECT rect;
	rect.left = 0;
	rect.right = 10000;
	rect.top = 0;
	rect.bottom = 10000;
	DrawText(dc, text, -1, &rect, DT_CALCRECT);

	auto width = rect.right - rect.left;
	auto height = rect.bottom - rect.top;

	rect.left = x - width / 2;
	rect.right = rect.left + width;
	rect.top = y - height / 2;
	rect.bottom = rect.top + height;
	DrawText(dc, text, -1, &rect, DT_TOP | DT_CENTER);
}

LRESULT CALLBACK WndProc(
   _In_ HWND   hwnd,
   _In_ UINT   message,
   _In_ WPARAM wParam,
   _In_ LPARAM lParam
)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		//TransformAndDraw(hwnd, hdc);
		//TransformText(hdc, greeting, false);
		DrawTextRotated(hdc, greeting, 400, 10, 90);
		EndPaint(hwnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

int CALLBACK WinMain(
   _In_ HINSTANCE hInstance,
   _In_ HINSTANCE hPrevInstance,
   _In_ LPSTR     lpCmdLine,
   _In_ int       nCmdShow
)
{
	WNDCLASSEX windowclass;

	windowclass.cbSize         = sizeof(WNDCLASSEX);
	windowclass.style          = CS_HREDRAW | CS_VREDRAW;
	windowclass.lpfnWndProc    = WndProc;
	windowclass.cbClsExtra     = 0;
	windowclass.cbWndExtra     = 0;
	windowclass.hInstance      = hInstance;
	windowclass.hIcon          = LoadIcon(hInstance, IDI_APPLICATION);
	windowclass.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	windowclass.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	windowclass.lpszMenuName   = nullptr;
	windowclass.lpszClassName  = windowClassName;
	windowclass.hIconSm        = LoadIcon(windowclass.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&windowclass))
	{
		MessageBox(nullptr,
			_T("Call to RegisterClassEx failed!"),
			windowTitle,
			0);

		return 1;
	}

	HWND hWnd = CreateWindow(
		windowClassName,
		windowTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		800, 600,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (!hWnd)
	{
		MessageBox(nullptr,
			_T("Call to CreateWindow failed!"),
			windowTitle,
			0);

		return 1;
	}

	ObliqueTextInitialisation();

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}

