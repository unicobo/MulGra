#include "Siv3D.hpp"

struct ControlButton
{
    Vec2 pos;
    double len;
    int direction;
    Color color;

    ControlButton(double _x, double _y, double _len) : pos(_x, _y),
                                                       len(_len),
                                                       color(Palette::Red){};

    ControlButton(double _x, double _y, double _len, int _direnction) : pos(_x, _y),
                                                                        len(_len)
    {
        direction = _direnction % 4;
        switch (direction)
        {
        case 0:
            color = Palette::Red;
            break;
        case 1:
            color = Palette::Blue;
            break;
        case 2:
            color = Palette::Yellow;
            break;
        case 3:
            color = Palette::Green;
            break;
        default:
            color = Palette::Red;
            break;
        }
    }

    void draw()
    {
        Rect(pos.x, pos.y, len, len).draw(color);
    }
};
