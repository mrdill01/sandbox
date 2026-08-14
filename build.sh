gcc src/*.c lib/*.c -o bin/quark.exe -std=c11 -Wall -lSDL2main -lSDL2 -lGL -lopenal -lm -lenet
./bin/quark.exe
