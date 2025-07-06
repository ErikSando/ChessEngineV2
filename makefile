CXX := g++
CXXFLAGS := -Wall -Wextra -Iinclude

SRCDIR := src
BINDIR := bin

SRCS := $(shell find $(SRCDIR) -name '*.cpp')

NAME := Main

all:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(BINDIR)/$(NAME) -DNDEBUG -Ofast

debug:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(BINDIR)/$(NAME)Debug
