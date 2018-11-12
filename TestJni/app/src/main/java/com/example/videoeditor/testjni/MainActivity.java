package com.example.videoeditor.testjni;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);


        int sum = 0;

        int[] arr = new int[10];
        for (int i = 0; i < arr.length; i++) {
            arr[i] = i;
            sum += i;
        }

        String obj = "obj";
        short s = 1;
        long l = 20;
        byte b = 127;
        test(s, 1, l, 1.0f, 30d, 'A', true, b, "中国",obj,new MyClass(), arr);

        tv.setText(getString() + sumArray(arr) + " sum: " + sum);

        ClassField classField = new ClassField();

        callJavaStaticMethod();
        callJavaNoStaticMethod();
        accessNoStaticField(classField);
        accessStaticField();
        Log.d("===>", "新的字符串：" + classField.getStr() + " 新的num: " + classField.getNum());

        callSuperInstanceMethod();
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native int getRandomNum();
    public native String getString();
    public static native void test(short s, int i, long l, float f,  double d,
                                   char c, boolean z, byte b, String str,
                                   Object obj, MyClass a_class, int[] arr);

    private native int sumArray(int[] arr);
    private static native void callJavaStaticMethod();
    public static native void callJavaNoStaticMethod();
    public static native void accessNoStaticField(ClassField obj);
    private static native void accessStaticField();
    public static native void callSuperInstanceMethod();
}

class MyClass{}

class ClassField {

    private static int num;

    private String str = "";

    public int getNum() {
        return num;
    }

    public void setNum(int num) {
        ClassField.num = num;
    }

    public String getStr() {
        return str;
    }

    public void setStr(String str) {
        this.str = str;
    }
}

class ClassMethod{

    private static void callStaticMethod(String str, int i) {
        System.out.format("ClassMethod::callStaticMethod called!-->str=%s," +
                " i=%d\n", str, i);
    }

    private void callNoStaticMethod(String str, int i) {
        System.out.format("ClassMethod::callInstanceMethod called!-->str=%s, " +
                "i=%d\n", str, i);
    }

}

class Animal {

    protected String name;

    public Animal(String name) {
        this.name = name;
        System.out.println("Animal Construct call...");
    }

    public String getName() {
        System.out.println("Animal.getName Call...");
        return "This is Animal";
    }

    public void run() {
        System.out.println("Animal.run...");
    }
}

class Cat extends Animal {

    public Cat(String name) {
        super(name);
        System.out.println("Cat Construct call....");
    }

    @Override
    public String getName() {
        return "My name is " + this.name;
    }

    @Override
    public void run() {
        System.out.println(name + " Cat.run...");
    }
}


