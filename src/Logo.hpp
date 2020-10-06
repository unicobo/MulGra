# pragma once
#include "Siv3D.hpp"
#include "Utils.hpp"
#include "GameObject.hpp"

class WhiteBlock : public GameObject
{

    const Color BLOCK_COLOR = Palette::White;
    const double ROUND_PROPORTION = 0.1;
    const double FRAME_PROPORTION = 0.02;
    const Color FRAME_COLOR = Palette::Black;
    const double EYE_SIZE_PROPORTION = 0.075;
    const double EYE_HEIGHT_PROPORTION = 0.2;
    const double EYE_MARIGN_PROPORTION = 0.5;
    const Color EYE_COLOR = Palette::Black;

public:
    WhiteBlock(Vec2 _base_pos , Vector2D<int> _pos_in_grid)
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

class Logo
{
    const Vec2 outer_pos;
    const Vec2 outer_size;
    Vec2 pos;
    Grid<GameObject*> grid = Grid<GameObject*>(30, 9);
    double grid_size;

    int fid[9][30] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                      {0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                      {0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 2, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                      {0, 1, 1, 1, 1, 1, 0, 2, 0, 2, 0, 0, 2, 0, 0, 3, 0, 0, 0, 0, 0, 4, 4, 4, 0, 4, 4, 4, 0, 0},
                      {0, 1, 0, 1, 0, 1, 0, 2, 0, 2, 0, 0, 2, 0, 0, 3, 0, 0, 3, 3, 0, 4, 0, 4, 0, 0, 0, 4, 0, 0},
                      {0, 1, 0, 0, 0, 1, 0, 2, 0, 2, 0, 0, 2, 0, 0, 3, 0, 0, 0, 3, 0, 4, 0, 0, 0, 4, 4, 4, 0, 0},
                      {0, 1, 0, 0, 0, 1, 0, 2, 0, 2, 0, 0, 2, 0, 0, 3, 3, 0, 0, 3, 0, 4, 0, 0, 0, 4, 0, 4, 0, 0},
                      {0, 1, 0, 0, 0, 1, 0, 2, 2, 2, 2, 0, 2, 2, 0, 0, 3, 3, 3, 3, 0, 4, 0, 0, 0, 4, 4, 4, 4, 0},
                      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

public:
    Logo(Vec2 _outer_pos, Vec2 _outer_size) 
        : outer_pos(_outer_pos)
        , outer_size(_outer_size)
        {
            grid_size = Min(_outer_size.x / grid.width(), _outer_size.y / grid.height());
            Vec2 center_pos = _outer_pos + _outer_size / 2;
            pos = Vec2(center_pos.x - grid_size * grid.width() / 2, center_pos.y - grid_size * grid.height() / 2);

            for(int i = 0; i < grid.height(); i++)for(int j = 0; j < grid.width(); j++)
            {
                switch (fid[i][j])
                {
                case 0:
                    // grid[i][j] = new WhiteBlock(pos, Vec2(j, i));
                    grid[i][j] = new Empty(pos, Vec2(j, i));
                    break;
                default:
                    // grid[i][j] = new Monster(pos, Vec2(j, i), (GameObjectId)(GameObjectId::RIGHT_MONSTER + Random<int>(3)));
                    grid[i][j] = new Monster(pos, Vec2(j, i), (GameObjectId)(GameObjectId::RIGHT_MONSTER + fid[i][j] - 1));
                    break;
                }
            }

        }

    void draw() const
    {
        for(int i = 0; i < grid.height(); i++)for(int j = 0; j < grid.width(); j++)grid[i][j]->draw(grid_size);
    }
    
};