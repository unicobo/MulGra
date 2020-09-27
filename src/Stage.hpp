# pragma once
#include "Siv3D.hpp"
#include "Utils.hpp"
#include "GameObject.hpp"

class Stage
{
    // Grid<int> impl; // TODO: kesu
    Grid<GameObject*> obj;

    const Vec2 BASE_POS;
    const Vec2 STAGE_SIZE;
    int col;
    int row;
    double grid_size;
    Array<Monster*> player_array[4];
    Array<Goal*> goal_array[4]; 

    typedef enum
    {
        READY = 0,
        MOVE,
        DROP,
        CHECK,
        CLEAR
    }State;
    
    State state = State::READY;
    Direction drop_direction;

    void swap_pos(Vector2D<int> pos1, Vector2D<int> pos2)
    {
        GameObject* tmp_obj = obj[pos1.y][pos1.x];
        obj[pos1.y][pos1.x] = obj[pos2.y][pos2.x];
        obj[pos2.y][pos2.x] = tmp_obj;
    }

    bool _move(Vector2D<int> pos, Operation op)
    {
        Direction dir = op.direction;
        if(!in_range(pos)) return false;
        GameObjectId id = get_id(pos);
        if(id == GameObjectId::BLOCK) return false;
        if(id == GameObjectId::EMPTY) return true;
        if(GameObjectId::RIGHT_MONSTER <= id && id <= GameObjectId::UP_MONSTER)
        {
            Vector2D<int> nxt_pos = pos + direction2vector2d(dir);
            if(_move(nxt_pos, op))
            {
                ((Monster*)(obj[pos.y][pos.x]))->move(op);
                swap_pos(pos, nxt_pos);
                return true;
            }
            return false;
        }
        // いつか追加するかも
        Print << U"ERROR";
        return false;
    }

    int _drop(Vector2D<int> pos)
    {
        GameObjectId current_id = get_id(pos);
        if(GameObjectId::RIGHT_MONSTER <= current_id && current_id <= GameObjectId::UP_MONSTER)
        {
            Direction dir = (Direction)(current_id - GameObjectId::RIGHT_MONSTER);
            Vector2D current_pos = pos + direction2vector2d(dir);
            int gap = 0;
            while(in_range(current_pos) && get_id(current_pos) == GameObjectId::EMPTY)
            {
                current_pos += direction2vector2d(dir);
                gap++;
            }
            if(gap)
            {
                ((Monster*)(obj[pos.y][pos.x]))->drop(gap);
                swap_pos(pos, pos + gap * direction2vector2d(dir));
            }
            return gap;
        }
        else return 0;
    }

public:
    Stage(Vec2 base_pos, Vec2 stage_size, Grid<GameObject*> _obj)
        : obj(_obj)
        , BASE_POS(base_pos)
        , STAGE_SIZE(stage_size)
        , col(_obj.width())
        , row(_obj.height())
        , grid_size(Min(stage_size.y/row, stage_size.x/col))
        {
            for(int i = 0; i < row; i++)for(int j = 0; j < col; j++)
            {
                GameObjectId current_id = get_id(Vector2D(j, i));
                if(GameObjectId::RIGHT_MONSTER <= current_id && current_id <= GameObjectId::UP_MONSTER)
                    player_array[current_id - GameObjectId::RIGHT_MONSTER] << (Monster*)obj[i][j];
            }
        }

    Stage(Vec2 _base_pos, Vec2 _stage_size)
        : BASE_POS(_base_pos)
        , STAGE_SIZE(_stage_size)
        {
            row = 0;
            col = 0;
            grid_size = 0;
        }

    void load_stage(int stage_id)
    {    
        String filename = U"../Resources/stages/stage{:0>2}"_fmt(stage_id);

        TextReader reader(filename);

        if(!reader)
        {
            throw Error(U"Failed to open `{}`"_fmt(filename));
        }

        String line;

        reader.readLine(line);
        col = Parse<int>(line);
        reader.readLine(line);
        row = Parse<int>(line);

        grid_size = Min(STAGE_SIZE.y / row, STAGE_SIZE.x / col);

        for(int i = 0; i < 4; i++)player_array[i].clear();

        Grid<GameObject*> new_obj(col, row);
        for(int i = 0; i < row; i++)
        {
            reader.readLine(line);
            Array<String> ids = line.split(' ');
            for(int j = 0; j < col; j++)
            {
                new_obj[i][j] = make_object(BASE_POS, (GameObjectId)Parse<int>(ids[j]), Vector2D<int>(j, i));
                GameObjectId id = new_obj[i][j]->get_id();
                if(GameObjectId::RIGHT_MONSTER <= id && id <= GameObjectId::UP_MONSTER)
                    player_array[id - GameObjectId::RIGHT_MONSTER] << (Monster*)new_obj[i][j];
            }
        }

        obj = new_obj;

        for(int i = 0; i < 4; i++)goal_array[i].clear();

        int goal_num;
        reader.readLine(line);
        goal_num = Parse<int>(line);

        for(int i = 0; i < goal_num; i++)
        {
            reader.readLine(line);
            Array<String> data = line.split(' ');
            GameObjectId id = (GameObjectId)Parse<int>(data[0]);
            Vector2D<int> pos_in_grid = Vector2D<int>(Parse<int>(data[1]), Parse<int>(data[2]));
            goal_array[id - GameObjectId::RIGHT_GOAL] << (Goal*)make_object(BASE_POS, id, pos_in_grid);
        }
    }

    GameObject** operator [](int n) { return obj[n]; }

    GameObject* get_obj(Vector2D<int> pos)
    {
        return obj[pos.y][pos.x];
    }

    GameObjectId get_id(Vector2D<int> pos)
    {
        return get_obj(pos)->get_id();
    }

    bool in_range(Vector2D<int> pos)
    {
        return 0 <= pos.x && pos.x < (int)obj.width() && 0 <= pos.y && pos.y < (int)obj.height();
    }

    bool update()
    {
        if(state == State::CLEAR)
        {
            Print << U"CLEAR";
            return false;
        }

        bool is_active = false;
        for(int i = 0; i < 4; i++)
            for(Monster* e : player_array[i])
            {
                e->update();
                is_active |= e->is_active();
            }
        
        if(is_active)return true;

        switch (state)
        {
        case State::MOVE:
            state = State::DROP;

        case State::DROP:
            {
                for(int i = 0; i <= 4; i++)
                {
                    if(i == 4)
                    {
                        state = State::CHECK;
                        break;
                    }
                    int sum_head = 0;
                    Direction current_direction = (Direction)((drop_direction + i) % 4);
                    for(Monster* e : player_array[current_direction])
                        sum_head += _drop(e->get_pos_in_grid());
                    
                    if(sum_head != 0)
                    {
                        drop_direction = (Direction)((current_direction + 1) % 4);
                        break;
                    }
                }
                if(state != State::CHECK)break;
            }
        case State::CHECK:
            {
                bool is_clear = true;
                for(int i = 0; i < 4; i++)
                {
                    for(Monster* e : player_array[i])
                    {
                        Vector2D<int> pos = e->get_pos_in_grid();
                        bool is_on_goal = false;
                        for(Goal* g : goal_array[i])
                        {
                            if((g->get_pos_in_grid()) == pos)is_on_goal = true;
                        }
                        if(!is_on_goal)is_clear = false;
                    }
                }
                if(is_clear)state = State::CLEAR;
                else state = State::READY;
                if(state != State::CLEAR)break;
            }
        case State::CLEAR:
            {
                Print << U"CLEAR";
                break;
            }
        default:
            break;
        }
    }

    void apply(Operation op)
    {
        if(state != State::READY)return;
        state = State::MOVE;
        Player player = op.player;
        Direction dir = op.direction;
        drop_direction = dir;
        for(Monster* e : player_array[player])
        {
            Vector2D<int> pos = e->get_pos_in_grid();
         
            if(_move(pos, op))
                _drop(pos - direction2vector2d(dir));
        }

        /*
        for(int i = 0; i < row; i++)
        {
            Array<int> array;
            for(int j = 0; j < col; j++)array << impl[i][j];
            Print << array;
        }
        */
    }

    void draw() const
    {
        for(int i = 0; i < row; i++)for(int j = 0; j < col; j++)
            obj[i][j]->draw(grid_size);
        for(int i = 0; i < 4; i++)for(GameObject* e : goal_array[i])
            e->draw(grid_size);
    }
};