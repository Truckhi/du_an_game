all:
	g++ -std=c++17 main.cpp -o Minesweeper.exe -mwindows -Isrc/include -Lsrc/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
	./Minesweeper.exe