// Character.cpp
#include "Character.h"
#include <android/log.h>

#define LOG_TAG "Character"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

Character::Character() : x(0.0f), y(0.0f) {}

void Character::move(float dx, float dy) {
    x += dx;
    y += dy;
    LOGI("Character moved to position: (%f, %f)", x, y);
}
