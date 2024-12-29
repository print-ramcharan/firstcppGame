// Puzzle.h
#pragma once

class Puzzle {
public:
    Puzzle();
    void solve();
    bool isSolved() const;

private:
    bool solved;
};
