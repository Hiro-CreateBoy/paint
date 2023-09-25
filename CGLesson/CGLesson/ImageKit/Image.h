#pragma once
struct Color
{
    uint8_t _blue;
    uint8_t _green;
    uint8_t _red;
    uint8_t _alpha;

    Color(uint8_t a, uint8_t r, uint8_t g, uint8_t b) : _blue(b), _green(g), _red(r), _alpha(a) {}
    Color() {}

    Color& operator=(uint32_t color)
    {
        _blue = color & 0xFF;
        _green = (color >> 8) & 0xFF;
        _red = (color >> 16) & 0xFF;
        _alpha = (color >> 24) & 0xFF;
        return *this;
    }

    uint32_t ColorNum()
    {
        uint32_t colorNum = (_alpha << 24) | (_red << 16) | (_green << 8) | _blue;
        return colorNum;
    }
};

class Image
{
public:
    Image(int width, int height) : _buf((Color*)malloc(sizeof(Color)* width* height)), _width(width), _height(height)
    {
        memset(_buf, 0, sizeof(Color) * _width * _height);
    }
    Image() :_buf(nullptr), _width(0), _height(0) { }
    ~Image() { free(_buf); }
    void Resize(int width, int height)
    {
        free(_buf);
        _height = height;
        _width = width;
        _buf = (Color*)malloc(sizeof(Color) * width * height);
        memset(_buf, 0, sizeof(Color) * _width * _height);
    }
    Color* PixelBuf() { return _buf; }
    int IWidth() { return _width; }
    int IHeight() { return _height; }

    void PutPixel(int x, int y, Color color)
    {
        if (x < _width and x > 0 and y < _height and y > 0)
            _buf[y * _width + x] = color;
    }
    void PutClearPixel(int x, int y, Color color, uint8_t alpha)
    {
        uint8_t orgPer = 255 - alpha;
        uint32_t nextRed = color._red * alpha;
        uint32_t nextGreen = color._green * alpha;
        uint32_t nextBlue = color._blue * alpha;
        if (x < _width && x > 0 && y < _height && y > 0)
        {
            Color& currentPixel = _buf[y * _width + x];

            currentPixel._red = (currentPixel._red * orgPer + nextRed) / 255;
            currentPixel._green = (currentPixel._green * orgPer + nextGreen) / 255;
            currentPixel._blue = (currentPixel._blue * orgPer + nextBlue) / 255;
        }
    }
    void Fill(int x, int y, int width, int height, Color color)
    {
        // クリッピング処理
        if (x < 0) {
            width += x;
            x = 0;
        }
        if (y < 0) {
            height += y;
            y = 0;
        }
        if (x + width > _width) {
            width -= (x + width - _width);
        }
        if (y + height > _height) {
            height -= (y + height - _height);
        }

        for (int i = y; i < y + height; i++) {
            for (int j = x; j < x + width; j++) {
                Color& currentPixel = _buf[i * _width + j];

                currentPixel._red = color._red;
                currentPixel._green = color._green;
                currentPixel._blue = color._blue;
            }
        }
    }
    void DrawRect(int x, int y, int width, int height, Color color, uint8_t alpha)
    {
        uint8_t orgPer = 255 - alpha;
        uint32_t nextRed = color._red * alpha;
        uint32_t nextGreen = color._green * alpha;
        uint32_t nextBlue = color._blue * alpha;

        // クリッピング処理
        if (x < 0) {
            width += x;
            x = 0;
        }
        if (y < 0) {
            height += y;
            y = 0;
        }
        if (x + width > _width) {
            width -= (x + width - _width);
        }
        if (y + height > _height) {
            height -= (y + height - _height);
        }

        for (int i = y; i < y + height; i++) {
            Color* ptr = &_buf[i * _width + x];

            for (int j = x; j < x + width; j++) {

                Color& currentPixel = *ptr;

                currentPixel._red = (currentPixel._red * orgPer + nextRed) / 255;
                currentPixel._green = (currentPixel._green * orgPer + nextGreen) / 255;
                currentPixel._blue = (currentPixel._blue * orgPer + nextBlue) / 255;
                //PutPixel(j, i, color);
                ptr++;
            }
        }
    }
    void Check(int cwidth, int cheight, Color color)
    {
        for (int j = 0; j < _height; j++)
        {
            for (int i = 0; i < _width; i++)
            {
                Fill(j * cwidth * 2, i * cheight * 2, cwidth, cheight, color);
                Fill(j * cwidth * 2 + cwidth, i * cheight * 2 + cheight, cwidth, cheight, color);
            }
        }
    }
    void Check2(int cwidth, int cheight, Color color)
    {
        for (int i = 0; i < _width; i++)
        {
            for (int j = 0; j < _height; j++)
            {
                Fill(j * cwidth * 2, i * cheight * 2, cwidth, cheight, color);
                Fill(j * cwidth * 2 + cwidth, i * cheight * 2 + cheight, cwidth, cheight, color);
            }
        }
    }
    /*void Line(int x0, int y0, int x1, int y1, Color color)
    {
        if (x0 != x1)
        {
            /*if (x0 > x1) {
                int maxX = x0;
                int maxY = y0;
                x0 = x1;
                y0 = y1;
                x1 = maxX;
                y1 = maxY;
            }
            double dxy = (double)(y0 - y1) / (x0 - x1);
            double py = y0;
            if (dxy > 1.0 or dxy < -1.0)
            {
                for (int x = x0; x <= x1; x++)
                {
                    int iy = (int)py;
                    for (int y = iy; y <= iy + (int)dxy; y++)
                    {
                        Color& currentPixel = _buf[y * _width + x];
                        Color& nextPixel = _buf[y * _width + x + 1];
                        currentPixel._red = color._red;
                        currentPixel._green = color._green;
                        currentPixel._blue = color._blue;
                        currentPixel._alpha -= (uint8_t)((255 / dxy) * (y - iy));
                        nextPixel._alpha = (uint8_t)((255 / dxy) * (y - iy));
                        nextPixel._red = color._red;
                        nextPixel._green = color._green;
                        nextPixel._blue = color._blue;
                    }
                    py += dxy;
                }
            }

            else
            {
                for (int x = x0; x <= x1; x++)
                {
                    int iy = (int)py;
                    double f = (double)(py - iy);
                    int a = (int)(f * 256);
                    Color& currentPixel = _buf[iy * _width + x];
                    Color& nextPixel = _buf[(iy + 1) * _width + x];
                    currentPixel._red = color._red;
                    currentPixel._green = color._green;
                    currentPixel._blue = color._blue;
                    currentPixel._alpha = 255 - a;
                    nextPixel._red = color._red;
                    nextPixel._green = color._green;
                    nextPixel._blue = color._blue;
                    nextPixel._alpha = a;
                    py += dxy;
                }
            }
        }
        else
        {
            for (int y = y0; y < y1; y++)
                _buf[y * _width + x1] = color;
        }
    }*/

    void DrawLine(double x0, double y0, double x1, double y1, Color color)
    {
        // swapForSteepLine: 傾きがy=xよりも上か下か。
        // swapForSteepLineがtrueの時はy方向にループを回し対応するxの点を打つ。
        //  swapForSteepLineがfalseの時はx方向にループを回し、対応するyの点を打つ。
        bool swapForSteepLine = abs(x0 - x1) < abs(y0 - y1);

        // (u0, v0) はx0, y0に対応する点だが、yの変化量＞ｘの変化量のとき(u0, v0)はy0,x0に対応する。
        // u1, v1も同じ
        // また、いつもu0 < u1となるように対応させる
        //x,yを入れ替えたときにクリッピングで範囲が変わるので、uvのときのuvWidth,uvHeight
        double u0;
        double v0;
        double u1;
        double v1;
        double uvWidth;
        double uvHeight;
        if (swapForSteepLine == true)
        {
            u0 = y0;
            v0 = x0;
            u1 = y1;
            v1 = x1;
            uvWidth = _height;
            uvHeight = _width;
        }
        else
        {
            u0 = x0;
            v0 = y0;
            u1 = x1;
            v1 = y1;
            uvWidth = _width;
            uvHeight = _height;
        }

        if (u0 > u1)  //uのループを1回にするために小さい順に
        {
            std::swap(u0, u1);
            std::swap(v0, v1);

        }

        // uが1変化した時にvがどれだけ変化したかを表す
        double duv = (double)(v0 - v1) / (u0 - u1);
        // vが1変化した時にuがどれだけ変化したかを表す
        double dvu = (double)(u0 - u1) / (v0 - v1);
        // 現時点での対応するvの値を表す。
        double pv = v0;

        // この時点でuは必ず変化凌の大きい方の軸になっている。
        // uが一つ変化するごとに対応するvの変化量は必ず-1以上、1以下となっている。
        for (int u = u0; u <= u1; u++)
        {
            // 現在のvの値から、小数点を切り捨てた整数のvの値
            int iv = (int)pv;
            //整数値の座標からどれだけ離れているのか判定
            //pvの小数点以下の部分を割合としアルファ値に計算する
            double f = (double)(pv - iv);

            int a = (int)(f * 256);

            // uがxかyかをswapForSteepLineで調べる。
            if (swapForSteepLine == true)
            {
                // uがyのケース。
                PutClearPixel(iv, u, color, color._alpha - a);
                PutClearPixel(iv + 1, u, color, a);
            }
            else
            {
                // uがxのケース
                PutClearPixel(u, iv, color, color._alpha - a);
                PutClearPixel(u, iv + 1, color, a);
            }
            pv += duv;
        }
    }

    double CalcBezier(double p0, double p1, double p2, double p3, double t)
    {
        double v0 = (1 - t) * (1 - t) * (1 - t) * p0;
        double v1 = 3 * t * (1 - t) * (1 - t) * p1;
        double v2 = 3 * t * t * (1 - t) * p2;
        double v3 = t * t * t * p3;

        return v0 + v1 + v2 + v3;
    }

    void DrawCurve(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, Color color)
    {
        //線の分割数
        int div = 30;
        double delta_t = 1.0 / div;

        //細かく分割した頂点ごとに線を繋ぐ
        for (int i = 0; i < div; i++)
        {
            double t0 = (double)i / div;
            double t1 = t0 + delta_t;

            double px0 = CalcBezier(x0, x1, x2, x3, t0);
            double py0 = CalcBezier(y0, y1, y2, y3, t0);

            double px1 = CalcBezier(x0, x1, x2, x3, t1);
            double py1 = CalcBezier(y0, y1, y2, y3, t1);

            DrawLine(px0, py0, px1, py1, color);
        }
    }

    void FillSphere(int cx, int cy, int r, Color color)
    {
        int r2 = r * r;
        int right = cx + r + 1;
        int bottom = cy + r + 1;
        int left = cx - r - 1;
        int up = cy - r - 1;
        int div = 5;
        double delta = 1.0 / div;

        // クリッピング処理
        if (cx - r < 0) {
            left = 0;
        }
        if (cy - r < 0) {
            up = 0;
        }
        if (right >= _width) {
            right -= (right - _width);
        }
        if (bottom >= _height) {
            bottom -= (bottom - _height);
        }

        for (int y = up; y < bottom; y++)
        {
            for (int x = left; x < right; x++)
            {
                int insideCount = 0;
                for (int n = 0; n < div; n++)
                {
                    for (int m = 0; m < div; m++)
                    {
                        double dx = x - cx;
                        double dy = y - cy;

                        dx = dx + delta * m;
                        dy = dy + delta * n;

                        double distance = dx * dx + dy * dy;

                        if (distance < r2)
                        {
                            insideCount = insideCount + 1;
                        }
                    }
                }

                uint8_t alpha = 255 * insideCount / (div * div);
                PutClearPixel(x, y, color, alpha);
                //PutPixel(x, y, color);
            }
        }
    }

    void FillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, Color color)
    {
        int edgex0 = x1 - x0;
        int edgey0 = y1 - y0;
        int edgex1 = x2 - x1;
        int edgey1 = y2 - y1;
        int edgex2 = x0 - x2;
        int edgey2 = y0 - y2;

        int minx = min(x0, min(x1, x2));
        int maxx = max(x0, max(x1, x2));
        int miny = min(y0, min(y1, y2));
        int maxy = max(y0, max(y1, y2));

        //三点の大きさで四角を判定
        //外積の計算からピクセルごとに内側か外側か判定する
        for (int py = miny; py <= maxy; py++)
        {
            for (int px = minx; px <= maxx; px++)
            {
                int vx0 = px - x0;
                int vy0 = py - y0;
                int vx1 = px - x1;
                int vy1 = py - y1;
                int vx2 = px - x2;
                int vy2 = py - y2;

                int cross1 = edgex0 * vy0 - edgey0 * vx0;
                int cross2 = edgex1 * vy1 - edgey1 * vx1;
                int cross3 = edgex2 * vy2 - edgey2 * vx2;

                bool inside = (cross1 >= 0 && cross2 >= 0 && cross3 >= 0) || (cross1 <= 0 && cross2 <= 0 && cross3 <= 0);

                if (inside == true)
                {
                    PutClearPixel(px, py, color, 128);
                }
            }
        }

        //境界部分をアンチエリアスにする
        //DrawLine(x0, y0, x1, y1, color);
        //DrawLine(x2, y2, x1, y1, color);
        //DrawLine(x0, y0, x2, y2, color);
    }

    //場合分けで三角形の面積を塗る関数
    void FillTriangleByCaseSlope(int x0, int y0, int x1, int y1, int x2, int y2, Color color)
    {
        // 三角形の頂点をy座標から見て一番大きい物を判定
        if (y0 > y1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        if (y1 > y2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        if (y0 > y1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }

        // 上部三角形と下部三角形の境界を見つける
        double dxy1 = (double)(x1 - x0) / (y1 - y0);
        double dxy2 = (double)(x2 - x0) / (y2 - y0);

        double x_start = x0;
        double x_end = x0;

        if ((dxy1 < dxy2 && dxy2 < 0)||(dxy1 < 0 && dxy2 > 0)||(dxy1 > 0 && dxy2 > dxy1))
        {
            // 上部三角形を塗りつぶす
            for (int y = y0; y < y1; y++) {
                for (int x = x_start; x <= x_end; x++) {
                    PutPixel(x, y, color);
                }
                x_start += dxy1;
                x_end += dxy2;
            }

            // 上部三角形と下部三角形の境界を見つける
            dxy1 = (double)(x2 - x1) / (y2 - y1);
            x_start = x1;
            x_end = x0 + dxy2 * (y1 - y0);

            // 下部三角形を塗りつぶす
            for (int y = y1; y <= y2; y++) {
                for (int x = x_start; x <= x_end; x++) {
                    PutPixel(x, y, color);
                }
                x_start += dxy1;
                x_end += dxy2;
            }
        }
        else
        {
            // 上部三角形を塗りつぶす
            for (int y = y0; y < y1; y++) {
                for (int x = x_start; x >= x_end; x--) {
                    PutPixel(x, y, color);
                }
                x_start += dxy1;
                x_end += dxy2;
            }

            // 上部三角形と下部三角形の境界を見つける
            dxy1 = (double)(x2 - x1) / (y2 - y1);
            x_start = x1;
            x_end = x0 + dxy2 * (y1 - y0);

            // 下部三角形を塗りつぶす
            for (int y = y1; y <= y2; y++) {
                for (int x = x_start; x >= x_end; x--) {
                    PutPixel(x, y, color);
                }
                x_start += dxy1;
                x_end += dxy2;
            }
        }
        
        DrawLine(x0, y0, x1, y1, color);
        DrawLine(x2, y2, x1, y1, color);
        DrawLine(x0, y0, x2, y2, color);
        /*color = 0xff000000;
        DrawLine(0, 0, x0, y0, color);
        color = 0xffff0000;
        DrawLine(0, 0, x1, y1, color);
        color = 0xff00ff00;
        DrawLine(0, 0, x2, y2, color);*/
    }


private:
    Color* _buf;
    int _height;
    int _width;
};