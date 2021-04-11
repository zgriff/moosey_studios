const TILE_MAP = {
  2: "grass"
}

const DECORATION_MAP = {
  1: "tree",
  2: "tree2",
  3: "tree3"
}

const TILE_SIZE = 50;

var customMapFormat = {
    name: "Roshamboogie map format",
    extension: "json",

    write: function(map, fileName) {
      let flipY = function(y){
        return map.height * TILE_SIZE - y;
      };
        var m = {
            width: map.width,
            height: map.height,
            totalOrbs: 0,
            totalEggs: 0,
            boundaries: [],
            tiles: [],
            gameObjects: [],
            decorations: []
        };

        for (var i = 0; i < map.layerCount; ++i) {
            var layer = map.layerAt(i);
            switch(layer.name){
              case "Hitboxes":
                for(obj of layer.objects){
                  boundary = []
                  if(obj.shape == MapObject.Rectangle){
                    boundary = [{
                      x: obj.x,
                      y: flipY(obj.y)
                    },{
                      x: obj.x + obj.width,
                      y: flipY(obj.y)
                    },{
                      x: obj.x,
                      y: flipY(obj.y + obj.height)
                    },{
                      x: obj.x + obj.width,
                      y: flipY(obj.y + obj.height)
                    }];
                  }else if(obj.shape == MapObject.Polygon){
                    for(point of obj.polygon){
                      boundary.push({x: point.x + obj.x, y: flipY(point.y + obj.y)});
                    }
                  }
                  m.boundaries.push(boundary)
                }
                break;
              case "Decorations":
                for(obj of layer.objects){
                  if(obj.tile.id in DECORATION_MAP){
                    m.decorations.push({
                      x: obj.x + obj.width/2,
                      y: flipY(obj.y - obj.height / 2),
                      asset: DECORATION_MAP[obj.tile.id]
                    });
                  }
                }
                m.decorations.sort(function(e1, e2){
                  return e2.y - e1.y;
                });
                break;
              case "GameObjects":
                for(obj of layer.objects){
                  if(obj.name == "egg_spawn"){
                    m.totalEggs += 1;
                  }else if(obj.name == "active_orb" || obj.name == "inactive_orb"){
                    m.totalOrbs += 1;
                  }
                  m.gameObjects.push({
                    x: obj.x,
                    y: flipY(obj.y),
                    type: obj.name
                  });
                }
                break;
              case "Background":
                for(let y = 0; y < layer.height; ++y){
                  for(let x = 0; x < layer.width; ++x){
                    if(layer.tileAt(x,y) != null){
                      m.tiles.push({
                        x: x,
                        y: map.height - y,
                        asset: TILE_MAP[layer.tileAt(x,y).id]
                      });
                    }
                  }
                }
                break;
              default:
                break;
            }
        }

        var file = new TextFile(fileName, TextFile.WriteOnly);
        file.write(JSON.stringify(m));
        file.commit();
    }
};

tiled.registerMapFormat("roshamboogie", customMapFormat);
