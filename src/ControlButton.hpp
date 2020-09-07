#include "Siv3D.hpp"

class ControlButton
{
private:
    Vec2 pos;
    double len;
    int direction;
    Color color;
    Rect rect;

    const double ROUND_PROPORTION = 0.1;
    const double FRAME_PROPORTION = 0.1;
    const int CLICKED_A = 150;

public:
    ControlButton(double _x, double _y, double _len, Color _color = Palette::Red, int _direction = 0) : pos(_x, _y),
                                                                                                        len(_len),
                                                                                                        color(_color),
                                                                                                        direction(_direction),
                                                                                                        rect(_x - _len / 2, _y - _len / 2, _len, _len){};

    void draw() const
    {
        rect.rounded(len * ROUND_PROPORTION).draw(Palette::White);
        if(rect.leftPressed())
            rect.rounded(len * ROUND_PROPORTION).drawFrame(len * FRAME_PROPORTION, 0, Color(color.r, color.g, color.b, CLICKED_A));
        else
            rect.rounded(len * ROUND_PROPORTION).drawFrame(len * FRAME_PROPORTION, 0, color);
    }

    bool clicked() const
    {
        return rect.leftClicked();
    }
};
