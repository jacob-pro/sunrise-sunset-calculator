#!/usr/bin/env rust-script
//! ```cargo
//! [dependencies]
//! bindgen = "=0.60.1"
//! ```
fn main() {
    let bindings = bindgen::Builder::default()
        .header("sunrise-sunset-calculator/include/ssc.h")
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        .derive_default(true)
        .ctypes_prefix("cty")
        .generate()
        .expect("Unable to generate bindings");
    bindings
        .write_to_file("c/binding.rs")
        .expect("Couldn't write bindings!");
}
