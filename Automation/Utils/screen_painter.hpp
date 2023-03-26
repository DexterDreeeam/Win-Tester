#pragma once

#include "../common.hpp"

class ScreenPainter
{
public:
    ScreenPainter()
    {
    }

    ~ScreenPainter()
    {
    }

    void Rect(const RECT& rc)
    {
        RedrawDesktop();

        //auto hWnd = GetConsoleWindow();
        //HDC hDC = GetDC(NULL);
        //HBRUSH hBsh = CreateSolidBrush(RGB(0, 0, 255));
        //FrameRect(hDC, &rc, hBsh);
        //DeleteObject(hBsh);
        //ReleaseDC(NULL, hDC);

        HDC hDC_Desktop = GetDC(0);
        /* Draw a simple blue rectangle on the desktop */
        HBRUSH blueBrush = CreateSolidBrush(RGB(212, 205, 0));
        FrameRect(hDC_Desktop, &rc, blueBrush);
    }

    void Rect(int left, int top, int right, int bottom)
    {
        RECT rc = { left, top, right, bottom };
        Rect(rc);
    }

    bool InitWindow()
    {
        ins = new ScreenPainter();

        auto appName = TEXT("SlimAuto");
        HINSTANCE hInstance = GetModuleHandle(NULL);
        HWND hwnd = GetConsoleWindow();
        WNDCLASS wndclass;

        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc = WndProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = hInstance;
        wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wndclass.lpszMenuName = NULL;
        wndclass.lpszClassName = appName;

        if (!RegisterClass(&wndclass))
        {
            return false;
        }

        hwnd = CreateWindow(
            appName,             // window class name
            appName,             // window caption
            WS_OVERLAPPEDWINDOW, // window style
            CW_USEDEFAULT,       // initial x position
            CW_USEDEFAULT,       // initial y position
            CW_USEDEFAULT,       // initial x size
            CW_USEDEFAULT,       // initial y size
            NULL,                // parent window handle
            NULL,                // window menu handle
            hInstance,           // program instance handle
            NULL);               // creation parameters

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
        this->LoopWindow();
        return true;
    }

    void LoopWindow(const std::vector<std::wstring>& infos = std::vector<std::wstring>())
    {
        ins->displayInfos = infos;

        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

private:
    void RedrawDesktop()
    {
        RedrawWindow(NULL, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_INTERNALPAINT | RDW_ERASE);
    }

    static LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        HDC hdc;
        PAINTSTRUCT ps;
        static POINT pt;
        TCHAR szMousePos[32];
        int ofst_y;

        switch (message)
        {
        case WM_CREATE:
            SetTimer(hwnd, 1, 1000 / 60, NULL);
            break;

        case WM_TIMER:
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);
            InvalidateRect(hwnd, NULL, TRUE);
            break;

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            ofst_y = 20;

            wsprintf(szMousePos, TEXT("Mouse Position: (%d,%d)"), pt.x, pt.y);

            TextOut(hdc, 0, ofst_y, szMousePos, lstrlen(szMousePos));
            for (const auto& info : ins->displayInfos)
            {
                ofst_y += 20;
                TextOutW(hdc, 0, ofst_y, info.c_str(), (int)info.size());
            }

            EndPaint(hwnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
            break;
        }
        return 0;
    }

    static ScreenPainter*      ins;
    std::vector<std::wstring>  displayInfos;
};

__declspec(selectany) ScreenPainter* ScreenPainter::ins = nullptr;
