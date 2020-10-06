# pragma once
#include "Siv3D.hpp"
#include "Utils.hpp"

typedef enum
{
    EMPTY = 0,
    BLOCK,
    RIGHT_MONSTER,
    DOWN_MONSTER,
    LEFT_MONSTER,
    UP_MONSTER,
    RIGHT_GOAL,
    DOWN_GOAL,
    LEFT_GOAL,
    UP_GOAL,
    RIGHT_TRANSBLOCK,
    DOWN_TRANSBLOCK,
    LEFT_TRANSBLOCK,
    UP_TRANSBLOCK,
    GAME_OBJECT
} GameObjectId;

class GameObject
{
protected:
    const Vec2 base_pos;
    Vector2D<int> pos_in_grid;
    GameObjectId id;

    // feature
    // const bool movable;

public:
    GameObject()
        : base_pos(Vec2(0, 0))
        {};

    GameObject(Vec2 _base_pos , Vector2D<int> _pos_in_grid, GameObjectId _id = GameObjectId::GAME_OBJECT)
        : base_pos(_base_pos)
        , pos_in_grid(_pos_in_grid)
        , id(_id)
        {};

    virtual void draw(double grid_size) const
    {
        static Font font(grid_size / 5);
        
        Rect(base_pos.x + grid_size * pos_in_grid.x, base_pos.y + grid_size * pos_in_grid.y, grid_size, grid_size)
            .draw(Palette::White).drawFrame(grid_size * 0.02, 0, Palette::Black);

        font(U"{}:{}"_fmt(pos_in_grid.x, pos_in_grid.y)).drawAt(base_pos.x + grid_size * pos_in_grid.x + grid_size / 2, base_pos.y + grid_size * pos_in_grid.y + grid_size / 2, Palette::Black);
    }

    GameObjectId get_id() { return id; }

    Vector2D<int> get_pos_in_grid() { return pos_in_grid; }
};

class Empty : public GameObject
{
public:
    Empty(Vec2 _base_pos , Vector2D<int> _pos_in_grid)
        : GameObject(_base_pos, _pos_in_grid, GameObjectId::EMPTY)
        {};
        
    void draw(double grid_size) const {}
};

class Block : public GameObject
{

    const Color BLOCK_COLOR = Palette::Black;
    const double ROUND_PROPORTION = 0.1;
    const double FRAME_PROPORTION = 0.02;
    const Color FRAME_COLOR = Palette::White;
    const double EYE_SIZE_PROPORTION = 0.075;
    const double EYE_HEIGHT_PROPORTION = 0.2;
    const double EYE_MARIGN_PROPORTION = 0.5;
    const Color EYE_COLOR = Palette::White;

public:
    Block(Vec2 _base_pos , Vector2D<int> _pos_in_grid)
        : GameObject(_base_pos, _pos_in_grid, GameObjectId::BLOCK)
        {};
        
    void draw(double grid_size) const
    {
        // Rect(base_pos.x + grid_size * pos_in_grid.x, base_pos.y + grid_size * pos_in_grid.y, grid_size, grid_size)
        //     .draw(BLOCK_COLOR);

        Vec2 draw_pos = Vec2(base_pos.x + grid_size * pos_in_grid.x, base_pos.y + grid_size * pos_in_grid.y);

        Rect(draw_pos.x, draw_pos.y, grid_size, grid_size)
            .rounded(grid_size * ROUND_PROPORTION).draw(BLOCK_COLOR).drawFrame(grid_size * FRAME_PROPORTION, 0, FRAME_COLOR);

        Circle(draw_pos + Vec2(grid_size / 2, grid_size / 2) - grid_size * EYE_HEIGHT_PROPORTION * direction2vec2(Direction::DOWN) + 
                     grid_size * EYE_MARIGN_PROPORTION / 2 * direction2vec2(Direction::RIGHT), 
                     grid_size * EYE_SIZE_PROPORTION).draw(EYE_COLOR);
        Circle(draw_pos + Vec2(grid_size / 2, grid_size / 2) - grid_size * EYE_HEIGHT_PROPORTION * direction2vec2(Direction::DOWN) + 
                     grid_size * EYE_MARIGN_PROPORTION / 2 * direction2vec2(Direction::LEFT), 
                     grid_size * EYE_SIZE_PROPORTION).draw(EYE_COLOR);
    }
};

class Monster : public GameObject
{

    Vector2D<int> pre_pos_in_grid;
    Vec2 pos;

    // move
    const double MOVE_SPEED_RATE = 2;
    Stopwatch move_stopwatch = Stopwatch(Duration(1 / MOVE_SPEED_RATE), true);

    // drop
    const double DROP_SPEED_RATE = 2;
    Stopwatch drop_stopwatch = Stopwatch(Duration(1 / DROP_SPEED_RATE), true);

    // design
    const double ROUND_PROPORTION = 0.1;
    const double FRAME_PROPORTION = 0.04;
    const Color FRAME_COLOR = Palette::Black;
    const double EYE_SIZE_PROPORTION = 0.075;
    const double EYE_HEIGHT_PROPORTION = 0.2;
    const double EYE_MARIGN_PROPORTION = 0.5;
    const Color EYE_COLOR = Palette::Black;

public:
    Monster(Vec2 _base_pos , Vector2D<int> _pos_in_grid, GameObjectId _id)
        : GameObject(_base_pos, _pos_in_grid, _id)
        , pre_pos_in_grid(_pos_in_grid)
        , pos(_pos_in_grid.x, _pos_in_grid.y)
        {};

    bool update()
    {
        if(move_stopwatch.isPaused())move_stopwatch.resume();
        if(drop_stopwatch.isPaused())drop_stopwatch.resume();

        if(is_moving())
        {
            double t = Min(move_stopwatch.sF(), 1.0 / MOVE_SPEED_RATE) * MOVE_SPEED_RATE;
            t = EaseOutExpo(t);
            Vec2 pre_pos = pos;
            Vec2 _pre_pos_in_grid(pre_pos_in_grid.x, pre_pos_in_grid.y);
            Vec2 _pos_in_grid(pos_in_grid.x, pos_in_grid.y);
            pos = _pre_pos_in_grid.lerp(_pos_in_grid, t);
            return pos != pre_pos;
        }
        else if(is_dropping())
        {
            double t = Min(drop_stopwatch.sF(), 1.0 / DROP_SPEED_RATE) * DROP_SPEED_RATE;
            // t = EaseInOutExpo(t);
            t = EaseOutBounce(t);
            Vec2 pre_pos = pos;
            Vec2 _pre_pos_in_grid(pre_pos_in_grid.x, pre_pos_in_grid.y);
            Vec2 _pos_in_grid(pos_in_grid.x, pos_in_grid.y);
            pos = _pre_pos_in_grid.lerp(_pos_in_grid, t);
            return pos != pre_pos;
        }
    }

    void pause() {move_stopwatch.pause(); drop_stopwatch.pause();}

    void move(Operation op)
    {
        if(!is_active())
        {
            move_stopwatch.restart();
            pre_pos_in_grid = pos_in_grid;
            Direction dir = op.direction;
            pos_in_grid = pre_pos_in_grid + direction2vec2(dir);
        }
    }

    bool is_moving()
    {
        return move_stopwatch.sF() <= 1 / MOVE_SPEED_RATE;
    }

    void drop(int gap)
    {
        if(gap && !is_active())
        {
            drop_stopwatch.restart();
            pre_pos_in_grid = pos_in_grid;
            Direction dir = (Direction)(id - GameObjectId::RIGHT_MONSTER);
            pos_in_grid = pre_pos_in_grid + gap * direction2vec2(dir);
        }
    }

    bool is_dropping()
    {
        return drop_stopwatch.sF() <= 1 / DROP_SPEED_RATE;
    }

    bool is_active()
    {
        return is_moving() || is_dropping();
    }

    void draw(double grid_size) const
    {
        Vec2 draw_pos = Vec2(base_pos.x + grid_size * pos.x, base_pos.y + grid_size * pos.y);
        Player player = (Player)(id - GameObjectId::RIGHT_MONSTER);
        Direction direction = (Direction)(id - GameObjectId::RIGHT_MONSTER);

        Rect(draw_pos.x, draw_pos.y, grid_size, grid_size).rounded(grid_size * ROUND_PROPORTION)
            .draw(player2color(player))
            .drawFrame(grid_size * FRAME_PROPORTION, 0, FRAME_COLOR);

        Circle(draw_pos + Vec2(grid_size / 2, grid_size / 2) - grid_size * EYE_HEIGHT_PROPORTION * direction2vec2(direction) + 
                     grid_size * EYE_MARIGN_PROPORTION / 2 * direction2vec2((Direction)((direction + 1) % 4)), 
                     grid_size * EYE_SIZE_PROPORTION).draw(EYE_COLOR);
        Circle(draw_pos + Vec2(grid_size / 2, grid_size / 2) - grid_size * EYE_HEIGHT_PROPORTION * direction2vec2(direction) + 
                     grid_size * EYE_MARIGN_PROPORTION / 2 * direction2vec2((Direction)((direction + 3) % 4)), 
                     grid_size * EYE_SIZE_PROPORTION).draw(EYE_COLOR);
    }
};

class Goal : public GameObject
{
    const double GOAL_SIZE_PROPORTION = 0.8;
    const double FRAME_PROPORTION = 0.1;
    const int A = 150;
public:
    Goal(Vec2 _base_pos , Vector2D<int> _pos_in_grid, GameObjectId _id)
        : GameObject(_base_pos, _pos_in_grid, _id)
        {}

    void draw(double grid_size) const
    {
        Vec2 draw_pos = Vec2(base_pos.x + grid_size * pos_in_grid.x, base_pos.y + grid_size * pos_in_grid.y) + grid_size * (1 - GOAL_SIZE_PROPORTION) / 2 * Vec2(1, 1);

        Rect(draw_pos.x, draw_pos.y, grid_size * GOAL_SIZE_PROPORTION, grid_size * GOAL_SIZE_PROPORTION)
            .drawFrame(grid_size * FRAME_PROPORTION, 0, Color(player2color((Player)(id - GameObjectId::RIGHT_GOAL)), A));
    }
};

class TransBlock : public GameObject
{
    // design
    const double ROUND_PROPORTION = 0.1;
    const double FRAME_PROPORTION = 0.04;
    const Color FRAME_COLOR = Palette::Black;
    const double EYE_SIZE_PROPORTION = 0.075;
    const double EYE_HEIGHT_PROPORTION = 0.2;
    const double EYE_MARIGN_PROPORTION = 0.5;
    const Color EYE_COLOR = Palette::Black;
public:
    TransBlock(Vec2 _base_pos , Vector2D<int> _pos_in_grid, GameObjectId _id)
        : GameObject(_base_pos, _pos_in_grid, _id)
        {}

    void draw(double grid_size) const
    {
        Vec2 draw_pos = Vec2(base_pos.x + grid_size * pos_in_grid.x, base_pos.y + grid_size * pos_in_grid.y);
        Player player = (Player)(id - GameObjectId::RIGHT_TRANSBLOCK);
        Direction direction = (Direction)(id - GameObjectId::RIGHT_TRANSBLOCK);

        {
            ColorF mul(1, 1, 1, 0.4);
            const ScopedColorMul2D state(mul);
            Rect(draw_pos.x, draw_pos.y, grid_size, grid_size).rounded(grid_size * ROUND_PROPORTION)
                .draw(player2color(player))
                .drawFrame(grid_size * FRAME_PROPORTION, 0, FRAME_COLOR);

            Circle(draw_pos + Vec2(grid_size / 2, grid_size / 2) - grid_size * EYE_HEIGHT_PROPORTION * direction2vec2(direction) + 
                        grid_size * EYE_MARIGN_PROPORTION / 2 * direction2vec2((Direction)((direction + 1) % 4)), 
                        grid_size * EYE_SIZE_PROPORTION).draw(EYE_COLOR);
            Circle(draw_pos + Vec2(grid_size / 2, grid_size / 2) - grid_size * EYE_HEIGHT_PROPORTION * direction2vec2(direction) + 
                        grid_size * EYE_MARIGN_PROPORTION / 2 * direction2vec2((Direction)((direction + 3) % 4)), 
                        grid_size * EYE_SIZE_PROPORTION).draw(EYE_COLOR);
        }
    }
};

GameObject* make_object(Vec2 base_pos, GameObjectId id, Vector2D<int> pos_in_grid)
{
    switch (id)
    {
    case GameObjectId::EMPTY:
        return new Empty(base_pos, pos_in_grid);
    case GameObjectId::BLOCK:
        return new Block(base_pos, pos_in_grid);
    case GameObjectId::RIGHT_MONSTER:
    case GameObjectId::DOWN_MONSTER:
    case GameObjectId::LEFT_MONSTER:
    case GameObjectId::UP_MONSTER:
        return new Monster(base_pos, pos_in_grid, id);
    case GameObjectId::RIGHT_GOAL:
    case GameObjectId::DOWN_GOAL:
    case GameObjectId::LEFT_GOAL:
    case GameObjectId::UP_GOAL:
        return new Goal(base_pos, pos_in_grid, id);
    case GameObjectId::RIGHT_TRANSBLOCK:
    case GameObjectId::DOWN_TRANSBLOCK:
    case GameObjectId::LEFT_TRANSBLOCK:
    case GameObjectId::UP_TRANSBLOCK:
        return new TransBlock(base_pos, pos_in_grid, id);
    default:
        return new GameObject(base_pos, pos_in_grid);
        break;
    }
}