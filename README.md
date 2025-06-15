# Concentration
A shape-matching memory game, written in C++ and developed with the Allegro game programming library.

![Screenshot](https://github.com/jthomas-25/concentration/gameplay.jpg)

### Requirements
+ [Allegro 5.2](https://github.com/liballeg/allegro_wiki/wiki/Quickstart#installation) or later
+ C++ compiler (the shell script uses g++)

### Compile and run
#### Windows (MinGW) / Linux / macOS
Run the ```setup.sh``` shell script, passing your operating system's name as an argument:
```
./setup.sh windows
```
```
./setup.sh linux
```
```
./setup.sh mac
```
#### Windows (Visual Studio 2015+)
+ [Create a project and install Allegro.](https://github.com/liballeg/allegro_wiki/wiki/Allegro-in-Visual-Studio)
+ When [configuring Allegro](https://github.com/liballeg/allegro_wiki/wiki/Allegro-in-Visual-Studio#configuration), enable the Truetype Font (TTF), Primitives, Dialog, and Font addons.]
+ Build and run from within Visual Studio.

Reference: [Allegro wiki](https://github.com/liballeg/allegro_wiki/wiki/Quickstart)
