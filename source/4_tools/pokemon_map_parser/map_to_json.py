import os
from pathlib import Path

import regis.rex_json

class Connection():
    def __init__(self, dir, map, offset):
        self.dir = dir
        self.map = map
        self.offset = offset

class ObjectEvent():
    def __init__(self, x, y, sprite, action, dir, text):
        self.x = x
        self.y = y
        self.sprite = sprite
        self.action = action
        self.dir = dir
        self.text = text
        return
    
class BgEvent():
    def __init__(self, x, y, text):
        self.x = x
        self.y = y
        self.text = text
        return
    
class Warp():
    def __init__(self, x, y, dst_map_id, dst_warp_id):
        self.x = x
        self.y = y
        self.dst_map_id = dst_map_id
        self.dst_warp_id = dst_warp_id
        return

def save_as_json(name, map_blocks, width, height, tilset, connections, border_block_idx, objects, object_events, bg_events, warps, texts, scripts, output):
    json = {}
    json["name"] = name
    json["width"] = width
    json["height"] = height
    json["tileset"] = os.path.join('Pokemon', 'tilesets', f'{tilset}.json')
    json["map_blocks"] = os.path.join('Pokemon', 'map_blocks', f'{map_blocks}.blk')
    json["border_block_idx"] = border_block_idx
    json["connections"] = []
    for connection in connections:
        connection_dict = {}
        connection_dict["direction"] = connection.dir
        connection_dict["map"] = os.path.join('Pokemon', 'maps', f'{connection.map}.json')
        connection_dict["offset"] = connection.offset
        json["connections"].append(connection_dict)

    json["objects"] = []
    for object in objects:
        object_dict = {}
        object_dict["name"] = object
        json["objects"].append(object_dict)    

    json["object_events"] = []
    for object_ev in object_events:
        object_ev_dict = {}
        object_ev_dict["x"] = int(object_ev.x)
        object_ev_dict["y"] = int(object_ev.y)
        object_ev_dict["sprite"] = object_ev.sprite
        object_ev_dict["action"] = object_ev.action
        object_ev_dict["direction"] = object_ev.dir
        object_ev_dict["text"] = object_ev.text
        json["object_events"].append(object_ev_dict)

    json["warps"] = []
    for warp in warps:
        warp_dict = {}
        warp_dict['x'] = int(warp.x)
        warp_dict['y'] = int(warp.y)
        try:
            warp_dict['dst_map_id'] = int(warp.dst_map_id)
        except ValueError as e:
            warp_dict['dst_map_id'] = -1 # if it should go to the "last map", this will raise
        warp_dict['dst_warp_id'] = int(warp.dst_warp_id)
        json["warps"].append(warp_dict)

    json["bg_events"] = []
    for bg_ev in bg_events:
        bg_ev_dict = {}
        bg_ev_dict["x"] = int(bg_ev.x)
        bg_ev_dict["y"] = int(bg_ev.y)
        bg_ev_dict["text"] = bg_ev.text
        json["bg_events"].append(bg_ev_dict)

    json['texts'] = []
    for text in texts.items():
        text_entry = {}
        text_entry['name'] = text[0]
        text_entry ['values'] = []
        for text_values in text[1]:
            text_entry ['values'].append(text_values)
        json['texts'].append(text_entry)

    json['scripts'] = []
    for script in scripts:
        json['scripts'].append(script)

    regis.rex_json.save_file(output, json)
            

def parse_map(filepath):
    map_constants_file = 'D:\\Engines\\Rex\\_docs\\data\\pokemon\\reference\\gen1\\pokered\\constants\\map_constants.asm'
    map_names_file = 'D:\\Engines\\Rex\\_docs\\data\\pokemon\\reference\\gen1\\pokered\\data\\maps\\names.asm'
    map_dimensions = {}
    with open(map_constants_file) as f:
        lines = f.readlines()
        for line in lines:
            line = line.strip()            
            if line.startswith('map_const'):
                line = line.removeprefix('map_const ')
                line = line[0:line.find(';')]
                line = line.strip()
                line = line.replace(' ', '')
                splitted = line.split(',')
                name = splitted[0]
                width = int(splitted[1])
                height = int(splitted[2])
                map_dimensions[name] = (width, height)

    map_id_to_name = {}
    with open(map_names_file) as f:
        lines = f.readlines()
        for line in lines:
            line = line.strip()
            line = line.replace('@', '')
            id = line[0:line.find('Name:')]
            map_name = line[line.find('"'):]
            map_name = map_name.replace('"', '')
            map_id_to_name[id] = map_name

    name = ''
    map_blocks = ''
    width = -1
    height = -1
    tileset = ''
    connections = []
    border_block = 0
    objects = []
    object_events = []
    bg_events = []
    warps = []
    texts = {}
    scripts = []
    is_reading_text = False
    is_processing_script = False
    text_key = ''
    with open(filepath) as f:
        lines = f.readlines()

        for line in lines:
            line = line.strip()

            if len(line) == 0:
                continue

            if 'MAP OBJECT' in line:
                continue

            if 'MAP HEADER' in line:
                continue

            line_og = line
            line = line.lower()

            if line.endswith("text::"):
                text_key = line_og
                text_key = text_key.removeprefix('_')
                text_key = text_key.removesuffix('Text::')
                texts[text_key] = []
                is_reading_text = True
                continue

# DEF text   EQUS "db TX_START,"    ; Start writing text.
# DEF next   EQUS "db \"<NEXT>\","  ; Move a line down. This is often used for UI, not for dialogue
# DEF line   EQUS "db \"<LINE>\","  ; Start writing at the bottom line.
# DEF para   EQUS "db \"<PARA>\","  ; Start a new paragraph. This clears the text box when started
# DEF cont   EQUS "db \"<CONT>\","  ; Scroll to the next line when all characters of the previous are written
# DEF done   EQUS "db \"<DONE>\""   ; End a text box.
# DEF prompt EQUS "db \"<PROMPT>\"" ; Prompt the player to end a text box (initiating some other event).

# DEF page   EQUS "db \"<PAGE>\","         ; Start a new Pokédex page.
# DEF dex    EQUS "db \"<DEXEND>\", \"@\"" ; End a Pokédex entry.

            # text
            if is_reading_text:
                if line == 'done':
                    is_reading_text = False
                    continue

                if line == 'text_end':
                    is_reading_text = False
                    continue

                line = line.replace('"', '')
                if line.startswith('text'):
                    entry = {}
                    entry['text'] = line_og.removeprefix('text ').replace('"', '')
                    entry['type'] = 'text'
                    texts[text_key].append(entry)
                    continue

                if line.startswith('next'):
                    entry = {}
                    entry['text'] = line_og.removeprefix('next ').replace('"', '')
                    entry['type'] = 'next'
                    texts[text_key].append(entry)
                    continue

                if line.startswith('line'):
                    entry = {}
                    entry['text'] = line_og.removeprefix('line ').replace('"', '')
                    entry['type'] = 'line'
                    texts[text_key].append(entry)
                    continue

                if line.startswith('para'):
                    entry = {}
                    entry['text'] = line_og.removeprefix('para ').replace('"', '')
                    entry['type'] = 'para'
                    texts[text_key].append(entry)
                    continue

                if line.startswith('cont'):
                    entry = {}
                    entry['text'] = line_og.removeprefix('cont ').replace('"', '')
                    entry['type'] = 'cont'
                    texts[text_key].append(entry)
                    continue

                if line.startswith('prompt'):
                    entry = {}
                    entry['text'] = line_og.removeprefix('prompt ').replace('"', '')
                    entry['type'] = 'prompt'
                    texts[text_key].append(entry)
                    continue

            # script
            if line.endswith('_scriptpointers:'):
                is_processing_script = True
                continue

            if is_processing_script:
                if line.startswith('dw_const'):
                    line = line.removeprefix('dx_const')
                    line = line.replace(' ', '')
                    scripts.append(line.split(',')[-1])

                if line.endswith(':'):
                    is_processing_script = False

            # name
            if line.endswith('_object:'):
                name = line_og.removesuffix('_Object:')
                name = map_id_to_name.get(name, name)
                continue

            # map blocks
            # blockset
            # tileset
            # width
            # height
            if line.startswith('map_header'): 
                line = line_og
                line = line.replace(' ', '')
                splitted = line.removeprefix('map_header').split(',')
                # map blocks
                map_blocks = splitted[0]
                # dimensions
                map_name = splitted[1]
                dimensions = map_dimensions[map_name]
                width = dimensions[0]
                height = dimensions[1]
                # tilset
                tileset = splitted[2]
                continue
            # connections
            if line.startswith('connection'):
                line = line.replace(' ', '')
                line = line.removeprefix('connection')
                splitted = line.split(',')
                # dir
                dir = splitted[0]
                # map dest
                map_dest = splitted[1]
                # offset
                offset = splitted[3]
                connections.append(Connection(dir, map_dest, offset))

            # border block idx
            if 'border block' in line:
                dollar_idx = line.find('$')
                border_block = int(line[dollar_idx+1:line.find(' ', dollar_idx)], 16)
                continue

            # objects
            if "const_export" in line:
                object = line.removeprefix("const_export ")
                objects.append(object)
                continue

            # object events
            if line.startswith('object_event'):
                line = line.replace(' ', '')
                splitted = line.removeprefix('object_event').split(',')
                # x
                x = splitted[0].strip()
                # y
                y = splitted[1].strip()
                # sprite
                sprite = splitted[2].strip()
                # action
                action = splitted[3].strip()
                # dir
                dir = splitted[4].strip()
                # text
                text = splitted[5].strip()
                object_events.append(ObjectEvent(x, y, sprite, action, dir, text))
                continue

            # bg events
            if line.startswith('bg_event'):
                line = line.replace(' ', '')
                splitted = line.removeprefix('bg_event').split(',')
                # x
                x = splitted[0].strip()
                # y
                y = splitted[1].strip()
                # bg text
                bg_text = splitted[2].strip()
                bg_events.append(BgEvent(x, y, bg_text))
                continue
                
            # warps
            if line.startswith('warp_event'):
                line = line.replace(' ', '')
                splitted = line.removeprefix('warp_event').split(',')
                # x
                x = splitted[0].strip()
                # y
                y = splitted[1].strip()
                # destination map ID
                dest_map_id = splitted[2].strip()
                # destination warp ID
                dest_warp_id = splitted[3].strip()
                warps.append(Warp(x, y, dest_map_id, dest_warp_id))
                continue

            

        output = os.path.dirname(filepath)
        output = os.path.join(output, os.path.pardir, 'maps3', f'{Path(filepath).stem}.json')
        save_as_json(name, map_blocks, width, height, tileset, connections, border_block, objects, object_events, bg_events, warps, texts, scripts, output)

if __name__ == '__main__':
    dir = 'D:\\Engines\\Rex\\data\\Pokemon\\maps2'

    test_map = 'D:\\Engines\\Rex\\data\\Pokemon\\maps2\\PalletTown.map'
    parse_map(test_map)
    
    files = os.listdir(dir)
    for file in files:
        parse_map(os.path.join(dir, file))