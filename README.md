# sbgd
sbgd is a small daemon which runs a swaybg process with a random wallpaper and
restarts it (changing the wallpaper) when it is killed.

## Building from source
you can build sbgd from source using
```
git clone https://github.com/podikoglou/sbgd
cd sbgd
make
```

dependencies:
- gcc (you can use clang using `make CC=clang++`)

## Usage 
You can run the daemon using:
```
sbgd ~/docs/walls
```

Where `~/docs/walls` is a directory containing wallpapers. A few notes:
- Accepetd file formats for wallpapers are (case insensitive): .jpg, .jpeg, .png

This doesn't run in the background as a daemon by itself. If you want it it to do that, use the `-d flag`
```
sbgd -d ~/docs/walls
```
