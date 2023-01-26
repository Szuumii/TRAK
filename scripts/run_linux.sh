
# takes an argument for compilation

# Exits and displays error if there is no argument provided
if [ $# -eq 0 ]; then
    echo "This script requires an argument."
    echo "Please provide the C file that you wish to compile."
    echo "Exiting."
    exit 1
fi

# if the a.out file already exists, move it to .a.out. Program should not execute if it does not compile properly.
FILE=a.out
if test -f "$FILE"; then
	mv a.out .a.out
fi

# If it doesn't work for you, try adding any of the libraries below.
#g++ $1 -lGL -lm -lX11 -lpthread -lXrandr -lXi -ldl -lglfw
g++ --std=c++17 $1 -lassimp -ldl -I./libs -lGL -lGLEW -lGLU -lglut -I./include -lfreeimage -o ./output.out
#./output.out