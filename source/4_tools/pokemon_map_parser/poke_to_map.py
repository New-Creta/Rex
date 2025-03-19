import os
from pathlib import Path

def main():
    map_objects_dir = 'D:\\Engines\\Rex\\_docs\\data\\pokemon\\reference\\gen1\\pokered\\data\\maps\\objects'
    map_headers_dir = 'D:\\Engines\\Rex\\_docs\\data\\pokemon\\reference\\gen1\\pokered\\data\\maps\\headers'
    map_text_dir = 'D:\\Engines\\Rex\\_docs\\data\\pokemon\\reference\\gen1\\pokered\\text'
    map_scripts_dir = 'D:\\Engines\\Rex\\_docs\\data\\pokemon\\reference\\gen1\\pokered\\scripts'
    map_audio_dir = 'D:\\Engines\\Rex\\_docs\\data\\pokemon\\reference\\gen1\\pokered\\audio\\headers'

    map_objects = os.listdir(map_objects_dir)
    map_headers = os.listdir(map_headers_dir)
    map_texts = os.listdir(map_text_dir)
    map_scripts = os.listdir(map_scripts_dir)
    map_audio = os.listdir(map_audio_dir)
    output = "D:\\Engines\\Rex\\data\\Pokemon\\maps2"
    for map_object in map_objects:
        stem = Path(map_object).stem
        if stem.endswith('_2'):
            stem = stem.removesuffix('_2')
        output_filename = f'{stem}.map'
        output_filepath = os.path.join(output, output_filename)
        with open(os.path.join(map_objects_dir, map_object)) as f:
            with open(output_filepath, 'w') as o:
                o.write(f'-- MAP OBJECT --\n\n')
                o.write(f.read())

    for map_header in map_headers:
        stem = Path(map_header).stem
        if stem.endswith('_2'):
            stem = stem.removesuffix('_2')
        output_filename = f'{stem}.map'
        output_filepath = os.path.join(output, output_filename)
        with open(os.path.join(map_headers_dir, map_header)) as f:
            with open(output_filepath, 'a') as o:
                o.write(f'\n\n-- MAP HEADER --\n\n')
                o.write(f.read())
    
    for map_text in map_texts:
        stem = Path(map_text).stem
        if stem.endswith('_2'):
            stem = stem.removesuffix('_2')
        output_filename = f'{stem}.map'
        output_filepath = os.path.join(output, output_filename)
        with open(os.path.join(map_text_dir, map_text)) as f:
            with open(output_filepath, 'a') as o:
                o.write(f'\n\n-- MAP TEXT --\n\n')
                o.write(f.read())

    for map_script in map_scripts:
        stem = Path(map_script).stem
        if stem.endswith('_2'):
            stem = stem.removesuffix('_2')
        output_filename = f'{stem}.map'
        output_filepath = os.path.join(output, output_filename)
        with open(os.path.join(map_scripts_dir, map_script)) as f:
            with open(output_filepath, 'a') as o:
                o.write(f'\n\n-- MAP SCRIPT --\n\n')
                try:
                    o.write(f.read())
                except UnicodeDecodeError as e:
                    print(f'Failed to write unicode character of {os.path.join(map_scripts_dir, map_script)}: {e}')

    # for map_music in map_audio:
    #     stem = Path(map_music).stem
    #     output_filename = f'{stem}.map'
    #     output_filepath = os.path.join(output, output_filename)
    #     with open(os.path.join(map_audio_dir, map_music)) as f:
    #         with open(output_filepath, 'a') as o:
    #             o.write(f'\n\n-- MAP AUDIO --\n\n')
    #             o.write(f.read())

if __name__ == "__main__":
    main()