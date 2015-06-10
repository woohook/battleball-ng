#include "gfxtargetwin.h"
#include <wingdi.h>

struct coloritem
{
  char* name;
  ulong color;
};

coloritem colormap[] =
{
  {"black", 0x000000},
  {"gray10", 0x1a1a1a},
  {"gray20", 0x333333},
  {"gray30", 0x4d4d4d},
  {"gray40", 0x666666},
  {"gray50", 0x7f7f7f},
  {"gray60", 0x999999},
  {"gray70", 0xb3b3b3},
  {"gray75", 0xbfbfbf},
  {"gray80", 0xcccccc},
  {"gray85", 0xd9d9d9},
  {"gray90", 0xe5e5e5},
  {"gray95", 0xf2f2f2},
  {"white", 0xffffff},
  {"saddle brown", 0x13458b},
  {"sienna", 0x2d52a0},
  {"brown", 0x2a2aa5},
  {"firebrick", 0x2222b2},
  {"red", 0x0000ff},
  {"orange red", 0x0045ff},
  {"dark orange", 0x008cff},
  {"orange", 0x00a5ff},
  {"gold", 0x00d7ff},
  {"yellow", 0x00ffff},
  {"green yellow", 0x2fffad},
  {"green", 0x00ff00},
  {"lawn green", 0x00fc7c},
  {"lime green", 0x32cd32},
  {"green3", 0x00cd00},
  {"forest green", 0x228b22},
  {"dark green", 0x006400},
  {"dark olive green", 0x2f6b55},
  {"olive drab", 0x238e6b},
  {"cyan", 0xffff00},
  {"light sky blue", 0xface87},
  {"deep sky blue", 0xffbf00},
  {"dodger blue", 0xff901e},
  {"blue", 0xff0000},
  {"steelblue1", 0xffb863},
  {"steelblue2", 0xeeac5c},
  {"steelblue3", 0xcd944f},
  {"steel blue", 0xb48246},
  {"steelblue4", 0x8b6436},
  {"slategray1", 0xffe2c6},
  {"slategray2", 0xeed3b9},
  {"slategray3", 0xcdb69f},
  {"slate gray", 0x908070},
  {"slategray4", 0x8b7b6c},
  {"navy", 0x800000},
  {"violet", 0xee82ee},
  {"turquoise", 0xd0e040},
  {"gray50", 0x7f7f7f},
  {"gray30", 0x4d4d4d}
};

HINSTANCE gfxTargetWin::hInstance;
int gfxTargetWin::nCmdShow;

gfxTargetWin* g_win = NULL;

// TODO: need this brush in global scope because SetForeground is marked const :-(
HBRUSH hBrush = NULL;
HPEN   hPen   = NULL;
HFONT  hFont  = NULL;
HDC    g_hDC  = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  if(hFont == NULL)
  {
    HDC hdc = GetDC(hWnd);
    g_hDC    = CreateCompatibleDC(hdc);
    hFont = CreateFont(13,6,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
              CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, FIXED_PITCH,TEXT("MS Sans Serif"));
    RECT rect;
    GetClientRect(hWnd, &rect);
    HBITMAP bitmap = CreateCompatibleBitmap (hdc, DEFAULTWINWIDTH, DEFAULTWINHEIGHT);
    SelectObject ( g_hDC, bitmap );
    ReleaseDC(hWnd,hdc);
    g_win->newWidth = 0;
  }

  if(g_win->lastMsg <= &g_win->MsgQ[99])
  {
    switch(Msg)
    {
    case WM_CLOSE:
        g_win->lastMsg->type    = BBE_Key;
        g_win->lastMsg->key     = KBK_q;
        g_win->lastMsg->pressed = false;
        g_win->lastMsg++;
        break;
    case WM_DESTROY:
        PostQuitMessage(WM_QUIT);
        break;
    case WM_SIZE:  // see WM_SIZING also
        if(lParam > 0)
        {
          g_win->newWidth  = LOWORD(lParam);
        }
        break;
    case WM_ERASEBKGND:
        return (LRESULT)1;
        break;
    case WM_KEYDOWN:
    case WM_KEYUP:
        g_win->lastMsg->key = KBK_None;

        if(wParam>='A' && wParam<='Z')
        {
          g_win->lastMsg->key = (KB_Key)(KBK_a + wParam - 'A');
        }
        else if(wParam==VK_UP)
        {
          g_win->lastMsg->key = KBK_Up;
        }
        else if(wParam==VK_DOWN)
        {
          g_win->lastMsg->key = KBK_Down;
        }
        else if(wParam==VK_LEFT)
        {
          g_win->lastMsg->key = KBK_Left;
        }
        else if(wParam==VK_RIGHT)
        {
          g_win->lastMsg->key = KBK_Right;
        }
        else if(wParam==VK_HOME)
        {
          g_win->lastMsg->key = KBK_Home;
        }
        else if(wParam==VK_END)
        {
          g_win->lastMsg->key = KBK_End;
        }
        else if(wParam==VK_TAB)
        {
          g_win->lastMsg->key = KBK_Tab;
        }
        else if(wParam==VK_ESCAPE)
        {
          g_win->lastMsg->key = KBK_Escape;
        }
        else if(wParam==VK_SPACE)
        {
          g_win->lastMsg->key = KBK_space;
        }
        else if(wParam==VK_INSERT)
        {
          g_win->lastMsg->key = KBK_Insert;
        }
        else if(wParam==VK_PRIOR)
        {
          g_win->lastMsg->key = KBK_Page_Up;
        }
        else if(wParam==VK_NEXT)
        {
          g_win->lastMsg->key = KBK_Page_Down;
        }
        else if(wParam==VK_NUMPAD2)
        {
          g_win->lastMsg->key = KBK_KP_Down;
        }
        else if(wParam==VK_NUMPAD3)
        {
          g_win->lastMsg->key = KBK_KP_Page_Down;
        }
        else if(wParam==VK_NUMPAD4)
        {
          g_win->lastMsg->key = KBK_KP_Left;
        }
        else if(wParam==VK_NUMPAD6)
        {
          g_win->lastMsg->key = KBK_KP_Right;
        }
        else if(wParam==VK_NUMPAD8)
        {
          g_win->lastMsg->key = KBK_KP_Up;
        }
        else if(wParam==VK_NUMPAD9)
        {
          g_win->lastMsg->key = KBK_KP_Page_Up;
        }

        if(g_win->lastMsg->key != KBK_None)
        {
          g_win->lastMsg->type = BBE_Key;
          g_win->lastMsg->pressed = (Msg==WM_KEYDOWN);
          g_win->lastMsg++;
        }
        break;
    default:
        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }
  }
  else
  {
    return DefWindowProc(hWnd, Msg, wParam, lParam);
  }

  return 0;
}

bool gfxTargetWin::OpenDisplay(char *dispName, int argc, char *argv[])
{
  g_win = this;
  nextMsg = MsgQ;
  lastMsg = MsgQ;

  LPCTSTR ClsName = "BBmain";
  LPCTSTR WindowCaption = "Battleball";

    WNDCLASSEX  WndClsEx;

    WndClsEx.cbSize        = sizeof(WNDCLASSEX);
    WndClsEx.style         = CS_HREDRAW | CS_VREDRAW;
    WndClsEx.lpfnWndProc   = WndProc;
    WndClsEx.cbClsExtra    = NULL;
    WndClsEx.cbWndExtra    = sizeof(void*);
    WndClsEx.hInstance     = hInstance;
    WndClsEx.hIcon         = LoadIcon(hInstance, IDI_APPLICATION);
    WndClsEx.hCursor       = LoadCursor(NULL, IDC_ARROW);
    WndClsEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    WndClsEx.lpszMenuName  = NULL;
    WndClsEx.lpszClassName = ClsName;
    WndClsEx.hIconSm       = LoadIcon(hInstance, IDI_APPLICATION);

    RegisterClassEx(&WndClsEx);

    hdc    = NULL;
    DWORD winStyle = WS_OVERLAPPEDWINDOW;
    DWORD winStyleEx = WS_EX_OVERLAPPEDWINDOW;

    RECT winRect;
    winRect.left = 0;
    winRect.top = 0;
    winRect.right = DEFAULTWINWIDTH + 1;
    winRect.bottom = DEFAULTWINHEIGHT + 1;
    AdjustWindowRectEx(&winRect, winStyle, FALSE, winStyleEx);

    hWnd = CreateWindowEx(winStyleEx,
                          ClsName,
                          WindowCaption,
                          winStyle,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          winRect.right - winRect.left,
                          winRect.bottom - winRect.top,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

  return true;
}

void gfxTargetWin::CloseDisplay()
{
  PostQuitMessage(WM_QUIT);
}

void gfxTargetWin::ResizeWindow(const pt2d& sz)
{
}

void gfxTargetWin::HandleResize(BBEvent *event, bool refit)
{
  DeleteObject(GetCurrentObject(g_hDC, OBJ_BITMAP));
  DeleteDC(g_hDC);

  HDC ohdc = GetDC(hWnd);
  g_hDC = CreateCompatibleDC(ohdc);

  RECT rect;
  GetClientRect(hWnd, &rect);
  HBITMAP bitmap = CreateCompatibleBitmap (ohdc, rect.right, rect.bottom);
  SelectObject ( g_hDC, bitmap );

  ReleaseDC(hWnd,ohdc);
}


ulong gfxTargetWin::Black() const
{
  return 0;
}

ulong gfxTargetWin::White() const
{
  return (ulong)0xFFFFFF;
}

void gfxTargetWin::SetForeground(ulong color) const
{
  if(hBrush != NULL)
  {
    DeleteObject(hBrush);
  }
  hBrush = CreateSolidBrush(color);
  SelectObject(hdc,hBrush);
  if(hPen != NULL)
  {
    DeleteObject(hPen);
  }
  hPen = CreatePen(PS_SOLID,1,color);
  SelectObject(hdc,hPen);
}

void gfxTargetWin::SetBackground(ulong color) const
{
}

void gfxTargetWin::AllocColors(char *colorNames[], int numColors)
{
  colors = new ulong[numColors];
  for(int i=0; i<numColors; i++)
  {
    colors[i] = 0;
    for(int c=0; c<53; c++)
    {
      if(strcmp(colorNames[i],colormap[c].name)==0)
      {
        colors[i] = colormap[c].color;
      }
    }
  }
}

ulong* gfxTargetWin::Colors()
{
  return colors;
}

void gfxTargetWin::DrawSegments(pt2d* segs, int numSegs) const
{
  if(hdc != NULL)
  {
    for(int i = 0; i < numSegs; i+=2)
    {
      MoveToEx(hdc, (int) segs[i].x, (int) segs[i].y, (LPPOINT) NULL); 
      LineTo(hdc, (int) segs[i+1].x, (int) segs[i+1].y); 
    }
  }
}

void gfxTargetWin::DrawLine(int x1, int y1, int x2, int y2)
{
  if(hdc != NULL)
  {
    MoveToEx(hdc, x1, y1, (LPPOINT) NULL); 
    LineTo(hdc, x2, y2); 
  }
}

void gfxTargetWin::DrawLines(pt2d* points, int npoints)
{
  if(hdc != NULL)
  {
    MoveToEx(hdc, (int) points[0].x, (int) points[0].y, (LPPOINT) NULL); 
    for(int i = 1; i < npoints; i++)
    {
      LineTo(hdc, (int) points[i].x, (int) points[i].y); 
    }
  }
}

void gfxTargetWin::DrawRectangle(const pt2d& p,const pt2d& sz)
{
  RECT rc;
  rc.left   = p.x;
  rc.top    = p.y;
  rc.right  = p.x + sz.x;
  rc.bottom = p.y + sz.y;
  FrameRect(hdc, &rc, hBrush);
}

void gfxTargetWin::DrawRectangle(int x, int y, unsigned int width, unsigned int height)
{
  RECT rc;
  rc.left   = x;
  rc.top    = y;
  rc.right  = x + width;
  rc.bottom = y + height;
  FrameRect(hdc, &rc, hBrush);
}

void gfxTargetWin::DrawString(const pt2d& p,const char* s)
{
  DrawImageString((int)p.x, (int)p.y, s, strlen(s));
}

void gfxTargetWin::DrawImageString(int x, int y, const char* string, int length)
{
  SelectObject(hdc,hFont);
  SetTextAlign(hdc, TA_LEFT | TA_BASELINE | TA_NOUPDATECP);

  // TODO: hack to force monospaced font
  for(int c = 0; c < length; c++)
    if(string[c] != ' ')
      TextOut(hdc, x+(c*6) - 2, y, string + c, 1);
}


void gfxTargetWin::FillRectangle(const pt2d& p,const pt2d& sz)
{
  RECT rc;
  rc.left   = p.x;
  rc.top    = p.y;
  rc.right  = p.x + sz.x;
  rc.bottom = p.y + sz.y;
  FillRect(hdc, &rc, hBrush);
}

void gfxTargetWin::FillPolygon(pt2d* points, int npoints, bool convex) const
{
  POINT winPoints[npoints];
  for(int i=0; i<npoints; i++)
  {
    winPoints[i].x = points[i].x;
    winPoints[i].y = points[i].y;
  }
  Polygon(hdc,winPoints,npoints);
}

void gfxTargetWin::DoubleBufferBegin()
{
  hdc = g_hDC;
}

void gfxTargetWin::DoubleBufferEnd()
{
  HDC ohdc = GetDC(hWnd);
  RECT rect;
  GetClientRect(hWnd, &rect);
  BitBlt(ohdc,0,0,rect.right,rect.bottom,hdc,0,0,SRCCOPY);
  ReleaseDC(hWnd, ohdc); 
}

void gfxTargetWin::Flush()
{
  DoubleBufferEnd();
  hdc = 0;
}

void gfxTargetWin::SetLineAttributes(unsigned int line_width, int line_style, int cap_style, int join_style)
{
}

int gfxTargetWin::Pending()
{
  MSG msg;
  while( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  if(newWidth > 0)
  {
    lastMsg->type  = BBE_Resize;
    lastMsg->width = newWidth;
    lastMsg++;
    newWidth = 0;
  }

  if(lastMsg > nextMsg)
  {
    return 1;
  }
  else
  {
    nextMsg = MsgQ;
    lastMsg = MsgQ;
    return 0;
  }
}

BBEvent gfxTargetWin::NextEvent()
{
  BBEvent event = *nextMsg;
  nextMsg++;
  return event;
}

