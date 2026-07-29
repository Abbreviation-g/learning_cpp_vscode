package com;

import java.net.URISyntaxException;
import java.nio.file.Files;
import java.nio.file.Path;

public class Hello {

    public native void helloFromCPP();

    static {
        Path dllPath = resolveDllPath();
        System.out.println(dllPath);
        System.load(dllPath.toString());
    }

    private static Path resolveDllPath() {
        Path fallbackPath = Path.of(System.getProperty("user.dir"), "hello.dll").toAbsolutePath().normalize();
        // try {
        //     Path codePath = Path.of(Hello.class.getProtectionDomain().getCodeSource().getLocation().toURI());
        //     Path baseDir = codePath.toFile().isFile() ? codePath.getParent() : codePath;
        //     Path jarSideDll = baseDir.resolve("hello.dll").toAbsolutePath().normalize();
        //     return Files.exists(jarSideDll) ? jarSideDll : fallbackPath;
        // } catch (URISyntaxException e) {
            
        // }
        return fallbackPath;
    }
}
