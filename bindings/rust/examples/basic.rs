use uuid7::Generator;

fn main() {
    let mut g = Generator::new().unwrap();
    for _ in 0..3 {
        let uuid = g.generate();
        println!("{}", hex::encode(uuid));
    }
}