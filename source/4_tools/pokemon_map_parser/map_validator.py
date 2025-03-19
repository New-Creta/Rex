import os
from pathlib import Path
import regis.rex_json

class Connection():
    def __init__(self, src, dst, dir, offset):
        self.src = src
        self.dst = dst
        self.dir = dir
        self.offset = offset

def opposite_dir(dir):
    if dir == 'north':
        return 'south'
    
    if dir == 'east':
        return 'west'
    
    if dir == 'south':
        return 'north'
    
    if dir == 'west':
        return 'east'

def connection_hash(conn : Connection):
    return f'{conn.src}{conn.dst}{conn.dir}{conn.offset}'

def main():
    map_files_dir = os.path.join('D:\\', 'Engines', 'Rex', 'data', 'Pokemon', 'maps3')
    map_files = os.listdir(os.path.join(map_files_dir))

    all_connections = []
    missing_connections = []

    for map_file in map_files:
        fullpath = os.path.join(map_files_dir, map_file)
        map_content = regis.rex_json.load_file(fullpath)

        if 'width' not in map_content:
            print(f'{fullpath} does not contain a width')
            continue

        if 'height' not in map_content:
            print(f'{fullpath} does not contain a height')
            continue

        width = map_content['width']
        height = map_content['height']

        if width < 1:
            print(f'{fullpath} has invalid width of {width}')
            continue
        
        if height < 1:
            print(f'{fullpath} has invalid height of {height}')
            continue

        if 'connections' not in map_content:
            print(f'{fullpath} does not hold any connections, its therefore unreachable')
            continue

        connections = map_content['connections']
        # if len(connections) == 0:
        #     print(f'{fullpath} has no connections')
        # else:
        for connection in connections:
            map_name = map_content['name']
            dst_map_path = connection['map']
            root = os.path.join('D:\\', 'Engines', 'Rex', 'data')
            dst_map_file = os.path.join(root, dst_map_path)
            dst_name = regis.rex_json.load_file(dst_map_file)['name']
            dir = connection['direction'].lower()
            offset = int(connection['offset'])
            # print(f'adding {map_name} -> {dst_name}')

            all_connections.append(Connection(map_name, dst_name, dir, offset))
            missing_connections.append(connection_hash(Connection(dst_name, map_name, opposite_dir(dir), -offset)))

    all_valid = True
    for connection in all_connections:
        hash = connection_hash(connection)
        if hash not in missing_connections:
            print(f'Missing connection!')
            print(f'src: {connection.src} - dst: {connection.dst}')
            all_valid = False

    if all_valid:
        print('All maps are valid')
    else:
        print('Some maps are invalid')

if __name__ == '__main__':
    main()