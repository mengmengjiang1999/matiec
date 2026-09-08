MATIEC_HANDWRITTEN_CXXFLAGS = -std=gnu++17 -g -Wall -Wextra -Wpointer-arith -Wwrite-strings -Wno-overloaded-virtual -Wno-unused -pthread
AM_CXXFLAGS = $(MATIEC_HANDWRITTEN_CXXFLAGS)
AM_LDFLAGS = -pthread
ARFLAGS = cr
