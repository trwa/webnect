.PHONY: clean

clean:
	rm app

Prova.pcm: Prova.cppm
	clang++ -std=c++20 Prova.cppm --precompile -o Prova.pcm

app: app.cpp Prova.pcm
	clang++ -std=c++20 app.cpp -fmodule-file=Prova=Prova.pcm Prova.pcm -o app #main.cpp kinect.cpp -o app -I./flatbuffers/include -I/usr/include/libfreenect -I/usr/include/opencv4 -lfreenect -lopencv_core -lopencv_highgui

all: app