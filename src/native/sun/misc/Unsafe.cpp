/*
 * Author: Yo Ka
 */

#include <iostream>
#include "../../../objects/object.h"
#include "../../../objects/array_object.h"
#include "../../../util/endianness.h"
#include "../../../runtime/frame.h"
#include "../../jni_inner.h"

/* todo
http://www.docjar.com/docs/api/sun/misc/Unsafe.html#park%28boolean,%20long%29
Block current Thread, returning when a balancing
unpark occurs, or a balancing unpark has
already occurred, or the Thread is interrupted, or, if not
absolute and time is not zero, the given time nanoseconds have
elapsed, or if absolute, the given deadline in milliseconds
since Epoch has passed, or spuriously (i.e., returning for no
"reason"). Note: This operation is in the Unsafe class only
because unpark is, so it would be strange to place it
elsewhere.
*/

// public native void park(boolean isAbsolute, long time);
static void park(JNIEnv *env, jobject _this, jboolean isAbsolute, jlong time)
{
    jvm_abort("park");
}

//  public native void unpark(Object thread);
static void unpark(JNIEnv *env, jobject _this, jobject thread)
{
    jvm_abort("unpark");
}

/*************************************    compare and swap    ************************************/

/*
 * 第一个参数为需要改变的对象，
 * 第二个为偏移量(参见函数 objectFieldOffset)，
 * 第三个参数为期待的值，
 * 第四个为更新后的值。
 *
 * 整个方法的作用即为若调用该方法时，value的值与expect这个值相等，那么则将value修改为update这个值，并返回一个true，
 * 如果调用该方法时，value的值与expect这个值不相等，那么不做任何操作，并范围一个false。
 *
 * public final native boolean compareAndSwapInt(Object o, long offset, int expected, int x);
 */
static jboolean compareAndSwapInt(JNIEnv *env, jobject _this, jref o, jlong offset, jint expected, jint x)
{
    jint *old;
    if (o->isArrayObject()) {
        old = (jint *)(((Array *) o)->index(offset));
    } else {
        assert(0 <= offset && offset < o->clazz->instFieldsCount);
        old = (jint *) (o->data + offset);
    }

    bool b = __sync_bool_compare_and_swap(old, expected, x);
    return b ? JNI_TRUE : JNI_FALSE;
}

// public final native boolean compareAndSwapLong(Object o, long offset, long expected, long x);
static jboolean compareAndSwapLong(JNIEnv *env, jobject _this, jref o, jlong offset, jlong expected, jlong x)
{
    jlong *old;
    if (o->isArrayObject()) {
        Array *ao = dynamic_cast<Array *>(o);  // todo
        old = (jlong *)(ao->index(offset));
    } else {
        assert(0 <= offset && offset < o->clazz->instFieldsCount);
        old = (jlong *)(o->data + offset);
    }

    bool b = __sync_bool_compare_and_swap(old, expected, x);  // todo
    return b ? JNI_TRUE : JNI_FALSE;
}

// public final native boolean compareAndSwapObject(Object o, long offset, Object expected, Object x)
static jboolean compareAndSwapObject(JNIEnv *env, jobject _this, jref o, jlong offset, jobject expected, jobject x)
{
    jref *old;
    if (o->isArrayObject()) {
        Array *ao = dynamic_cast<Array *>(o);  // todo
        old = (jref *)(ao->index(offset));
//        old = arrobj_get(jref, o, offset);
    } else {
        assert(0 <= offset && offset < o->clazz->instFieldsCount);
        old = (jref *)(o->data + offset);
    }

    bool b = __sync_bool_compare_and_swap(old, expected, x);
    return b ? JNI_TRUE : JNI_FALSE;
}

/*************************************    class    ************************************/
/** Allocate an instance but do not run any constructor. Initializes the class if it has not yet been. */
// public native Object allocateInstance(Class<?> type) throws InstantiationException;
static jobject allocateInstance(JNIEnv *env, jobject _this, jclass type)
{
    jvm_abort("allocateInstance");
}

// public native Class defineClass(String name, byte[] b, int off, int len,
//                                  ClassLoader loader, ProtectionDomain protectionDomain)
static jclass __defineClass(JNIEnv *env, jobject _this, jstring name, 
                jbyteArray b, jint off, jint len, jobject loader, jobject protectionDomain)
{
    jvm_abort("defineClass");
}

// public native void ensureClassInitialized(Class<?> c);
static void ensureClassInitialized(JNIEnv *env, jobject _this, jclsref c)
{
    initClass(c);
//    c->clinit(); // todo 是不是这样搞？
}

// public native long staticFieldOffset(Field f);
static jlong staticFieldOffset(JNIEnv *env, jobject _this, jfieldID f)
{
    jvm_abort("staticFieldOffset");
}

// public native Object staticFieldBase(Field f);
static jobject staticFieldBase(JNIEnv *env, jobject _this, jfieldID f)
{
    jvm_abort("staticFieldBase");
}
/*************************************    object    ************************************/

// public native int arrayBaseOffset(Class<?> type)
static jint arrayBaseOffset(JNIEnv *env, jobject _this, jclass type)
{
    return 0; // todo
}

// public native int arrayIndexScale(Class<?> type)
static jint arrayIndexScale(JNIEnv *env, jobject _this, jclass type)
{
    return 1; // todo
}

// public native long objectFieldOffset(Field field)
static jlong objectFieldOffset(JNIEnv *env, jobject _this, jref field)
{
    auto offset = field->getIntField(S(slot), S(I));
    return offset;
}

// private native long objectFieldOffset1(Class<?> c, String name);
static jlong objectFieldOffset1(JNIEnv *env, jobject _this, jclass c, jstring name)
{
    // todo
    jvm_abort("objectFieldOffset1");
}

// public native boolean getBoolean(Object o, long offset);
static jboolean getBoolean(JNIEnv *env, jobject _this, jobject o, jlong offset)
{
    jvm_abort("getBoolean");
}

// public native void putBoolean(Object o, long offset, boolean x);
static void putBoolean(JNIEnv *env, jobject _this, jobject o, jlong offset, jboolean x)
{
    jvm_abort("putBoolean");
}

// public native byte getByte(Object o, long offset);
static jbyte obj_getByte(JNIEnv *env, jobject _this, jobject o, jlong offset)
{
    jvm_abort("obj_getByte");
}

// public native void putByte(Object o, long offset, byte x);
static void obj_putByte(JNIEnv *env, jobject _this, jobject o, jlong offset, jbyte x)
{
    jvm_abort("obj_putByte");
}

// public native char getChar(Object o, long offset);
static jchar obj_getChar(JNIEnv *env, jobject _this, jobject o, jlong offset)
{
    jvm_abort("obj_getChar");
}

// public native void putChar(Object o, long offset, char x);
static void obj_putChar(JNIEnv *env, jobject _this, jobject o, jlong offset, jchar x)
{
    jvm_abort("obj_putChar");
}

// public native short getShort(Object o, long offset);
static jshort obj_getShort(JNIEnv *env, jobject _this, jobject o, jlong offset)
{
    jvm_abort("obj_getShort");
}

// public native void putShort(Object o, long offset, short x);
static void obj_putShort(JNIEnv *env, jobject _this, jobject o, jlong offset, jshort x)
{
    jvm_abort("obj_putShort");
}

// public native int getInt(Object o, long offset);
static jint obj_getInt(JNIEnv *env, jobject _this, jobject o, jlong offset)
{
    jvm_abort("obj_getInt");
}

// public native void putInt(Object o, long offset, int x);
static void obj_putInt(JNIEnv *env, jobject _this, jobject o, jlong offset, jint x)
{
    jvm_abort("obj_putInt");
}

// public native long getLong(Object o, long offset);
static jlong obj_getLong(JNIEnv *env, jobject _this, jobject o, jlong offset)
{
    jvm_abort("obj_getLong");
}

// public native void putLong(Object o, long offset, long x);
static void obj_putLong(JNIEnv *env, jobject _this, jobject o, jlong offset, jlong x)
{
    jvm_abort("obj_putLong");
}

// public native float getFloat(Object o, long offset);
static jfloat obj_getFloat(JNIEnv *env, jobject _this, jobject o, jlong offset)
{
    jvm_abort("obj_getFloat");
}

// public native void putFloat(Object o, long offset, float x);
static void obj_putFloat(JNIEnv *env, jobject _this, jobject o, jlong offset, jfloat x)
{
    jvm_abort("obj_putFloat");
}

// public native double getDouble(Object o, long offset);
static jdouble obj_getDouble(JNIEnv *env, jobject _this, jobject o, jlong offset)
{
    jvm_abort("obj_getDouble");
}

// public native void putDouble(Object o, long offset, double x);
static void obj_putDouble(JNIEnv *env, jobject _this, jobject o, jlong offset, jdouble x)
{
    jvm_abort("obj_putDouble");
}

// public native Object getObject(Object o, long offset);
static jobject getObject(JNIEnv *env, jobject _this, jobject o, jlong offset)
{
    jvm_abort("getObject");
}

// public native void putObject(Object o, long offset, Object x);
static void putObject(JNIEnv *env, jobject _this, jobject o, jlong offset, jobject x)
{
    jvm_abort("putObject");
}

// public native boolean getBooleanVolatile(Object o, long offset);
static jboolean getBooleanVolatile(JNIEnv *env, jobject _this, jobject o, jlong offset)
{
    jvm_abort("getBooleanVolatile");
}

// public native byte getByteVolatile(Object o, long offset);
static jbyte getByteVolatile(JNIEnv *env, jobject _this, jobject o, jlong offset)
{
    jvm_abort("getByteVolatile");
}

// public native char getCharVolatile(Object o, long offset);
static jchar getCharVolatile(JNIEnv *env, jobject _this, jobject o, jlong offset)
{
    jvm_abort("getCharVolatile");
}

// public native short getShortVolatile(Object o, long offset);
static jshort getShortVolatile(JNIEnv *env, jobject _this, jobject o, jlong offset)
{
    jvm_abort("getShortVolatile");
}

// public native int getIntVolatile(Object o, long offset);
static jint getIntVolatile(JNIEnv *env, jobject _this, jref o, jlong offset)
{
    // todo Volatile

    jint value;
    if (o->isArrayObject()) {
        Array *ao = dynamic_cast<Array *>(o);  // todo
        value = ao->get<jint>(offset);
//        value = arrobj_get(jint, o, offset);  // todo
    } else {
        assert(0 <= offset && offset < o->clazz->instFieldsCount);
        value = o->data[offset];//o->getInstFieldValue<jint>(offset);  // todo
    }
    return value;
}

// public native long getLongVolatile(Object o, long offset);
static jlong getLongVolatile(JNIEnv *env, jobject _this, jref o, jlong offset)
{
    jvm_abort("getLongVolatile");
}

// public native float getFloatVolatile(Object o, long offset);
static jfloat getFloatVolatile(JNIEnv *env, jobject _this, jref o, jlong offset)
{
    jvm_abort("getFloatVolatile");
}

// public native double getDoubleVolatile(Object o, long offset);
static jdouble getDoubleVolatile(JNIEnv *env, jobject _this, jref o, jlong offset)
{
    jvm_abort("getDoubleVolatile");
}

// public native void putIntVolatile(Object o, long offset, int x);
static void putIntVolatile(JNIEnv *env, jobject _this, jref o, jlong offset, jint x)
{
    jvm_abort("putIntVolatile");
}

// public native void putBooleanVolatile(Object o, long offset, boolean x);
static void putBooleanVolatile(JNIEnv *env, jobject _this, jref o, jlong offset, jboolean x)
{
    jvm_abort("putBooleanVolatile");
}

// public native void putByteVolatile(Object o, long offset, byte x);
static void putByteVolatile(JNIEnv *env, jobject _this, jref o, jlong offset, jbyte x)
{
    jvm_abort("putByteVolatile");
}

// public native void putShortVolatile(Object o, long offset, short x);
static void putShortVolatile(JNIEnv *env, jobject _this, jref o, jlong offset, jshort x)
{
    jvm_abort("putShortVolatile");
}

// public native void putCharVolatile(Object o, long offset, char x);
static void putCharVolatile(JNIEnv *env, jobject _this, jref o, jlong offset, jchar x)
{
    jvm_abort("putCharVolatile");
}

// public native void putLongVolatile(Object o, long offset, long x);
static void putLongVolatile(JNIEnv *env, jobject _this, jref o, jlong offset, jlong x)
{
    jvm_abort("putLongVolatile");
}

// public native void putFloatVolatile(Object o, long offset, float x);
static void putFloatVolatile(JNIEnv *env, jobject _this, jref o, jlong offset, jfloat x)
{
    jvm_abort("putFloatVolatile");
}

// public native void putDoubleVolatile(Object o, long offset, double x);
static void putDoubleVolatile(JNIEnv *env, jobject _this, jref o, jlong offset, jdouble x)
{
    jvm_abort("putDoubleVolatile");
}
// -------------------------
// public native Object getObjectVolatile(Object o, long offset);
static jref getObjectVolatile(JNIEnv *env, jobject _this, jref o, jlong offset)
{
    // todo Volatile

    jref value;
    if (o->isArrayObject()) {
        Array *ao = dynamic_cast<Array *>(o);  // todo
        value = ao->get<jref>(offset);
    } else {
        assert(0 <= offset && offset < o->clazz->instFieldsCount);
        value = *(jref *)(o->data + offset);//o->getInstFieldValue<jref>(offset);  // todo
    }
    return value;
}

// public native void putObjectVolatile(Object o, long offset, Object x);
static void putObjectVolatile(JNIEnv *env, jobject _this, jref o, jlong offset, jref x)
{
    // todo Volatile
    if (o->isArrayObject()) {
        auto arr = (Array *) o;
        arr->set(offset, x);
    } else {
        o->setFieldValue(offset, x);
    }
}

// public native Object getOrderedObject(Object o, long offset);
static void getOrderedObject(JNIEnv *env, jobject _this, jobject o, jlong offset)
{
    jvm_abort("getOrderedObject");
}

// public native void putOrderedObject(Object o, long offset, Object x);
static void putOrderedObject(JNIEnv *env, jobject _this, jobject o, jlong offset, jobject x)
{
    jvm_abort("putOrderedObject");
}

/** Ordered/Lazy version of {@link #putIntVolatile(Object, long, int)}  */
// public native void putOrderedInt(Object o, long offset, int x);
static void putOrderedInt(JNIEnv *env, jobject _this, jobject o, jlong offset, jint x)
{
    jvm_abort("putOrderedInt");
}

/** Ordered/Lazy version of {@link #putLongVolatile(Object, long, long)} */
// public native void putOrderedLong(Object o, long offset, long x);
static void putOrderedLong(JNIEnv *env, jobject _this, jobject o, jlong offset, jlong x)
{
    jvm_abort("putOrderedLong");
}

/*************************************    unsafe memory    ************************************/
// todo 说明 unsafe memory

/*
 * todo
 * 分配内存方法还有重分配内存方法都是分配的堆外内存，
 * 返回的是一个long类型的地址偏移量。这个偏移量在你的Java程序中每块内存都是唯一的。
 */
// public native long allocateMemory(long bytes);
static jlong allocateMemory(JNIEnv *env, jobject _this, jlong bytes)
{
    auto p = (u1 *) malloc(sizeof(char)*bytes);
    if (p == nullptr) {
        // todo error
    }
    return (jlong) (intptr_t) p;
}

// public native long reallocateMemory(long address, long bytes);
static jlong reallocateMemory(JNIEnv *env, jobject _this, jlong address, jlong bytes)
{
    return (jlong) (intptr_t) realloc((void *) (intptr_t) address, (size_t) bytes); // 有内存泄漏
}

// public native void freeMemory(long address);
static void freeMemory(JNIEnv *env, jobject _this, jlong address)
{
    free((void *) (intptr_t) address);
}

// public native int addressSize();
static jint addressSize(JNIEnv *env, jobject _this)
{
    return sizeof(jlong); // todo
}

// public native void putByte(long address, byte x);
static void putByte(JNIEnv *env, jobject _this, jlong address, jbyte x)
{
    *(jbyte *) (intptr_t) address = changeToBigEndian(x); // todo java按大端存储？？？？？？？
}

// public native byte getByte(long address);
static jbyte getByte(JNIEnv *env, jobject _this, jlong address)
{
    return *(jbyte *) (intptr_t) address;
}

// public native void putChar(long address, char x);
static void putChar(JNIEnv *env, jobject _this, jlong address, jchar x)
{
    *(jchar *) (intptr_t) address = changeToBigEndian(x);
}

// public native char getChar(long address);
static jchar getChar(JNIEnv *env, jobject _this, jlong address)
{
    return *(jchar *) (intptr_t) address;
}

// public native void putShort(long address, short x);
static void putShort(JNIEnv *env, jobject _this, jlong address, jshort x)
{
    *(jshort *) (intptr_t) address = changeToBigEndian(x);
}

// public native short getShort(long address);
static jshort getShort(JNIEnv *env, jobject _this, jlong address)
{
    return *(jshort *) (intptr_t) address;
}

// public native void putInt(long address, int x);
static void putInt(JNIEnv *env, jobject _this, jlong address, jint x)
{
    *(jint *) (intptr_t) address = changeToBigEndian(x);
}

// public native int getInt(long address);
static jint getInt(JNIEnv *env, jobject _this, jlong address)
{
    return *(jint *) (intptr_t) address;
}

// public native void putLong(long address, long x);
static void putLong(JNIEnv *env, jobject _this, jlong address, jlong x)
{
    *(jlong *) (intptr_t) address = changeToBigEndian(x);
}

// public native long getLong(long address);
static jlong getLong(JNIEnv *env, jobject _this, jlong address)
{
    return *(jlong *) (intptr_t) address;
}

// public native void putFloat(long address, float x);
static void putFloat(JNIEnv *env, jobject _this, jlong address, jfloat x)
{
    *(jfloat *) (intptr_t) address = changeToBigEndian(x);
}

// public native float getFloat(long address);
static jfloat getFloat(JNIEnv *env, jobject _this, jlong address)
{
    return *(jfloat *) (intptr_t) address;
}

// public native void putDouble(long address, double x);
static void putDouble(JNIEnv *env, jobject _this, jlong address, jdouble x)
{
    *(jdouble *) (intptr_t) address = changeToBigEndian(x);
}

// public native double getDouble(long address);
static jdouble getDouble(JNIEnv *env, jobject _this, jlong address)
{
    return *(jdouble *) (intptr_t) address;
}

// public native void putAddress(long address, long x);
static void putAddress(JNIEnv *env, jobject _this, jlong address, jlong x)
{
    putLong(env, _this, address, x);
}

// public native long getAddress(long address);
static jlong getAddress(JNIEnv *env, jobject _this, jlong address)
{
    return getLong(env, _this, address);
}

/**
  499        * Sets all bytes in a given block of memory to a fixed value
  500        * (usually zero).
  501        *
  502        * <p>This method determines a block's base address by means of two parameters,
  503        * and so it provides (in effect) a <em>double-register</em> addressing mode,
  504        * as discussed in {@link #getInt(Object,long)}.  When the object reference is null,
  505        * the offset supplies an absolute base address.
  506        *
  507        * <p>The stores are in coherent (atomic) units of a size determined
  508        * by the address and length parameters.  If the effective address and
  509        * length are all even modulo 8, the stores take place in 'long' units.
  510        * If the effective address and length are (resp.) even modulo 4 or 2,
  511        * the stores take place in units of 'int' or 'short'.
  512        *
  513        * @since 1.7
  514        */
// public native void setMemory(Object o, long offset, long bytes, byte value);
static void setMemory(JNIEnv *env, jobject _this, jobject o, jlong offset, jlong bytes, jbyte value)
{
    jvm_abort("setMemory"); // todo
}

/**
  529        * Sets all bytes in a given block of memory to a copy of another
  530        * block.
  531        *
  532        * <p>This method determines each block's base address by means of two parameters,
  533        * and so it provides (in effect) a <em>double-register</em> addressing mode,
  534        * as discussed in {@link #getInt(Object,long)}.  When the object reference is null,
  535        * the offset supplies an absolute base address.
  536        *
  537        * <p>The transfers are in coherent (atomic) units of a size determined
  538        * by the address and length parameters.  If the effective addresses and
  539        * length are all even modulo 8, the transfer takes place in 'long' units.
  540        * If the effective addresses and length are (resp.) even modulo 4 or 2,
  541        * the transfer takes place in units of 'int' or 'short'.
  542        *
  543        * @since 1.7
  544        */
// public native void copyMemory(Object srcBase, long srcOffset, Object destBase, long destOffset, long bytes);
static void copyMemory(JNIEnv *env, jobject _this, 
                    jobject srcBase, jlong srcOffset, 
                    jobject destBase, jlong destOffset, jlong bytes)
{
    jvm_abort("copyMemory"); // todo
}

/**
  988        * Gets the load average in the system run queue assigned
  989        * to the available processors averaged over various periods of time.
  990        * This method retrieves the given <tt>nelem</tt> samples and
  991        * assigns to the elements of the given <tt>loadavg</tt> array.
  992        * The system imposes a maximum of 3 samples, representing
  993        * averages over the last 1,  5,  and  15 minutes, respectively.
  994        *
  995        * @params loadavg an array of double of size nelems
  996        * @params nelems the number of samples to be retrieved and
  997        *         must be 1 to 3.
  998        *
  999        * @return the number of samples actually retrieved; or -1
 1000        *         if the load average is unobtainable.
 1001        */
// public native int getLoadAverage(double[] loadavg, int nelems);
static jint getLoadAverage(JNIEnv *env, jobject _this, jdoubleArray loadavg, jint nelems)
{
    jvm_abort("getLoadAverage"); // todo
}

// (Ljava/lang/Class;)Z
static jboolean shouldBeInitialized(JNIEnv *env, jobject _this, jclsref c)
{
    // todo
    return c->state >= Class::INITED ? JNI_TRUE : JNI_FALSE;
}

/**
 * Report the size in bytes of a native memory page (whatever that is).
 * This value will always be a power of two.
 * 
 * public native int pageSize();
*/
static jint pageSize(JNIEnv *env, jobject _this)
{
    jvm_abort("pageSize"); // todo
}

/**
  815        * Define a class but do not make it known to the class loader or system dictionary.
  816        * <p>
  817        * For each CP entry, the corresponding CP patch must either be null or have
  818        * the a format that matches its tag:
  819        * <ul>
  820        * <li>Integer, Long, Float, Double: the corresponding wrapper object type from java.lang
  821        * <li>Utf8: a string (must have suitable syntax if used as signature or name)
  822        * <li>Class: any java.lang.Class object
  823        * <li>String: any object (not just a java.lang.String)
  824        * <li>InterfaceMethodRef: (NYI) a method handle to invoke on that call site's arguments
  825        * </ul>
  826        * @params hostClass context for linkage, access control, protection domain, and class loader
  827        * @params data      bytes of a class file
  828        * @params cpPatches where non-null entries exist, they replace corresponding CP entries in data
  829        */
// public native Class defineAnonymousClass(Class hostClass, byte[] data, Object[] cpPatches);
static jclass defineAnonymousClass(JNIEnv *env, jobject _this, 
                    jclass hostClass, jbyteArray data, jobjectArray cpPatches)
{
    jvm_abort("defineAnonymousClass"); // todo
}

/** Lock the object.  It must get unlocked via {@link #monitorExit}. */
//  public native void monitorEnter(Object o);
static void monitorEnter(JNIEnv *env, jobject _this, jobject o)
{
    jvm_abort("monitorEnter"); // todo
}

/**
 * Unlock the object.  It must have been locked via monitorEnter.
 * 
 * public native void monitorExit(Object o);
 */
static void monitorExit(JNIEnv *env, jobject _this, jobject o)
{
    jvm_abort("monitorExit"); // todo
}

/**
  848        * Tries to lock the object.  Returns true or false to indicate
  849        * whether the lock succeeded.  If it did, the object must be
  850        * unlocked via {@link #monitorExit}.
  851        */
// public native boolean tryMonitorEnter(Object o);
static jboolean tryMonitorEnter(JNIEnv *env, jobject _this, jobject o)
{
    jvm_abort("tryMonitorEnter"); // todo
}

/** Throw the exception without telling the verifier. */
// public native void throwException(Throwable ee);
static void throwException(JNIEnv *env, jobject _this, jthrowable ee)
{
    jvm_abort("throwException"); // todo
}

// "()V"
static void loadFence(JNIEnv *env, jobject _this)
{
    jvm_abort("loadFence"); // todo
}

// "()V"
static void storeFence(JNIEnv *env, jobject _this)
{
    jvm_abort("storeFence"); // todo
}

// "()V"
static void fullFence(JNIEnv *env, jobject _this)
{
    jvm_abort("fullFence"); // todo
}

#undef CLD
#define CLD "Ljava/lang/ClassLoader;"

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "park", "(ZJ)V", (void *) park },
        { "unpark", "(Ljava/lang/Object;)V", (void *) unpark },

        // compare and swap
        { "compareAndSwapInt", "(Ljava/lang/Object;JII)Z", (void *) compareAndSwapInt },
        { "compareAndSwapLong", "(Ljava/lang/Object;JJJ)Z", (void *) compareAndSwapLong },
        { "compareAndSwapObject", "(" OBJ "J" OBJ OBJ ")Z", (void *) compareAndSwapObject },

        // class
        { "allocateInstance", "(Ljava/lang/Class;)Ljava/lang/Object;", (void *) allocateInstance },
        { "defineClass", "(" STR "[BII" CLD "Ljava/security/ProtectionDomain;)" CLS, (void *) __defineClass },
        { "ensureClassInitialized", "(Ljava/lang/Class;)V", (void *) ensureClassInitialized },
        { "staticFieldOffset", "(Ljava/lang/reflect/Field;)J", (void *) staticFieldOffset },
        { "staticFieldBase", "(Ljava/lang/reflect/Field;)" OBJ, (void *) staticFieldBase },

        // Object
        { "arrayBaseOffset", "(Ljava/lang/Class;)I", (void *) arrayBaseOffset },
        { "arrayIndexScale", "(Ljava/lang/Class;)I", (void *) arrayIndexScale },
        { "objectFieldOffset", "(Ljava/lang/reflect/Field;)J", (void *) objectFieldOffset },
        { "objectFieldOffset0", "(Ljava/lang/reflect/Field;)J", (void *) objectFieldOffset },
        { "objectFieldOffset1", "(Ljava/lang/Class;Ljava/lang/String;)J", (void *) objectFieldOffset1 },

        { "getBoolean", "(Ljava/lang/Object;J)Z", (void *) getBoolean },
        { "putBoolean", "(Ljava/lang/Object;JZ)V", (void *) putBoolean },
        { "getByte", "(Ljava/lang/Object;J)B", (void *) obj_getByte },
        { "putByte", "(Ljava/lang/Object;JB)V", (void *) obj_putByte },
        { "getChar", "(Ljava/lang/Object;J)C", (void *) obj_getChar },
        { "putChar", "(Ljava/lang/Object;JC)V", (void *) obj_putChar },
        { "getShort", "(Ljava/lang/Object;J)S", (void *) obj_getShort },
        { "putShort", "(Ljava/lang/Object;JS)V", (void *) obj_putShort },
        { "getInt", "(Ljava/lang/Object;J)I", (void *) obj_getInt },
        { "putInt", "(Ljava/lang/Object;JI)V", (void *) obj_putInt },
        { "getLong", "(Ljava/lang/Object;J)J", (void *) obj_getLong },
        { "putLong", "(Ljava/lang/Object;JJ)V", (void *) obj_putLong },
        { "getFloat", "(Ljava/lang/Object;J)F", (void *) obj_getFloat },
        { "putFloat", "(Ljava/lang/Object;JF)V", (void *) obj_putFloat },
        { "getDouble", "(Ljava/lang/Object;J)D", (void *) obj_getDouble },
        { "putDouble", "(Ljava/lang/Object;JD)V", (void *) obj_putDouble },
        { "getObject", "(Ljava/lang/Object;J)Ljava/lang/Object;", (void *) getObject },
        { "putObject", "(Ljava/lang/Object;JLjava/lang/Object;)V", (void *) putObject },
        { "getObjectVolatile", "(Ljava/lang/Object;J)Ljava/lang/Object;", (void *) getObjectVolatile },
        { "putObjectVolatile", "(Ljava/lang/Object;JLjava/lang/Object;)V", (void *) putObjectVolatile },
        { "getOrderedObject", "(Ljava/lang/Object;J)Ljava/lang/Object;", (void *) getOrderedObject },
        { "putOrderedObject", "(Ljava/lang/Object;JLjava/lang/Object;)V", (void *) putOrderedObject },
        { "putOrderedInt", "(Ljava/lang/Object;JI)V", (void *) putOrderedInt },
        { "putOrderedLong", "(Ljava/lang/Object;JJ)V", (void *) putOrderedLong },

        { "putIntVolatile", "(Ljava/lang/Object;JI)V", (void *) putIntVolatile },
        { "putBooleanVolatile", "(Ljava/lang/Object;JZ)V", (void *) putBooleanVolatile },
        { "putByteVolatile", "(Ljava/lang/Object;JB)V", (void *) putByteVolatile },
        { "putShortVolatile", "(Ljava/lang/Object;JS)V", (void *) putShortVolatile },
        { "putCharVolatile", "(Ljava/lang/Object;JC)V", (void *) putCharVolatile },
        { "putLongVolatile", "(Ljava/lang/Object;JJ)V", (void *) putLongVolatile },
        { "putFloatVolatile", "(Ljava/lang/Object;JF)V", (void *) putFloatVolatile },
        { "putDoubleVolatile", "(Ljava/lang/Object;JD)V", (void *) putDoubleVolatile },

        { "getIntVolatile", "(Ljava/lang/Object;J)I", (void *) getIntVolatile },
        { "getBooleanVolatile", "(Ljava/lang/Object;J)Z", (void *) getBooleanVolatile },
        { "getByteVolatile", "(Ljava/lang/Object;J)B", (void *) getByteVolatile },
        { "getShortVolatile", "(Ljava/lang/Object;J)S", (void *) getShortVolatile },
        { "getCharVolatile", "(Ljava/lang/Object;J)C,", (void *) getCharVolatile },
        { "getLongVolatile", "(Ljava/lang/Object;J)J", (void *) getLongVolatile },
        { "getFloatVolatile", "(Ljava/lang/Object;J)F", (void *) getFloatVolatile },
        { "getDoubleVolatile", "(Ljava/lang/Object;J)D", (void *) getDoubleVolatile },

        // unsafe memory
        { "allocateMemory", "(J)J", (void *) allocateMemory },
        { "reallocateMemory", "(JJ)J", (void *) reallocateMemory },
        { "setMemory", "(Ljava/lang/Object;JJB)V", (void *) setMemory },
        { "copyMemory", "(Ljava/lang/Object;JLjava/lang/Object;JJ)V", (void *) copyMemory },
        { "freeMemory", "(J)V", (void *) freeMemory },
        { "addressSize", "()I", (void *) addressSize },
        { "putAddress", "(JJ)V", (void *) putAddress },
        { "getAddress", "(J)J", (void *) getAddress },
        { "putByte", "(JB)V", (void *) putByte },
        { "getByte", "(J)B", (void *) getByte },
        { "putShort", "(JS)V", (void *) putShort },
        { "getShort", "(J)S", (void *) getShort },
        { "putChar", "(JC)V", (void *) putChar },
        { "getChar", "(J)C", (void *) getChar },
        { "putInt", "(JI)V", (void *) putInt },
        { "getInt", "(J)I", (void *) getInt },
        { "putLong", "(JJ)V", (void *) putLong },
        { "getLong", "(J)J", (void *) getLong },
        { "putFloat", "(JF)V", (void *) putFloat },
        { "getFloat", "(J)F", (void *) getFloat },
        { "putDouble", "(JD)V", (void *) putDouble },
        { "getDouble", "(J)D", (void *) getDouble },

        { "shouldBeInitialized", "(Ljava/lang/Class;)Z", (void *) shouldBeInitialized },
        { "getLoadAverage", "([DI)I", (void *) getLoadAverage },
        { "pageSize", "()I", (void *) pageSize },
        { "defineAnonymousClass", "(Ljava/lang/Class;[B[Ljava/lang/Object;)" CLS, (void *) defineAnonymousClass },
        { "monitorEnter", "(Ljava/lang/Object;)V", (void *) monitorEnter },
        { "monitorExit", "(Ljava/lang/Object;)V", (void *) monitorExit },
        { "tryMonitorEnter", "(Ljava/lang/Object;)Z", (void *) tryMonitorEnter },
        { "throwException", "(Ljava/lang/Throwable;)V", (void *) throwException },

        { "loadFence", "()V", (void *) loadFence },
        { "storeFence", "()V", (void *) storeFence },
        { "fullFence", "()V", (void *) fullFence },
};

void sun_misc_Unsafe_registerNatives()
{
    registerNatives("sun/misc/Unsafe", methods, ARRAY_LENGTH(methods));

    registerNatives("jdk/internal/misc/Unsafe", methods, ARRAY_LENGTH(methods));
}