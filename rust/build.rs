use std::env;
use std::path::PathBuf;

fn main() {
    // Configure and generate bindings.
    let bindings = bindgen::builder()
        .header("../inc/libnet.h")
        //.clang_arg("-x").clang_arg("c++")
        .generate()
        .expect("Couldn't write bindings!");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());

    // Write the generated bindings to an output file.
    bindings
        .write_to_file(out_path.join("bindings.rs"))//"../inc/libnet.rs" 好家伙生成的文件达20多MB，66万行之多，运行也得几分钟。
        .expect("Couldn't write bindings!");

        // cxx_build::bridge("src/main.rs")  // returns a cc::Build
        // .file("../inc/libnet.h")
        // .flag_if_supported("-std=c++11")
        // .compile("cxxbridge-demo");
}
