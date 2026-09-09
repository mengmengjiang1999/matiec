MATIEC_PIC_CXXFLAGS = -fPIC -fvisibility=hidden -fvisibility-inlines-hidden
MATIEC_HANDWRITTEN_CXXFLAGS = -std=gnu++17 -g -Wall -Wextra -Wpointer-arith -Wwrite-strings -Wno-overloaded-virtual -Wno-unused -pthread $(MATIEC_PIC_CXXFLAGS)
AM_CXXFLAGS = $(MATIEC_HANDWRITTEN_CXXFLAGS)
AM_LDFLAGS = -pthread
ARFLAGS = cr
