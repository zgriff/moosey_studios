var customMapFormat = {
    name: "Roshamboogie map format",
    extension: "json",

    write: function(map, fileName) {
        var m = {
            width: map.width,
            height: map.height,
            boundaries: [],
            tiles: [],
            swap_stations: [],
            decorations: [],
            egg_spawn_locations: [],
            orb_spawn_locations: [],
            player_spawn_locations: []
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
                break;
              case "GameObjects":
                for(obj of layer.objects){
                  dest = []
                  switch(obj.name){
                    case "swap_station":
                      dest = m.swap_stations;
                      break;
                    case "egg_spawn":
                      dest = m.egg_spawn_locations;
                      break;
                    case "player_spawn":
                      dest = m.player_spawn_locations;
                      break;
                    default:
                      break;
                  }
                  dest.push({
                    x: obj.x,
                    y: obj.y
                  });
                }
                break;
              case "Background":
                break;
              default:
                break;
            }
            // if (layer.isTileLayer) {
            //     var rows = [];
            //     for (y = 0; y < layer.height; ++y) {
            //         var row = [];
            //         for (x = 0; x < layer.width; ++x)
            //             row.push(layer.cellAt(x, y).tileId);
            //         rows.push(row);
            //     }
            //     m.layers.push(rows);
            // }
        }

        var file = new TextFile(fileName, TextFile.WriteOnly);
        file.write(JSON.stringify(m));
        file.commit();
    }
};

tiled.registerMapFormat("roshamboogie", customMapFormat);
