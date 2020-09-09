#include <Siv3D.hpp>
#include "ControlButton.hpp"

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

using Stage = Grid<int>;

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
    Stage result(col, row);

    for(int i = 0; i < row; i++)
    {
        for(int j = 0; j < col; j++)
        {
            reader.readLine(line);
            result[i][j] = Parse<int>(line);
        }
    }
    return result;
}

class Game : public App::Scene
{
private:
    ControlButton button;
public:
    Game(const InitData &init)
        : IScene(init), button(400, 400, 100, Palette::Red, 0)
    {
    }

    void update() override
    {
    }

    void draw() const override
    {
<<<<<<< HEAD
        button.draw();
        if(button.released())
            Print << U"RELEASED!";
=======
        Scene::SetBackground(ColorF(0.2, 0.8, 0.6));

        m_texture.drawAt(Cursor::Pos());

        // 現在のスコアを表示
        FontAsset(U"ScoreFont")(U"Score: {}"_fmt(getData().score)).draw(40, 40);
        FontAsset(U"ScoreFont")(U"Stage: {}"_fmt(getData().n_stage)).draw(40, 60);
>>>>>>> develop
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