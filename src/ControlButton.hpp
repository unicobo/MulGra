#include "Siv3D.hpp"

class ControlButton
{
private:
    Vec2 pos;
    double len;
    int direction;
    Color color;
    Rect rect;

    // button size
    const double ROUND_PROPORTION = 0.1;
    const double FRAME_PROPORTION = 0.1;

    // arrow size
    const double ARROW_LENGTH_PROPORTION = 0.3;
    const double ARROW_WIDTH_PROPORTION = 0.2;
    const double ARROW_HEAD_WIDTH_PROPORTION = 0.4;
    const double ARROW_HEAD_HEIGHT_PROPORTION = 0.3;
    
    // hoge
    const double dx[4] = {ARROW_LENGTH_PROPORTION, 0, -ARROW_LENGTH_PROPORTION, 0};
    const double dy[4] = {0, ARROW_LENGTH_PROPORTION, 0, -ARROW_LENGTH_PROPORTION};

    // color
    const Color BASE_COLOR = Palette::White;
    const int CLICKED_A = 150;

public:
    ControlButton(double _x, double _y, double _len, Color _color = Palette::Red, int _direction = 0) : pos(_x, _y),
                                                                                                        len(_len),
                                                                                                        color(_color),
                                                                                                        direction(_direction % 4),
                                                                                                        rect(_x - _len / 2, _y - _len / 2, _len, _len){};

    void draw() const
    {
        rect.rounded(len * ROUND_PROPORTION).draw(BASE_COLOR);

        Color current_color = rect.leftPressed() ? Color(color.r, color.g, color.b, CLICKED_A) : color;

        rect.rounded(len * ROUND_PROPORTION).drawFrame(len * FRAME_PROPORTION, 0, current_color);

        Line(pos.x - len * dx[direction], pos.y - len * dy[direction], pos.x + len * dx[direction], pos.y + len * dy[direction])
            .drawArrow(len * ARROW_WIDTH_PROPORTION, Vec2(len * ARROW_HEAD_WIDTH_PROPORTION, len * ARROW_HEAD_HEIGHT_PROPORTION), current_color);
    }

    bool clicked() const
    {
        return rect.leftClicked();
    }

    bool released() const
    {
        return rect.leftReleased();
    }
};
