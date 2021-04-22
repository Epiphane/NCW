/*
 *  The double-to-int coercion for NaN and +/- Inf is platform specific.
 *  Similarly values outside the 48-bit range may coerce to an integer
 *  in a platform specific manner.
 *
 *  (Node.js rejects such values when noAssert==false, Duktape doesn't.
 *  This testcase uses noAssert=true.)
 *
 *  Current Duktape coercion for these is potentially platform specific
 *  and differs from Node.js coercion.
 */

/*@include util-buffer.js@*/

/* XXX: skip until a reasonable expect string exists. */
/*---
{
    "custom": true,
    "skip": true
}
---*/

/*===
special write values
-Infinity 1 00000000000000000000000000000000
-Infinity 1 00000000000000000000000000000000
-Infinity 1 00000000000000000000000000000000
-Infinity 1 00000000000000000000000000000000
-Infinity 2 00000000000000000000000000000000
-Infinity 2 00000000000000000000000000000000
-Infinity 2 0000ff00000000000000000000000000
-Infinity 2 00ff0000000000000000000000000000
-Infinity 3 00000000000000000000000000000000
-Infinity 3 00000000000000000000000000000000
-Infinity 3 0000ffff000000000000000000000000
-Infinity 3 00ffff00000000000000000000000000
-Infinity 4 00000000000000000000000000000000
-Infinity 4 00000000000000000000000000000000
-Infinity 4 0000ffffff0000000000000000000000
-Infinity 4 00ffffff000000000000000000000000
-Infinity 5 00000000000000000000000000000000
-Infinity 5 00000000000000000000000000000000
-Infinity 5 0000ffffffff00000000000000000000
-Infinity 5 00ffffffff0000000000000000000000
-Infinity 6 00000000000000000000000000000000
-Infinity 6 00000000000000000000000000000000
-Infinity 6 0000ffffffffff000000000000000000
-Infinity 6 00ffffffffff00000000000000000000
Infinity 1 00000000000000000000000000000000
Infinity 1 00000000000000000000000000000000
Infinity 1 00000000000000000000000000000000
Infinity 1 00000000000000000000000000000000
Infinity 2 00000000000000000000000000000000
Infinity 2 00000000000000000000000000000000
Infinity 2 00000000000000000000000000000000
Infinity 2 00000000000000000000000000000000
Infinity 3 00000000000000000000000000000000
Infinity 3 00000000000000000000000000000000
Infinity 3 00000000000000000000000000000000
Infinity 3 00000000000000000000000000000000
Infinity 4 00000000000000000000000000000000
Infinity 4 00000000000000000000000000000000
Infinity 4 00000000000000000000000000000000
Infinity 4 00000000000000000000000000000000
Infinity 5 00000000000000000000000000000000
Infinity 5 00000000000000000000000000000000
Infinity 5 00000000000000000000000000000000
Infinity 5 00000000000000000000000000000000
Infinity 6 00000000000000000000000000000000
Infinity 6 00000000000000000000000000000000
Infinity 6 00000000000000000000000000000000
Infinity 6 00000000000000000000000000000000
NaN 1 00000000000000000000000000000000
NaN 1 00000000000000000000000000000000
NaN 1 00000000000000000000000000000000
NaN 1 00000000000000000000000000000000
NaN 2 00000000000000000000000000000000
NaN 2 00000000000000000000000000000000
NaN 2 00000000000000000000000000000000
NaN 2 00000000000000000000000000000000
NaN 3 00000000000000000000000000000000
NaN 3 00000000000000000000000000000000
NaN 3 00000000000000000000000000000000
NaN 3 00000000000000000000000000000000
NaN 4 00000000000000000000000000000000
NaN 4 00000000000000000000000000000000
NaN 4 00000000000000000000000000000000
NaN 4 00000000000000000000000000000000
NaN 5 00000000000000000000000000000000
NaN 5 00000000000000000000000000000000
NaN 5 00000000000000000000000000000000
NaN 5 00000000000000000000000000000000
NaN 6 00000000000000000000000000000000
NaN 6 00000000000000000000000000000000
NaN 6 00000000000000000000000000000000
NaN 6 00000000000000000000000000000000
-393991379165713 1 00ef0000000000000000000000000000
-393991379165713 1 00ef0000000000000000000000000000
-393991379165713 1 00ef0000000000000000000000000000
-393991379165713 1 00ef0000000000000000000000000000
-393991379165713 2 00efde00000000000000000000000000
-393991379165713 2 00deef00000000000000000000000000
-393991379165713 2 00efdd00000000000000000000000000
-393991379165713 2 00ddef00000000000000000000000000
-393991379165713 3 00efdecd000000000000000000000000
-393991379165713 3 00cddeef000000000000000000000000
-393991379165713 3 00efddcc000000000000000000000000
-393991379165713 3 00ccddef000000000000000000000000
-393991379165713 4 00efdecdbc0000000000000000000000
-393991379165713 4 00bccddeef0000000000000000000000
-393991379165713 4 00efddccbb0000000000000000000000
-393991379165713 4 00bbccddef0000000000000000000000
-393991379165713 5 00efdecdbcab00000000000000000000
-393991379165713 5 00abbccddeef00000000000000000000
-393991379165713 5 00efddccbbaa00000000000000000000
-393991379165713 5 00aabbccddef00000000000000000000
-393991379165713 6 00efdecdbcab9a000000000000000000
-393991379165713 6 009aabbccddeef000000000000000000
-393991379165713 6 00efddccbbaa99000000000000000000
-393991379165713 6 0099aabbccddef000000000000000000
-281474976710656 1 00000000000000000000000000000000
-281474976710656 1 00000000000000000000000000000000
-281474976710656 1 00000000000000000000000000000000
-281474976710656 1 00000000000000000000000000000000
-281474976710656 2 00000000000000000000000000000000
-281474976710656 2 00000000000000000000000000000000
-281474976710656 2 0000ff00000000000000000000000000
-281474976710656 2 00ff0000000000000000000000000000
-281474976710656 3 00000000000000000000000000000000
-281474976710656 3 00000000000000000000000000000000
-281474976710656 3 0000ffff000000000000000000000000
-281474976710656 3 00ffff00000000000000000000000000
-281474976710656 4 00000000000000000000000000000000
-281474976710656 4 00000000000000000000000000000000
-281474976710656 4 0000ffffff0000000000000000000000
-281474976710656 4 00ffffff000000000000000000000000
-281474976710656 5 00000000000000000000000000000000
-281474976710656 5 00000000000000000000000000000000
-281474976710656 5 0000ffffffff00000000000000000000
-281474976710656 5 00ffffffff0000000000000000000000
-281474976710656 6 00000000000000000000000000000000
-281474976710656 6 00000000000000000000000000000000
-281474976710656 6 0000ffffffffff000000000000000000
-281474976710656 6 00ffffffffff00000000000000000000
-281474976710655 1 00010000000000000000000000000000
-281474976710655 1 00010000000000000000000000000000
-281474976710655 1 00010000000000000000000000000000
-281474976710655 1 00010000000000000000000000000000
-281474976710655 2 00010100000000000000000000000000
-281474976710655 2 00010100000000000000000000000000
-281474976710655 2 00010000000000000000000000000000
-281474976710655 2 00000100000000000000000000000000
-281474976710655 3 00010101000000000000000000000000
-281474976710655 3 00010101000000000000000000000000
-281474976710655 3 00010000000000000000000000000000
-281474976710655 3 00000001000000000000000000000000
-281474976710655 4 00010101010000000000000000000000
-281474976710655 4 00010101010000000000000000000000
-281474976710655 4 00010000000000000000000000000000
-281474976710655 4 00000000010000000000000000000000
-281474976710655 5 00010101010100000000000000000000
-281474976710655 5 00010101010100000000000000000000
-281474976710655 5 00010000000000000000000000000000
-281474976710655 5 00000000000100000000000000000000
-281474976710655 6 00010101010101000000000000000000
-281474976710655 6 00010101010101000000000000000000
-281474976710655 6 00010000000000000000000000000000
-281474976710655 6 00000000000001000000000000000000
-140737488355329 1 00ff0000000000000000000000000000
-140737488355329 1 00ff0000000000000000000000000000
-140737488355329 1 00ff0000000000000000000000000000
-140737488355329 1 00ff0000000000000000000000000000
-140737488355329 2 00ff0000000000000000000000000000
-140737488355329 2 0000ff00000000000000000000000000
-140737488355329 2 00ffff00000000000000000000000000
-140737488355329 2 00ffff00000000000000000000000000
-140737488355329 3 00ff0000000000000000000000000000
-140737488355329 3 000000ff000000000000000000000000
-140737488355329 3 00ffffff000000000000000000000000
-140737488355329 3 00ffffff000000000000000000000000
-140737488355329 4 00ff0000000000000000000000000000
-140737488355329 4 00000000ff0000000000000000000000
-140737488355329 4 00ffffffff0000000000000000000000
-140737488355329 4 00ffffffff0000000000000000000000
-140737488355329 5 00ff0000000000000000000000000000
-140737488355329 5 0000000000ff00000000000000000000
-140737488355329 5 00ffffffffff00000000000000000000
-140737488355329 5 00ffffffffff00000000000000000000
-140737488355329 6 00ff0000000080000000000000000000
-140737488355329 6 008000000000ff000000000000000000
-140737488355329 6 00ffffffffff7f000000000000000000
-140737488355329 6 007fffffffffff000000000000000000
-140737488355328 1 00000000000000000000000000000000
-140737488355328 1 00000000000000000000000000000000
-140737488355328 1 00000000000000000000000000000000
-140737488355328 1 00000000000000000000000000000000
-140737488355328 2 00000000000000000000000000000000
-140737488355328 2 00000000000000000000000000000000
-140737488355328 2 0000ff00000000000000000000000000
-140737488355328 2 00ff0000000000000000000000000000
-140737488355328 3 00000000000000000000000000000000
-140737488355328 3 00000000000000000000000000000000
-140737488355328 3 0000ffff000000000000000000000000
-140737488355328 3 00ffff00000000000000000000000000
-140737488355328 4 00000000000000000000000000000000
-140737488355328 4 00000000000000000000000000000000
-140737488355328 4 0000ffffff0000000000000000000000
-140737488355328 4 00ffffff000000000000000000000000
-140737488355328 5 00000000000000000000000000000000
-140737488355328 5 00000000000000000000000000000000
-140737488355328 5 0000ffffffff00000000000000000000
-140737488355328 5 00ffffffff0000000000000000000000
-140737488355328 6 00000000000080000000000000000000
-140737488355328 6 00800000000000000000000000000000
-140737488355328 6 0000ffffffff7f000000000000000000
-140737488355328 6 007fffffffff00000000000000000000
140737488355327 1 00ff0000000000000000000000000000
140737488355327 1 00ff0000000000000000000000000000
140737488355327 1 00ff0000000000000000000000000000
140737488355327 1 00ff0000000000000000000000000000
140737488355327 2 00ffff00000000000000000000000000
140737488355327 2 00ffff00000000000000000000000000
140737488355327 2 00ffff00000000000000000000000000
140737488355327 2 00ffff00000000000000000000000000
140737488355327 3 00ffffff000000000000000000000000
140737488355327 3 00ffffff000000000000000000000000
140737488355327 3 00ffffff000000000000000000000000
140737488355327 3 00ffffff000000000000000000000000
140737488355327 4 00ffffffff0000000000000000000000
140737488355327 4 00ffffffff0000000000000000000000
140737488355327 4 00ffffffff0000000000000000000000
140737488355327 4 00ffffffff0000000000000000000000
140737488355327 5 00ffffffffff00000000000000000000
140737488355327 5 00ffffffffff00000000000000000000
140737488355327 5 00ffffffffff00000000000000000000
140737488355327 5 00ffffffffff00000000000000000000
140737488355327 6 00ffffffffff7f000000000000000000
140737488355327 6 007fffffffffff000000000000000000
140737488355327 6 00ffffffffff7f000000000000000000
140737488355327 6 007fffffffffff000000000000000000
140737488355328 1 00000000000000000000000000000000
140737488355328 1 00000000000000000000000000000000
140737488355328 1 00000000000000000000000000000000
140737488355328 1 00000000000000000000000000000000
140737488355328 2 00000000000000000000000000000000
140737488355328 2 00000000000000000000000000000000
140737488355328 2 00000000000000000000000000000000
140737488355328 2 00000000000000000000000000000000
140737488355328 3 00000000000000000000000000000000
140737488355328 3 00000000000000000000000000000000
140737488355328 3 00000000000000000000000000000000
140737488355328 3 00000000000000000000000000000000
140737488355328 4 00000000000000000000000000000000
140737488355328 4 00000000000000000000000000000000
140737488355328 4 00000000000000000000000000000000
140737488355328 4 00000000000000000000000000000000
140737488355328 5 00000000000000000000000000000000
140737488355328 5 00000000000000000000000000000000
140737488355328 5 00000000000000000000000000000000
140737488355328 5 00000000000000000000000000000000
140737488355328 6 00000000000080000000000000000000
140737488355328 6 00800000000000000000000000000000
140737488355328 6 00000000000080000000000000000000
140737488355328 6 00800000000000000000000000000000
281474976710655 1 00ff0000000000000000000000000000
281474976710655 1 00ff0000000000000000000000000000
281474976710655 1 00ff0000000000000000000000000000
281474976710655 1 00ff0000000000000000000000000000
281474976710655 2 00ffff00000000000000000000000000
281474976710655 2 00ffff00000000000000000000000000
281474976710655 2 00ffff00000000000000000000000000
281474976710655 2 00ffff00000000000000000000000000
281474976710655 3 00ffffff000000000000000000000000
281474976710655 3 00ffffff000000000000000000000000
281474976710655 3 00ffffff000000000000000000000000
281474976710655 3 00ffffff000000000000000000000000
281474976710655 4 00ffffffff0000000000000000000000
281474976710655 4 00ffffffff0000000000000000000000
281474976710655 4 00ffffffff0000000000000000000000
281474976710655 4 00ffffffff0000000000000000000000
281474976710655 5 00ffffffffff00000000000000000000
281474976710655 5 00ffffffffff00000000000000000000
281474976710655 5 00ffffffffff00000000000000000000
281474976710655 5 00ffffffffff00000000000000000000
281474976710655 6 00ffffffffffff000000000000000000
281474976710655 6 00ffffffffffff000000000000000000
281474976710655 6 00ffffffffffff000000000000000000
281474976710655 6 00ffffffffffff000000000000000000
17592186044416 1 00000000000000000000000000000000
17592186044416 1 00000000000000000000000000000000
17592186044416 1 00000000000000000000000000000000
17592186044416 1 00000000000000000000000000000000
17592186044416 2 00000000000000000000000000000000
17592186044416 2 00000000000000000000000000000000
17592186044416 2 00000000000000000000000000000000
17592186044416 2 00000000000000000000000000000000
17592186044416 3 00000000000000000000000000000000
17592186044416 3 00000000000000000000000000000000
17592186044416 3 00000000000000000000000000000000
17592186044416 3 00000000000000000000000000000000
17592186044416 4 00000000000000000000000000000000
17592186044416 4 00000000000000000000000000000000
17592186044416 4 00000000000000000000000000000000
17592186044416 4 00000000000000000000000000000000
17592186044416 5 00000000000000000000000000000000
17592186044416 5 00000000000000000000000000000000
17592186044416 5 00000000000000000000000000000000
17592186044416 5 00000000000000000000000000000000
17592186044416 6 00000000000010000000000000000000
17592186044416 6 00100000000000000000000000000000
17592186044416 6 00000000000010000000000000000000
17592186044416 6 00100000000000000000000000000000
393991379165713 1 00110000000000000000000000000000
393991379165713 1 00110000000000000000000000000000
393991379165713 1 00110000000000000000000000000000
393991379165713 1 00110000000000000000000000000000
393991379165713 2 00112200000000000000000000000000
393991379165713 2 00221100000000000000000000000000
393991379165713 2 00112200000000000000000000000000
393991379165713 2 00221100000000000000000000000000
393991379165713 3 00112233000000000000000000000000
393991379165713 3 00332211000000000000000000000000
393991379165713 3 00112233000000000000000000000000
393991379165713 3 00332211000000000000000000000000
393991379165713 4 00112233440000000000000000000000
393991379165713 4 00443322110000000000000000000000
393991379165713 4 00112233440000000000000000000000
393991379165713 4 00443322110000000000000000000000
393991379165713 5 00112233445500000000000000000000
393991379165713 5 00554433221100000000000000000000
393991379165713 5 00112233445500000000000000000000
393991379165713 5 00554433221100000000000000000000
393991379165713 6 00112233445566000000000000000000
393991379165713 6 00665544332211000000000000000000
393991379165713 6 00112233445566000000000000000000
393991379165713 6 00665544332211000000000000000000
===*/

function specialWriteValuesTest() {
    [ -1/0, 1/0, 0/0,
      -0x1665544332211,    // -2^48
      -0x1000000000000,
      -0xffffffffffff,
      -0x800000000001,     // -2^47 - 1
      -0x800000000000,     // -2^47  (minimum 48-bit signed value)
      0x7fffffffffff,      // 2^47 - 1  (maximum 48-bit signed value)
      0x800000000000,
      0xffffffffffff,
      0x100000000000,
      0x1665544332211 ].forEach(function (value) {
        [ 1, 2, 3, 4, 5, 6 ].forEach(function (length) {
            var buf = new Buffer(16);

            buf.fill(0x00);
            buf.writeUIntLE(value, 1, length, true);
            print(value, length, printableNodejsBuffer(buf));

            buf.fill(0x00);
            buf.writeUIntBE(value, 1, length, true);
            print(value, length, printableNodejsBuffer(buf));

            buf.fill(0x00);
            buf.writeIntLE(value, 1, length, true);
            print(value, length, printableNodejsBuffer(buf));

            buf.fill(0x00);
            buf.writeIntBE(value, 1, length, true);
            print(value, length, printableNodejsBuffer(buf));
        });
    });
}

try {
    print('special write values');
    specialWriteValuesTest();
} catch (e) {
    print(e.stack || e);
}
