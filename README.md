# Pong
Clone of the classical 1972 Pong game.

## Preview
![pong-resized](https://user-images.githubusercontent.com/15930386/160948252-63ed9b00-49da-4cf1-ae0e-bd2cc582c12d.gif)

## Run (Windows)
Just run `pong.exe`

## Setup and compilation (Ubuntu)
Compiling it yourself always has some benefits (take a peek to the `main.cpp` source file to find out).

First, install the Allegro5 library:

```console
sudo apt update
sudo apt install liballegro5-dev
```

### Compile

Run `make`

In case you want to compile it manually, you can, by running:
```console
gcc main.cpp -Wall -Wextra `pkg-config --cflags --libs allegro-5 allegro_acodec-5 allegro_audio-5 allegro_color-5 allegro_dialog-5 allegro_font-5 allegro_image-5 allegro_main-5 allegro_memfile-5 allegro_physfs-5 allegro_primitives-5 allegro_ttf-5` -o main
```

### Run
run `./main` after compiling.

### Run using Wine
You can also execute it using [Wine](https://wiki.winehq.org/Ubuntu):
```console
wine pong.exe
```
(but where's the fun on that).
