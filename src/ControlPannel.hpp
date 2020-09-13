# pragma once
#include "Siv3D.hpp"
#include "ControlButton.hpp"
#include "Utils.hpp"

class ControlPannel
{
private:
    Vec2 pos;
    double len;
    Rect rect;
    static const int BUTTON_NUM = 8;
    ControlButton control_buttons[BUTTON_NUM];

    // button position
    const double BUTTON_DIST_PROPOTION = 0.35;
    const double BUTTON_MARGN_PROPOTION = 0.25;

    // button size
    const double BUTTON_SIZE_PROPOTION = 0.2;

    // button direction
    const double BUTTON_DIRECTIONS[BUTTON_NUM] = {Direction::UP, Direction::DOWN, Direction::RIGHT, Direction::LEFT, Direction::DOWN, Direction::UP, Direction::LEFT, Direction::RIGHT}; 

    // color
    const Color PANNEL_COLOR = Palette::White;
    const Color BUTTON_COLOR[BUTTON_NUM / 2] = {Palette::Red, Palette::Blue, Palette::Green, Palette::Orange};

    // hoge
    const double dx[BUTTON_NUM] = {BUTTON_DIST_PROPOTION, BUTTON_DIST_PROPOTION, BUTTON_MARGN_PROPOTION / 2, -BUTTON_MARGN_PROPOTION / 2, -BUTTON_DIST_PROPOTION, -BUTTON_DIST_PROPOTION, -BUTTON_MARGN_PROPOTION / 2, BUTTON_MARGN_PROPOTION / 2};
    const double dy[BUTTON_NUM] = {-BUTTON_MARGN_PROPOTION / 2, BUTTON_MARGN_PROPOTION / 2, BUTTON_DIST_PROPOTION, BUTTON_DIST_PROPOTION, BUTTON_MARGN_PROPOTION / 2, -BUTTON_MARGN_PROPOTION / 2, -BUTTON_DIST_PROPOTION, -BUTTON_DIST_PROPOTION};

public:
    ControlPannel(double _x, double _y, double _len) 
        : pos(_x, _y), len(_len), rect(_x - len / 2, _y - len / 2, _len, _len)
        {
            for(int i = 0; i < 8; i++)
            {
                control_buttons[i].set_pos(Vec2(_x + _len * dx[i], _y + _len * dy[i]));
                control_buttons[i].set_len(_len * BUTTON_SIZE_PROPOTION);
                control_buttons[i].set_color(BUTTON_COLOR[i / 2]);
                control_buttons[i].set_direction(BUTTON_DIRECTIONS[i]);
            }
        };

    Operation get_operation() const
    {
        for(int i = 0; i < BUTTON_NUM; i++)
        {
            Operation ope = control_buttons[i].get_operation();
            if(ope.is_valid_operation())
                return ope;
        }
        return Operation();
    }

    void draw() const
    {
        rect.draw(PANNEL_COLOR);
        for(int i = 0; i < BUTTON_NUM; i++) control_buttons[i].draw();
    }
};