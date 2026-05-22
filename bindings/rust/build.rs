fn main() {
    cc::Build::new()
        .include("csrc")
        .file("csrc/uuid7_core.c")
        .file("csrc/clock.c")
        .file("csrc/prng.c")
        .compile("uuid7");

    println!("cargo:rerun-if-changed=csrc/uuid7.h");
    println!("cargo:rerun-if-changed=csrc/internal.h");
    println!("cargo:rerun-if-changed=csrc/uuid7_core.c");
    println!("cargo:rerun-if-changed=csrc/clock.c");
    println!("cargo:rerun-if-changed=csrc/prng.c");
}
