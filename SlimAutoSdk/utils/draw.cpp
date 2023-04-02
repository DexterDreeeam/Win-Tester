#include "common.hpp"
#include "draw.hpp"
// #include <gdiplusgraphics.h>

extern HINSTANCE g_hIns;
const int maxWidth = 1000;
const int maxHeight = 1000;
unsigned char* g_data = NULL;
HWND hwnd = NULL;
HDC hcdc = NULL;
HBITMAP hbmp = NULL;

void framer::draw(slim::area aa)
{
    static mutex mtx = {};
    if (!mtx.try_lock())
    {
        return;
    }

    if (!hwnd)
    {
        TCHAR szAppName[] = TEXT("WinTesterLayered");
        WNDCLASSEX wndClass;

        wndClass.cbSize = sizeof(WNDCLASSEX);
        wndClass.cbClsExtra = 0;
        wndClass.cbWndExtra = 0;
        wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wndClass.hIconSm = NULL;
        wndClass.hInstance = g_hIns;
        wndClass.lpszClassName = szAppName;
        wndClass.lpszMenuName = NULL;
        wndClass.lpfnWndProc = DefWindowProc;
        wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_IME | CS_DBLCLKS;
        RegisterClassEx(&wndClass);
        hwnd = CreateWindowEx(
            WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST, szAppName, szAppName, WS_POPUP,
            0, 0, 0, 0, NULL, NULL, g_hIns, NULL);

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);

        if (g_data == NULL)
        {
            g_data = new unsigned char[maxWidth * maxHeight * 4 + 20];
            // memset(g_data, 176, maxWidth * maxHeight * 4);
            for (int r = 0; r < maxHeight; ++r)
            {
                for (int c = 0; c < maxWidth; ++c)
                {
                    unsigned char* p = g_data + (r * maxWidth + c) * 4;
                    p[0] = (unsigned char)150;
                    p[1] = (unsigned char)250;
                    p[2] = (unsigned char)250;
                    p[3] = (unsigned char)30;
                }
            }
        }
    }

    update(aa);
    mtx.unlock();
}

void framer::update(slim::area aa)
{
    static slim::area last = slim::area();
    if (aa.left == last.left && aa.right == last.right && aa.top == last.top && aa.bottom == last.bottom)
    {
        return;
    }

    last = aa;
    int width = min(maxWidth, aa.right - aa.left);
    int height = min(maxHeight, aa.bottom - aa.top);

    HDC hdc;
    hdc = GetDC(hwnd);
    if (hcdc == NULL)
    {
        hcdc = CreateCompatibleDC(NULL);
        hbmp = CreateCompatibleBitmap(hdc, maxWidth, maxHeight);
        SelectObject(hcdc, hbmp);
    }

    BITMAPINFO bmpInfo;
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = maxWidth;
    bmpInfo.bmiHeader.biHeight = -maxHeight;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biCompression = BI_RGB;
    bmpInfo.bmiHeader.biBitCount = 32;
    SetDIBits(hdc, hbmp, 0, maxHeight, g_data, &bmpInfo, DIB_RGB_COLORS);
    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 64;
    blend.AlphaFormat = AC_SRC_ALPHA;
    POINT pSrc = { 0, 0 };
    POINT pDst = { aa.left, aa.top };
    SIZE sizeWnd = { width, height };
    UpdateLayeredWindow(hwnd, hdc, &pDst, &sizeWnd, hcdc, &pSrc, NULL, &blend, ULW_ALPHA);
    ReleaseDC(hwnd, hdc);
}
