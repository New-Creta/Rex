from tile import Tile
from block import Block
from PIL import Image
from pathlib import Path
import argparse
import os
import util
import regis.diagnostics

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-bpp', help='path to .2bpp file')
    parser.add_argument('-bst', help='path to .bst file')
    parser.add_argument('-blk', help='path to .blk file')
    parser.add_argument('-width', help='width of blk in block numbers')
    parser.add_argument('-px_width', help='width of blk in pixels')
    parser.add_argument('-height', help='height of blk in block numbers')
    parser.add_argument('-px_height', help='height of blk in pixels')
    parser.add_argument('-output', help='path to output file, by default it\'ll use the blk name')
    args, unknown = parser.parse_known_args()
    
    output = args.output
    if not output:
        blk_stem = Path(args.blk).stem
        output = os.path.join(f'{blk_stem}.png')

    regis.diagnostics.log_info(f'BPP: {os.path.abspath(args.bpp)}')
    regis.diagnostics.log_info(f'BST: {os.path.abspath(args.bst)}')
    regis.diagnostics.log_info(f'BLK: {os.path.abspath(args.blk)}')
    regis.diagnostics.log_info(f'OUTPUT: {os.path.abspath(output)}')

    tileset = util.load_file(args.bpp)
    blockset = util.load_file(args.bst)
    map = util.load_file(args.blk)

    tiles = util.read_tiles(tileset)
    blocks = util.read_blocks(blockset)

    num_blocks_per_row = 0
    if args.width:
        num_blocks_per_row = int(args.width)
    elif args.px_width:
        num_blocks_per_row = int(args.px_width) / Block.width_px
    num_blocks_per_column = 0
    if args.height:
        num_blocks_per_column = int(args.height)
    elif args.px_height:
        num_blocks_per_column = int(args.height) / Block.height_px

    width = num_blocks_per_row * Block.width_px
    height = num_blocks_per_column * Block.height_px
    global img
    img = Image.new('RGBA', (width, height), 'white')

    for i in range(0, num_blocks_per_column):
        for j in range(0, num_blocks_per_row):
            block_idx_in_map = (i * num_blocks_per_row) + j
            block_idx = map[block_idx_in_map]
            x_offset = j * Block.width_px
            y_offset = i * Block.height_px
            util.draw_block(x_offset, y_offset, tiles, blocks[block_idx], img)

    img.save(output)

if __name__ == '__main__':
    main()