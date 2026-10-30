# Checkers for the Gamebuino AKA

A port of the GP2X version of Checkers to the Gamebuino AKA. Play checkers against the computer:
you are blue at the bottom, the computer is red at the top. Jumps have to be made when there is
one, a piece that jumped and can jump again has to, and a piece that reaches the other side
becomes a king that can also go backwards. The options set how far ahead the computer looks (four
difficulties) and whether it also counts how many pieces can jump or move. The panel on the right
shows whose turn it is and the last moves. The graphics, sounds and music are the GP2X version's,
at the same 320x240 the AKA's screen has.

## Controls

| AKA | In a game | In the menus |
|---|---|---|
| D-pad / joystick | move the cursor; with a piece picked up, two directions held together move it along that diagonal (it jumps when it can) | choose; left / right change an option |
| A | pick up a piece; with a piece picked up, move it to the square the cursor is on; after the game, back to the title screen | select; change an option |
| B | drop the piece that is picked up; cancel | back (options) |
| MENU | leave the game (asks first) | back (options) |
| L1 / R1 | volume down / up | volume down / up |

The squares a picked up piece can go to are shown in green, the cursor in white. **Quit** in the
title screen's menu, or **RUN + MENU** held for half a second anywhere in the game, goes back to the
launcher; RUN on its own switches the AKA off as in every game.

The GP2X moved a picked up piece with its diagonal buttons. The AKA has none: two directions held
together do the same, or move the cursor to the square and press A.

## Installing

Copy the `SD_files/CHECKERS` folder to the root of the SD card and start Checkers from the launcher.
The folder holds `firmware.bin`, `meta.json`, `screen.bmp`, the music and the `LICENSE`. The game
writes the volume, the difficulty and the jump option to `settings.dat` there, as soon as they
change.

## Building

ESP-IDF 5.5 and the official [Gamebuino AKA library](https://github.com/jmp42/Gamebuino_AKA_lib),
which is used where it is checked out (`C:/github/Gamebuino_AKA_lib` by default, or set
`AKA_LIB_DIR`):

```
idf.py set-target esp32s3
idf.py build
```

`build/checkers.bin` is copied to `SD_files/CHECKERS/firmware.bin` by the build.

The graphics, font, sounds and music are the GP2X version's original files in `assets/`, converted
by `tools/convert_assets.py` (Pillow, and ffmpeg for the sounds and music):

```
python tools/convert_assets.py <ffmpeg>
```

It writes `main/assets/assets.bin` (the images, the font and the sounds, built into the firmware)
and the SD card folder's music and `screen.bmp`.

## Changes from the GP2X version

- Fixed bugs of the GP2X version:
  - When the computer had no move left, the game asked it again every frame and played the winner
    sound over and over.
  - A new game still showed the moves of the game before.
  - The computer's jumps over several pieces were listed from the last jump to the first.
  - Showing the steps of a move one by one read an iterator that was never set (it was never used
    that way).
  - A damaged `settings.dat` could give a difficulty the computer can not play at.
- Taken over from the later Playdate version:
  - MENU in a game asks "Are you sure you want to quit the current game?" first (the image is the
    Playdate one, coloured as the GP2X panels).
  - A draw plays a sound of its own (the Playdate version's `draw.wav`).
  - In the options A changes the option instead of leaving the screen.
- A picked up piece is moved with the cursor and A, or with two directions held. While a piece has
  to jump on it can not be dropped, and a jump it can not make plays the wrong move sound.
- The settings are saved when they change, the GP2X saved them when it quit. The TV out screen
  position, and the R + d-pad on the title screen that moved it, are gone; SELECT no longer quits
  from the title screen, Quit does.
- The three other computer players of the GP2X code, which the game never used, are left out.

## Source layout

The GP2X version was a single `main.cpp`, split up here:

| File | What it is |
|---|---|
| `main/main.cpp` | start up and the state loop |
| `main/titlescreen.cpp`, `options.cpp`, `game.cpp` | one file per game state |
| `main/CBord.cpp` | the board, the rules and the moves |
| `main/ComputerAi.cpp` | the computer player, a min max search with alpha beta pruning |
| `main/CSelector.cpp`, `CMainMenu.cpp`, `COptionMenu.cpp` | the cursor, the title menu and the options menu |
| `main/gamefuncs.cpp`, `commonvars.cpp` | what the states share: images, volume, settings, music, the globals |
| `main/gfx.cpp`, `sound.cpp`, `input.cpp`, `platform.cpp` | what SDL did on the GP2X, on the AKA library |

## Credits

Checkers by Willems Davy (Willems Soft).

The music is "Checkers Rag" by Harry J. Lincoln (1913), the MIDI with notes by John Cowles
(5/1/96), from [primeshop.com/midlist3.htm](http://primeshop.com/midlist3.htm).

## License

MIT, see [LICENSE](LICENSE).
