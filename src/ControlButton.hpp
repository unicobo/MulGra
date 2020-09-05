#include "Siv3D.hpp"

class ControlButton
{
private:
    Vec2 pos;
    double len;
    int direction;
    // Color color;
    double color;
    Rect rect;

    const double ROUND_PROPORTION = 0.1;
    const double FRAME_PROPORTION = 0.1;
    const double S = 1;
    const double CLICKED_S = 0.7;
    const double V = 1;
    const HSV RED = HSV(0, 1, 0.8);
    const HSV CLICKED_RED = HSV(0, 0.5, 0.8);
    // const Color RED = Palette::Red;
    // const Color CLICKED_RED = Palette::Pink;

public:
    ControlButton(double _x, double _y, double _len) : pos(_x, _y),
                                                       len(_len),
                                                       color(0),
                                                       rect(_x, _y, _len, _len){};

    ControlButton(double _x, double _y, double _len, int _direnction) : pos(_x, _y),
                                                                        len(_len),
                                                                        rect(_x, _y, _len, _len)
    {
        direction = _direnction % 4;
        switch (direction)
        {
        case 0:
            color = 0;
            break;
        case 1:
            color = 60;
            break;
        case 2:
            color = 120;
            break;
        case 3:
            color = 240;
            break;
        }
    }

    void draw() const
    {
        if (rect.leftPressed())
            rect.rounded(len * ROUND_PROPORTION).draw(HSV(color, CLICKED_S, V));
        else
            rect.rounded(len * ROUND_PROPORTION).draw(HSV(color, S, V));
        if(rect.mouseOver())
            rect.rounded(len * ROUND_PROPORTION).drawFrame(len * FRAME_PROPORTION, 0, HSV(color, CLICKED_S, V));
        else
            rect.rounded(len * ROUND_PROPORTION).drawFrame(len * FRAME_PROPORTION, 0, HSV(color, S, V));
    }

    bool clicked() const
    {
        return rect.leftClicked();
    }
};
