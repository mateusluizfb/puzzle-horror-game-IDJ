CXX = g++
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CXXFLAGS = -g -Wall -std=c++17 -I/usr/include/SDL2 \
		-I../engine/include -I../engine/include/core -I../engine/include/math \
		-I../engine/include/rendering -I../engine/include/physics -I../engine/include/input \
		-I../engine/include/audio -I../engine/include/tiles -I../engine/include/camera \
		-I../engine/include/utils -I../engine/include/lib \
		-I../game/include -I../game/include/entities -I../game/include/controllers \
		-I../game/include/states -I../game/include/systems
	LDFLAGS = -L/usr/lib/x86_64-linux-gnu -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lgtest -lgtest_main -pthread
else
	BREW_PREFIX := $(shell brew --prefix)
	GTEST_PREFIX := $(shell brew --prefix googletest)

	CXXFLAGS = -g -Wall -std=c++17 -I$(BREW_PREFIX)/include/SDL2 -I$(GTEST_PREFIX)/include \
		-I../engine/include -I../engine/include/core -I../engine/include/math \
		-I../engine/include/rendering -I../engine/include/physics -I../engine/include/input \
		-I../engine/include/audio -I../engine/include/tiles -I../engine/include/camera \
		-I../engine/include/utils -I../engine/include/lib \
		-I../game/include -I../game/include/entities -I../game/include/controllers \
		-I../game/include/states -I../game/include/systems
	LDFLAGS = -L$(BREW_PREFIX)/lib -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lgtest -lgtest_main -pthread
endif

# Collect all engine and game source files
engine_sources = $(wildcard ../engine/src/*/*.cpp)
game_sources = $(wildcard ../game/src/*/*.cpp)
test_sources = $(engine_sources) $(game_sources) $(wildcard ../test/*.cpp) $(wildcard ../test/engine/*.cpp) $(wildcard ../test/game/*.cpp)

BIN = test_runner

clean:
	rm -f $(BIN)
	rm -f *.o src/*.o
	rm -rf minimal_sdl2_**
	rm -rf game.**

build-test:
	$(CXX) $(CXXFLAGS) $(test_sources) -o $(BIN) $(LDFLAGS)

test: build-test
	./$(BIN)

test-bench:
	$(CXX) $(CXXFLAGS) ./TestBench.cpp -o test_bench $(LDFLAGS)
	./test_bench
	rm -f test_bench

# ex: make test-file file=TestInputManager
test-file: build-test
	./$(BIN) --gtest_filter=$(file).*
