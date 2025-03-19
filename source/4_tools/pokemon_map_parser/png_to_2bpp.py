from tile import Tile
from block import Block
from PIL import Image
from pathlib import Path
import argparse
import os
import util
import math
import array
import regis.diagnostics

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-png', help='path to png file')
    parser.add_argument('-output', help='path to output file, by default it\'ll use the png name')
    args, unknown = parser.parse_known_args()

    output = args.output
    if not output:
        png_stem = Path(args.png).stem
        output = os.path.join(f'{png_stem}.2bpp')

    regis.diagnostics.log_info(f'PNG: {os.path.abspath(args.png)}')
    regis.diagnostics.log_info(f'OUTPUT: {os.path.abspath(output)}')

    img = Image.open(args.png)
    img = img.convert('RGB')
    width, height = img.size
       
    pix_vals = list(img.getdata())
    num_tiles = int(len(pix_vals)/(Tile.width_px * Tile.height_px))
    tile_bytes : list[array.array] = [array.array('B') for _ in range(num_tiles)]

    filebytes = array.array('B')
    num_tile_columns = int(width / Tile.width_px)
    num_tile_rows = int(height / Tile.height_px)

    # creates the 2bpp file
    #5232
    for row_idx in range(0, num_tile_rows):
        for column_idx in range(0, num_tile_columns):
            tile_idx = (row_idx * num_tile_columns) + column_idx
            for tile_row_idx in range(0, Tile.height_px):
                tile_column_in_png = tile_idx % num_tile_columns
                tile_data_column_offset = (tile_column_in_png * Tile.width_px)
                tile_data_offset = tile_data_column_offset + (row_idx * Tile.height_px * width) + (tile_row_idx * width)
                row_vals = pix_vals[tile_data_offset:tile_data_offset+Tile.width_px]
                even_byte, odd_byte = util.serialize_tile_row(row_vals)
                tile_bytes[tile_idx].append(even_byte)
                tile_bytes[tile_idx].append(odd_byte)

    
    # strip out all non-tiles at the end
    # a non tile is defined as being all white, at the end of a 2bppp
    has_non_tiles = True
    while has_non_tiles:
        last_tile_bytes = tile_bytes[-1]
        for byte in last_tile_bytes:
            if byte != 0:
                has_non_tiles = False
                break
        if has_non_tiles:
            tile_bytes.pop()

    for bytes in tile_bytes:
        filebytes.extend(bytes)

    filebytes.tofile(open(output, 'wb'))

if __name__ == '__main__':
    main()