CXX := x86_64-w64-mingw32-g++
LIBS := -ld3d9

%.exe: %.cpp
	$(CXX) $< -o $@ $(LIBS)


all: d3d9-mode-list.exe
