var kFloor = -0.25;

var kDeep = [0, 0, 128, 0.01];
var kShallow = [0, 0, 255, 0.025];
var kShore = [0, 128, 255, 0.05];
var kSand = [240, 240, 64, 0.1];
var kGrass = [32, 160, 0, 0.2];
var kDirt = [51, 33, 20, 0.75];
var kRock = [128, 128, 128, 0.76];
var kSnow = [255, 255, 255, 0.85];

var kGradient = [
   [0, 0, 128, 0],
   kDeep,
   kShallow,
   kShore,
   kSand,
   kDirt,
   kGrass,
   kRock,
   kSnow,
   [255, 255, 255, 1]
];

function ComputeColor(block, globalX, globalY, globalZ) {
   if (block.a === 0) {
      return;
   }

   var py = globalY / chunk.height;

   var source = kGradient[0];
   var dest = kGradient[1];
   var perc = 0;
   for (var i = kGradient.length - 2; i > 0; i--) {
      if (py > kGradient[i][3]) {
         source = kGradient[i - 1];
         dest = kGradient[i];
         perc = (py - source[3]) / (dest[3] - source[3]);
         break;
      }
   }

   block.r = perc * dest[0] + (1 - perc) * source[0];
   block.g = perc * dest[1] + (1 - perc) * source[1];
   block.b = perc * dest[2] + (1 - perc) * source[2];
}

function ComputeBlock(block, globalX, globalY, globalZ) {
   var noiseVal = Game.GetNoise(globalX, globalY, globalZ);

   var y = globalY;
   var py = ((y + 1) / chunk.height);

   var value = -Math.pow(py, 1.25);
   value += noiseVal;

   block.a = value > kFloor ? 255 : 0;
   if (y == 0) {
      block.a = 255;
   }

   ComputeColor(block, globalX, globalY, globalZ);
}

function Generate(chunkX, chunkY, chunkZ) {
   for (var y = 0; y < chunk.height; y++) {
      for (var x = 0; x < chunk.width; x++) {
         for (var z = 0; z < chunk.width; z++) {
            var block = chunk.Get(x, y, z);
            var globalX = (chunkX + 100) * chunk.width + x;
            var globalY = (chunkY +   0) * chunk.height + y;
            var globalZ = (chunkZ + 100) * chunk.depth + z;

            ComputeBlock(block, globalX, globalY, globalZ);
         }
      }
   }
}
