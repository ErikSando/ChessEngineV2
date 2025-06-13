CXX := g++
CXXFLAGS := -I include

SRCDIR := src
BINDIR := bin

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')

NAME := Main

all:
	$(CXX) -Ofast -o $(BINDIR)/$(NAME) $(SRCS) $(CXXFLAGS) -D NDEBUG

prev:
	$(CXX) -Ofast -o $(BINDIR)/$(NAME)Previous $(SRCS) $(CXXFLAGS) -D NDEBUG

debug:
	$(CXX) -o $(BINDIR)/$(NAME)Debug $(SRCS) $(CXXFLAGS)

win:
	$(CXX) -o $(BINDIR)/$(NAME)Windows.exe $(SRCS) $(CXXFLAGS) -D NDEBUG
