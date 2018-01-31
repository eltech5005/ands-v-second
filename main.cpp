#include <iostream>
#include "screen.h"
#include "shape.h"

using namespace std;

// Дополнительная "сборная" фигура
class myshape : public rectangle
{                // Моя фигура ЯВЛЯЕТСЯ прямоугольником
    line *l_eye; // левый глаз – моя фигура СОДЕРЖИТ линию
    line *r_eye; // правый глаз
    line *mouth; // рот
  public:
    myshape(point, point);
    void draw();
    void move(int, int);
};

myshape::myshape(point a, point b) : rectangle(a, b)
{ // myshape запускает конструктор базового класса rectangle !
    int ll = neast().x - swest().x + 1;
    int hh = neast().y - swest().y + 1;
    l_eye = new line(point(swest().x + 2, swest().y + hh * 3 / 4), 2);
    r_eye = new line(point(swest().x + ll - 4, swest().y + hh * 3 / 4), 2);
    mouth = new line(point(swest().x + 2, swest().y + hh / 4), ll - 4);
}

void myshape::draw()
{
    rectangle ::draw();
    int a = (swest().x + neast().x) / 2;
    int b = (swest().y + neast().y) / 2;
    put_point(point(a, b));
}

void myshape ::move(int a, int b)
{
    rectangle ::move(a, b);
    l_eye->move(a, b);
    r_eye->move(a, b);
    mouth->move(a, b);
}

int main()
{
    screen_init();

    right_triangle *p1 = new right_triangle(point(1, 5), point(6, 1));
    p1->flip_vertically();
    shape_refresh(); 
    std::cin.get();

    
    p1->flip_horisontally();
    shape_refresh();
    std::cin.get();

    return 0;
}