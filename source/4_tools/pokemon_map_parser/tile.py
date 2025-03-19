def tile_pixel_value(idx, even_byte, odd_byte):
    even_value = (even_byte >> (7 - idx)) & 0x1
    odd_value = (odd_byte >> (7 - idx)) & 0x1
    odd_value <<= 1

    return even_value | odd_value

class Tile():
    width_in_bytes = 16
    width_px = 8
    height_px = 8

    def __init__(self, tile_bytes):
        self.pixels = []
        num_bytes_to_parse = int(len(tile_bytes) / 2)
        for i in range(0, num_bytes_to_parse):
            even_byte = tile_bytes[(i * 2) + 0]
            odd_byte = tile_bytes[(i * 2) + 1]
            i += 1

            self.pixels.append(tile_pixel_value(0, even_byte, odd_byte))
            self.pixels.append(tile_pixel_value(1, even_byte, odd_byte))
            self.pixels.append(tile_pixel_value(2, even_byte, odd_byte))
            self.pixels.append(tile_pixel_value(3, even_byte, odd_byte))
            self.pixels.append(tile_pixel_value(4, even_byte, odd_byte))
            self.pixels.append(tile_pixel_value(5, even_byte, odd_byte))
            self.pixels.append(tile_pixel_value(6, even_byte, odd_byte))
            self.pixels.append(tile_pixel_value(7, even_byte, odd_byte))