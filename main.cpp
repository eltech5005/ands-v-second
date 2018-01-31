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
    try {
        screen_init();
        // ----- Объявление набора фигур -----
        rotatable *p1 = new rectangle(point(0, 0), point(14, 5));
        shape *p2 = new line(point(0, 15), 17);
        shape *p3 = new myshape(point(15, 10), point(27, 18));
        right_triangle *p4 = new right_triangle(point(31, 5), point(36, 1));
        right_triangle *p5 = new right_triangle(point(41, 5), point(46, 1));
        right_triangle *p6 = new right_triangle(point(31, 15), point(36, 11));
        right_triangle *p7 = new right_triangle(point(41, 15), point(46, 11));

        // ----- Вывод начального набора фигур -----
        shape_refresh(); std::cin.get();

        // ----- Выполнение отражений и переворотов и вывод на экран -----
        p1->rotate_right();
        p4->flip_vertically();
        p6->flip_vertically();
        shape_refresh(); std::cin.get(); 
        
        // ----- Перемещение фигур и вывод результата -----
        p3->move(30,0);
        p3->move(-10,10);

        up(p2, p3);
        up(p1, p2);
        lftUp(p4,p2);
        rgtUp(p5,p2);
        lftDwn(p7,p3);
        rgtDwn(p6,p3);
        p6->move(-6,4);
        p7->move(6,4);
        shape_refresh(); 
    }    
    catch (right_triangle::Exception e){
		std::cout << "Сработало исключение " << e.what() << "\nЗначения точек:";
		e.showPoints();
		std::cout << endl;
	}
    catch (shape::onScrException e) {
		cout << "Точка "; e.showPoint(); cout << " за пределами экрана" << endl;
	}

    std::cin.get();

    return 0;
}