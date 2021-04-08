const TILE_MAP = {
  0: "grass"
}

const DECORATION_MAP = {
  0: "tree"
}

var customMapFormat = {
    name: "Roshamboogie map format",
    extension: "json",

    write: function(map, fileName) {
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
                      y: obj.y
                    },{
                      x: obj.x + obj.width,
                      y: obj.y
                    },{
                      x: obj.x,
                      y: obj.y + obj.height
                    },{
                      x: obj.x + obj.width,
                      y: obj.y + obj.height
                    }];
                  }else if(obj.shape == MapObject.Polygon){
                    for(point of obj.polygon){
                      boundary.push({x: point.x, y: point.y});
                    }
                  }
                  m.boundaries.push(boundary)
                }
                break;
              case "Decorations":
                for(obj of layer.objects){
                  m.decorations.push({
                    x: obj.x,
                    y: obj.y,
                    asset: DECORATION_MAP[obj.tile.id]
                  });
                }
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
                    y: obj.y,
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
                        y: y,
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
