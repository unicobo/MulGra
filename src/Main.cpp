#include <Siv3D.hpp>
#include "ControlPannel.hpp"
#include "Monster.hpp"
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
    Grid<GameObject> obj;

    const Vec2 BASE_POS;
    const Vec2 STAGE_SIZE;
    const int col;
    const int row;
    const double grid_size;

public:
    Stage(Vec2 base_pos, Vec2 stage_size, int col, int row)
        : BASE_POS(base_pos)
        , STAGE_SIZE(stage_size)
        , impl(Grid<int>(col, row))
        , obj(Grid<GameObject>(col, row))
        , col(col)
        , row(row)
        , grid_size(Min(stage_size.y/row, stage_size.x/col))
        {
            for(int i = 0; i < row; i++)for(int j = 0; j < col; j++)obj[i][j] = GameObject(BASE_POS, Vector2D(j, i), grid_size);
        }

    int* operator [](int n) { return impl[n]; }

    bool is_valid_operation();

    void draw() const
    {
        // static Font font(60);
        // double height = (double)Scene::Height()/row;
        // double width = (double)Scene::Width()/col;
        // double size = Min(height, width);

        // for(int i = 0; i < row; i++)
        //     for(int j = 0; j < col; j++)
        //     {
        //         Rect(size * j, size * i, size, size).draw(Palette::White).drawFrame(size * 0.02, 0, Palette::Black);
        //         font(U"{}"_fmt(impl[i][j])).drawAt(size * j + size / 2, size * i + size / 2, Palette::Black);
        //     }

        for(int i = 0; i < row; i++)for(int j = 0; j < col; j++)
            obj[i][j].draw();
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
    Stage result(Vec2(0,0), Vec2(400, 500), col, row);

    for(int i = 0; i < row; i++)
    {
        reader.readLine(line);
        Array<String> ids = line.split(' ');
        for(int j = 0; j < col; j++)result[i][j] = Parse<int>(ids[j]);
    }

    return result;
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
    }

    void draw() const override
    {
        // button.draw();
        // if(button.released())
        //     Print << U"RELEASED!";

        stage.draw();
        pannel.draw();
        if(pannel.get_operation()) Print << pannel.get_operation().value().to_string();
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