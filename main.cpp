// calculadora.cpp
// Calculadora sencilla con interfaz gráfica usando Win32 API.
// Compila en Windows con MinGW o MSVC.

#include <windows.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <cwchar>
#include <cmath>

#ifdef _MSC_VER
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#endif

// ---------------------------------------------------------------------
// Constantes de diseño
// ---------------------------------------------------------------------
constexpr int MARGIN  = 10;
constexpr int GAP     = 8;
constexpr int BTN_W   = 70;
constexpr int BTN_H   = 50;
constexpr int EDIT_H  = 44;
constexpr int COLS    = 3;
constexpr int ROWS    = 5;

constexpr int CONTENT_W = COLS * BTN_W + (COLS - 1) * GAP;
constexpr int CLIENT_W  = MARGIN * 2 + CONTENT_W;
constexpr int CLIENT_H  = MARGIN + EDIT_H + GAP + ROWS * BTN_H + (ROWS - 1) * GAP + MARGIN;

// ---------------------------------------------------------------------
// IDs de controles
// ---------------------------------------------------------------------
enum ControlId : int
{
    IDC_EDIT = 100,
    IDC_BACK = 256,

    IDC_0 = '0',
    IDC_1,
    IDC_2,
    IDC_3,
    IDC_4,
    IDC_5,
    IDC_6,
    IDC_7,
    IDC_8,
    IDC_9,

    IDC_DOT   = '.',
    IDC_ADD   = '+',
    IDC_SUB   = '-',
    IDC_MUL   = '*',
    IDC_DIV   = '/',
    IDC_EQ    = '=',
    IDC_CLEAR = 'C'
};

// ---------------------------------------------------------------------
// Estado global de la calculadora
// ---------------------------------------------------------------------
HWND g_edit = nullptr;

std::wstring g_display   = L"0";
double       g_pending   = 0.0;
wchar_t      g_op        = 0;
bool         g_newEntry  = true;
bool         g_error     = false;

// ---------------------------------------------------------------------
// Utilidades
// ---------------------------------------------------------------------
std::wstring FormatNumber(double value)
{
    if (!std::isfinite(value))
        return L"Error";

    if (value == 0.0)
        return L"0";

    std::wostringstream ss;
    ss << std::setprecision(12) << value;
    return ss.str();
}

void SetDisplay(const std::wstring& text)
{
    g_display = text;
    if (g_edit)
        SetWindowTextW(g_edit, g_display.c_str());
}

double CurrentValue()
{
    if (g_display.empty())
        return 0.0;

    return std::wcstod(g_display.c_str(), nullptr);
}

bool Calculate(double a, double b, wchar_t op, double& out)
{
    switch (op)
    {
        case L'+':
            out = a + b;
            break;

        case L'-':
            out = a - b;
            break;

        case L'*':
            out = a * b;
            break;

        case L'/':
            if (b == 0.0)
                return false;
            out = a / b;
            break;

        default:
            return false;
    }

    return std::isfinite(out);
}

void ClearAll()
{
    g_display  = L"0";
    g_pending  = 0.0;
    g_op       = 0;
    g_newEntry = true;
    g_error    = false;

    SetDisplay(g_display);
}

void ShowError()
{
    g_display  = L"Error";
    g_pending  = 0.0;
    g_op       = 0;
    g_newEntry = true;
    g_error    = true;

    SetDisplay(g_display);
}

// ---------------------------------------------------------------------
// Acciones de botones
// ---------------------------------------------------------------------
void PressDigit(int digit)
{
    if (g_error)
        ClearAll();

    wchar_t ch = static_cast<wchar_t>(L'0' + digit);

    if (g_newEntry)
    {
        g_display  = ch;
        g_newEntry = false;
    }
    else
    {
        if (g_display == L"0")
        {
            g_display = ch;
        }
        else if (g_display.length() < 32)
        {
            g_display += ch;
        }
    }

    SetDisplay(g_display);
}

void PressDot()
{
    if (g_error)
        ClearAll();

    if (g_newEntry)
    {
        g_display  = L"0.";
        g_newEntry = false;
    }
    else if (g_display.find(L'.') == std::wstring::npos)
    {
        if (g_display.length() < 32)
            g_display += L'.';
    }

    SetDisplay(g_display);
}

void PressOperator(wchar_t op)
{
    if (g_error)
        return;

    double current = CurrentValue();

    if (g_op != 0 && !g_newEntry)
    {
        double result = 0.0;

        if (!Calculate(g_pending, current, g_op, result))
        {
            ShowError();
            return;
        }

        g_pending = result;
        SetDisplay(FormatNumber(result));
    }
    else
    {
        g_pending = current;
    }

    g_op       = op;
    g_newEntry = true;
}

void PressEquals()
{
    if (g_error || g_op == 0)
        return;

    double current = CurrentValue();
    double result  = 0.0;

    if (!Calculate(g_pending, current, g_op, result))
    {
        ShowError();
        return;
    }

    g_pending  = 0.0;
    g_op       = 0;
    g_newEntry = true;

    SetDisplay(FormatNumber(result));
}

void PressBackspace()
{
    if (g_error)
    {
        ClearAll();
        return;
    }

    if (g_newEntry)
        return;

    if (!g_display.empty())
        g_display.pop_back();

    if (g_display.empty() || g_display == L"-")
    {
        g_display  = L"0";
        g_newEntry = true;
    }

    SetDisplay(g_display);
}

// ---------------------------------------------------------------------
// Botones
// ---------------------------------------------------------------------
struct ButtonInfo
{
    const wchar_t* label;
    int id;
};

const ButtonInfo BUTTONS[] =
{
    { L"C",  IDC_CLEAR },
    { L"<-", IDC_BACK  },
    { L"/",  IDC_DIV   },

    { L"7",  IDC_7     },
    { L"8",  IDC_8     },
    { L"*",  IDC_MUL   },

    { L"4",  IDC_4     },
    { L"5",  IDC_5     },
    { L"-",  IDC_SUB   },

    { L"1",  IDC_1     },
    { L"2",  IDC_2     },
    { L"+",  IDC_ADD   },

    { L"0",  IDC_0     },
    { L".",  IDC_DOT   },
    { L"=",  IDC_EQ    }
};

constexpr int BUTTON_COUNT = static_cast<int>(sizeof(BUTTONS) / sizeof(BUTTONS[0]));

// ---------------------------------------------------------------------
// Procedimiento de ventana
// ---------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            HFONT font = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));

            // Pantalla / display
            g_edit = CreateWindowExW(
                WS_EX_CLIENTEDGE,
                L"EDIT",
                L"0",
                WS_CHILD | WS_VISIBLE | ES_RIGHT | ES_READONLY | ES_AUTOHSCROLL,
                MARGIN, MARGIN, CONTENT_W, EDIT_H,
                hwnd,
                reinterpret_cast<HMENU>(IDC_EDIT),
                GetModuleHandleW(nullptr),
                nullptr
            );

            SendMessageW(g_edit, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);

            // Botones
            for (int i = 0; i < BUTTON_COUNT; ++i)
            {
                int row = i / COLS;
                int col = i % COLS;

                int x = MARGIN + col * (BTN_W + GAP);
                int y = MARGIN + EDIT_H + GAP + row * (BTN_H + GAP);

                HWND button = CreateWindowExW(
                    0,
                    L"BUTTON",
                    BUTTONS[i].label,
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    x, y, BTN_W, BTN_H,
                    hwnd,
                    reinterpret_cast<HMENU>(static_cast<LONG_PTR>(BUTTONS[i].id)),
                    GetModuleHandleW(nullptr),
                    nullptr
                );

                SendMessageW(button, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
            }

            return 0;
        }

        case WM_COMMAND:
        {
            if (HIWORD(wParam) == BN_CLICKED)
            {
                int id = LOWORD(wParam);

                if (id >= IDC_0 && id <= IDC_9)
                {
                    PressDigit(id - IDC_0);
                }
                else if (id == IDC_DOT)
                {
                    PressDot();
                }
                else if (id == IDC_CLEAR)
                {
                    ClearAll();
                }
                else if (id == IDC_BACK)
                {
                    PressBackspace();
                }
                else if (id == IDC_ADD || id == IDC_SUB || id == IDC_MUL || id == IDC_DIV)
                {
                    PressOperator(static_cast<wchar_t>(id));
                }
                else if (id == IDC_EQ)
                {
                    PressEquals();
                }

                return 0;
            }
            break;
        }

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------
// Punto de entrada para Windows
// ---------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    WNDCLASSW wc = {};

    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = L"CalculadoraSencilla";
    wc.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    wc.hIcon         = LoadIconW(nullptr, IDI_APPLICATION);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);

    if (!RegisterClassW(&wc))
    {
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
            return 1;
    }

    RECT rc = { 0, 0, CLIENT_W, CLIENT_H };
    AdjustWindowRectEx(
        &rc,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        FALSE,
        0
    );

    HWND hwnd = CreateWindowExW(
        0,
        wc.lpszClassName,
        L"Calculadora",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rc.right - rc.left,
        rc.bottom - rc.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!hwnd)
        return 1;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return static_cast<int>(msg.wParam);
}
