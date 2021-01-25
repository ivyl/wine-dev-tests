CXX := x86_64-w64-mingw32-g++
LIBS := -ldinput8

%.exe: %.cpp
	$(CXX) $< -o $@ $(LIBS)


all: dinput-test.exe
