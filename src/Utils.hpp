# pragma once
#include "Siv3D.hpp"

double dx[4] = {1, 0, -1, 0};
double dy[4] = {0, 1, 0, -1};

typedef enum
{
    RED,
    BLUE,
    GREEN,
    ORANGE
} Player;

typedef enum
{
    RIGHT,
    DOWN,
    LEFT,
    UP
} Direction;

Color player2color(Player player)
{
    static const Color _player2color[4] = {Palette::Red, Palette::Blue, Palette::Green, Palette::Orange};
    return _player2color[player];
}

Vec2 direction2vec2(Direction direction)
{
    static const Vec2 _direction2vec2[4] = {Vec2(1, 0), Vec2(0, 1), Vec2(-1, 0), Vec2(0, -1)};
    return _direction2vec2[direction];
}

Vector2D<int> direction2vector2d(Direction direction)
{
    static const Vector2D<int> _direction2vector2d[4] = {Vector2D<int>(1, 0), Vector2D<int>(0, 1), Vector2D<int>(-1, 0), Vector2D<int>(0, -1)};
    return _direction2vector2d[direction];
}

struct Operation
{
    Player player;

    Direction direction;

    Operation(Player _player, Direction _direction) : player(_player), direction(_direction) {};

    String to_string()
    {
        static const String player2string[] = {U"RED", U"BLUE", U"GREEN", U"ORANGE"};
        static const String direction2string[] = {U"RIGHT", U"DOWN", U"LEFT", U"UP"};
        return player2string[player] + U" : " + direction2string[direction];
    }
};

bool in(int a, int b, int c)
{
    // a in b..c (including)
    return b <= a && a <= c;
}

bool ine(int a, int b, int c)
{
    // a in b..c (excluding)
    return b < a && a < c;
}