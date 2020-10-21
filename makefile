default: prog

prog: main.o shaderLoader.o
	g++ main.o shaderLoader.o -o prog -lglew32 -lglu32 -lglfw3 -lgdi32 -lopengl32
main.o: main.cpp
	g++ main.cpp -c -I C:\Users\ManiacalDrWatts\Desktop\OpenGL\include
shaderLoader.o: shaderLoader.cpp
	g++ shaderLoader.cpp -c -I C:\Users\ManiacalDrWatts\Desktop\OpenGL\include

clean:
	rm -rf *.o prog.exe