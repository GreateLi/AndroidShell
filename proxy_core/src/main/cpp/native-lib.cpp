#include <jni.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <memory.h>
#include "rc4.h"
#define LOG_TAG "native-jni"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG, __VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG, __VA_ARGS__)
#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)
#define LOGF(...)  __android_log_print(ANDROID_LOG_FATAL,LOG_TAG, __VA_ARGS__)
#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))

unsigned char*  readFile(const char* file, int *len)
{
    FILE *fp;
    fp = fopen(file, "rb");
    if (fp != NULL) {
        fseek(fp, 0L, SEEK_END);
        unsigned long filesize = ftell(fp);
        *len = filesize;
        if (filesize > 0) {
            unsigned char* fileBuffer = new unsigned char[filesize + 1];
            // unsigned char *fileBuffer = new unsigned char[filesize + 1];
            rewind(fp);//rewind函数作用等同于 (void)fseek(stream, 0L, SEEK_SET);
            fileBuffer[filesize] = '\0';
            int ret = fread(fileBuffer, sizeof(char), filesize, fp);

            fclose(fp);
            fp = NULL;
            if (ret <= 0) {
                return NULL;
            }
            return fileBuffer;
        }

        fclose(fp);
        fp = NULL;
    }

    return NULL;
}
std::string jstring2str(JNIEnv *env, jstring jstr) {
    char *rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("GB2312");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    std::string stemp(rtn);
    free(rtn);
    return stemp;
}

bool  writeFileDex(const char * path, const unsigned char * data,long size)
{
    FILE *fp;
    if ((fp = fopen(path, "wb")) == NULL)
    {
        // LOGE("file cannot open %s", path);
        return false;
    }
    else
    {
        fwrite(data, 1, size, fp);
    }
    fclose(fp);

    return true;
}
//extern "C" JNIEXPORT jstring JNICALL
//Java_cn_com_jni_dexshell_MainActivity_stringFromJNI(
//        JNIEnv *env,
//        jobject jobj/* this */, jstring id) {
//    std::string nid = jstring2str(env, id);
//    std::string hello = "Hello from C++";
//    return env->NewStringUTF(nid.c_str());
//}

using namespace std;
//cn.com.jni.proxy_core.Utils.native_rc4_de(
static const char *const kClassJniTest =
        "cn/com/jni/proxy_core/Utils";



void rc4Encrypt(unsigned char * data, int len)
{
    unsigned char s[256] = { 0 } ;//S-box
    char    key[256] = "8rrh1086omGe8qF0";

    rc4::rc4_init(s, (unsigned char*)key, strlen(key));//已经完成了初始化
    rc4::rc4_run((unsigned char*)s, (unsigned char*)data, len);//解密
}
jbyteArray  JNI_native_rc4(JNIEnv *env, jobject thizz,jstring filename,jstring outname) {
    LOGI("  native_1init");

    string fname = jstring2str (env,filename);
    string oname = jstring2str (env,outname);

    int flen = 0;
    unsigned char* data =  readFile(fname.c_str(),&flen);

    if(nullptr!= data)
    {
        rc4Encrypt(data,flen);
        jbyte *by = (jbyte*)data;
        jbyteArray jarray = env->NewByteArray(flen);

        //4. 赋值
        env->SetByteArrayRegion(jarray, 0, flen, by);
         delete []data;

        return jarray;
    }

//    if(! writeFileDex(oname.c_str(),data,flen))
//    {
//        LOGE("open file :%s failed",oname.c_str());
//    }
    return nullptr;
}

static const JNINativeMethod gMethods[] = {
        {
                "native_rc4_de",
                "(Ljava/lang/String;Ljava/lang/String;)[B",
                (void*) JNI_native_rc4
        }
};

static int registerNativeMethods(JNIEnv *env, const char *className, const JNINativeMethod *gMethod,
                                 int numMethods) {
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        LOGI(" JNI reg faild:%s",className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethod, numMethods) < 0) {
        LOGI(" JNI reg method failed:%s",gMethod->name);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

static int register_along_jni(JNIEnv *env) {

    return registerNativeMethods(env, kClassJniTest, gMethods,
                                 NELEM(gMethods));
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void * /* reserved */) {

    JNIEnv *env = NULL;
    jint result = -1;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        LOGE("ERROR: JNI version error");

        return JNI_ERR;
    }
    if (register_along_jni(env) == -1) {
        LOGE("ERROR:  JNI_OnLoad failed");
        return JNI_ERR;
    }
    result = JNI_VERSION_1_6;
    return result;
}