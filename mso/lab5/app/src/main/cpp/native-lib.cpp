#include <jni.h>
#include <string>
#include <vector>

extern "C" JNIEXPORT jstring JNICALL
Java_silentsky_lab5_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jintArray JNICALL
Java_silentsky_lab5_MainActivity_removeDuplicates(
        JNIEnv* env,
        jobject thisObject,
        jintArray array
        ) {

    int array_length = env->GetArrayLength(array);
    int * convertedArray = env->GetIntArrayElements(array, NULL);

    std::vector<int> vector;

    for (int i = 0; i < array_length; i++) {
        vector.push_back(convertedArray[i]);
    }
    std::sort(vector.begin(), vector.end());
    vector.erase(std::unique(vector.begin(), vector.end()), vector.end());

    jintArray return_array = env->NewIntArray(vector.size());

    for (int i = 0; i < vector.size(); i++) {
        int val = vector.at(i);
        env->SetIntArrayRegion(return_array, i, 1, &val);
    }

    return return_array;
}