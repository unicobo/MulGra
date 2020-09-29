# pragma once
#include "Siv3D.hpp"
#include "Utils.hpp"

class ControlButton
{
private:
    Vec2 pos;
    double len;
    Player player;
    Direction direction; // 0:right 1:down 2:left 3:up 
    Rect rect;

    // button size
    const double ROUND_PROPORTION = 0.1;
    const double FRAME_PROPORTION = 0.1;

    // arrow size
    const double ARROW_LENGTH_PROPORTION = 0.3;
    const double ARROW_WIDTH_PROPORTION = 0.2;
    const double ARROW_HEAD_WIDTH_PROPORTION = 0.4;
    const double ARROW_HEAD_HEIGHT_PROPORTION = 0.3;
    const double ARROW_LENGTH_PROPORTION_X[4] = {dx[0] * ARROW_LENGTH_PROPORTION, dx[1] * ARROW_LENGTH_PROPORTION, dx[2] * ARROW_LENGTH_PROPORTION, dx[3] * ARROW_LENGTH_PROPORTION};
    const double ARROW_LENGTH_PROPORTION_Y[4] = {dy[0] * ARROW_LENGTH_PROPORTION, dy[1] * ARROW_LENGTH_PROPORTION, dy[2] * ARROW_LENGTH_PROPORTION, dy[3] * ARROW_LENGTH_PROPORTION};


    // color
    const Color BASE_COLOR = Palette::White;
    const int CLICKED_A = 150;

    void update_rect() {rect.set(pos.x - len / 2, pos.y - len / 2, len, len);}

public:
    ControlButton()
        : pos(0, 0), len(0), player(Player::RED), direction(Direction::RIGHT), rect(0, 0, 0, 0){};

    ControlButton(double _x, double _y, double _len, Player _player = Player::RED, Direction _direction = Direction::RIGHT)
        : pos(_x, _y), len(_len), player(_player), direction(_direction), rect(_x - _len / 2, _y - _len / 2, _len, _len){};

    void draw() const
    {
        rect.rounded(len * ROUND_PROPORTION).draw(BASE_COLOR);

        Color color = player2color(player);
        Color current_color = rect.leftPressed() ? Color(color, CLICKED_A) : color;

        rect.rounded(len * ROUND_PROPORTION).drawFrame(len * FRAME_PROPORTION, 0, current_color);

        Line(pos.x - len * ARROW_LENGTH_PROPORTION_X[direction], pos.y - len * ARROW_LENGTH_PROPORTION_Y[direction], 
             pos.x + len * ARROW_LENGTH_PROPORTION_X[direction], pos.y + len * ARROW_LENGTH_PROPORTION_Y[direction])
            .drawArrow(len * ARROW_WIDTH_PROPORTION, Vec2(len * ARROW_HEAD_WIDTH_PROPORTION, len * ARROW_HEAD_HEIGHT_PROPORTION), current_color);
    }

    void set_pos(Vec2 _pos) { pos = _pos; update_rect();}

    void set_len(double _len) { len = _len; update_rect();}

    void set_player(Player _player) { player = _player;}

    void set_direction(Direction _direction) { direction = _direction; }

    bool clicked() const
    {
        // return rect.leftClicked();
        return this->rect.leftClicked();
    }

    bool released() const
    {
        return rect.leftReleased();
    }

    Optional<Operation> get_operation() const
    {
        if(released())
            return Operation(player, direction);
        return none;
    }
};
