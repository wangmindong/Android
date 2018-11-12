#include <jni.h>
#include <string>
#include <stdlib.h>

void accessNoStaticField(JNIEnv *env, jclass this_cls, jobject obj);
void accessStaticField(JNIEnv *env, jclass this_cls);
void call_static_method(JNIEnv *env, jobject obj);
void call_no_static_method(JNIEnv *env, jobject obj);
jint get_random_num(JNIEnv *env, jobject);
jstring get_string(JNIEnv *env, jobject);
void test(JNIEnv *env, jclass aclass, jshort s, jint i, jlong l, jfloat f,
          jdouble d, jchar c, jboolean z, jbyte b, jstring j_str,
          jobject obj, jclass j_class, jintArray int_array);
jint sumArray(JNIEnv *env, jobject thisObj,jintArray j_int_array);
void callSuperInstanceMethod(JNIEnv *env, jclass cls);



/*需要注册的函数列表，放在JNINativeMethod 类型的数组中，
以后如果需要增加函数，只需在这里添加就行了
参数：
1.java代码中用native关键字声明的函数名字符串
2.签名（传进来参数类型和返回值类型的说明）
3.C/C++中对应函数的函数名（地址）
*/
static JNINativeMethod getMethods[] = {
        {"getRandomNum", "()I",(void*)get_random_num},
        {"getString", "()Ljava/lang/String;", (void*)get_string},
        {"test", "(SIJFDCZBLjava/lang/String;Ljava/lang/Object;Lcom/example/videoeditor/testjni/MyClass;[I)V", (void*)test},
        {"sumArray", "([I)I", (void*)sumArray},
        {"callJavaStaticMethod", "()V", (void*)call_static_method},
        {"callJavaNoStaticMethod", "()V", (void*)call_no_static_method},
        {"accessNoStaticField", "(Lcom/example/videoeditor/testjni/ClassField;)V", (void*)accessNoStaticField},
        {"accessStaticField", "()V", (void*)accessStaticField},
        {"callSuperInstanceMethod", "()V", (void*)callSuperInstanceMethod},
};
//此函数通过调用JNI中 RegisterNatives 方法来注册我们的函数
static int registerNativeMethods(JNIEnv* env, const char* className,JNINativeMethod* getMethods,int methodsNum){
    jclass clazz;
    //找到声明native方法的类
    clazz = env->FindClass(className);
    if(clazz == NULL){
        return JNI_FALSE;
    }
    //注册函数 参数：java类 所要注册的函数数组 注册函数的个数
    if(env->RegisterNatives(clazz,getMethods,methodsNum) < 0){
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

static int registerNatives(JNIEnv* env){
    //指定类的路径，通过FindClass 方法来找到对应的类
    const char* className  = "com/example/videoeditor/testjni/MainActivity";
    return registerNativeMethods(env,className,getMethods, sizeof(getMethods)/ sizeof(getMethods[0]));
}
//回调函数 在这里面注册函数
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved){
    JNIEnv* env = NULL;
    //判断虚拟机状态是否有问题
    if(vm->GetEnv((void**)&env,JNI_VERSION_1_6)!= JNI_OK){
        return -1;
    }

    //开始注册函数 registerNatives -》registerNativeMethods -》env->RegisterNatives
    if(!registerNatives(env)){
        return -1;
    }
    //返回jni 的版本
    return JNI_VERSION_1_6;
}

void accessNoStaticField(JNIEnv *env, jclass this_cls, jobject obj){
    jclass clazz;
    jfieldID  fid;
    jstring j_str;
    jstring j_newStr;
    const char* c_str;

    // 1.获取AccessField类的Class引用
    clazz = env->GetObjectClass(obj);
    if(clazz == NULL){
        return;
    }

    // 2.获取AccessField类实例变量str的属性ID
    fid = env->GetFieldID(clazz, "str", "Ljava/lang/String;");
    if(fid == NULL){
        return;
    }

    // 3.获取实例变量str的值
    j_str = (jstring)env->GetObjectField(obj, fid);


    // 4.将unicode编码的java字符串转换成为C风格的字符串
    c_str = env->GetStringUTFChars(j_str,NULL);
    if(env->ExceptionCheck()){
        env->ExceptionDescribe();
        env->ExceptionClear();
        return;
    }
    if(c_str == NULL){
        return;
    }

    env->ReleaseStringUTFChars(j_str, c_str);

    // 5.修改实例变量的值
    j_newStr = env->NewStringUTF("This is C String");
    if(j_newStr == NULL){
        return;
    }

    env->SetObjectField(obj, fid, j_newStr);

    // 6.删除局部引用
    env->DeleteLocalRef(clazz);
    env->DeleteLocalRef(j_str);
    env->DeleteLocalRef(j_newStr);
}

void accessStaticField(JNIEnv *env, jclass this_cls){
    jclass clazz;
    jfieldID fid;
    jint num;

    // 1.获取ClassField类的Class引用
    clazz = env->FindClass("com/example/videoeditor/testjni/ClassField");
    if(clazz == NULL){
        return;
    }

    // 2.获取ClassField类静态变量num的属性ID
    fid = env->GetStaticFieldID(clazz, "num", "I");
    if(fid == NULL){
        return;
    }

    // 3.获取静态变量num的值
    num = env->GetStaticIntField(clazz, fid);
    env->SetStaticIntField(clazz, fid, 100);

    // 4.删除局部引用
    env->DeleteLocalRef(clazz);
}


// 调用静态方法示例
void call_static_method(JNIEnv *env, jobject obj){
    jclass clazz = NULL;
    jstring str_arg = NULL;
    jmethodID  mid_static_method;

    // 1.从classpath路径下搜索ClassMethod这个类，并返回该类的Class对象
    clazz = env->FindClass("com/example/videoeditor/testjni/ClassMethod");
    if(clazz == NULL){
        return;
    }

    // 2.从clazz类中查找callStaticMethod方法
    mid_static_method = env->GetStaticMethodID(clazz,"callStaticMethod","(Ljava/lang/String;I)V");
    if(mid_static_method == NULL){
        printf("找不到callStaticMethod这个静态方法");
        return;
    }

    // 3.调用clazz类的callStaticMethod静态方法
    str_arg = env->NewStringUTF("我是静态方法");
    env->CallStaticVoidMethod(clazz,mid_static_method, str_arg, 100);

    // 4.删除局部引用
    env->DeleteLocalRef(clazz);
    env->DeleteLocalRef(str_arg);
}

// 调用非静态方法示例
void call_no_static_method(JNIEnv *env, jobject obj){
    jclass clazz = NULL;
    jobject jobj = NULL;
    jmethodID  mid_construct = NULL;
    jmethodID  mid_instance = NULL;
    jstring str_arg = NULL;

    // 1.从classpath路径下搜索ClassMethod这个类，并返回该类的Class对象
    clazz = env->FindClass("com/example/videoeditor/testjni/ClassMethod");
    if(clazz == NULL){
        printf("找不到com/example/videoeditor/testjni/ClassMethod这个类");
        return;
    }

    // 2.获取类的默认构造方法ID
    mid_construct = env->GetMethodID(clazz, "<init>", "()V");
    if( mid_construct == NULL){
        printf("找不到默认的构造方法");
        return;
    }

    // 3.查找实例方法的ID
    mid_instance = env->GetMethodID(clazz, "callNoStaticMethod", "(Ljava/lang/String;I)V");
    if(mid_instance == NULL){
        return;
    }

    // 4.创建该类的实例
    jobj = env->NewObject(clazz, mid_construct);
    if(jobj == NULL){
        printf("在com/example/videoeditor/testjni/ClassMethod类中找不到callNoStaticMethod方法");
        return;
    }

    // 5.调用对象的实例方法
    str_arg = env->NewStringUTF("我是实例方法");
    env->CallVoidMethod(jobj, mid_instance, str_arg, 200);

    // 6.删除局部引用
    env->DeleteLocalRef(clazz);
    env->DeleteLocalRef(jobj);
    env->DeleteLocalRef(str_arg);
}

//native 方法实现
jint get_random_num(JNIEnv *env, jobject /* this */){
    return rand();
}

jstring get_string(JNIEnv *env, jobject /* this */){
    return env->NewStringUTF("hello jni");
}

// 演示JNI函数的签名以及字符串的使用
void test(JNIEnv *env, jclass aclass, jshort s, jint i, jlong l, jfloat f, jdouble d, jchar c, jboolean z, jbyte b, jstring j_str,
          jobject obj, jclass j_class, jintArray int_array){
    printf("s=%hd, i=%d, l=%ld, f=%f, d=%lf, c=%c, z=%c, b=%d", s, i, l, f, d, c, z, b);
    const char* c_str = NULL;
    const jchar* c_str2 = NULL;
    c_str2 = env->GetStringChars(j_str, NULL);
    c_str = env->GetStringUTFChars(j_str, false);
    env->ReleaseStringUTFChars(j_str, c_str);
    int u_len = env->GetStringLength(j_str);
    int len = env->GetStringUTFLength(j_str);
    int c_len = strlen(c_str);
    if(c_str == NULL){
        return;
    }


    printf("%s", c_str);
}

// 演示数组的用法
jint sumArray(JNIEnv *env, jobject thisObj,jintArray j_int_array){
    jint i, sum = 0;
    jint *c_array;
    jint arr_len;
    jboolean isCopy;

//    // 第一种写法
//    // 1.获取数组长度
//    arr_len = env->GetArrayLength(j_int_array);
//    // 2.根据数组长度和数组元素的数据类型申请存放java数组元素的缓冲区
//    c_array = (jint*)malloc(sizeof(jint) * arr_len);
//    // 3.初始化缓冲区
//    memset(c_array, 0, sizeof(jint)*arr_len);
//    // 4.拷贝java数组中的所有元素到缓冲区
//    env->GetIntArrayRegion(j_int_array, 0, arr_len, c_array);
//
//    if(env->ExceptionCheck()){
//        env->ExceptionDescribe();
//        env->ExceptionClear();
//        return -1;
//    }
//
//    for(int i = 0; i < arr_len; i++){
//        sum += c_array[i];
//    }
//
//    free(c_array);
//    return sum;

//     第二种写法
    c_array = env->GetIntArrayElements(j_int_array, NULL);
    if(c_array == NULL){
        return 0;
    }

    arr_len = env->GetArrayLength(j_int_array);
    for(int i = 0; i < arr_len; i++){
        sum += c_array[i];
    }

    env->ReleaseIntArrayElements(j_int_array, c_array, 0);
    return sum;

//    // 第三种写法
//    c_array = (jint*)env->GetPrimitiveArrayCritical(j_int_array, &isCopy);
//    if(c_array == NULL){
//        return 0;
//    }
//    arr_len = env->GetArrayLength(j_int_array);
//    for(int i = 0; i < arr_len; i++){
//        sum += c_array[i];
//    }
//
//    env->ReleasePrimitiveArrayCritical(j_int_array, c_array, 0);
//    return sum;
}

void callSuperInstanceMethod(JNIEnv *env, jclass cls){
    jclass cls_cat;
    jclass cls_animal;
    jmethodID mid_cat_init;
    jmethodID mid_run;
    jmethodID mid_getName;
    jstring c_str_name;
    jobject obj_cat;
    const char* name = NULL;

    // 1.获取Cat类的class引用
    cls_cat = env->FindClass("com/example/videoeditor/testjni/Cat");
    if(cls_cat == NULL){
        return;
    }

    // 2.获取Cat的构造方法Id(构造方法名称统一为:<init>)
    mid_cat_init = env->GetMethodID(cls_cat, "<init>", "(Ljava/lang/String;)V");
    if(mid_cat_init == NULL){
        return;// 没有找到只有一个参数为String的构造方法
    }

    // 3.创建一个String对象，作为构造方法的参数
    c_str_name = env->NewStringUTF("汤姆猫");
    if(c_str_name == NULL){
        return; // 创建字符串失败(内存不够)
    }

    // 4.创建Cat对象的实例(调用对象的构造方法并且初始化对象)
    obj_cat = env->NewObject(cls_cat, mid_cat_init, c_str_name);
    if(obj_cat == NULL){
        return;
    }

    // 5.调用Cat父类Animal的run和getName方法
    cls_animal = env->FindClass("com/example/videoeditor/testjni/Animal");
    if(cls_animal == NULL){
        return;
    }

    // 例子1：调用父类的run方法
    // 获取父类Animal中run方法的id
    mid_run = env->GetMethodID(cls_animal, "run", "()V");
    if(mid_run == NULL){
        return;
    }

    // 注意：obj_cat是Cat的实例，cls_animal是Animal的Class引用，mid_run是Animal类中的方法ID
    env->CallNonvirtualVoidMethod(obj_cat, cls_animal, mid_run);

    // 例2：调用父类的getName方法
    // 获取父类Animal中getName方法的id
    mid_getName = env->GetMethodID(cls_animal, "getName", "()Ljava/lang/String;");
    if (mid_getName == NULL) {
        return;
    }

    c_str_name = (jstring)env->CallNonvirtualObjectMethod(obj_cat, cls_animal, mid_getName);
    name = env->GetStringUTFChars(c_str_name, NULL);
    printf("In C: Animal Name is %s\n", name);

    // 释放从java层获取到的字符串所分配的内存
    env->ReleaseStringUTFChars( c_str_name, name);

    // 删除局部引用（jobject或jobject的子类才属于引用变量），允许VM释放被局部变量所引用的资源
    env->DeleteLocalRef(cls_cat);
    env->DeleteLocalRef(cls_animal);
    env->DeleteLocalRef(c_str_name);
    env->DeleteLocalRef(obj_cat);

}


