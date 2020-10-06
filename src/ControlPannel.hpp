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

    // pannel
    const double PANNEL_PROPORTION = 0.95;

    // button position
    const double BUTTON_DIST_PROPOTION = 0.35;
    const double BUTTON_MARGN_PROPOTION = 0.25;

    // button size
    const double BUTTON_SIZE_PROPOTION = 0.2;

    // button direction
    const Direction BUTTON_DIRECTIONS[BUTTON_NUM] = {Direction::UP, Direction::DOWN, Direction::RIGHT, Direction::LEFT, Direction::DOWN, Direction::UP, Direction::LEFT, Direction::RIGHT};

    // color
    const Color PANNEL_COLOR = Color(0, 0, 0, 0);

    // hoge
    const double dx[BUTTON_NUM] = {BUTTON_DIST_PROPOTION, BUTTON_DIST_PROPOTION, BUTTON_MARGN_PROPOTION / 2, -BUTTON_MARGN_PROPOTION / 2, -BUTTON_DIST_PROPOTION, -BUTTON_DIST_PROPOTION, -BUTTON_MARGN_PROPOTION / 2, BUTTON_MARGN_PROPOTION / 2};
    const double dy[BUTTON_NUM] = {-BUTTON_MARGN_PROPOTION / 2, BUTTON_MARGN_PROPOTION / 2, BUTTON_DIST_PROPOTION, BUTTON_DIST_PROPOTION, BUTTON_MARGN_PROPOTION / 2, -BUTTON_MARGN_PROPOTION / 2, -BUTTON_DIST_PROPOTION, -BUTTON_DIST_PROPOTION};

public:
    ControlPannel(Vec2 _outer_pos, Vec2 _outer_size)
        : pos(_outer_pos + _outer_size / 2)
        , rect(pos.x - len / 2, pos.y - len / 2, len, len)
        {
            len = Min(_outer_size.x, _outer_size.y) * PANNEL_PROPORTION;
            for(int i = 0; i < 8; i++)
            {
                control_buttons[i].set_pos(Vec2(pos.x + len * dx[i], pos.y + len * dy[i]));
                control_buttons[i].set_len(len * BUTTON_SIZE_PROPOTION);
                control_buttons[i].set_player((Player)(i / 2));
                control_buttons[i].set_direction(BUTTON_DIRECTIONS[i]);
            }
        };

    Optional<Operation> get_operation() const
    {
        for(int i = 0; i < BUTTON_NUM; i++)
        {
            auto op = control_buttons[i].get_operation();
            if(op)
                return op;
        }
        return none;
    }

    void draw() const
    {
        rect.draw(PANNEL_COLOR);
        for(int i = 0; i < BUTTON_NUM; i++) control_buttons[i].draw();
    }
};