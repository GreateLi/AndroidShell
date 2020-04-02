package cn.com.jni.proxy_core;

import android.app.Application;
import android.app.Instrumentation;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.ArrayMap;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import dalvik.system.InMemoryDexClassLoader;

public class ProxyApplication extends Application {
    //定义好的加密后的文件的存放路径
    private String app_name;
    private String app_version;
    private String so_path;
    private static final String TAG = "filename";
    /**
     * ActivityThread创建Application之后调用的第一个方法
     * 可以在这个方法中进行解密，同时把dex交给Android去加载
     * @param base
     */
    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
        //获取用户填入的metaData
        getMetaData();

        //得到当前apk文件
        File apkFile = new File(getApplicationInfo().sourceDir);

        //把apk解压  这个目录中的内容需要root权限才能使用
        File versionDir = getDir(app_name+"_" + app_version,MODE_PRIVATE);

        File appDir = new File(versionDir,"app");
        File dexDir = new File(appDir,"dexDir");

        so_path =  appDir.getAbsolutePath()+File.separator+"lib"+File.separator+Build.CPU_ABI;
        //得到我们需要加载的dex文件
        List<File> dexFiles = new ArrayList<>();
        List<ByteBuffer> ByteBufferList = new ArrayList<>();
        //进行解密 （最好做md5文件校验）
        if (!dexDir.exists() || dexDir.list().length == 0){
            //把apk解压到appDir
            if (!appDir.exists())
            {
                Zip.unZip(apkFile,appDir);
                Log.e("filename","unZip---"+appDir.getAbsolutePath());
            }

            //获取目录下所有的文件
            File[] files = appDir.listFiles();
            for (File file:files){
                String name = file.getName();

                if (name.endsWith(".dex") && !TextUtils.equals(name,"classes.dex")){
                    try{
                        Log.e("filename","---"+file.getAbsolutePath());
                       byte[] bytes = Utils.native_rc4_de(file.getAbsolutePath(),file.getAbsolutePath());
                        dexFiles.add(file);
                        if(null != bytes)
                        {
                            ByteBuffer buffer =   ByteBuffer.wrap(bytes);
                            ByteBufferList.add(buffer);
                        }

                        Log.e("filename","---"+file.getAbsolutePath());
                    }catch (Exception e){
                        e.printStackTrace();
                    }
                }
            }
        }else {
            for (File file:dexDir.listFiles()){
                dexFiles.add(file);
            }
        }
        try {
            loadDexByMem(base,ByteBufferList);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static Object getPathList(Object obj) throws ClassNotFoundException, NoSuchFieldException, IllegalAccessException {
       // return getField(obj, Class.forName("dalvik.system.BaseDexClassLoader"), "pathList");
        return RefInvoke.getFieldOjbect("dalvik.system.BaseDexClassLoader",obj,"pathList");
    }
    private void loadDexByMem(Context base, List<ByteBuffer> ByteBufferList) throws Exception{
        try {

            ByteBuffer[] byteBuffers = new ByteBuffer[ByteBufferList.size()];
            int i =0;
            for(ByteBuffer buffer:ByteBufferList)
            {
                byteBuffers[i]=buffer;
                i++;
            }

            // 配置动态加载环境
            //反射获取主线程对象，并从中获取所有已加载的package信息，并中找到当前的LoadApk对象的弱引用

            //创建一个新的inMemoryDexClassLoader用于加载源Apk，
            //  父节点的inMemoryDexClassLoader使其遵循双亲委托模型

            //getClassLoader()等同于 (ClassLoader) RefInvoke.getFieldOjbect()
            //但是为了替换掉父节点我们需要通过反射来获取并修改其值
            Object currentActivityThread = RefInvoke.invokeStaticMethod(
                    "android.app.ActivityThread", "currentActivityThread",
                    new Class[] {}, new Object[] {});
            String packageName = base.getPackageName();
            Log.e("filename","packageName:"+packageName);
            ArrayMap mPackages = (ArrayMap) RefInvoke.getFieldOjbect(
                    "android.app.ActivityThread", currentActivityThread,
                    "mPackages");

            WeakReference wr = (WeakReference) mPackages.get(packageName);


            Log.e("filename","android.app.LoadedApk");
            InMemoryDexClassLoader inMemoryDexClassLoader = new InMemoryDexClassLoader(byteBuffers,
                    (ClassLoader) RefInvoke.getFieldOjbect(
                            "android.app.LoadedApk", wr.get(), "mClassLoader"));

            ClassLoader gclassloader = (ClassLoader) RefInvoke.getFieldOjbect(
                    "android.app.LoadedApk", wr.get(), "mClassLoader");

            Log.e(TAG,"父classloader:"+gclassloader);

            //将父节点DexClassLoader替换
            RefInvoke.setFieldOjbect("android.app.LoadedApk", "mClassLoader",
                    wr.get(), inMemoryDexClassLoader);

            Log.i(TAG,"gclassloader classloader:"+inMemoryDexClassLoader);

            Log.e("filename","11 app_name: "+app_name);
            //创建用户真实的application  （MyApplication）
            Class<?> delegateClass = null;
            delegateClass =inMemoryDexClassLoader.loadClass(app_name);

            delegate = (Application) delegateClass.newInstance();
        }catch (Exception e){
            e.printStackTrace();
        }
    }


    private void getMetaData(){
        try {
            ApplicationInfo applicationInfo = getPackageManager().getApplicationInfo(
                    getPackageName(), PackageManager.GET_META_DATA);
            Bundle metaData = applicationInfo.metaData;
            if (null != metaData){
                if (metaData.containsKey("app_name")){
                    app_name = metaData.getString("app_name");
                }
                if (metaData.containsKey("app_version")){
                    app_version = metaData.getString("app_version");
                }
            }
        }catch (Exception e){
            e.printStackTrace();
        }
    }

    /**
     * 开始替换application
     */
    @Override
    public void onCreate() {
        super.onCreate();
        try {
            bindRealApplication();
        }catch (Exception e){
            e.printStackTrace();
        }
    }

    /**
     * 让代码走入if的第三段中
     * @return
     */
    @Override
    public String getPackageName() {
        if (!TextUtils.isEmpty(app_name)){
            return "";
        }
        return super.getPackageName();
    }

    @Override
    public Context createPackageContext(String packageName, int flags) throws PackageManager.NameNotFoundException {
        Log.e("filename","createPackageContext:packageName"+packageName);
        if (TextUtils.isEmpty(app_name)){
            return super.createPackageContext(packageName, flags);
        }
        try {
            bindRealApplication();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return delegate;

    }

    boolean isBindReal;
    Application delegate;
    //下面主要是通过反射系统源码的内容，然后进行处理，把我们的内容加进去处理
    private void bindRealApplication() throws Exception{
        if (isBindReal){
            return;
       }
        if (TextUtils.isEmpty(app_name)){
            return;
        }

        Log.e("filename","bindRealApplication: ");
        //得到attchBaseContext(context) 传入的上下文 ContextImpl
        Context baseContext = getBaseContext();

        //得到attch()方法
        Method attach = Application.class.getDeclaredMethod("attach",Context.class);
        attach.setAccessible(true);
        attach.invoke(delegate,baseContext);


        //获取ContextImpl ----> ,mOuterContext(app);  通过Application的attachBaseContext回调参数获取
        Class<?> contextImplClass = Class.forName("android.app.ContextImpl");
        //获取mOuterContext属性
        Field mOuterContextField = contextImplClass.getDeclaredField("mOuterContext");
        mOuterContextField.setAccessible(true);
        mOuterContextField.set(baseContext,delegate);

        //ActivityThread  ----> mAllApplication(ArrayList)  ContextImpl的mMainThread属性
        Field mMainThreadField = contextImplClass.getDeclaredField("mMainThread");
        mMainThreadField.setAccessible(true);
        Object mMainThread = mMainThreadField.get(baseContext);

        //ActivityThread  ----->  mInitialApplication       ContextImpl的mMainThread属性
        Class<?> activityThreadClass = Class.forName("android.app.ActivityThread");
        Field mInitialApplicationField = activityThreadClass.getDeclaredField("mInitialApplication");
        mInitialApplicationField.setAccessible(true);
        mInitialApplicationField.set(mMainThread,delegate);

        //ActivityThread ------>  mAllApplications(ArrayList)   ContextImpl的mMainThread属性
        Field mAllApplicationsField = activityThreadClass.getDeclaredField("mAllApplications");
        mAllApplicationsField.setAccessible(true);
        ArrayList<Application> mApplications = (ArrayList<Application>) mAllApplicationsField.get(mMainThread);
        mApplications.remove(this);
        mApplications.add(delegate);

        //LoadedApk ----->  mApplicaion             ContextImpl的mPackageInfo属性
        Field mPackageInfoField = contextImplClass.getDeclaredField("mPackageInfo");
        mPackageInfoField.setAccessible(true);
        Object mPackageInfo = mPackageInfoField.get(baseContext);


        Class<?> loadedApkClass = Class.forName("android.app.LoadedApk");
        Field mApplicationField = loadedApkClass.getDeclaredField("mApplication");
        mApplicationField.setAccessible(true);
        mApplicationField.set(mPackageInfo,delegate);

        //修改ApplicationInfo  className  LoadedApk
        Field mApplicationInfoField = loadedApkClass.getDeclaredField("mApplicationInfo");
        mApplicationInfoField.setAccessible(true);
        ApplicationInfo mApplicationInfo = (ApplicationInfo) mApplicationInfoField.get(mPackageInfo);
        mApplicationInfo.className = app_name;
       insertNativeLibraryPathElements(new File(so_path),delegate.getApplicationContext());

        delegate.onCreate();
        isBindReal = true;
    }

    public static void insertNativeLibraryPathElements(File soDirFile, Context context){
        ClassLoader pathClassLoader =  context.getClassLoader();
        Object pathList = null;
        try {
            pathList = getPathList(pathClassLoader);
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        }
        if(pathList != null) {
            Field nativeLibraryPathElementsField = null;
            try {

                Method makePathElements;
                Object invokeMakePathElements;
                boolean isNewVersion = Build.VERSION.SDK_INT > Build.VERSION_CODES.N_MR1;
                //调用makePathElements
                makePathElements = isNewVersion?pathList.getClass().getDeclaredMethod("makePathElements", List.class):pathList.getClass().getDeclaredMethod("makePathElements", List.class,List.class,ClassLoader.class);
                makePathElements.setAccessible(true);
                ArrayList<IOException> suppressedExceptions = new ArrayList<>();
                List<File> nativeLibraryDirectories = new ArrayList<>();
                nativeLibraryDirectories.add(soDirFile);
                List<File> allNativeLibraryDirectories = new ArrayList<>(nativeLibraryDirectories);
                //获取systemNativeLibraryDirectories
                Field systemNativeLibraryDirectoriesField = pathList.getClass().getDeclaredField("systemNativeLibraryDirectories");
                systemNativeLibraryDirectoriesField.setAccessible(true);
                List<File> systemNativeLibraryDirectories = (List<File>) systemNativeLibraryDirectoriesField.get(pathList);
                Log.i("insertNativeLibrary","systemNativeLibraryDirectories "+systemNativeLibraryDirectories);
                allNativeLibraryDirectories.addAll(systemNativeLibraryDirectories);
                invokeMakePathElements = isNewVersion?makePathElements.invoke(pathClassLoader, allNativeLibraryDirectories):makePathElements.invoke(pathClassLoader, allNativeLibraryDirectories,suppressedExceptions,pathClassLoader);
                Log.i("insertNativeLibrary","makePathElements "+invokeMakePathElements);

                nativeLibraryPathElementsField = pathList.getClass().getDeclaredField("nativeLibraryPathElements");
                nativeLibraryPathElementsField.setAccessible(true);
                Object list = nativeLibraryPathElementsField.get(pathList);
                Log.i("insertNativeLibrary","nativeLibraryPathElements "+list);
                Object dexElementsValue = combineArray(list, invokeMakePathElements);
                //把组合后的nativeLibraryPathElements设置到系统中
                nativeLibraryPathElementsField.set(pathList,dexElementsValue);
            } catch (NoSuchFieldException e) {
                e.printStackTrace();
            } catch (IllegalAccessException e) {
                e.printStackTrace();
            } catch (NoSuchMethodException e) {
                e.printStackTrace();
            } catch (InvocationTargetException e) {
                e.printStackTrace();
            }

        }
    }

    public static Object combineArray(Object hostDexElementValue, Object pluginDexElementValue) {
        //获取原数组类型
        Class<?> localClass = hostDexElementValue.getClass().getComponentType();
        Log.i("insertNativeLibrary","localClass "+localClass);
        //获取原数组长度
        int i = Array.getLength(hostDexElementValue);
        //插件数组加上原数组的长度
        int j = i + Array.getLength(pluginDexElementValue);
        //创建一个新的数组用来存储
        Object result = Array.newInstance(localClass, j);
        //一个个的将dex文件设置到新数组中
        for (int k = 0; k < j; ++k) {
            if (k < i) {
                Array.set(result, k, Array.get(hostDexElementValue, k));
            } else {
                Array.set(result, k, Array.get(pluginDexElementValue, k - i));
            }
        }
        return result;
    }
}
