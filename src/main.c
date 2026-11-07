#include "raylib.h"
#include "game.h"

int main(void) {
    InitWindow(1100, 700, "CineRush");
    SetTargetFPS(60);

    iniciarJogo();

    CloseWindow();
    return 0;
}