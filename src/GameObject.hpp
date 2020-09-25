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
    GAME_OBJECT
} GameObjectId;

class GameObject
{
protected:
    Vec2 base_pos; 
    Vector2D<int> pos_in_grid;
    double grid_size;
    GameObjectId id;

public:
    GameObject(){};

    GameObject(Vec2 _base_pos , Vector2D<int> _pos_in_grid, double _grid_size, GameObjectId _id = GameObjectId::GAME_OBJECT)
        : base_pos(_base_pos)
        , pos_in_grid(_pos_in_grid)
        , grid_size(_grid_size)
        , id(_id)
        {};

    virtual void draw() const
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
    Empty(Vec2 _base_pos , Vector2D<int> _pos_in_grid, double _grid_size)
        : GameObject(_base_pos, _pos_in_grid, _grid_size, GameObjectId::EMPTY)
        {};
        
    void draw() const {}
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
    Block(Vec2 _base_pos , Vector2D<int> _pos_in_grid, double _grid_size)
        : GameObject(_base_pos, _pos_in_grid, _grid_size, GameObjectId::BLOCK)
        {};
        
    void draw() const
    {
        // Rect(base_pos.x + grid_size * pos_in_grid.x, base_pos.y + grid_size * pos_in_grid.y, grid_size, grid_size)
        //     .draw(BLOCK_COLOR);

        Vec2 pos = Vec2(base_pos.x + grid_size * pos_in_grid.x, base_pos.y + grid_size * pos_in_grid.y);

        Rect(pos.x, pos.y, grid_size, grid_size)
            .rounded(grid_size * ROUND_PROPORTION).draw(BLOCK_COLOR).drawFrame(grid_size * FRAME_PROPORTION, 0, FRAME_COLOR);

        Circle(pos + Vec2(grid_size / 2, grid_size / 2) - grid_size * EYE_HEIGHT_PROPORTION * direction2vec2(Direction::DOWN) + 
                     grid_size * EYE_MARIGN_PROPORTION / 2 * direction2vec2(Direction::RIGHT), 
                     grid_size * EYE_SIZE_PROPORTION).draw(EYE_COLOR);
        Circle(pos + Vec2(grid_size / 2, grid_size / 2) - grid_size * EYE_HEIGHT_PROPORTION * direction2vec2(Direction::DOWN) + 
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

    // design
    const double ROUND_PROPORTION = 0.1;
    const double FRAME_PROPORTION = 0.04;
    const Color FRAME_COLOR = Palette::Black;
    const double EYE_SIZE_PROPORTION = 0.075;
    const double EYE_HEIGHT_PROPORTION = 0.2;
    const double EYE_MARIGN_PROPORTION = 0.5;
    const Color EYE_COLOR = Palette::Black;

public:
    Monster(Vec2 _base_pos , Vector2D<int> _pos_in_grid, double _grid_size, GameObjectId _id)
        : GameObject(_base_pos, _pos_in_grid, _grid_size, _id)
        , pre_pos_in_grid(_pos_in_grid)
        {};

    bool update()
    {
        ;
    }

    void move(Operation op)
    {
        move_stopwatch.restart();
        pre_pos_in_grid = pos_in_grid;
        Direction dir = op.direction;
        pos_in_grid = pre_pos_in_grid + direction2vec2(dir);
    }

    void draw() const
    {
        Vec2 pos = Vec2(base_pos.x + grid_size * pos_in_grid.x, base_pos.y + grid_size * pos_in_grid.y);
        Player player = (Player)(id - GameObjectId::RIGHT_MONSTER);
        Direction direction = (Direction)(id - GameObjectId::RIGHT_MONSTER);

        Rect(pos.x, pos.y, grid_size, grid_size).rounded(grid_size * ROUND_PROPORTION)
            .draw(player2color(player))
            .drawFrame(grid_size * FRAME_PROPORTION, 0, FRAME_COLOR);

        Circle(pos + Vec2(grid_size / 2, grid_size / 2) - grid_size * EYE_HEIGHT_PROPORTION * direction2vec2(direction) + 
                     grid_size * EYE_MARIGN_PROPORTION / 2 * direction2vec2((Direction)((direction + 1) % 4)), 
                     grid_size * EYE_SIZE_PROPORTION).draw(EYE_COLOR);
        Circle(pos + Vec2(grid_size / 2, grid_size / 2) - grid_size * EYE_HEIGHT_PROPORTION * direction2vec2(direction) + 
                     grid_size * EYE_MARIGN_PROPORTION / 2 * direction2vec2((Direction)((direction + 3) % 4)), 
                     grid_size * EYE_SIZE_PROPORTION).draw(EYE_COLOR);
    }
};