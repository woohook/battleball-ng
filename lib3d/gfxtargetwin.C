#include "gfxtargetwin.h"

HINSTANCE gfxTargetWin::hInstance;
int gfxTargetWin::nCmdShow;

gfxTargetWin* g_win = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  if(g_win->lastMsg <= &g_win->MsgQ[99])
  {
    switch(Msg)
    {
    case WM_DESTROY:
        PostQuitMessage(WM_QUIT);
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
}

void gfxTargetWin::SetBackground(ulong color) const
{
}

void gfxTargetWin::AllocColors(char *colorNames[], int numColors)
{
}

ulong* gfxTargetWin::Colors()
{
  return colors;
}

void gfxTargetWin::DrawSegments(pt2d* segs, int numSegs) const
{
}

void gfxTargetWin::DrawLine(int x1, int y1, int x2, int y2)
{
}

void gfxTargetWin::DrawLines(pt2d* points, int npoints)
{
}

void gfxTargetWin::DrawRectangle(const pt2d& p,const pt2d& sz)
{
}

void gfxTargetWin::DrawRectangle(int x, int y, unsigned int width, unsigned int height)
{
}

void gfxTargetWin::DrawString(const pt2d& p,const char* s)
{
}

void gfxTargetWin::DrawImageString(int x, int y, const char* string, int length)
{
}


void gfxTargetWin::FillRectangle(const pt2d& p,const pt2d& sz)
{
}

void gfxTargetWin::FillPolygon(pt2d* points, int npoints, bool convex) const
{
}

void gfxTargetWin::DoubleBufferBegin()
{
}

void gfxTargetWin::DoubleBufferEnd()
{
}

void gfxTargetWin::Flush()
{
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

