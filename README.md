# sbgd
sbgd is a small daemon which runs a swaybg process with a random wallpaper and
restarts it (changing the wallpaper) when it is killed.

## Building from source
You can build sbgd from source using
```
git clone https://github.com/podikoglou/sbgd
cd sbgd
make
```

You can then install it with
```
sudo make install
```

Dependencies:
- gcc (you can use clang using `make CC=clang++`)

## Usage 
You can run the daemon using:
```
sbgd ~/docs/walls
```

Where `~/docs/walls` is a directory containing wallpapers. A few notes:
- Accepted file formats for wallpapers are (case insensitive): .jpg, .jpeg, .png

This doesn't run in the background as a daemon by itself. If you want that, use the `-d` flag
```
sbgd -d ~/docs/walls
```

To change your wallpaper, simply kill `swaybg`:
```
pkill swaybg
```

### Using with river
You can add this to make sbgd auto-start with river:
```
riverctl spawn "sbgd ~/docs/walls"
```

And set a keybinding to change your wallpaper:
```
riverctl map normal Alt+Shift L spawn "pkill swaybg"
```
