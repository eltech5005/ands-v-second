// -----
// Файл shape.h -- библиотека фигур
// -----

using namespace std;

char screen[XMAX][YMAX];

int on_screen(int a, int b);

enum color
{
    black = '*',
    white = ' '
};

void screen_init()
{
    for (int y = 0; y < YMAX; y++)
        for (int x = 0; x < XMAX; x++)
            screen[x][y] = white;
}

void put_point(int a, int b)
{
    if (on_screen(a, b))
        screen[a][b] = black;
}

/* ---
Рисование отрезка прямой (x0,y0) - (x1,y1).
Уравнение прямой: b(x-x0) + a(y-y0) = 0.
Минимизируется величина abs(eps),
где eps = 2*(b(x-x0)) + a(y-y0).
 --- */
void put_line(int x0, int y0, int x1, int y1)
{
    int dx = 1;
    int a = x1 - x0;

    if (a < 0)
        dx = -1, a = -a;
    int dy = 1;
    int b = y1 - y0;

    if (b < 0)
        dy = -1, b = -b;

    int two_a = 2 * a;
    int two_b = 2 * b;
    int xcrit = -b + two_a;
    int eps = 0;

    for (;;)
    {
        put_point(x0, y0);
        if (x0 == x1 && y0 == y1)
            break;
        if (eps <= xcrit)
            x0 += dx, eps += two_b;
        if (eps >= a || a < b)
            y0 += dy, eps -= two_a;
    }
}

void screen_clear() { screen_init(); } // Очистка экрана
void screen_refresh()                  // Обновление экрана
{
    for (int y = YMAX - 1; 0 <= y; --y)
    {                                  // с верхней строки до нижней
        for (int x = 0; x < XMAX; ++x) // от левого столбца до правого
            std::cout << screen[x][y];
        std::cout << '\n';
    }
}

// -----
// Библиотека фигур
// -----

// Виртуальный базовый класс "фигура"
struct shape
{
    static shape *list;
    shape *next;

    shape()
    {
        next = list;
        list = this;
    }

    virtual point north() const = 0;
    virtual point south() const = 0;
    virtual point east() const = 0;
    virtual point west() const = 0;
    virtual point neast() const = 0;
    virtual point seast() const = 0;
    virtual point nwest() const = 0;
    virtual point swest() const = 0;

    virtual void draw() = 0;
    virtual void move(int, int) = 0;

    class onScrException
    {
        point p;
    public:
        onScrException(point p0) : p(p0) {};
        void showPoint() {
            std::cout << '(' << p.x << ", " << p.y << ')';
        }
    };

};

// Проверка попадания на экран
int on_screen(int a, int b)
{
    return (0 <= a && a < XMAX && 0 <= b && b < YMAX)?1:throw shape::onScrException(point(a, b));
}

shape *shape ::list = nullptr; // Инициализация списка фигур

// Фигуры, пригодные к повороту
class rotatable : virtual public shape
{
  public:
    virtual void rotate_left() = 0;  // Повернуть влево
    virtual void rotate_right() = 0; // Повернуть вправо
};

// Фигуры, пригодные к зеркальному отражению
class reflectable : virtual public shape
{
  public:
    virtual void flip_horisontally() = 0; // Отразить горизонтально
    virtual void flip_vertically() = 0;   // Отразить вертикально
};

/* ---
Отрезок прямой ["w", "e" ].
north( ) определяет точку "выше центра отрезка и так далеко на север, как самая его северная точка", и т. п. 
 --- */
class line : public shape
{
    point w, e;

  public:
    line(point a, point b) : w(a), e(b){};
    line(point a, int L) : w(point(a.x + L - 1, a.y)), e(a){};
        point north() const { return point((w.x + e.x) / 2, (w.y + e.y) / 2); }
    point south() const { return point((w.x + e.x) / 2, (w.y + e.y) / 2); }
    point east() const { return e; }
    point west() const { return w; }
    point neast() const { return e; }
    point seast() const { return e; }
    point nwest() const { return w; }
    point swest() const { return w; }

    void move(int a, int b)
    {
        w.x += a;
        w.y += b;
        e.x += a;
        e.y += b;
    }
    void draw() { put_line(w, e); }
};

// Прямоугольник
class rectangle : public rotatable
{
    /* ---
        nw ------ n ------ ne
        |		           |
        |		           |
        w	     c         e
        | 		           |
        |		           |
        sw ------ s ------ se 
     --- */

    point sw, ne;

  public:
    rectangle(point, point);

    point north() const { return point((sw.x + ne.x) / 2, ne.y); }
    point south() const { return point((sw.x + ne.x) / 2, sw.y); }
    point east() const { return point(sw.x, (sw.y + ne.y) / 2); }
    point west() const { return point(ne.x, (sw.y + ne.y) / 2); }
    point neast() const { return ne; }
    point seast() const { return point(sw.x, ne.y); }
    point nwest() const { return point(ne.x, sw.y); }
    point swest() const { return sw; }

    // Поворот вправо относительно se
    void rotate_right()
    {
        int w = ne.x - sw.x, h = ne.y - sw.y;
        sw.x = ne.x - h * 2;
        ne.y = sw.y + w / 2;
    }

    // Поворот влево относительно sw
    void rotate_left()
    {
        int w = ne.x - sw.x, h = ne.y - sw.y;
        ne.x = sw.x + h * 2;
        ne.y = sw.y + w / 2;
    }

    void move(int a, int b)
    {
        sw.x += a;
        sw.y += b;
        ne.x += a;
        ne.y += b;
    }

    void draw();
};

rectangle::rectangle(point a, point b)
{
    if (a.x <= b.x)
    {
        if (a.y <= b.y)
            sw = a, ne = b;
        else
            sw = point(a.x, b.y), ne = point(b.x, a.y);
    }
    else
    {
        if (a.y <= b.y)
            sw = point(b.x, a.y), ne = point(a.x, b.y);
        else
            sw = b, ne = a;
    }
}

void rectangle::draw()
{
    point nw(sw.x, ne.y);
    point se(ne.x, sw.y);
    put_line(nw, ne);
    put_line(ne, se);
    put_line(se, sw);
    put_line(sw, nw);
}

// Прямоугольный треугольник
class right_triangle : public rotatable, public reflectable
{

    /* ---
        nw        n         ne          
        |    -
        |       -
        w         c         e
        |              -
        |                 -
        sw ------ s ------  se 
     --- */

    point h1, h2, a1;

  public:
    right_triangle(point, point);

    int width() const { return (max(h1.x, h2.x) - min(h1.x, h2.x)); }
    int margin_x() const { return min(min(h1.x, h2.x), a1.x); }
    int height() const { return (max(h1.y, h2.y) - min(h1.y, h2.y)); }
    int margin_y() const { return min(min(h1.y, h2.y), a1.y); }

    point north() const { return point(margin_x() + width() / 2, margin_y() + height()); }
    point south() const { return point(margin_x() + width() / 2, margin_y()); }
    point west() const { return point(margin_x(), margin_y() + height() / 2); }
    point east() const { return point(margin_x() + width(), margin_y() + height() / 2); }
    point nwest() const { return point(margin_x(), margin_y() + height()); }
    point neast() const { return point(margin_x() + width(), margin_y() + height()); }
    point swest() const { return point(margin_x(), margin_y()); }
    point seast() const { return point(margin_x() + width(), margin_y()); }

    // Поворот влево относительно sw
    void rotate_left()
    {
        int w = width();
        int h = height();

        h1.y -= h;
        a1.x += h;
        h2.x = h1.x + h;
        h2.y += w;

    }

    // Поворот вправо относительно se
    void rotate_right()
    {
        int w = width();
        int h = height();

        h1.y = a1.y + w;
        h1.x = h2.x;
        a1.y = h1.y;
        a1.x = h1.x-h;
        h2.x = a1.x;

    }

    // Отразить горизонтально
    void flip_horisontally(){
        if (a1.y == h1.y)
            a1.y = h2.y;
        else 
            a1.y = h1.y;
        swap (h1.y, h2.y);
        
    };

    // Отразить вертикально
    void flip_vertically(){
        if (a1.x == h1.x)
            a1.x = h2.x;
        else 
            a1.x = h1.x;
        swap (h1.x, h2.x);
    };   

    void move(int a, int b)
    {
        h1.x += a;
        h1.y += b;
        h2.x += a;
        h2.y += b;
        a1.x += a;
        a1.y += b;
    }

    void draw();

    class Exception
    {
        point a;
        point b;
        point c;
        std::string w;
    
        public:
            Exception(point p0, point p1, point p2, std::string str) : 
                a(p0), b(p1), c(p2), w(str) {};
            std::string what() { return w; }

            void showPoints() {
                std::cout << '(' << a.x << ", " << a.y << ") ";
                std::cout << '(' << b.x << ", " << b.y << ") ";
                std::cout << '(' << c.x << ", " << c.y << ')';
            }

    };

};

/* ---
   При начальном формировании фигурвы мы считаем, что треугольник изображён именно так, как показано на рисунке в задании
   --- */
right_triangle::right_triangle(point a, point b)
{
     if (a.x < 0 || a.y < 0 
        || b.x < 0 || b.y < 0) {
        throw Exception(a, b, point(-228,-228), 
            "в конструкторе Triangle\nНекорректные параметры");
    }

    if (a.x <= b.x)
    {
        if (a.y >= b.y)
            h1 = a, h2 = b;
        else
            h1 = point(a.x, b.y), h2 = point(b.x, a.y);
    }
    else
    {
        if (b.y >= a.y)
            h1 = b, h2 = a;
        else
            h1 = point(b.x, a.y), h2 = point(a.x, b.y);
    }

    a1 = point(h1.x, h2.y);
}

void right_triangle::draw()
{
    put_line(h1, a1);
    put_line(a1, h2);
    put_line(h1, h2);
}

// Перерисовка всех фигур
// Здесь используется динамическое связывание!!!
void shape_refresh()
{
    screen_clear();
    for (shape *p = shape ::list; p; p = p->next)
        p->draw();
    screen_refresh();
}

// Поместить p над q
// Здесь используется динамическое связывание!!!
void up(shape *p, const shape *q)
{
    point n = q->north();
    point s = p->south();
    p->move(n.x - s.x, n.y - s.y + 1);
}

void lftUp(shape* p, const shape* q)// Поместить p слева над q
{
    p->move(q->nwest().x - p->swest().x, q->nwest().y - p->swest().y + 1);
}

void rgtUp(shape* p, const shape* q)// Поместить p справа над q
{
    p->move(q->neast().x - p->seast().x, q->nwest().y - p->swest().y + 1);
}

void rgtDwn(shape* p, const shape* q)// Поместить p справа под q
{
    p->move(q->east().x - p->west().x, q->swest().y - p->nwest().y);
}

void lftDwn(shape* p, const shape* q)// Поместить p справа под q
{
    p->move(q->west().x - p->east().x, q->swest().y - p->nwest().y);
}