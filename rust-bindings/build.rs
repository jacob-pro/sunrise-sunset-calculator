use std::path::PathBuf;

fn main() {
    // Builds the project in the directory located in `sunrise-sunset-calculator`, installing it into $OUT_DIR
    let dst = cmake::Config::new("sunrise-sunset-calculator")
        .build_target("ssc")
        .build();

    println!(
        "cargo:rustc-link-search={}",
        format!("{}/build/", dst.display())
    );
    println!("cargo:rustc-link-lib=static=ssc");

    let bindings = bindgen::Builder::default()
        .header("sunrise-sunset-calculator/include/ssc.h")
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        .derive_default(true)
        .ctypes_prefix("cty")
        .generate()
        .expect("Unable to generate bindings");

    let out_path = PathBuf::from(std::env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("ssc.rs"))
        .expect("Couldn't write bindings!");
}
