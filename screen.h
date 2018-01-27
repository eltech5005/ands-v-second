// -----
// Файл screen.h - поддержка работы с экраном
// -----

const int XMAX = 80; // Размер экрана
const int YMAX = 40;

// Точка на экране
class point
{

  public:
    int x, y;

    point() {}
    point(int a, int b) : x(a), y(b) {}
};
// -----

// Набор утилит для работы с экраном
// -----
// Вывод точки
void put_point(int a, int b);
void put_point(point p) { put_point(p.x, p.y); }

// Вывод линии
void put_line(int, int, int, int);
void put_line(point a, point b) { put_line(a.x, a.y, b.x, b.y); }

extern void screen_init();    // Создание экрана
extern void screen_destroy(); // Удаление экрана
extern void screen_refresh(); // Обновление
extern void screen_clear();   // Очистка
