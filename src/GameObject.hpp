# pragma once
#include "Siv3D.hpp"
#include "Utils.hpp"

typedef enum
{
    EMPTY = 0,
    BLOCK,
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
        static Font font(60);
        
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