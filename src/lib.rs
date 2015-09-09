extern crate libc;

use libc::{c_void, c_int};//, c_char, c_float};//, c_ulong, c_long, c_uint, c_uchar, size_t};

pub type RustCb = extern fn(data : *mut c_void);
pub type PressedCb = extern fn(data : *mut c_void, device : c_int, x : c_int, y : c_int);

//#[repr(C)]
//pub struct Evas_Object;


extern "C" {
}


