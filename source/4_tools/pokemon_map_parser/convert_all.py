import regis.diagnostics
import regis.util
from pathlib import Path
import os

def main():
    blk_files = []
    bst_files = []
    png_files = []
    tilesets = {}
    for root,dirs,files in os.walk('D:\\Games\\pokered-master\\maps'):
        for file in files:
            filepath = Path(file)
            if filepath.suffix == '.blk':
                blk_files.append(os.path.join(root, file))

    for root,dirs,files in os.walk('D:\\Games\\pokered-master\\gfx\\blocksets'):
        for file in files:
            filepath = Path(file)
            if filepath.suffix == '.bst':
                bst_files.append(os.path.join(root, file))

    for root,dirs,files in os.walk('D:\\Games\\pokered-master\\gfx\\tilesets'):
        for file in files:
            filepath = Path(file)
            if filepath.suffix == '.png':
                png_files.append(os.path.join(root, file))

    output = r'D:\Engines\Rex\data\Pokemon\ported'
    png_to_bpp = r'D:\Engines\Rex\source\4_tools\pokemon_map_parser\png_to_2bpp.py'
    bst_to_png = r'D:\Engines\Rex\source\4_tools\pokemon_map_parser\bst_to_png.py'
    blk_to_png = r'D:\Engines\Rex\source\4_tools\pokemon_map_parser\blk_to_png.py'
    for png in png_files:
        png_stem = Path(png).stem
        output_path = os.path.join(output, f"{png_stem}.2bpp")
        tilesets[png_stem] = output_path
        if not os.path.isfile(output_path):
            os.system(f'py {png_to_bpp} -png={png} -output={output_path}')

    for bst in bst_files:
        bst_stem = Path(bst).stem
        if bst_stem in tilesets:
            output_path = os.path.join(output, f"bst_{bst_stem}.png")
            if not os.path.isfile(output_path):
                tileset_path = tilesets[bst_stem]
                cmd = f'py {bst_to_png} -bpp={tileset_path} -bst={bst} -output={output_path}'
                regis.diagnostics.log_info(f'Running: {cmd}')
                os.system(cmd)

            
    return

if __name__ == '__main__':
    main()