# takes an argument for compilation

# Exits and displays error if there is no argument provided
if [ $# -eq 0 ]; then
    echo "This script requires an argument."
    echo "Please provide the Cpp file that you wish to compile."
    echo "Exiting."
    exit 1
fi

FILE=a.out
if test -f "$FILE"; then
    mv a.out .a.out
fi

clang++ -std=c++17 -fdiagnostics-color=always -Wall -g -I./libs -L./libs/glad /opt/homebrew/Cellar/assimp/5.2.5/lib/libassimp.5.2.4.dylib ./libs/GLFW/MacOS/libglfw.3.3.dylib $1 ./libs/glad/glad.c -o ./app.out -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -framework CoreFoundation -Wno-deprecated
