#include <Siv3D.hpp>
#include "ControlPannel.hpp"
// #include "Monster.hpp"
// #include "GameObject.hpp"
#include "Stage.hpp"

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

class Game : public App::Scene
{
private:
    ControlPannel pannel;
    Stage stage;

    ColorF mul;
    bool is_pause = false;

public:
    Game(const InitData &init)
        : IScene(init), pannel(600, 400, 300), stage(Vec2(20, 20), Vec2(400, 500)), mul(0.3, 0.3, 0.3)
    {
        int n_stage = getData().n_stage;
        if(stage.load_stage(n_stage))
        {
            Print << U"Not implemented yet!";
            stage.state = Stage::State::CLEAR;
        }
    }

    void update() override
    {
        if(KeyT.pressed())
        {
            // test
        }
        if(KeyP.down())
        {
            is_pause = !is_pause;
            if(is_pause)stage.pause();
        }

        // usual
        if (!is_pause)
        {
            // reload
            if(KeyR.pressed())
                stage.load_stage(1);

            // update and apply
            Optional<Operation> op = pannel.get_operation();
            if(op)stage.apply(op.value());
            stage.update();

            // clear
            if (stage.is_game_clear())
            {
                if (SimpleGUI::ButtonAt(U"Back to the title", Scene::Center()))
                {
                    changeScene(STitle);
                }
            }
        }
        // pause
        else if(SimpleGUI::ButtonAt(U"Back to the title", Scene::Center()))
        {
            changeScene(STitle);
        }
        
    }

    void draw() const override
    {
        // button.draw();
        // if(button.released())
        //     Print << U"RELEASED!";

        if (is_pause) {
            {
                const ScopedColorMul2D state(mul);

                stage.draw();
                pannel.draw();
            }

            SimpleGUI::ButtonAt(U"Back to the title", Scene::Center());
        } else {
            stage.draw();
            pannel.draw();

            if(stage.is_game_clear())
                SimpleGUI::ButtonAt(U"Back to the title", Scene::Center());
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