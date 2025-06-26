CXX := g++
CXXFLAGS := -Wall -Wextra -I include

SRCDIR := src
BINDIR := bin

SRCS := $(shell find $(SRCDIR) -name '*.cpp')

NAME := Main

all:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(BINDIR)/$(NAME) -Ofast -DNDEBUG

debug:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(BINDIR)/$(NAME)Debug