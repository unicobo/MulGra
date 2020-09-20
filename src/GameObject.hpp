# pragma once
#include "Siv3D.hpp"
#include "Utils.hpp"

typedef enum
{
    Empty = 0,
    Block
} GameObjectId;

class GameObject
{
    Vec2 base_pos; 
    Vector2D<int> pos_in_grid;
    double grid_size;

public:
    GameObject(){};

    GameObject(Vec2 _base_pos , Vector2D<int> _pos_in_grid, double _grid_size)
        : base_pos(_base_pos)
        , pos_in_grid(_pos_in_grid)
        , grid_size(_grid_size)
        {};

    void draw() const
    {
        static Font font(60);
        
        Rect(base_pos.x + grid_size * pos_in_grid.x, base_pos.y + grid_size * pos_in_grid.y, grid_size, grid_size)
            .draw(Palette::White).drawFrame(grid_size * 0.02, 0, Palette::Black);

        font(U"{}:{}"_fmt(pos_in_grid.x, pos_in_grid.y)).drawAt(base_pos.x + grid_size * pos_in_grid.x + grid_size / 2, base_pos.y + grid_size * pos_in_grid.y + grid_size / 2, Palette::Black);
    }

    Vector2D<int> get_pos_in_grid() { return pos_in_grid; }
};