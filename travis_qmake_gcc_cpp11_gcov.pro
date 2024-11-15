# Collect all .cpp files recursively in the current directory
SOURCES += $$files($$PWD/*.cpp, true)

# Compiler flags
QMAKE_CXXFLAGS += -Wall -Wextra -Weffc++ -Werror

# gcov for coverage
QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
LIBS += -lgcov

# Specify compiler and standard
QMAKE_CXX = g++-5
QMAKE_LINK = g++-5
QMAKE_CC = gcc-5
QMAKE_CXXFLAGS += -std=c++11