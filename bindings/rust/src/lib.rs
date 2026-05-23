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
}
