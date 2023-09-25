// CGLesson.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "CGLesson.h"
#include "cg_widget.h"
#include "ImageKit/Image.h"

#include <random>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

#define MAX_LOADSTRING 100

struct BezierCurveDrawer
{
    int _isFirstClick; // 最初のクリックを記録するフラグ
    double _prevX0, _prevY0, _controlX0, _controlY0, _controlX1, _controlY1; // 3回目までのクリック座標
    bool _controlPoint;

    BezierCurveDrawer()
    {
        _isFirstClick = 0;
    }

    void AddControlPoint(int x, int y, Color color, Image& _image)
    {
        if (_isFirstClick == 0 && !_controlPoint)
        {
            // 始点の座標を記録する
            _prevX0 = x;
            _prevY0 = y;
            _isFirstClick++;
            _controlPoint = true;
        }
        else if (_isFirstClick == 1)
        {
            // 1つ目の制御点を記録する
            _controlX0 = x;
            _controlY0 = y;
            _isFirstClick++;
        }
        else if (_isFirstClick == 2)
        {
            // 2つ目の制御点を記録する
            _controlX1 = x;
            _controlY1 = y;
            _isFirstClick++;
        }
        else
        {
            // 4以上の偶数回目のクリックの場合、始点から現在の座標まで曲線を引く
            _image.DrawCurve(_prevX0, _prevY0, _controlX0, _controlY0, _controlX1, _controlY1, x, y, color);

            //次の始点に4以上の偶数回目のクリック座標を代入
            //２つ目の制御点に4以上の偶数回目の座標から対称化した座標を代入
            _prevX0 = x;
            _prevY0 = y;
            _controlX0 = 2 * x - _controlX1;
            _controlY0 = 2 * y - _controlY1;
            _isFirstClick = 2;
        }
    }
};

struct LineDrawer
{
    int _prevX0, _prevY0;
    bool _isFirstClick;

    void DrawLineAtClick(int x, int y, Image& _image)
    {
        Color lineColor(255, 255, 0, 0);  // 線の色を設定
        _image.DrawLine(_prevX0, _prevY0, x, y, lineColor);
        _prevX0 = x;
        _prevY0 = y;
    }

    void RecordClickPoint(int x, int y, Image& _image)
    {
        if (!_isFirstClick)
        {
            // 最初のクリックの場合、クリック位置を記録するだけ
            _prevX0 = x;
            _prevY0 = y;
            _isFirstClick = true;
        }
        else
        {
            // 2回目のクリックの場合、前回の座標から現在の座標まで線を引く
            DrawLineAtClick(x, y, _image);
            _isFirstClick = false;
        }
    }
};

struct CGLessonWidget : public pgn::CGWidget
{
    Image _image;
    BezierCurveDrawer CurveControl;
    LineDrawer LineControl;
    
public:
    ~CGLessonWidget()
    {
       
    }

    void OnResize(int width, int height) override
    {
        _image.Resize(width, height);

        Color* _buf = _image.PixelBuf();
        
        Color color(255, 255, 255, 255);
        Fill(color);
        //DrawRandomPoints();
        //DrawRandomRects();
        //DrawCheck();
        //TestRandomLine();
        //TestRandomCurve();
        //TestRandomSphere();
        //TestRandomTriangle();
    }
    void OnPaint(HDC hdc) override
    {
        Color* _buf = _image.PixelBuf();
        memcpy(Buffer(), _buf, sizeof(uint32_t) * Width() * Height());
        Display();
    }
    void Fill(Color color)
    {
        Color* _buf = _image.PixelBuf();
        
        for (int y = 0; y < Height(); y++)
        {
            for (int x = 0; x < Width(); x++)
            {
                _image.PutPixel(x, y, color);
                
            }
        }
    }
    void DrawRandomPoints()
    {    
        std::random_device rd;
        std::mt19937 gen(rd());
        for (int i = 0; i < 10000; i++)
        {
            std::uniform_int_distribution<> RandomX(-1, 1500);
            std::uniform_int_distribution<> RandomY(-1, 1500);
            std::uniform_int_distribution<> RandomColor(0, 255);
            int x = RandomX(gen);
            int y = RandomY(gen);
            uint8_t a = RandomColor(gen);
            uint8_t r = RandomColor(gen);
            uint8_t g = RandomColor(gen);
            uint8_t b = RandomColor(gen);
            Color color(a, r, g, b);
            _image.PutPixel(x, y, color);
        }

    }
    void DrawRandomRects()
    {
        DWORD startTime = timeGetTime();

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> RandomX(1, Width() - 1);
        std::uniform_int_distribution<> RandomY(1, Height() - 1);
        std::uniform_int_distribution<> RandomColor(0, 255);
        for (int i = 0; i < 5000; i++)
        {
            int x = RandomX(gen);
            int y = RandomY(gen);
            int width = RandomX(gen);
            int height = RandomY(gen);
            uint8_t a = RandomColor(gen);
            uint8_t r = RandomColor(gen);
            uint8_t g = RandomColor(gen);
            uint8_t b = RandomColor(gen);
            Color color(a, r, g, b);
            //_image.DrawRect(x, y, width, height, color, a);
            _image.Fill(x, y, width, height, color);
        }
        DWORD endTime = timeGetTime();
        DWORD elapsedTime = endTime - startTime;
    }
    void DrawCheck()
    {
        Color color(128, 128, 128, 128);
        _image.Check(15, 15, color);
    }
    void TestRandomLine()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> RandomX(-1000, 3000);
        std::uniform_int_distribution<> RandomY(-1000, 3000);
        std::uniform_int_distribution<> RandomColor(0, 255);
        Color color(255, 0, 0, 0);
        for (int i = 0; i < 10000; i++)
        {
            int x0 = RandomX(gen);
            int y0 = RandomY(gen);
            int x1 = RandomX(gen);
            int y1 = RandomY(gen);

            _image.DrawLine(x0, y0, x1, y1, color);
        }
    }
    
    void TestRandomCurve()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> RandomX(-100, 700);
        std::uniform_int_distribution<> RandomY(-100, 700);
        std::uniform_int_distribution<> RandomColor(0, 255);
        Color color(255, 0, 0, 0);
        for (int i = 0; i < 10; i++)
        {
            int x0 = RandomX(gen);
            int y0 = RandomY(gen);
            int x1 = RandomX(gen);
            int y1 = RandomY(gen);
            int x2 = RandomX(gen);
            int y2 = RandomY(gen);
            int x3 = RandomX(gen);
            int y3 = RandomY(gen);

            _image.DrawCurve(x0, y0, x1, y1, x2, y2, x3, y3, color);
        }
    }

    void TestRandomSphere()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> RandomX(-100, 2000);
        std::uniform_int_distribution<> RandomY(-100, 2000);
        std::uniform_int_distribution<> RandomColor(1, 255);

        for (int i = 0; i < 100; i++)
        {
            int x = RandomX(gen);
            int y = RandomY(gen);
            int R = RandomColor(gen) / 2.0;
            uint8_t a = RandomColor(gen);
            uint8_t r = RandomColor(gen);
            uint8_t g = RandomColor(gen);
            uint8_t b = RandomColor(gen);
            Color color(a, r, g, b);

            _image.FillSphere(x, y, R, color);
        }
    }

    void TestRandomTriangle()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> RandomX(0, 1300);
        std::uniform_int_distribution<> RandomY(0, 650);
        std::uniform_int_distribution<> RandomColor(0, 255);

        for (int i = 0; i < 5; i++)
        {
            int x0 = RandomX(gen);
            int y0 = RandomY(gen);
            int x1 = RandomX(gen);
            int y1 = RandomY(gen);
            int x2 = RandomX(gen);
            int y2 = RandomY(gen);
            int x3 = RandomX(gen);
            int y3 = RandomY(gen);
            //uint8_t a = RandomColor(gen);
            uint8_t r = RandomColor(gen);
            uint8_t g = RandomColor(gen);
            uint8_t b = RandomColor(gen);
            Color color(255, r, g, b);

            _image.FillTriangleByCaseSlope(x0, y0, x1, y1, x2, y2, color);
        }
    }
    
    void DrawLineAtClick(int x, int y)
    {
        Color lineColor(255, 255, 0, 0);  // 線の色を設定
        _image.DrawLine(CurveControl._prevX0, CurveControl._prevY0, x, y, lineColor);
        CurveControl._prevX0 = x;
        CurveControl._prevY0 = y;
    }

    void TestClickLine(int x, int y)
    {
        LineControl.RecordClickPoint(x, y, _image);
    }
    void TestClickCurve(int x, int y)
    {
        Color color(255, 255, 0, 0);
        
        CurveControl.AddControlPoint(x, y, color, _image);
    }

    void ToScreen()
    {
        Color* _buf = _image.PixelBuf();
        memcpy(Buffer(), _buf, sizeof(uint32_t) * Width() * Height());
        Display();
    }

    void OnTouchPress(Touch touch, int x, int y) override
    {
        // マウスの左ボタンを押した時
        if (touch & TouchLeft)
        {
            TestClickCurve(x, y);
            //TestClickLine(x, y);
            ToScreen();
        }
        if (touch & TouchRight)
        {
            static int a = 100;
            Color color(255, 255, 0, 0);
            //_image.DrawCurve(0, 0+a, 800, 800+a, 1000, 100+a, 0, 10+a, color);
            //color._red = 0;
            _image.DrawLine(-10, a, 300, 100 + a, color);
            _image.DrawLine(-10 + a, 1200, 400 + a, 100, color);
            //color._red = 0;
            //_image.DrawLine(1090, 1200, 1500, 100, color);
            //_image.DrawRect(x, y, 100, 100, color, color._alpha);
            //_image.PutClearPixel(x, y, color, color._alpha);
            
            a += 100;
            ToScreen();
        }

    }
    void OnTouchMove(Touch touch, int x, int y)
    {
        // タッチ移動（マウス移動）イベントが起きた時の処理を書く
        std::wstring s = L"(" + std::to_wstring(x) + L"," + std::to_wstring(y) + L")";
        //_image.DrawSphere(x, y, 10);
        //Color* _buf = _image.PixelBuf();
        //memcpy(Buffer(), _buf, sizeof(uint32_t) * Width() * Height());
        //Display();

        // タッチ位置 (マウス位置) をウィンドウのタイトルバーに表示する
        SetWindowTitle(s);
    }
};

CGLessonWidget g_widget;


// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CGLESSON, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CGLESSON));

    MSG msg;

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CGLESSON));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CGLESSON);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    /*
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;*/

    return g_widget.WndProc(hInst, hWnd, message, wParam, lParam);
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


