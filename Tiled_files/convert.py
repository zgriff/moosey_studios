import sys
import json
import os

import pytmx

f = sys.argv[1]

# tmx = ulvl.TMX.load(f)
tmx = pytmx.TiledMap(f)

out = {}

out["width"] = tmx.width
out["height"] = tmx.height
out["boundaries"] = [];
out["tiles"] = [];
out["swap_stations"] = [];
out["decorations"] = [];
out["egg_spawn_locations"] = [];
out["orb_spawn_locations"] = [];
out["player_spawn_locations"] = [];

# write out filename.tmx to filename.json
raw_name = os.path.splitext(os.path.basename(f))[0]
out_path = os.path.join(os.path.dirname(f), "..", "..", "Roshamboogie", "assets", "maps", raw_name+".json")
out_path = os.path.normpath(out_path)
with open(out_path, 'w') as fp:
    json.dump(out, fp)
