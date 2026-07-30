package com;

import java.nio.file.Path;

public class Hello {

    public native void helloFromCPP();

    static {
        Path dllPath = resolveDllPath();
        System.out.println(dllPath);
        System.load(dllPath.toString());
    }

    private static Path resolveDllPath() {
        return Path.of(System.getProperty("user.dir"), "libhello.dll").toAbsolutePath().normalize();
    }
}
