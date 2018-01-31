// -----
// Файл shape.h -- библиотека фигур
// -----

char screen[XMAX][YMAX];

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

// Проверка попадания на экран
int on_screen(int a, int b)
{
    return 0 <= a && a < XMAX && 0 <= b && b < YMAX;
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
};

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
    point north() const { return point((w.x + e.x) / 2, e.y < w.y ? w.y : e.y); }
    point south() const { return point((w.x + e.x) / 2, e.y < w.y ? e.y : w.y); }
    point east() const { return point((w.x + e.x) / 2, e.y < w.y ? e.y : w.y); }
    point west() const { return point((w.x + e.x) / 2, e.y < w.y ? e.y : w.y); }
    point neast() const { return point((w.x + e.x) / 2, e.y < w.y ? e.y : w.y); }
    point seast() const { return point((w.x + e.x) / 2, e.y < w.y ? e.y : w.y); }
    point nwest() const { return point((w.x + e.x) / 2, e.y < w.y ? e.y : w.y); }
    point swest() const { return point((w.x + e.x) / 2, e.y < w.y ? e.y : w.y); }

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
        {n,nw}
        |    -
        |       -
        w          {c,ne}
        |              -
        |                 -
        sw ------ s ------ {e,se} 
     --- */

    point n, e;

  public:
    right_triangle(point, point);

    /* ---
     Мы считаем, что описание точек фигуры за пределами фигуры не имеет смысла
     для сохранения единообразия фигур, мы объявляем некоторые точки эквивалентными.

     Самая северная и самая северо-западная точки в данном случае совпадают.
     Анологично с самой восточной и юго-восточной.
     Самой северо-восточной точкой логично считать середину гипиотенузы треугольника.

     В качестве центра фигуры мы рассматриваем центр описанной вокруг неё окружности.
     Эта точка также будет совпадать с серединой гипотенузы.
     --- */
    
    point north() const { return n; }
    point south() const { return point((n.x + e.x) / 2, e.y); }
    point east()  const { return e; }
    point west()  const { return point(n.x, (e.y + n.y) / 2); }
    point neast() const { return point((n.x + e.x)/2, (n.y + e.y)/2); } // https://goo.gl/Fd8DKG
    point seast() const { return e; }
    point nwest() const { return n; }
    point swest() const { return point(n.x,e.y); }

    // Поворот вправо относительно se
    void rotate_right()
    {
        int w = e.x - n.x;
        int h = n.y - e.y;

        n.y = e.y;
        n.x = e.x - h;
        e.y=e.y+w;        
    }


    // Поворот влево относительно sw
    void rotate_left()
    {
        int w = e.x - n.x;
        int h = n.y - e.y;

        n.y = e.y + w;
        n.x = n.x + h;
        e.x = e.x - w;
    }

    void flip_horisontally() {}; // Отразить горизонтально
    void flip_vertically() {};   // Отразить вертикально


    void move(int a, int b)
    {
        n.x += a;
        n.y += b;
        e.x += a;
        e.y += b;
    }

    void draw();
};


right_triangle::right_triangle(point a, point b)
{
    if (a.x <= b.x)
    {
        if (a.y >= b.y)
            n = a, e = b;
        else
            n = point(a.x, b.y), e = point(b.x, a.y);
    }
    else
    {
        if (b.y >= a.y)
            n = b, e = a;
        else
            n = point(b.x, a.y), e = point(a.x, b.y);
    }
}

void right_triangle::draw()
{
    point sw = swest();
    put_line(n, e);
    put_line(n, sw);
    put_line(sw, e);
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
