const Module = require('./uuid7.js');
const uuid7 = require('./uuid7_wasm.js');

Module.onRuntimeInitialized = () => {
    // Cara simpel: init sekali, lalu langsung generate
    uuid7.init(Module);

    console.log("UUID v7 WebAssembly Test");
    console.log("========================");

    let prev = null;
    const count = 5;

    for (let i = 0; i < count; i++) {
        const s = uuid7.generateString();
        console.log(`${i + 1}: ${s}`);

        if (prev && s <= prev) {
            console.error("ERROR: UUID tidak monoton naik!");
            process.exit(1);
        }
        prev = s;
    }

    console.log(`\nBerhasil menghasilkan ${count} UUID monoton tanpa error.`);

    // Cara manual (tetap tersedia)
    const gen = new uuid7.UUID7Generator(Module);
    console.log("\nContoh via generator manual:", gen.generateString());
    gen.destroy();
};
