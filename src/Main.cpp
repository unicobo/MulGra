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
    // Grid<int> impl; // TODO: kesu
    Grid<GameObject*> obj;

    const Vec2 BASE_POS;
    const Vec2 STAGE_SIZE;
    int col;
    int row;
    double grid_size;
    Array<Monster*> player_array[4];

    typedef enum
    {
        READY = 0,
        MOVE,
        DROP
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
        bool is_active = false;
        for(int i = 0; i < 4; i++)
            for(Monster* e : player_array[i])
            {
                e->update();
                is_active |= e->is_active();
            }

        if(!is_active && state == State::MOVE)state = State::DROP;
        
        if(!is_active && state == State::DROP)
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
    }
};

class Game : public App::Scene
{
private:
    ControlPannel pannel;
    Stage stage;

public:
    Game(const InitData &init)
        : IScene(init), pannel(600, 400, 100), stage(Vec2(20, 20), Vec2(400, 500))
    {
        stage.load_stage(1);
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