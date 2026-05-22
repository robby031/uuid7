const Module = require('./uuid7.js');
const { UUID7Generator } = require('./uuid7_wasm.js');

Module.onRuntimeInitialized = () => {
    const gen = new UUID7Generator(Module);

    console.log("UUID v7 WebAssembly Test");
    console.log("========================");

    let prev = null;
    let count = 5;

    for (let i = 0; i < count; i++) {
        const hex = gen.generateHex();
        const bytes = gen.generate();

        console.log(`${i + 1}: ${hex}`);

        if (prev && hex <= prev) {
            console.error(`ERROR: UUID tidak monoton naik!`);
            process.exit(1);
        }
        prev = hex;
    }

    gen.destroy();
    console.log(`\nBerhasil menghasilkan ${count} UUID monoton tanpa error.`);
};