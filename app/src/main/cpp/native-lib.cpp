#include <jni.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <memory.h>

#define LOG_TAG "my-jni"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG, __VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG, __VA_ARGS__)
#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)
#define LOGF(...)  __android_log_print(ANDROID_LOG_FATAL,LOG_TAG, __VA_ARGS__)
#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))




extern "C" JNIEXPORT jstring JNICALL
Java_cn_com_jni_dexshell_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject jobj/* this */, jstring id) {

    std::string hello = "Hello from C++ ok";
    return env->NewStringUTF(hello.c_str());
}

