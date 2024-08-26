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
    parser.add_argument('-bst', help='path to .bst file')
    parser.add_argument('-output', help='path to output file, by default it\'ll use the bst name')
    args, unknown = parser.parse_known_args()
    
    output = args.output
    if not output:
        bst_stem = Path(args.bst).stem
        output = os.path.join(f'{bst_stem}_bst.png')

    regis.diagnostics.log_info(f'BPP: {os.path.abspath(args.bpp)}')
    regis.diagnostics.log_info(f'BST: {os.path.abspath(args.bst)}')
    regis.diagnostics.log_info(f'OUTPUT: {os.path.abspath(output)}')

    tileset = util.load_file(args.bpp)
    blockset = util.load_file(args.bst)

    tiles = util.read_tiles(tileset)
    blocks = util.read_blocks(blockset)

    blocks_per_row = min(len(blocks), 4)
    blocks_per_column = int(math.ceil(len(blocks)/blocks_per_row))
    width = blocks_per_row * Block.width_px
    height = blocks_per_column * Block.height_px
    img = Image.new('RGBA', (width, height), 'white')

    for i in range(0, blocks_per_column):
        for j in range(0, blocks_per_row):
            block_idx = (i * blocks_per_row) + j
            if (block_idx == len(blocks)):
                break
            if (block_idx > len(blocks)):
                continue

            x_offset = j * Block.width_px
            y_offset = i * Block.height_px
            util.draw_block(x_offset, y_offset, tiles, blocks[block_idx], img)

    img.save(output)

if __name__ == '__main__':
    main()