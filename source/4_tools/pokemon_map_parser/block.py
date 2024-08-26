class Block():
    indices_per_block = 16
    num_tiles_per_row = 4
    num_tiles_per_column = 4
    width_px = 32
    height_px = 32
    pixels_per_block = width_px * height_px

    def __init__(self, tile_indices):
        self.tile_indices = tile_indices