#include <jni.h>
#include <stdio.h>
#include "com_Hello.h"
/*
 * Class:     com_Hello
 * Method:    helloFromCPP
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_Hello_helloFromCPP
  (JNIEnv *, jobject) {

    printf("%s\n", "im from cpp");
  }

