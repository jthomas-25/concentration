#!/bin/bash

compiler="g++"
src_files=$(find src/ -name "*.cpp")

run_on_linux () {
    allegro_addons="allegro-5 allegro_primitives-5 allegro_font-5 allegro_ttf-5 allegro_dialog-5"
    ${compiler} ${src_files} -o concentration $(pkg-config ${allegro_addons} --libs --cflags)
    ./concentration
}

run_on_mac () {
    allegro_addons="allegro-5 allegro_main-5 allegro_primitives-5 allegro_font-5 allegro_ttf-5 allegro_dialog-5"
    ${compiler} ${src_files} -o concentration $(pkg-config ${allegro_addons} --libs --cflags)
    ./concentration
}

run_on_windows () {
    allegro_addons="-lallegro -lallegro_primitives -lallegro_font -lallegro_ttf -lallegro_dialog"
    ${compiler} ${src_files} -o concentration.exe ${allegro_addons}
    concentration.exe
}

if [ $# -eq 1 ]
then
    os=$1
    if [[ $os = "linux" || $os = "Linux" ]];
    then
        run_on_linux
    elif [[ $os = "mac" || $os = "Mac" ]];
    then
        run_on_mac
    elif [[ $os = "windows" || $os = "Windows" ]];
    then
        run_on_windows
    else
        echo "Usage: ./setup.sh [linux|mac|windows]"
    fi
else
    echo "Usage: ./setup.sh [linux|mac|windows]"
fi
