var VOX = require('./vox');

var parser = new VOX.Parser();

let files = [
   'body.vox',
   'body1.vox',
   'body2.vox',
   'body3.vox',
   'body4.vox',
   'body5.vox',
   'boot.vox',
   // 'dwarf-body-female.vox',
   // 'dwarf-body.vox',
   // 'dwarf-hair-base-female.vox',
   // 'dwarf-hair-base-male.vox',
   // 'dwarf-hair-f01.vox',
   // 'dwarf-hair-f02.vox',
   // 'dwarf-hair-f03.vox',
   // 'dwarf-hair-f04.vox',
   // 'dwarf-hair-f05.vox',
   // 'dwarf-hair-m01.vox',
   // 'dwarf-hair-m02.vox',
   // 'dwarf-hair-m03.vox',
   // 'dwarf-head-f01.vox',
   // 'dwarf-head-f02.vox',
   // 'dwarf-head-f03.vox',
   // 'dwarf-head-f04.vox',
   // 'dwarf-head-f05.vox',
   // 'dwarf-head-female.vox',
   // 'dwarf-head-m01.vox',
   // 'dwarf-head-m02.vox',
   // 'dwarf-head-m03.vox',
   // 'dwarf-head-m04.vox',
   // 'dwarf-head-m05.vox',
   // 'dwarf-head-male.vox',
   // 'dwarf-head-temp.vox',
   // 'dwarf-head.vox',
   'elf-hair-base.vox',
   'elf-hair-f01.vox',
   'elf-hair-f02.vox',
   'elf-hair-f03.vox',
   'elf-hair-f04.vox',
   'elf-hair-f05.vox',
   'elf-hair-f06.vox',
   'elf-hair-f07.vox',
   'elf-hair-f08.vox',
   'elf-hair-f09.vox',
   'elf-hair-f10.vox',
   'elf-hair-m01.vox',
   'elf-hair-m02.vox',
   'elf-hair-m03.vox',
   'elf-hair-m04.vox',
   'elf-hair-m05.vox',
   'elf-hair-m06.vox',
   'elf-hair-m07.vox',
   'elf-hair-m08.vox',
   'elf-hair-m09.vox',
   'elf-hair-m10.vox',
   'elf-head-dark.vox',
   'elf-head-f01.vox',
   'elf-head-f02.vox',
   'elf-head-f03.vox',
   'elf-head-f04.vox',
   'elf-head-f05.vox',
   'elf-head-f06.vox',
   'elf-head-female.vox',
   'elf-head-m01.vox',
   'elf-head-m02.vox',
   'elf-head-m03.vox',
   'elf-head-m04.vox',
   'foot.vox',
   // 'frogman-hair-base.vox',
   // 'frogman-hair-f01.vox',
   // 'frogman-hair-f02.vox',
   // 'frogman-hair-f03.vox',
   // 'frogman-hair-f04.vox',
   // 'frogman-hair-m01.vox',
   // 'frogman-hair-m02.vox',
   // 'frogman-hair-m03.vox',
   // 'frogman-hair-m04.vox',
   // 'frogman-hair-m05.vox',
   // 'frogman-hand.vox',
   // 'frogman-head-f01.vox',
   // 'frogman-head-f02.vox',
   // 'frogman-head-f03.vox',
   // 'frogman-head-f04.vox',
   // 'frogman-head-m01.vox',
   // 'frogman-head-m02.vox',
   // 'frogman-head-m03.vox',
   // 'frogman-head-m04.vox',
   // 'frogman-head-m05.vox',
   // 'goblin-hair-base-female.vox',
   // 'goblin-hair-base-male.vox',
   // 'goblin-hair-f01.vox',
   // 'goblin-hair-f02.vox',
   // 'goblin-hair-f03.vox',
   // 'goblin-hair-f04.vox',
   // 'goblin-hair-f05.vox',
   // 'goblin-hair-f06.vox',
   // 'goblin-hair-m01.vox',
   // 'goblin-hair-m02.vox',
   // 'goblin-hair-m03.vox',
   // 'goblin-hair-m04.vox',
   // 'goblin-hair-m05.vox',
   // 'goblin-hair-m06.vox',
   // 'goblin-hand.vox',
   // 'goblin-head-f01.vox',
   // 'goblin-head-f02.vox',
   // 'goblin-head-f03.vox',
   // 'goblin-head-f04.vox',
   // 'goblin-head-f05.vox',
   // 'goblin-head-m01.vox',
   // 'goblin-head-m02.vox',
   // 'goblin-head-m03.vox',
   // 'goblin-head-m04.vox',
   // 'goblin-head-m05.vox',
   // 'goblin-head.vox',
   // 'hand.vox',
   'hand2.vox',
   // 'head1.vox',
   // 'head2.vox',
   // 'head3.vox',
   'human-hair-base.vox',
   'human-hair-f01.vox',
   'human-hair-f02.vox',
   'human-hair-f03.vox',
   'human-hair-f04.vox',
   'human-hair-f05.vox',
   'human-hair-f06.vox',
   'human-hair-f07.vox',
   'human-hair-m01.vox',
   'human-hair-m02.vox',
   'human-hair-m03.vox',
   'human-hair-m04.vox',
   'human-hair-m05.vox',
   'human-hair-m06.vox',
   'human-hair-m07.vox',
   'human-hair-m08.vox',
   'human-hair-m09.vox',
   'human-hair-m10.vox',
   'human-hair-m11.vox',
   'human-hair-m12.vox',
   'human-hair-m13.vox',
   'human-hair-m14.vox',
   'human-hair-m15.vox',
   'human-head-f01.vox',
   'human-head-f02.vox',
   'human-head-f03.vox',
   'human-head-f04.vox',
   'human-head-f05.vox',
   'human-head-f06.vox',
   'human-head-m01.vox',
   'human-head-m02.vox',
   'human-head-m03.vox',
   'human-head-m04.vox',
   'human-head-m05.vox',
   'human-head-m06.vox',
   // 'lizard-body.vox',
   // 'lizard-foot.vox',
   // 'lizard-hair-base-female.vox',
   // 'lizard-hair-base.vox',
   // 'lizard-hair-m01.vox',
   // 'lizard-hair-m02.vox',
   // 'lizard-hair-m03.vox',
   // 'lizard-hair-m04.vox',
   // 'lizard-hair-m05.vox',
   // 'lizard-hair-m06.vox',
   // 'lizard-hand.vox',
   // 'lizard-head-f01.vox',
   // 'lizard-head-f02.vox',
   // 'lizard-head-f03.vox',
   // 'lizard-head-f04.vox',
   // 'lizard-head-f05.vox',
   // 'lizard-head-m01.vox',
   // 'lizard-head-m02.vox',
   // 'mermaid-body.vox',
   // 'mermaid-hair-f01.vox',
   // 'mermaid-hair-f02.vox',
   // 'mermaid-hair-f03.vox',
   // 'mermaid-hand.vox',
   // 'mermaid-head-f01.vox',
   // 'mermaid-head-f02.vox',
   // 'mermaid-head-f03.vox',
   // 'merman-body.vox',
   // 'merman-hair-m01.vox',
   // 'merman-hair-m02.vox',
   // 'merman-hair-m03.vox',
   // 'merman-head-m01.vox',
   // 'merman-head-m02.vox',
   // 'merman-head-m03.vox',
   // 'orc-hair-base-female.vox',
   // 'orc-hair-base.vox',
   // 'orc-hair-f01.vox',
   // 'orc-hair-f02.vox',
   // 'orc-hair-f03.vox',
   // 'orc-hair-f04.vox',
   // 'orc-hair-m01.vox',
   // 'orc-hair-m02.vox',
   // 'orc-hair-m03.vox',
   // 'orc-hair-m04.vox',
   // 'orc-hair-m05.vox',
   // 'orc-hair-m06.vox',
   // 'orc-hair-m07.vox',
   // 'orc-hand.vox',
   // 'orc-head-f.vox',
   // 'orc-head-f01.vox',
   // 'orc-head-f02.vox',
   // 'orc-head-f03.vox',
   // 'orc-head-f04.vox',
   // 'orc-head-f05.vox',
   // 'orc-head-f2.vox',
   // 'orc-head-m01.vox',
   // 'orc-head-m02.vox',
   // 'orc-head-m03.vox',
   // 'orc-head-m04.vox',
   // 'orc-head-m05.vox',
   // 'orc-head.vox',
   // 'undead-candle-skull.vox',
   // 'undead-candle1.vox',
   // 'undead-candle2.vox',
   // 'undead-candle3.vox',
   // 'undead-hair-base.vox',
   // 'undead-hair-f01.vox',
   // 'undead-hair-f02.vox',
   // 'undead-hair-f03.vox',
   // 'undead-hair-f04.vox',
   // 'undead-hair-f05.vox',
   // 'undead-hair-f06.vox',
   // 'undead-hair-m01.vox',
   // 'undead-hair-m02.vox',
   // 'undead-hair-m03.vox',
   // 'undead-hair-m04.vox',
   // 'undead-hair-m05.vox',
   // 'undead-hair-m06.vox',
   // 'undead-hand.vox',
   // 'undead-head-f01.vox',
   // 'undead-head-f02.vox',
   // 'undead-head-f03.vox',
   // 'undead-head-f04.vox',
   // 'undead-head-f05.vox',
   // 'undead-head-f06.vox',
   // 'undead-head-m01.vox',
   // 'undead-head-m02.vox',
   // 'undead-head-m03.vox',
   // 'undead-head-m04.vox',
   // 'undead-head-m05.vox',
   // 'undead-head-m06.vox',
   // 'undead-head.vox',
];
let palette = [];

// files = ['body4.vox'];

function Add(color) {
   if(!palette.find(c => {
      return c.r === color.r &&
             c.g === color.g &&
             c.b === color.b;
   }))
   {
      palette.push(color);
   }
}

function parseFile(i) {
   if (i >= files.length) {
      return {then: cb => cb()};
   }
   const filename = files[i];
   return parser.parse('./Assets/vox/' + filename).then(voxData => {
      voxData.palette.filter(c => c.r + c.g + c.b + c.a !== 0).forEach(c => {
         Add(c);
      });

      return parseFile(i + 1);
   });
}

function pad(str, length) {
   while (str.length < length) {
       str = '0' + str;
   }
  
   return str;
}

parseFile(0).then(() => {
   palette.sort((c1, c2) => {
      if (c1.a !== c2.a) return c1.a - c2.a;
      if (c1.r !== c2.r) return c1.r - c2.r;
      if (c1.g !== c2.g) return c1.g - c2.g;
      if (c1.b !== c2.b) return c1.b - c2.b;
      console.error('this shouldnt happen');
      return 0;
   });

   while (palette.length < 253)
   {
      palette.unshift({ r: 0, g: 0, b: 0, a: 255 });
   }

   palette.unshift({ r: 255, g: 255, b: 255, a: 255 });
   palette.unshift({ r: 0, g: 0, b: 0, a: 0 });
   palette.push({ r: 0, g: 0, b: 0, a: 0 });

   return palette;
}).then(palette => {
   return palette.map(color => {
      return '0x' + pad(color.a.toString(16), 2) + pad(color.b.toString(16), 2) + pad(color.g.toString(16), 2) + pad(color.r.toString(16), 2)
   });
}).then(palette => {
   let res = [];
   for (let i = 0; i < 256; i += 16)
   {
      res.push(palette.slice(i, i + 16).join(', ') + ',');
   }
   return res.join('\n');
}).then(res => console.log(res));