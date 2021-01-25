CXX := x86_64-w64-mingw32-gcc

%.exe: %.c
	$(CXX) $< -o $@ -lhid


all: rawinput-devices.exe
