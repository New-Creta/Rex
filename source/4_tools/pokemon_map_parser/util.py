import array
import os
import math
from tile import Tile
from block import Block
from PIL import Image

_white_hue = 0
_light_hue = 1
_dark_hue = 2
_black_hue = 3
_bpp_to_color = {
    _white_hue : (255, 255, 255, 255),
    _light_hue : (170, 170, 170, 255),
    _dark_hue : (85, 85, 85, 255),
    _black_hue : (0, 0, 0, 255),
}

def load_file(filepath):
    filebytes= array.array('B')
    size = os.path.getsize(filepath)
    filebytes.fromfile(open(filepath, "rb"), size)
    return filebytes

def read_tiles(tileset):
    tiles : list[Tile] = []
    num_tiles = int(len(tileset) / Tile.width_in_bytes)
    for i in range (0, num_tiles):
        offset = i * Tile.width_in_bytes
        tile_bytes = tileset[offset:offset+Tile.width_in_bytes]
        tiles.append(Tile(tile_bytes))

    return tiles

def read_blocks(blockset):
    blocks : list[Block] = []
    num_blocks = int(len(blockset)/Block.indices_per_block)
    for i in range(0, num_blocks):
        offset = i * Block.indices_per_block
        blocks.append(Block(blockset[offset:offset+Block.indices_per_block]))
    
    return blocks

def draw_tile(start_x, start_y, tile : Tile, img: Image.Image):
    for row in range(0, Tile.width_px):
        pixel_x = start_x
        pixel_y = start_y + row
        for pixel_idx in range(0, Tile.width_px):
            abs_pixel_idx = (row * Tile.width_px) + pixel_idx
            pixel = tile.pixels[abs_pixel_idx]
            img.putpixel((pixel_x, pixel_y), _bpp_to_color[pixel])
            pixel_x += 1

def draw_block(start_x, start_y, tiles : list[Tile], block : Block, img: Image.Image):
    for row in range(0, Block.num_tiles_per_row):
        for column in range(0, Block.num_tiles_per_column):
            tile_top_left_x = start_x + column * Tile.height_px
            tile_top_left_y = start_y + row * Tile.width_px
            abs_idx_in_block_indices = (row * Block.num_tiles_per_row) + column
            block_tile_idx = block.tile_indices[abs_idx_in_block_indices]
            if block_tile_idx < len(tiles):
                tile = tiles[block_tile_idx]
                draw_tile(tile_top_left_x, tile_top_left_y, tile, img)

def serialize_tile_row(px_vals):
    def all_same_value(px):
        return len(set(px)) == 1

    even_byte = 0
    odd_byte = 0
    byte_max = 256
    hues = [
        (1, 1),
        (1, 0),
        (0, 1),
        (0, 0)
    ]

    for i in range(0, len(px_vals)):
        px = px_vals[i]
        if not all_same_value(px):
            raise Exception('Pixel is not gray scale')
        v = px[0]
        hue_idx = int(v / (byte_max / len(hues)))
        high, low = hues[hue_idx]
        even_byte |= (low << 7 - i)
        odd_byte |= (high << 7 - i)
        
    return even_byte, odd_byte