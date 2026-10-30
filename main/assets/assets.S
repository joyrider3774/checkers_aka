/* assets.bin (tools/convert_assets.py) as dm_assets, aligned for the 16 bit pixels in it */
    .section .rodata.dm_assets
    .balign 4
    .global dm_assets
dm_assets:
    .incbin "assets.bin"
