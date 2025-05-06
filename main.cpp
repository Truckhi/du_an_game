#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <sstream>
#include <cmath>

void PlaceMines();
void CalculateNumbers();
void RevealCell(int x, int y);
bool CheckWin();
void DrawBoard();
void DrawMenu();
void HandleClick(int x, int y, bool rightClick);
void ResetGame();

const int SIZE = 10;
const int CELL_SIZE = 40;
const int MINES = 15;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;

struct Cell {
    bool isMine = false;
    bool isRevealed = false;
    bool isFlagged = false;
    int adjacentMines = 0;
};

Cell board[SIZE][SIZE];

enum GameState {
    MENU,
    GAME
};

GameState currentState = MENU;

void InitSDL() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    window = SDL_CreateWindow("DO MIN", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SIZE * CELL_SIZE, SIZE * CELL_SIZE + 50, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    font = TTF_OpenFont("NotoSans-Regular.ttf", 24);

    if (!font) {
        std::cout << "Khong the mo font: " << TTF_GetError() << std::endl;
    }
}

void QuitSDL() {
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

void ResetGame() {
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            board[y][x] = Cell();
        }
    }
    PlaceMines();
    CalculateNumbers();
    currentState = GAME;
}

void PlaceMines() {
    srand(static_cast<unsigned int>(time(0)));
    int placed = 0;
    while (placed < MINES) {
        int x = rand() % SIZE;
        int y = rand() % SIZE;
        if (!board[y][x].isMine) {
            board[y][x].isMine = true;
            placed++;
        }
    }
}

void CalculateNumbers() {
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            if (board[y][x].isMine) continue;

            int count = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int nx = x + dx, ny = y + dy;
                    if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE && board[ny][nx].isMine) {
                        count++;
                    }
                }
            }
            board[y][x].adjacentMines = count;
        }
    }
}

void RevealCell(int x, int y) {
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE || board[y][x].isRevealed || board[y][x].isFlagged) return;

    board[y][x].isRevealed = true;

    if (board[y][x].adjacentMines == 0 && !board[y][x].isMine) {
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                RevealCell(x + dx, y + dy);
            }
        }
    }
}

bool CheckWin() {
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            if (!board[y][x].isMine && !board[y][x].isRevealed) {
                return false;
            }
        }
    }
    return true;
}

void DrawBoard() {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            SDL_Rect cellRect = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE };

            if (board[y][x].isRevealed) {
                SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
                if (board[y][x].isMine) {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                }
            }
            else {
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                if (board[y][x].isFlagged) {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                }
            }

            SDL_RenderFillRect(renderer, &cellRect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &cellRect);

            if (board[y][x].isRevealed && !board[y][x].isMine) {
                SDL_Color textColor = { 0, 0, 255 };
                std::ostringstream ss;
                ss << (board[y][x].adjacentMines == 0 ? 1 : board[y][x].adjacentMines);
                std::string text = ss.str();

                SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

                int textW, textH;
                SDL_QueryTexture(texture, NULL, NULL, &textW, &textH);
                SDL_Rect textRect = { x * CELL_SIZE + (CELL_SIZE - textW) / 2, y * CELL_SIZE + (CELL_SIZE - textH) / 2, textW, textH };

                SDL_RenderCopy(renderer, texture, NULL, &textRect);
                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
            }
        }
    }

    // Vẽ nút quay lại hình tròn và mũi tên cong
    int cx = 25;
    int cy = SIZE * CELL_SIZE + 25;
    int radius = 20;
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if (dx * dx + dy * dy <= radius * radius) {
                SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
                SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
            }
        }
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int angle = 180; angle <= 320; angle += 5) {
        double rad = angle * M_PI / 180.0;
        int x = (int)(cx + cos(rad) * (radius - 8));
        int y = (int)(cy + sin(rad) * (radius - 8));
        SDL_RenderDrawPoint(renderer, x, y);
    }
    SDL_RenderDrawLine(renderer, cx - 10, cy - 5, cx - 5, cy - 12);
    SDL_RenderDrawLine(renderer, cx - 10, cy - 5, cx - 2, cy - 4);

    SDL_RenderPresent(renderer);
}

void DrawMenu() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Color textColor = { 0, 0, 0 };
    std::string startText = "Click to Start Game";

    SDL_Surface* surface = TTF_RenderText_Solid(font, startText.c_str(), textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    int textW, textH;
    SDL_QueryTexture(texture, NULL, NULL, &textW, &textH);
    SDL_Rect textRect = { (SIZE * CELL_SIZE - textW) / 2, (SIZE * CELL_SIZE - textH) / 2, textW, textH };

    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    SDL_RenderPresent(renderer);
}

void HandleClick(int x, int y, bool rightClick) {
    int cellX = x / CELL_SIZE;
    int cellY = y / CELL_SIZE;

    if (currentState == MENU) {
        currentState = GAME;
        ResetGame();
        return;
    }

    if (currentState == GAME && y >= SIZE * CELL_SIZE) {
        int cx = 25, cy = SIZE * CELL_SIZE + 25, radius = 20;
        int dx = x - cx, dy = y - cy;
        if (dx * dx + dy * dy <= radius * radius) {
            ResetGame();
            return;
        }
    }

    if (cellX < 0 || cellX >= SIZE || cellY < 0 || cellY >= SIZE) return;

    if (rightClick) {
        if (!board[cellY][cellX].isRevealed) {
            board[cellY][cellX].isFlagged = !board[cellY][cellX].isFlagged;
        }
    }
    else {
        if (!board[cellY][cellX].isRevealed && !board[cellY][cellX].isFlagged) {
            if (board[cellY][cellX].isMine) {
                std::cout << "Game Over!" << std::endl;
                currentState = MENU;
            }
            else {
                RevealCell(cellX, cellY);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    InitSDL();

    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    HandleClick(e.button.x, e.button.y, false);
                }
                else if (e.button.button == SDL_BUTTON_RIGHT) {
                    HandleClick(e.button.x, e.button.y, true);
                }
            }
        }

        if (currentState == MENU) {
            DrawMenu();
        }
        else {
            DrawBoard();
        }
    }

    QuitSDL();
    return 0;
}
