use std::sync::{Mutex, OnceLock};

#[repr(C)]
struct Uuid7Ctx {
    _private: [u8; 0], // opaque
}

unsafe extern "C" {
    fn uuid7_create() -> *mut Uuid7Ctx;
    fn uuid7_destroy(ctx: *mut Uuid7Ctx);
    fn uuid7_generate(ctx: *mut Uuid7Ctx, out: *mut u8);
}

/// Generator UUID v7
pub struct Generator {
    ctx: *mut Uuid7Ctx,
}

impl Generator {
    pub fn new() -> Result<Self, String> {
        let ctx = unsafe { uuid7_create() };
        if ctx.is_null() {
            Err("Gagal membuat konteks uuid7".to_string())
        } else {
            Ok(Generator { ctx })
        }
    }

    pub fn generate(&mut self) -> [u8; 16] {
        let mut buf = [0u8; 16];
        unsafe {
            uuid7_generate(self.ctx, buf.as_mut_ptr());
        }
        buf
    }
}

impl Drop for Generator {
    fn drop(&mut self) {
        if !self.ctx.is_null() {
            unsafe { uuid7_destroy(self.ctx) };
            self.ctx = std::ptr::null_mut();
        }
    }
}

// Generator bisa dipindah antar thread (Send), tapi tidak bisa di-share
// secara bersamaan (&Generator antar thread) karena C state tidak thread-safe.
// Untuk concurrent use, bungkus dengan Arc<Mutex<Generator>>.
unsafe impl Send for Generator {}

static DEFAULT_GEN: OnceLock<Mutex<Generator>> = OnceLock::new();

fn default_gen() -> &'static Mutex<Generator> {
    DEFAULT_GEN.get_or_init(|| {
        Mutex::new(Generator::new().expect("uuid7: gagal membuat konteks"))
    })
}

/// Buat UUID v7 menggunakan generator bawaan yang di-inisialisasi otomatis.
/// Aman digunakan dari banyak thread secara bersamaan.
pub fn generate() -> [u8; 16] {
    default_gen().lock().unwrap().generate()
}

/// Format raw UUID bytes ke string standar RFC 4122:
/// xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
pub fn to_string(uuid: &[u8; 16]) -> String {
    const HX: &[u8] = b"0123456789abcdef";
    let mut buf = [0u8; 36];
    let mut j = 0usize;
    for (i, &b) in uuid.iter().enumerate() {
        if i == 4 || i == 6 || i == 8 || i == 10 {
            buf[j] = b'-';
            j += 1;
        }
        buf[j] = HX[(b >> 4) as usize];
        buf[j + 1] = HX[(b & 0xf) as usize];
        j += 2;
    }
    String::from_utf8(buf.to_vec()).unwrap()
}

/// Buat UUID v7 dan langsung kembalikan sebagai string berformat
/// xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx.
pub fn generate_string() -> String {
    to_string(&generate())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_version_and_monotonic() {
        let mut g = Generator::new().unwrap();
        let u1 = g.generate();
        let u2 = g.generate();

        assert_eq!(u1[6] >> 4, 0x7);
        assert_eq!(u2[6] >> 4, 0x7);

        assert!(u1 < u2, "UUID1 harus lebih kecil dari UUID2");
    }

    #[test]
    fn test_free_functions() {
        let u1 = generate();
        let u2 = generate();
        assert!(u1 < u2, "UUID1 harus lebih kecil dari UUID2");

        let s = generate_string();
        let parts: Vec<&str> = s.split('-').collect();
        assert_eq!(parts.len(), 5);
        assert_eq!(parts[2].chars().next().unwrap(), '7', "versi harus 7");
    }

    #[test]
    fn test_to_string_format() {
        let u = generate();
        let s = to_string(&u);
        let parts: Vec<&str> = s.split('-').collect();
        assert_eq!(parts.len(), 5);
        assert_eq!([parts[0].len(), parts[1].len(), parts[2].len(), parts[3].len(), parts[4].len()],
                   [8, 4, 4, 4, 12]);
    }
}
