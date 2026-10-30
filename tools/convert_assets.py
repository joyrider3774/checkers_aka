#!/usr/bin/env python3
"""Converts the assets of the GP2X version of Checkers into what the Gamebuino AKA build uses.

    python tools/convert_assets.py <ffmpeg.exe>

Reads the original files from assets/ (graphics/, sound/, music/checkers.ogg and font.ttf). Writes:
  main/assets/assets.bin   the images, the font and the sound effects, in the LCD's own pixel
                           format, built into the firmware
  main/assets/assets.h     where each of them sits in assets.bin
  SD_files/CHECKERS/         the folder that goes on the SD card: the music and screen.bmp

Images become 16 bit pixels in the order the AKA's panel takes them (lcd_color_rgb in the
library: red in the low bits). The game loaded every image with SDL_DisplayFormat, which drops
an alpha channel, so the PNGs' alpha is not used: only the images the game gave a colour key
get an alpha plane, 0 where they are magenta (255,0,255). The result panels were also given a
surface alpha of 225 (SDL_SetAlpha), which goes into their alpha plane as well. confirmquit.png and
sound/draw.wav are the later Playdate version's, the image coloured as the GP2X result panels.

The TrueType font is rendered once here at the size the game opened it at, into an alpha
bitmap per character, the AKA draws those in any colour.

Sounds are resampled to 44100 Hz mono 16 bit, the only format the library's WAV player takes;
the music is decoded from checkers.ogg by ffmpeg and written with the plain 44 byte WAV header
the library's player checks for.
"""
import os
import struct
import subprocess
import sys

from PIL import Image, ImageDraw, ImageFont

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC_DIR = os.path.join(ROOT, "assets")
OUT_DIR = os.path.join(ROOT, "main", "assets")
SD_DIR = os.path.join(ROOT, "SD_files", "CHECKERS")

# name in the code, file, whether the game set a magenta colour key on it, its surface alpha
IMAGES = [
    ("TITLESCREEN", "titlescreen", False, 255),
    ("PLAY1", "play1", True, 255),
    ("PLAY2", "play2", True, 255),
    ("OPTIONS1", "options1", True, 255),
    ("OPTIONS2", "options2", True, 255),
    ("QUIT1", "quit1", True, 255),
    ("QUIT2", "quit2", True, 255),
    ("VERYEASY1", "veryeasy1", True, 255),
    ("VERYEASY2", "veryeasy2", True, 255),
    ("EASY1", "easy1", True, 255),
    ("EASY2", "easy2", True, 255),
    ("HARD1", "hard1", True, 255),
    ("HARD2", "hard2", True, 255),
    ("VERYHARD1", "veryhard1", True, 255),
    ("VERYHARD2", "veryhard2", True, 255),
    ("JUMPENABLED1", "jumpenabled1", True, 255),
    ("JUMPENABLED2", "jumpenabled2", True, 255),
    ("JUMPDISABLED1", "jumpdisabled1", True, 255),
    ("JUMPDISABLED2", "jumpdisabled2", True, 255),
    ("BLACK", "black", False, 255),
    ("WHITE", "white", False, 255),
    ("BLUE", "blue", False, 255),
    ("RED", "red", False, 255),
    ("REDKING", "redking", False, 255),
    ("BLUEKING", "blueking", False, 255),
    ("PANEL", "panel", False, 255),
    ("PLAYER", "player", False, 255),
    ("CPU", "cpu", False, 255),
    ("PLAYERWINS", "playerwins", True, 225),
    ("CPUWINS", "cpuwins", True, 225),
    ("DRAW", "draw", True, 225),
    ("CONFIRMQUIT", "confirmquit", True, 225),
]

# name in the code, file, size the game opened it at (TTF_OpenFont)
FONTS = [
    ("FONT", "font.ttf", 14),
]

# same order as the SND_ constants of the game
SOUNDS = ["select", "good", "wrong", "start", "winner", "loser", "menu", "drop", "draw"]

# the music, it plays all the time
MUSIC = ["checkers"]


def lcd_color(r, g, b):
    return (r >> 3) | ((g >> 2) << 5) | ((b >> 3) << 11)


class Blob:
    def __init__(self):
        self.data = bytearray()

    def align(self):
        while len(self.data) % 4:
            self.data.append(0)

    def add(self, raw):
        self.align()
        offset = len(self.data)
        self.data += raw
        return offset


def surface_planes(path, keyed, surface_alpha):
    """pixels and alpha plane (None when opaque) of an image"""
    rgba = Image.open(path).convert("RGBA")
    w, h = rgba.size
    pixels = bytearray()
    alpha = bytearray()
    opaque = True
    for r, g, b, a in rgba.getdata():
        a = 0 if keyed and (r, g, b) == (255, 0, 255) else surface_alpha
        if a != 255:
            opaque = False
        pixels += struct.pack("<H", lcd_color(r, g, b) if a else 0)
        alpha.append(a)
    return w, h, bytes(pixels), (None if opaque else bytes(alpha))


def font_glyphs(blob, path, size):
    font = ImageFont.truetype(path, size)
    ascent, descent = font.getmetrics()
    height = ascent + descent
    entries = []
    for code in range(32, 127):
        ch = chr(code)
        advance = int(round(font.getlength(ch)))
        left, _, right, _ = font.getbbox(ch, anchor="la")
        x0 = min(0, left)
        gw = max(advance, right) - x0
        if gw <= 0 or ch == " ":
            entries.append("{%d, 0, 0, -1}" % advance)
            continue
        img = Image.new("L", (gw, height), 0)
        ImageDraw.Draw(img).text((-x0, 0), ch, font=font, fill=255, anchor="la")
        off = blob.add(img.tobytes())
        entries.append("{%d, %d, %d, %d}" % (advance, gw, x0, off))
    return height, font.font.height, entries


def ffmpeg_pcm(ffmpeg, path):
    """the file as 44100 Hz mono signed 16 bit samples"""
    return subprocess.run([ffmpeg, "-hide_banner", "-loglevel", "error", "-i", path,
                           "-ac", "1", "-ar", "44100", "-f", "s16le", "-acodec", "pcm_s16le", "-"],
                          check=True, stdout=subprocess.PIPE).stdout


def write_wav(path, pcm):
    with open(path, "wb") as f:
        f.write(struct.pack("<4sI4s4sIHHIIHH4sI", b"RIFF", 36 + len(pcm), b"WAVE", b"fmt ", 16, 1, 1,
                            44100, 88200, 2, 16, b"data", len(pcm)))
        f.write(pcm)


def write_bmp(path, im):
    """24 bit BMP, what the launcher shows next to the game's name"""
    im = im.convert("RGB")
    w, h = im.size
    row = (w * 3 + 3) & ~3
    with open(path, "wb") as f:
        f.write(struct.pack("<2sIHHI", b"BM", 54 + row * h, 0, 0, 54))
        f.write(struct.pack("<IiiHHIIiiII", 40, w, h, 1, 24, 0, row * h, 2835, 2835, 0, 0))
        for y in range(h - 1, -1, -1):
            line = bytearray()
            for x in range(w):
                r, g, b = im.getpixel((x, y))
                line += bytes((b, g, r))
            line += b"\0" * (row - w * 3)
            f.write(line)


def main():
    if len(sys.argv) != 2:
        print(__doc__)
        sys.exit(1)
    ffmpeg = sys.argv[1]
    gfx = os.path.join(SRC_DIR, "graphics")
    os.makedirs(OUT_DIR, exist_ok=True)
    blob = Blob()
    h = ["// Generated by tools/convert_assets.py from the GP2X version's graphics, font and sounds.",
         "// Offsets are into assets.bin (dm_assets), -1 = not there.", "#pragma once", "#include <stdint.h>", "",
         "struct AssetSurface { int16_t w, h; int32_t pixels, alpha; };",
         "struct AssetGlyph { int8_t advance; int8_t w; int8_t x; int32_t alpha; };",
         "struct AssetFont { int16_t height, lineSkip; const AssetGlyph* glyphs; };",
         "struct AssetSound { int32_t samples; uint32_t count; };", ""]

    # the images, in the firmware
    names = []
    for name, file, keyed, surface_alpha in IMAGES:
        w, hh, px, a = surface_planes(os.path.join(gfx, file + ".png"), keyed, surface_alpha)
        pxo = blob.add(px)
        ao = blob.add(a) if a else -1
        h.append("static const AssetSurface ASSET_%s = {%d, %d, %d, %d};" % (name, w, hh, pxo, ao))
        names.append('"%s"' % file)
    h.append("")
    h.append("// the file names of the images")
    h.append("static const char* const ASSET_IMAGE_NAMES[] = {%s};" % ", ".join(names))
    h.append("")

    for name, file, size in FONTS:
        height, line_skip, glyphs = font_glyphs(blob, os.path.join(SRC_DIR, file), size)
        h.append("// %s at %d pixels, characters 32 to 126" % (file, size))
        h.append("static const AssetGlyph %s_GLYPHS[] = {\n    %s};" % (name, ",\n    ".join(glyphs)))
        h.append("static const AssetFont ASSET_%s = {%d, %d, %s_GLYPHS};" % (name, height, line_skip, name))
        h.append("")

    sounds = []
    for s in SOUNDS:
        pcm = ffmpeg_pcm(ffmpeg, os.path.join(SRC_DIR, "sound", s + ".wav"))
        sounds.append("{%d, %d}" % (blob.add(pcm), len(pcm) // 2))
    h.append("static const AssetSound ASSET_SOUNDS[] = {%s};" % ", ".join(sounds))
    h.append("")

    with open(os.path.join(OUT_DIR, "assets.bin"), "wb") as f:
        f.write(blob.data)
    with open(os.path.join(OUT_DIR, "assets.h"), "w", newline="\n") as f:
        f.write("\n".join(h))
    print("assets.bin: %d bytes" % len(blob.data))

    os.makedirs(os.path.join(SD_DIR, "music"), exist_ok=True)
    for music in MUSIC:
        write_wav(os.path.join(SD_DIR, "music", music + ".wav"),
                  ffmpeg_pcm(ffmpeg, os.path.join(SRC_DIR, "music", music + ".ogg")))
    write_bmp(os.path.join(SD_DIR, "screen.bmp"), Image.open(os.path.join(gfx, "titlescreen.png")).resize((160, 120), Image.LANCZOS))
    print("SD folder written to %s" % SD_DIR)


if __name__ == "__main__":
    main()
