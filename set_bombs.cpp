#include <SDL.h>
#include <iostream>
#include <vector>
#include <ctime>

const int SIZE = 10; 
const int CELL_SIZE = 40; 
const int MINES = 15; 

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

struct Cell {
    bool isMine = false;
    bool isRevealed = false;
    int adjacentMines = 0;
};

Cell board[SIZE][SIZE];

void InitSDL() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("set_bombs", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SIZE * CELL_SIZE, SIZE * CELL_SIZE, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void QuitSDL() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void PlaceMines() {
    srand(time(0));
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
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE || board[y][x].isRevealed) return;

    board[y][x].isRevealed = true;

    if (board[y][x].adjacentMines == 0 && !board[y][x].isMine) {
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                RevealCell(x + dx, y + dy);
            }
        }
    }
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
            }

            SDL_RenderFillRect(renderer, &cellRect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &cellRect);
        }
    }

    SDL_RenderPresent(renderer);
}

void HandleClick(int x, int y) {
    int cellX = x / CELL_SIZE;
    int cellY = y / CELL_SIZE;

    if (!board[cellY][cellX].isRevealed) {
        if (board[cellY][cellX].isMine) {
            std::cout << "Game Over!" << std::endl;
            exit(0); 
        }
        else {
            RevealCell(cellX, cellY);
        }
    }
}

int main(int argc, char* argv[]) {
    InitSDL();
    PlaceMines();
    CalculateNumbers();

    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                HandleClick(e.button.x, e.button.y);
            }
        }
        DrawBoard();
    }

    QuitSDL();
    return 0;
}