.PHONY: clean

clean:
	rm app

app: main.cpp
	clang++ -std=c++20 main.cpp -o app -I/usr/include/libfreenect -I/usr/include/opencv4 -lfreenect -lopencv_core -lopencv_highgui

all: app