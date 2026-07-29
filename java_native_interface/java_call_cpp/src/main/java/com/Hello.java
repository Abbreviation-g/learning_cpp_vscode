package com;

public class Hello {

    public native void helloFromCPP();

    static {
        System.out.println(System.getProperty("user.dir") +"/"+"hello.dll");
        System.load(System.getProperty("user.dir") +"/"+"hello.dll");
    }
}
