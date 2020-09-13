# pragma once
#include "Siv3D.hpp"

double dx[4] = {1, 0, -1, 0};
double dy[4] = {0, 1, 0, -1};

enum Direction
{
    RIGHT,
    DOWN,
    LEFT,
    UP
};

enum Player
{
    RED,
    BLUE,
    GREEN,
    ORANGE
};

struct Operation
{
    int kind;
    int direction;

    Operation() : kind(-1), direction(-1) {};

    Operation(int _kind, int _direction) : kind(_kind), direction(_direction) {};

    Operation(Color _kind, int _direction) : direction(_direction)
    {
        if(_kind == Palette::Red) kind = 0;
        if(_kind == Palette::Blue) kind = 1;
        if(_kind == Palette::Green) kind = 2;
        if(_kind == Palette::Orange) kind = 3;
    }

    bool is_valid_operation() const
    {
        return kind != -1 && direction != -1;
    }

    String to_string()
    {
        String ret = U"";
        switch (kind)
        {
        case 0:
            ret += U"RED";
            break;
        case 1:
            ret += U"BLUE";
            break;
        case 2:
            ret += U"GREEN";
            break;
        case 3:
            ret += U"ORANGE";
            break;
        default:
            ret += U"Invalid Operation";
            return ret;
            break;
        }

        ret += U" : ";

        switch (direction)
        {
        case 0:
            ret += U"RIGHT";
            break;
        case 1:
            ret += U"DOWN";
            break;
        case 2:
            ret += U"LEFT";
            break;
        case 3:
            ret += U"UP";
            break;
        default:
            ret += U"No Operation";
            break;
        }

        return ret;
    }
};
