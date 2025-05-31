NAME = Main

all:
	g++ -Ofast -o bin/$(NAME).exe src/*.cpp -I include -D NDEBUG

debug:
	g++ -o bin/$(NAME)Debug.exe src/*.cpp -I include

win:
	g++ -o bin/$(NAME)Windows.exe src/*.cpp -I include -D NDEBUG
