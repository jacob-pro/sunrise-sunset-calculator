fn main() {
    let dst = cmake::Config::new("sunrise-sunset-calculator")
        .build_target("ssc")
        .build();
    println!(
        "cargo:rustc-link-search={}",
        format!("{}/build/", dst.display())
    );
    println!("cargo:rustc-link-lib=static=ssc");
}
