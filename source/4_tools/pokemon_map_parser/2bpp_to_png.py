from tile import Tile
from block import Block
from PIL import Image
from pathlib import Path
import argparse
import os
import util
import math
import regis.diagnostics

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-bpp', help='path to .2bpp file')
    parser.add_argument('-output', help='path to output file, by default it\'ll use the bpp name')
    args, unknown = parser.parse_known_args()
    
    output = args.output
    if not output:
        bpp_stem = Path(args.bpp).stem
        output = os.path.join(f'{bpp_stem}.png')

    regis.diagnostics.log_info(f'BPP: {os.path.abspath(args.bpp)}')
    regis.diagnostics.log_info(f'OUTPUT: {os.path.abspath(output)}')

    tileset = util.load_file(args.bpp)
    tiles = util.read_tiles(tileset)
    
    tiles_per_row = 16
    tiles_per_column = int(math.ceil(len(tiles)/tiles_per_row))
    width = tiles_per_row * Tile.width_px
    height = tiles_per_column * Tile.height_px
    img = Image.new('RGBA', (width, height), 'white')

    for i in range(0, tiles_per_column):
        for j in range(0, tiles_per_row):
            tile_idx = (i * tiles_per_row) + j
            # it's possible not the entire grid of tiles is filled up
            if tile_idx > len(tiles) - 1:
                break

            x_offset = j * Tile.width_px
            y_offset = i * Tile.height_px
            tile = tiles[tile_idx]
            util.draw_tile(x_offset, y_offset, tile, img)

    img.save(output)

if __name__ == '__main__':
    main()