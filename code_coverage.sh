@echo off
# Generate Makefiles using Premake
premake5 gmake2

# Compile the unittest_launcher target in release mode
make unittest_launcher config=release

# Run the compiled unit test executable
./vendor/release-linux/unittest_launcher

# Generate coverage report using lcov
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.info
lcov --list coverage.info