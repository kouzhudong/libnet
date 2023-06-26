// Suppress the flurry of warnings caused by using "C" naming conventions
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

//应该有yml的配置方法。
//bindgen对位的处理还不太好。
// #[link(name = "..\\x64\\Debug\\libnet")]
// extern "system"
// {
// 	//可以为空。
// }

// include!(concat!(env!("OUT_DIR"), "\\bindings.rs"));

fn main() {//测试的时候，要把dll复制到exe的目录下。
    println!("Hello, world!");

    // unsafe{
    //     EnumTcpTable();
    // }
}
