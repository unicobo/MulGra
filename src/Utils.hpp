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
