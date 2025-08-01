/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_maplibre_jni_ResourceOptions */

#ifndef _Included_com_maplibre_jni_ResourceOptions
#define _Included_com_maplibre_jni_ResourceOptions
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     com_maplibre_jni_ResourceOptions
 * Method:    nativeDefault
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_maplibre_jni_ResourceOptions_nativeDefault
  (JNIEnv *, jclass);

/*
 * Class:     com_maplibre_jni_ResourceOptions
 * Method:    nativeDestroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_maplibre_jni_ResourceOptions_nativeDestroy
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_maplibre_jni_ResourceOptions
 * Method:    nativeClone
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_maplibre_jni_ResourceOptions_nativeClone
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_maplibre_jni_ResourceOptions
 * Method:    nativeSetApiKey
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_maplibre_jni_ResourceOptions_nativeSetApiKey
  (JNIEnv *, jclass, jlong, jstring);

/*
 * Class:     com_maplibre_jni_ResourceOptions
 * Method:    nativeSetCachePath
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_maplibre_jni_ResourceOptions_nativeSetCachePath
  (JNIEnv *, jclass, jlong, jstring);

/*
 * Class:     com_maplibre_jni_ResourceOptions
 * Method:    nativeSetAssetPath
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_maplibre_jni_ResourceOptions_nativeSetAssetPath
  (JNIEnv *, jclass, jlong, jstring);

/*
 * Class:     com_maplibre_jni_ResourceOptions
 * Method:    nativeSetMaximumCacheSize
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_com_maplibre_jni_ResourceOptions_nativeSetMaximumCacheSize
  (JNIEnv *, jclass, jlong, jlong);

/*
 * Class:     com_maplibre_jni_ResourceOptions
 * Method:    nativeGetApiKey
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_maplibre_jni_ResourceOptions_nativeGetApiKey
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_maplibre_jni_ResourceOptions
 * Method:    nativeGetCachePath
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_maplibre_jni_ResourceOptions_nativeGetCachePath
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_maplibre_jni_ResourceOptions
 * Method:    nativeGetAssetPath
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_maplibre_jni_ResourceOptions_nativeGetAssetPath
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_maplibre_jni_ResourceOptions
 * Method:    nativeGetMaximumCacheSize
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_maplibre_jni_ResourceOptions_nativeGetMaximumCacheSize
  (JNIEnv *, jclass, jlong);

#ifdef __cplusplus
}
#endif
#endif
