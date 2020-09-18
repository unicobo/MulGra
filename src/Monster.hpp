# pragma once
#include "Siv3D.hpp"
#include "Utils.hpp"

class Monster
{
private:
    Vec2 pos;
    Vec2 pre_pos;
    Vec2 nxt_pos;
    Player player;
    Direction direction;
    double size;
    Rect rect;

    // movement
    const double SPEED_RATE = 2;
    Stopwatch move_stopwatch = Stopwatch(Duration(1 / SPEED_RATE), true);

    // drop
    const double DROP_RATE = 1.5;
    Stopwatch drop_stopwatch = Stopwatch(Duration(1 / DROP_RATE), true);

    // design
    const double ROUND_PROPORTION = 0.1;
    const double FRAME_PROPORTION = 0.03;
    const Color FRAME_COLOR = Palette::Black;
    const double EYE_SIZE_PROPORTION = 0.075;
    const double EYE_HEIGHT_PROPORTION = 0.2;
    const double EYE_MARIGN_PROPORTION = 0.5;
    const Color EYE_COLOR = Palette::Black;

    void update_rect()
    {
        rect.set(pos.x - size / 2, pos.y - size / 2, size, size);
    }

public:
    Monster(Vec2 _pos, double _size, Player _player, Direction _direction)
        : pos(_pos), pre_pos(_pos), nxt_pos(_pos), size(_size), player(_player), direction(_direction), rect(pos.x - size / 2, pos.y - size / 2, size, size){};

    void set_pos(Vec2 _pos) { pos = _pos; update_rect();}

    void set_size(double _size) { size = _size; }

    void update()
    {
        // Print << move_stopwatch.sF() << U" : " << drop_stopwatch.sF();
        if(is_moving())
        {
            double t = Min(move_stopwatch.sF(), 1.0 / SPEED_RATE) * SPEED_RATE;
            t = EaseInOutExpo(t);
            set_pos(pre_pos.lerp(nxt_pos, t));
        }
        else if(is_dropping())
        {
            double t = Min(drop_stopwatch.sF(), 1.0 / DROP_RATE) * DROP_RATE;
            t = EaseOutBounce(t);
            set_pos(pre_pos.lerp(nxt_pos, t));
        }
    }

    void move(Operation op)
    {
        if(!is_moving() && !is_dropping() && op.player == player && (direction + 1) % 2 == op.direction % 2)
        {
            move_stopwatch.restart();
            pre_pos = nxt_pos;
            nxt_pos = pre_pos + size * direction2vec2(op.direction);
        }
    }

    bool is_moving()
    {
        return move_stopwatch.sF() <= 1 / SPEED_RATE;
    }

    void drop(int head)
    {
        if(!is_moving() && !is_dropping())
        {
            drop_stopwatch.restart();
            pre_pos = nxt_pos;
            nxt_pos = pre_pos + size * head * direction2vec2(direction);
        }
    }

    bool is_dropping()
    {
        return drop_stopwatch.sF() <= 1 / DROP_RATE;
    }

    void draw() const
    {
        rect.rounded(size * ROUND_PROPORTION).draw(player2color(player)).drawFrame(size * FRAME_PROPORTION, 0, FRAME_COLOR);
        Circle(pos - size * EYE_HEIGHT_PROPORTION * direction2vec2(direction) + 
                     size * EYE_MARIGN_PROPORTION / 2 * direction2vec2((Direction)((direction + 1) % 4)), 
                     size * EYE_SIZE_PROPORTION).draw(EYE_COLOR);
        Circle(pos - size * EYE_HEIGHT_PROPORTION * direction2vec2(direction) + 
                     size * EYE_MARIGN_PROPORTION / 2 * direction2vec2((Direction)((direction + 3) % 4)), 
                     size * EYE_SIZE_PROPORTION).draw(EYE_COLOR);
    }
};