
#include <windows.h>
#include <limits.h>
#include <math.h>
#include "TradeCoreAPI.h"

#define CANDLEWIDTH 4

static char g_szClassName[] = "MyWindowClass";
static HINSTANCE g_hInst = NULL;

const UINT idTimer1 = 1;
UINT nTimerDelay = 10000;

HBITMAP hbmPoint;
HBITMAP hbmPointBuy;
HBITMAP hbmPointSell;
BITMAP bm;

int deltaValue = 4;

void DrawPoint(HDC hdc, int pointX, int pointY, bool type){
  HDC hdcMemory;
  hdcMemory = CreateCompatibleDC(hdc);
  if (type){
    SelectObject(hdcMemory, hbmPointBuy);
  } else {
    SelectObject(hdcMemory, hbmPointSell);
  }
  BitBlt(hdc, pointX, pointY, bm.bmWidth, bm.bmHeight, hdcMemory, 0, 0, SRCAND);
  DeleteDC(hdcMemory);
}

void EraseAll(HDC hdc, HWND hwnd){
   RECT rc_root;
   GetClientRect(hwnd, &rc_root);
   RECT rc;
   rc.left = 0;
   rc.top = 0;
   rc.right = rc_root.right;
   rc.bottom = rc_root.bottom;
   FillRect(hdc, &rc, (HBRUSH)(COLOR_BTNFACE+1));
}

void genCandle(HDC hdc, int high, int open_price, int close_price, int low, int pos){
  int offset = 0;
  int body_start = 0;
  int body_end = 0;
  bool body_color = false;
  if (open_price <= close_price){
    body_start = open_price;
    body_end = close_price;
    body_color = false;
  } else {
    body_start = close_price;
    body_end = open_price;
    body_color = true;
  }
  offset = pos * CANDLEWIDTH;
  for (int x = offset; x < offset + CANDLEWIDTH - 1; x++){  
    for (int y = high; y <= low; y++){
      if (x == offset + ((int) CANDLEWIDTH / 2) - 1 ||
         (y >= body_start && y <= body_end)){
        DrawPoint(hdc, x, y, body_color);
      }
    }
  }
}

void UpdateAll(HDC hdc, HWND hwnd){
  RECT rc_root;
  double global_high = 0;
  double global_low = ULONG_MAX;
  double range_true = 0;
  double range_k = 0;
  const double range_scope = 90;
  int c_range = 0;
  int c_offset = 0;
  GetClientRect(hwnd, &rc_root);
  TradeCoreAPI tradecore = TradeCoreAPI();
  tradecore.doLoad();
  if (tradecore.getStatus() && tradecore.data_size > 0){
    for (unsigned int n = 0; n < tradecore.data_size; n++){
      if (tradecore.candles[n].high > global_high) global_high = tradecore.candles[n].high;
      if (tradecore.candles[n].low < global_low) global_low = tradecore.candles[n].low;
    }
    range_true = global_high - global_low;
    range_k = ((double) rc_root.bottom) / range_true * (range_scope / 100);
    //genCandle(hdc, 0, 20, 50, 80, 2);
    //genCandle(hdc, 0, 50, 20, 80, 3);
    c_range = (int) round(rc_root.right / CANDLEWIDTH);
    if (c_range <= tradecore.data_size){
      c_offset = tradecore.data_size - c_range;
    } else {
      c_offset = 0;
    }
    int chart_offset = (int) round(rc_root.bottom * ((100 - range_scope) / 100) / 2);
    for (unsigned int c = 0; c < c_range && c < tradecore.data_size; c++){
      genCandle(hdc,
                (int) abs((int) round((tradecore.candles[c + c_offset].high - global_low - range_true) * range_k)) + chart_offset,
                (int) abs((int) round((tradecore.candles[c + c_offset].open_price - global_low - range_true) * range_k)) + chart_offset,
                (int) abs((int) round((tradecore.candles[c + c_offset].close_price - global_low - range_true)* range_k)) + chart_offset,
                (int) abs((int) round((tradecore.candles[c + c_offset].low - global_low - range_true) * range_k)) + chart_offset,
                c);
    } 
  }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch(Message)
   {
      case WM_CREATE:
         hbmPoint = LoadBitmap(g_hInst, "POINTBMP");
         hbmPointBuy = LoadBitmap(g_hInst, "POINTBUYBMP");
         hbmPointSell = LoadBitmap(g_hInst, "POINTSELLBMP");
         if(!hbmPoint){
            MessageBox(hwnd, "Load of resources failed.", "Error",
               MB_OK | MB_ICONEXCLAMATION);
            return -1;
         }

         GetObject(hbmPoint, sizeof(bm), &bm);
         SetTimer(hwnd, idTimer1, nTimerDelay, NULL);

      break;
      case WM_TIMER:
         if(hbmPoint)
         {
            HDC hdcWindow;
            hdcWindow = GetDC(hwnd);
            EraseAll(hdcWindow, hwnd);
            UpdateAll(hdcWindow, hwnd);

            ReleaseDC(hwnd, hdcWindow);
         }
      break;
      case WM_PAINT:
         if(hbmPoint)
         {
            PAINTSTRUCT ps;
            HDC hdcWindow;
            hdcWindow = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
         }
      break;
      case WM_CLOSE:
         DestroyWindow(hwnd);
      break;
      case WM_DESTROY:
         KillTimer(hwnd, idTimer1);
         
         DeleteObject(hbmPoint);
         PostQuitMessage(0);
      break;
      default:
         return DefWindowProc(hwnd, Message, wParam, lParam);
   }
   return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
   LPSTR lpCmdLine, int nCmdShow)
{
   WNDCLASSEX WndClass;
   HWND hwnd;
   MSG Msg;

   g_hInst = hInstance;

   WndClass.cbSize        = sizeof(WNDCLASSEX);
   WndClass.style         = 0;
   WndClass.lpfnWndProc   = WndProc;
   WndClass.cbClsExtra    = 0;
   WndClass.cbWndExtra    = 0;
   WndClass.hInstance     = g_hInst;
   WndClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
   WndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
   WndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
   WndClass.lpszMenuName  = NULL;
   WndClass.lpszClassName = g_szClassName;
   WndClass.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

   if(!RegisterClassEx(&WndClass))
   {
      MessageBox(0, "Window Registration Failed!", "Error!",
         MB_ICONEXCLAMATION | MB_OK | MB_SYSTEMMODAL);
      return 0;
   }

   hwnd = CreateWindowEx(
      WS_EX_CLIENTEDGE,
      g_szClassName,
      "TradeViwer 0.1",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
      NULL, NULL, g_hInst, NULL);

   if(hwnd == NULL)
   {
      MessageBox(0, "Window Creation Failed!", "Error!",
         MB_ICONEXCLAMATION | MB_OK | MB_SYSTEMMODAL);
      return 0;
   }

   ShowWindow(hwnd, nCmdShow);
   UpdateWindow(hwnd);

   while(GetMessage(&Msg, NULL, 0, 0))
   {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
   }
   return Msg.wParam;
}

