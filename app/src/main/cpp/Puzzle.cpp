// Puzzle.cpp
#include "Puzzle.h"
#include <android/log.h>

#define LOG_TAG "Puzzle"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

Puzzle::Puzzle() : solved(false) {}

void Puzzle::solve() {
    solved = true;
    LOGI("Puzzle solved!");
}

bool Puzzle::isSolved() const {
    return solved;
}
