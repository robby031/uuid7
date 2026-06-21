use uuid7::{generate_string, Generator};

fn main() {
    // Cara simpel: langsung panggil fungsi tanpa buat generator
    println!("=== Cara Simpel (tanpa setup) ===");
    for _ in 0..3 {
        println!("{}", generate_string());
    }

    // Cara manual: buat generator sendiri (untuk kontrol penuh)
    println!("\n=== Cara Manual (kontrol penuh) ===");
    let mut g = Generator::new().unwrap();
    for _ in 0..3 {
        let uuid = g.generate();
        println!("{}", uuid7::to_string(&uuid));
    }
}
