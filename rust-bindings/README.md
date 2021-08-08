# Sunrise Sunset Calculator Rust Bindings

[![Build status](https://github.com/jacob-pro/sunrise-sunset-calculator/actions/workflows/rust.yml/badge.svg)](https://github.com/jacob-pro/sunrise-sunset-calculator/actions/workflows/cmake.yml)
[![crates.io](https://img.shields.io/crates/v/sunrise-sunset-calculator.svg)](https://crates.io/crates/sunrise-sunset-calculator)
[![docs.rs](https://docs.rs/sunrise-sunset-calculator/badge.svg)](https://docs.rs/crate/sunrise-sunset-calculator)

A crate that binds to the Sunrise Sunset Calculator C library, providing an idiomatic Rust interface.

If you are using IntelliJ Rust don't forget to enable `include!` 
[macro support](https://blog.jetbrains.com/rust/2021/04/08/intellij-rust-updates-for-2021-1/#proc-macros) 
to get code completion.

## Safety

This crate uses the `unsafe` keyword is used because it links to a C implementation. However the C library itself does 
not make any memory allocations, use multithreading, handle user input, or otherwise make any system calls, so safety 
should not be an issue.

## no_std

Like the calculator itself this crate can be used in `no_std` environments. Just make sure to use:

```
sunrise-sunset-calculator = { version = "0.1", default-features = false }
```

(The only difference is that `sunrise_sunset_calculator::SscError` will no longer implement `std::error::Error`)

## Example

```
$ cargo run --example sunrise-sunset-calculator -- lookup --location london

Using time: 1628381118
Using coordinates: 51.5073219, -0.1276474
Visible: false
Sun set at:     2021-08-07 20:37 (+01:00)
Sun rises at:   2021-08-08 05:35 (+01:00)
```

## Development on Windows

Building and publishing this crate uses Symlinks; if you are on Windows ensure that first you have enabled 
[suitable permissions](https://github.com/git-for-windows/git/wiki/Symbolic-Links) and that you have either
enabled Symlinks during the Git for Windows install or set `git config –global core.symlinks true`.
