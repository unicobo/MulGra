#include <Siv3D.hpp>
#include "ControlPannel.hpp"
// #include "Monster.hpp"
#include "GameObject.hpp"

const String team_name = U"unicobo";

enum Scenes
{
    STitle,
    SMenu,
    SGame,
};

struct GameData
{
    int32 score = 0;
    int32 n_stage = 0;
};

using App = SceneManager<Scenes, GameData>;

// タイトルシーン
class Title : public App::Scene
{
public:
    Title(const InitData &init)
        : IScene(init)
    {
    }

    void update() override
    {
        if (MouseL.down())
        {
            changeScene(SMenu, 0.1);
        }
    }

    void draw() const override
    {
        Scene::SetBackground(ColorF(0.3, 0.4, 0.5));

        FontAsset(U"TitleFont")(U"MulGra").drawAt(400, 100);

        FontAsset(U"ScoreFont")(U"Made by: {}"_fmt(team_name)).draw(520, 540);

        Circle(Cursor::Pos(), 50).draw(Palette::Orange);
    }
};

class Menu : public App::Scene
{
private:
    bool game_select;

public:
    Menu(const InitData &init)
        : IScene(init), game_select(false)
    {
    }

    void update() override
    {
        const Point center = Scene::Center();

        if (game_select)
        {
            double height = Scene::Height()/4;
            double width = Scene::Width()/4;
            for (int i = 1; i < 4; i++)
            {
                for (int j = 1; j < 4; j++)
                {
                    if(SimpleGUI::ButtonAt(U"{}"_fmt((i-1)*3 + j), Vec2(width*j, height*i + 50)))
                    {
                        getData().n_stage = (i-1)*3 + j;
                        changeScene(SGame);
                    }
                }
            }
        }
        else
        {
            if (SimpleGUI::ButtonAt(U"Stage Select", Vec2(center.x, center.y - 50)))
            {
                game_select = true;
            }

            if (SimpleGUI::ButtonAt(U"Back To Title", Vec2(center.x, center.y + 50)))
            {
                changeScene(STitle);
            }
        }
    }

    void draw() const override
    {
        FontAsset(U"TitleFont")(U"MulGra").drawAt(400, 100);
    }
};

class Stage
{
    Grid<int> impl;
    Grid<GameObject*> obj;

    const Vec2 BASE_POS;
    const Vec2 STAGE_SIZE;
    const int col;
    const int row;
    const double grid_size;
    Array<Monster*> player_array[4];

    typedef enum
    {
        READY = 0,
        MOVE,
        DROP
    }State;
    
    State state = State::READY;
    Direction drop_direction;

    GameObject* make_object(GameObjectId id, Vector2D<int> pos_in_grid)
    {
        switch (id)
        {
        case GameObjectId::EMPTY:
            return new Empty(BASE_POS, pos_in_grid, grid_size);
            break;
        case GameObjectId::BLOCK:
            return new Block(BASE_POS, pos_in_grid, grid_size);
            break;
        case GameObjectId::RIGHT_MONSTER:
            return new Monster(BASE_POS, pos_in_grid, grid_size, id);
            break;
        case GameObjectId::DOWN_MONSTER:
            return new Monster(BASE_POS, pos_in_grid, grid_size, id);
            break;
        case GameObjectId::LEFT_MONSTER:
            return new Monster(BASE_POS, pos_in_grid, grid_size, id);
            break;
        case GameObjectId::UP_MONSTER:
            return new Monster(BASE_POS, pos_in_grid, grid_size, id);
            break;
        default:
            return new GameObject(BASE_POS, pos_in_grid, grid_size);
            break;
        }
    }


void swap_pos(Vector2D<int> pos1, Vector2D<int> pos2)
    {
        int tmp_impl = impl[pos1.y][pos1.x];
        impl[pos1.y][pos1.x] = impl[pos2.y][pos2.x];
        impl[pos2.y][pos2.x] = tmp_impl;
        GameObject* tmp_obj = obj[pos1.y][pos1.x];
        obj[pos1.y][pos1.x] = obj[pos2.y][pos2.x];
        obj[pos2.y][pos2.x] = tmp_obj;
    }

    bool _move(Vector2D<int> pos, Operation op)
    {
        Direction dir = op.direction;
        if(!in_range(pos)) return false;
        GameObjectId id = (GameObjectId)(impl[pos.y][pos.x]);
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
        if(GameObjectId::RIGHT_MONSTER <= impl[pos.y][pos.x] && impl[pos.y][pos.x] <= GameObjectId::UP_MONSTER)
        {
            Direction dir = (Direction)(impl[pos.y][pos.x] - GameObjectId::RIGHT_MONSTER);
            Vector2D current_pos = pos + direction2vector2d(dir);
            int gap = 0;
            while(in_range(current_pos) && impl[current_pos.y][current_pos.x] == GameObjectId::EMPTY)
            {
                current_pos += direction2vector2d(dir);
                gap++;
            }
            ((Monster*)(obj[pos.y][pos.x]))->drop(gap);
            swap_pos(pos, pos + gap * direction2vector2d(dir));
            return gap;
        }
        else return 0;
    }

public:
    Stage(Vec2 base_pos, Vec2 stage_size, Grid<int> _impl)
        : impl(_impl)
        , obj(Grid<GameObject*>(_impl.width(), _impl.height()))
        , BASE_POS(base_pos)
        , STAGE_SIZE(stage_size)
        , col(_impl.width())
        , row(_impl.height())
        , grid_size(Min(stage_size.y/row, stage_size.x/col))
        {
            for(int i = 0; i < row; i++)for(int j = 0; j < col; j++)
            {
                obj[i][j] = make_object((GameObjectId)impl[i][j], Vector2D<int>(j, i));
                if(GameObjectId::RIGHT_MONSTER <= impl[i][j] && impl[i][j] <= GameObjectId::UP_MONSTER)
                    player_array[(int)(impl[i][j] - GameObjectId::RIGHT_MONSTER)] << (Monster*)obj[i][j];
            }
        }

    int* operator [](int n) { return impl[n]; }

    bool is_valid_operation();

    bool in_range(Vector2D<int> pos)
    {
        return 0 <= pos.x && pos.x < (int)impl.width() && 0 <= pos.y && pos.y < (int)impl.height();
    }

    bool update()
    {
        // Print << state << U" : " << Time::GetSec();
        bool updated = false;
        for(int i = 0; i < 4; i++)
            for(Monster* e : player_array[i])
                updated |= e->update();

        if(!updated && state == State::MOVE)state = State::DROP;
        
        if(!updated && state == State::DROP)
        {
            for(int i = 0; i <= 4; i++)
            {
                if(i == 4)
                {
                    state = State::READY;
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
        }
    }

    void apply(Operation op)
    {
        if(state != State::READY)return;
        Player player = op.player;
        Direction dir = op.direction;
        state = State::MOVE;
        drop_direction = dir;
        for(Monster* e : player_array[player])
        {
            Vector2D<int> pos = e->get_pos_in_grid();
         
            if(_move(pos, op))
                _drop(pos - direction2vector2d(dir));
        }
    }

    void draw() const
    {
        for(int i = 0; i < row; i++)for(int j = 0; j < col; j++)
            obj[i][j]->draw();
    }
};


Stage loadStage(String filename)
{
    TextReader reader(filename);

    if (!reader)
    {
        throw Error(U"Failed to open `{}`"_fmt(filename));
    }

    String line;

    reader.readLine(line);
    int col = Parse<int>(line);
    reader.readLine(line);
    int row = Parse<int>(line);
    // Stage result(Vec2(20,20), Vec2(400, 500), col, row);

    Grid<int> impl(col, row);
    for(int i = 0; i < row; i++)
    {
        reader.readLine(line);
        Array<String> ids = line.split(' ');
        for(int j = 0; j < col; j++)impl[i][j] = Parse<int>(ids[j]);
    }

    return Stage(Vec2(20, 20), Vec2(400, 500), impl);
}

class Game : public App::Scene
{
private:
    ControlPannel pannel;
    Stage stage;

public:
    Game(const InitData &init)
        : IScene(init), pannel(600, 400, 100), stage(loadStage(U"../Resources/stages/stage01"))
    {
    }

    void update() override
    {
        if(KeyT.pressed())
        {
            // test
        }
        std::optional<Operation> op = pannel.get_operation();
        stage.update();
        if(op)stage.apply(op.value());
    }

    void draw() const override
    {
        // button.draw();
        // if(button.released())
        //     Print << U"RELEASED!";

        stage.draw();
        pannel.draw();
        std::optional<Operation> op = pannel.get_operation();
        if(op) 
        {
            Print << op.value().to_string();
        }
    }
};

void Main()
{
    FontAsset::Register(U"TitleFont", 60, Typeface::Heavy);
    FontAsset::Register(U"ScoreFont", 30, Typeface::Bold);

    // シーンマネージャーを作成
    // ここで GameData も初期化される
    App manager;

    manager.add<Title>(STitle);
    manager.add<Menu>(SMenu);
    manager.add<Game>(SGame);

    while (System::Update())
    {
        // 現在のシーンを実行
        if (!manager.update())
        {
            break;
        }
    }
}