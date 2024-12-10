#include <windows.h>
#include <mmsystem.h>
#include <fstream>
#include <string>
#include <gdiplus.h>
#include "resource.h" // Включаем файл заголовка с определениями ресурсов

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

// Глобальные переменные
HBITMAP hbmp;
LOGFONT fontt;
HFONT hfmy;

// Прототипы функций
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawBitmap(HDC hdc, HBITMAP hBitmap, int xs, int ys, RECT rect);
void DrawGradientBorder(Graphics& graphics, RECT rect, Color startColor, Color endColor, int borderWidth);

// Основная функция
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t szClassName[] = L"Shablon";
    WNDCLASSEX wndClass;
    HWND hwnd;
    MSG msg;

    wndClass.cbSize = sizeof(wndClass);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = szClassName;
    wndClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION2));

    RegisterClassEx(&wndClass);

    hwnd = CreateWindowEx(
        0,
        szClassName,
        L"Окно программы",
        WS_OVERLAPPEDWINDOW,
        50, 50, 500, 500,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    hbmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
    if (hbmp == NULL) {
        MessageBox(NULL, L"Не удалось загрузить битмап", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    sndPlaySound(L"red.wav", SND_FILENAME | SND_ASYNC);

    // Инициализация GDI+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Завершение работы GDI+
    GdiplusShutdown(gdiplusToken);

    return msg.wParam;
}

// Функция для рисования битмапа
void DrawBitmap(HDC hdc, HBITMAP hBitmap, int xs, int ys, RECT rect) {
    SetStretchBltMode(hdc, COLORONCOLOR);
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
    BITMAP bm;
    GetObject(hBitmap, sizeof(bm), &bm);
    StretchBlt(hdc, xs, ys, rect.right, rect.bottom, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
    SelectObject(hdcMem, hOldBitmap);
    DeleteDC(hdcMem);
}

// Функция для рисования градиентной рамки
void DrawGradientBorder(Graphics& graphics, RECT rect, Color startColor, Color endColor, int borderWidth) {
    // Рисование верхней границы
    LinearGradientBrush topBrush(
        Point(rect.left, rect.top),
        Point(rect.right, rect.top),
        startColor,
        endColor);
    graphics.FillRectangle(&topBrush, rect.left, rect.top, rect.right - rect.left, borderWidth);

    // Рисование нижней границы
    LinearGradientBrush bottomBrush(
        Point(rect.left, rect.bottom),
        Point(rect.right, rect.bottom),
        startColor,
        endColor);
    graphics.FillRectangle(&bottomBrush, rect.left, rect.bottom - borderWidth, rect.right - rect.left, borderWidth);

    // Рисование левой границы
    LinearGradientBrush leftBrush(
        Point(rect.left, rect.top),
        Point(rect.left, rect.bottom),
        startColor,
        endColor);
    graphics.FillRectangle(&leftBrush, rect.left, rect.top, borderWidth, rect.bottom - rect.top);

    // Рисование правой границы
    LinearGradientBrush rightBrush(
        Point(rect.right, rect.top),
        Point(rect.right, rect.bottom),
        startColor,
        endColor);
    graphics.FillRectangle(&rightBrush, rect.right - borderWidth, rect.top, borderWidth, rect.bottom - rect.top);
}

// Обработчик сообщений окна
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;   //создание структуры ps содержит информацию о текущем состоянии рисования
        HDC hdc = BeginPaint(hwnd, &ps);  //вызов функции которая подготавливает контекст окна для рисования и заполняет структуру
        RECT rect;
        GetClientRect(hwnd, &rect);
        int x = rect.right;
        int y = rect.bottom;
        int yv, yniz, xl, xr;

        if (y > x) {
            yv = (y - x) / 2;
            yniz = (y - x) / 2 + x;
            xl = 0;
            xr = x;
        }
        else {
            xl = (x - y) / 2;
            xr = (x - y) / 2 + y;
            yv = 0;
            yniz = y;
        }

        DrawBitmap(hdc, hbmp, 0, 0, rect);
        SetMapMode(hdc, MM_ANISOTROPIC); //MM_ANISOTROPIC-чтобы звнять всю область

        // Рисование градиентной рамки
        Graphics graphics(hdc);
        DrawGradientBorder(graphics, { xl, yv, xr, yniz }, Color(255, 150, 220, 150), Color(255, 255, 0, 0), 10);

        std::ifstream file("mypas.pas", std::ios::binary);
        if (file.is_open()) {
            file.seekg(0, std::ios::end);
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);
            std::string buffer(size, ' ');
            file.read(&buffer[0], size);
            file.close();

            // Преобразование строки из ASCII в Unicode
            int wchars_num = MultiByteToWideChar(CP_UTF8, 0, buffer.c_str(), -1, NULL, 0);
            std::wstring wbuffer(wchars_num, 0);
            MultiByteToWideChar(CP_UTF8, 0, buffer.c_str(), -1, &wbuffer[0], wchars_num);

            RECT rectmy = { xl, yv, xr, yniz };
            fontt.lfHeight = -14;
            fontt.lfWidth = 0;
            fontt.lfEscapement = 0;
            fontt.lfOrientation = 0;
            fontt.lfWeight = FW_NORMAL;
            fontt.lfItalic = 0;
            fontt.lfUnderline = 0;
            fontt.lfStrikeOut = 0;
            fontt.lfCharSet = DEFAULT_CHARSET;
            fontt.lfOutPrecision = OUT_DEFAULT_PRECIS;
            fontt.lfClipPrecision = CLIP_DEFAULT_PRECIS;
            fontt.lfQuality = DEFAULT_QUALITY;
            fontt.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
            wcscpy_s(fontt.lfFaceName, LF_FACESIZE, L"Courier");

            hfmy = CreateFontIndirect(&fontt);
            SelectObject(hdc, hfmy);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(255, 255, 255));
            DrawText(hdc, wbuffer.c_str(), wbuffer.size(), &rectmy, DT_CENTER);
            DeleteObject(hfmy);
        }

        EndPaint(hwnd, &ps); //WM_PAINT-сообщение что контекс освобождём правильно и ОС будет уведомлена о завершении процесса
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
