/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include <string.h>
#include <mem.h>
#include "jclass.h"
#include "access.h"
#include "../../jvm.h"
#include "jfield.h"
#include "jmethod.h"
#include "../../classfile/attribute.h"
#include "jmethod.h"
#include "access.h"
#include "primitive_types.h"

// 计算实例字段的个数，同时给它们编号
static void calc_instance_field_id(struct jclass *c)
{
    assert(c != NULL);

    int id = 0;
    if (c->super_class != NULL) {
        id = c->super_class->instance_fields_count; // todo 父类的私有变量是不是也算在了里面，不过问题不大，浪费点空间吧了
        assert(id >= 0);
    }

    for (int i = 0; i < c->fields_count; i++) {
        if (!IS_STATIC(c->fields[i]->access_flags))  {
            c->fields[i]->id = id++;
            if (c->fields[i]->category_two) {
                id++;
            }
        }
    }

    assert(id >= 0);
    c->instance_fields_count = id;

    VM_MALLOCS(struct slot, c->instance_fields_count, values);
    c->inited_instance_fields_values = values;

    // 将父类中的变量拷贝过来
    if (c->super_class != NULL) {
        memcpy(values, c->super_class->inited_instance_fields_values,
               c->super_class->instance_fields_count * sizeof(struct slot));
    }
    // 初始化本类中的实例变量
    for (int i = 0; i < c->fields_count; i++) {
        struct jfield *field = c->fields[i];
        if (!IS_STATIC(field->access_flags)) {
            assert(field->id < c->instance_fields_count);
            switch (field->descriptor[0]) {
                case 'B':
                case 'C':
                case 'I':
                case 'S':
                case 'Z':
                    values[field->id] = islot(0);
                    break;
                case 'F':
                    values[field->id] = fslot(0.0f);
                    break;
                case 'J':
                    values[field->id] = lslot(0L);
                    values[field->id + 1] = phslot;
                    break;
                case 'D':
                    values[field->id] = dslot(0.0);
                    values[field->id + 1] = phslot;
                    break;
                default:
                    values[field->id] = rslot(NULL);
                    break;
            }
        }
    }

    // todo 保证 values 的每一项都被初始化了
}

// 计算静态字段的个数，同时给它们编号
static void calc_static_field_id(struct jclass *c)
{
    assert(c != NULL);

    int id = 0;
    for (int i = 0; i < c->fields_count; i++) {
        if (IS_STATIC(c->fields[i]->access_flags)) {
            c->fields[i]->id = id++;
            if (c->fields[i]->category_two) {
                id++;
            }
        }
    }
    c->static_fields_count = id;

    VM_MALLOCS(struct slot, c->static_fields_count, values);
    c->static_fields_values = values;
    // 初始化本类中的静态变量
    for (int i = 0; i < c->fields_count; i++) {
        struct jfield *field = c->fields[i];
        if (IS_STATIC(field->access_flags)) {
            assert(field->id < c->static_fields_count);
            switch (field->descriptor[0]) {
                case 'B':
                case 'C':
                case 'I':
                case 'S':
                case 'Z':
                    values[field->id] = islot(0);
                    break;
                case 'F':
                    values[field->id] = fslot(0.0f);
                    break;
                case 'J':
                    values[field->id] = lslot(0L);
                    values[field->id + 1] = phslot;
                    break;
                case 'D':
                    values[field->id] = dslot(0.0);
                    values[field->id + 1] = phslot;
                    break;
                default:
                    values[field->id] = rslot(NULL);
                    break;
            }
        }
    }

    // todo 保证 values 的每一项都被初始化了
}

struct jclass *jclass_create_by_classfile(struct classloader *loader, struct classfile *cf)
{
    if (loader == NULL || cf == NULL) {
        jvm_abort("%p, %p", loader, cf);
    }

    VM_MALLOC(struct jclass, c);

    c->magic = cf->magic;
    c->major_version = cf->major_version;
    c->minor_version = cf->minor_version;
    c->loader = loader;
    c->inited = false;
    c->access_flags = cf->access_flags;

    // 先解析属性，因为下面建立 Runtime Constant Pool 时需要用到属性中的值。
    for (int i = 0; i < cf->attributes_count; i++) {
        struct attribute_common *attr = cf->attributes[i];

        // todo class attributes
        if (strcmp(attr->name, InnerClasses) == 0) {
            //          printvm("not parse attr: InnerClasses\n");
        } else if (strcmp(attr->name, EnclosingMethod) == 0) {  // 可选属性
            //           printvm("not parse attr: EnclosingMethod\n");
        } else if (strcmp(attr->name, Synthetic) == 0) {
            set_synthetic(&c->access_flags);  // todo
        } else if (strcmp(attr->name, Signature) == 0) {  // 可选属性
            //            printvm("not parse attr: Signature\n");
        } else if (strcmp(attr->name, SourceFile) == 0) {  // 可选属性
//            SourceFileAttr *a = static_cast<SourceFileAttr *>(attr);
//            sourcefileName = rtcp->getStr(a->sourcefileIndex);
            //            printvm("not parse attr: SourceFile\n");  // todo
        } else if (strcmp(attr->name, SourceDebugExtension) == 0) {  // 可选属性
            //      printvm("not parse attr: SourceDebugExtension\n");
        } else if (strcmp(attr->name, Deprecated) == 0) {  // 可选属性
            //        printvm("not parse attr: Deprecated\n");
        } else if (strcmp(attr->name, RuntimeVisibleAnnotations) == 0) {
            //         printvm("not parse attr: RuntimeVisibleAnnotations\n");
        } else if (strcmp(attr->name, RuntimeInvisibleAnnotations) == 0) {
            //        printvm("not parse attr: RuntimeInvisibleAnnotations\n");
        } else if (strcmp(attr->name, BootstrapMethods) == 0) {
            c->bootstrap_methods_attribute = (struct bootstrap_methods_attribute *) attr; // todo
        }
    }

    c->rtcp = rtcp_create(cf->constant_pool, cf->constant_pool_count, c->bootstrap_methods_attribute);

    c->class_name = rtcp_get_class_name(c->rtcp, cf->this_class);
    c->pkg_name = strdup(c->class_name);
    char *p = strrchr(c->pkg_name, '/');
    if (p == NULL) {
        c->pkg_name[0] = 0; // 包名可以为空
    } else {
        *p = 0; // 得到包名
    }

    if (cf->super_class == 0) { // why 0
        c->super_class = NULL; // 可以没有父类
    } else {
        c->super_class = classloader_load_class(loader, rtcp_get_class_name(c->rtcp, cf->super_class));
        // 从父类中拷贝继承来的field   todo 要不要从新new个field不然delete要有问题，继承过来的field的类名问题
//        for_each(superClass->instanceFields.begin(), superClass->instanceFields.end(), [](JField *f) {
//            if (!f->isPrivate()) {
//                instanceFields.push_back(new JField(*f));
//            }
//        });
    }

    c->interfaces_count = cf->interfaces_count;
    c->interfaces = malloc(sizeof(struct jclass *) * c->interfaces_count); // todo NULL
    for (int i = 0; i < c->interfaces_count; i++) {
        const char *interface_name = rtcp_get_class_name(c->rtcp, cf->interfaces[i]);
        if (interface_name[0] == 0) { // empty
            printvm("error\n"); // todo
        } else {
            c->interfaces[i] = classloader_load_class(loader, interface_name);
        }
    }

    c->fields_count = cf->fields_count;
    c->fields = malloc(sizeof(struct jfield *) * c->fields_count);
    for (int i = 0; i < c->fields_count; i++) {
        c->fields[i] = jfield_create(c, cf->fields + i);
    }

    calc_static_field_id(c);
    calc_instance_field_id(c);

    c->methods_count = cf->methods_count;
    c->methods = malloc(sizeof(struct jmethod *) * c->methods_count);
    for (int i = 0; i < c->methods_count; i++) {
        c->methods[i] = jmethod_create(c, cf->methods + i);
    }

    classfile_destroy(cf);
    return c;
}

struct jclass* jclass_create_primitive_class(struct classloader *loader, const char *class_name)
{
    // todo class_name 是不是基本类型
    VM_MALLOC(struct jclass, c);
    c->access_flags = ACC_PUBLIC;
    c->pkg_name = ""; // todo 包名
    c->class_name = strdup(class_name);  // todo
    c->loader = loader;
    c->inited = true;
    return c;
}

struct jclass* jclass_create_arr_class(struct classloader *loader, const char *class_name)
{
    // todo class_name 是不是 array
    VM_MALLOC(struct jclass, c);
    c->access_flags = ACC_PUBLIC;
    c->pkg_name = ""; // todo 包名
    c->class_name = strdup(class_name);  // todo NULL
    c->loader = loader;
    c->inited = true; // 数组类不需要初始化
    c->super_class = classloader_load_class(loader, "java/lang/Object");

    c->interfaces_count = 2;
    c->interfaces = malloc(sizeof(struct jclass *) * 2);
    c->interfaces[0] = classloader_load_class(loader, "java/lang/Cloneable");
    c->interfaces[1] = classloader_load_class(loader, "java/io/Serializable");

    return c;
}

void jclass_destroy(struct jclass *c)
{
    if (c == NULL) {
        // todo
        return;
    }

    for (int i = 0; i < c->methods_count; i++) {
        jmethod_destroy(c->methods[i]);
    }

    for (int i = 0; i < c->fields_count; i++) {
        jfield_destroy(c->fields[i]);
    }

    rtcp_destroy(c->rtcp);

    // todo

    free(c);
}

void jclass_clinit(struct jclass *c, struct stack_frame *invoke_frame)
{
    if (c->inited) {
        return;
    }

    struct jmethod *method = jclass_get_method(c, "<clinit>", "()V"); // todo 并不是每个类都有<clinit>方法？？？？？
    if (method != NULL) {
        if (!IS_STATIC(method->access_flags)) {
            // todo error
            printvm("error\n");
        }

//        sf_invoke_method(invoke_frame, method, NULL);
        jthread_invoke_method(invoke_frame->thread, method, NULL);
    }

    c->inited = true;

    /*
     * 超类放在最后判断，
     * 这样可以保证超类的初始化方法对应的帧在子类上面，
     * 使超类初始化方法先于子类执行。
     */
    if (c->super_class != NULL) {
        jclass_clinit(c->super_class, invoke_frame);
    }
}

void jclass_get_public_fields(struct jclass *c, struct jfield* fields[], int *count)
{
    *count = 0;
    for (int i = 0; i < c->fields_count; i++) {
        if (IS_PUBLIC(c->fields[i]->access_flags)) {
            fields[*count] = c->fields[i];
            (*count)++;
        }
    }
}

struct jfield* jclass_lookup_field(struct jclass *c, const char *name, const char *descriptor)
{
    for (int i = 0; i < c->fields_count; i++) {
        if (strcmp(c->fields[i]->name, name) == 0 && strcmp(c->fields[i]->descriptor, descriptor) == 0) {
            return c->fields[i];
        }
    }

    // todo 在父类中查找
    struct jfield *field;
    if (c->super_class != NULL) {
        if ((field = jclass_lookup_field(c->super_class, name, descriptor)) != NULL)
            return field;
    }

    // todo 在父接口中查找
    for (int i = 0; i < c->interfaces_count; i++) {
        if ((field = jclass_lookup_field(c->interfaces[i], name, descriptor)) != NULL)
            return field;
    }

    // java.lang.NoSuchFieldError  todo
    printvm("java.lang.NoSuchFieldError. %s, %s, %s\n", c->class_name, name, descriptor);
    return NULL;
}

struct jfield* jclass_lookup_static_field(struct jclass *c, const char *name, const char *descriptor)
{
    struct jfield* field = jclass_lookup_field(c, name, descriptor);
    // todo field == nullptr
    if (!IS_STATIC(field->access_flags)) {
        // todo java.lang.IncompatibleClassChangeError
        jvm_abort("java.lang.IncompatibleClassChangeError");
    }
    return field;
}

struct jfield* jclass_lookup_instance_field(struct jclass *c, const char *name, const char *descriptor)
{
    struct jfield* field = jclass_lookup_field(c, name, descriptor);
    // todo field == nullptr
    if (IS_STATIC(field->access_flags)) {
        // todo java.lang.IncompatibleClassChangeError
        jvm_abort("java.lang.IncompatibleClassChangeError");
    }
    return field;
}


struct jmethod* jclass_get_method(struct jclass *c, const char *name, const char *descriptor)
{
    for (int i = 0; i < c->methods_count; i++) {
        if (strcmp(c->methods[i]->name, name) == 0 && strcmp(c->methods[i]->descriptor, descriptor) == 0) {
            return c->methods[i];
        }
    }
    return NULL;
}

struct jmethod* jclass_get_constructor(struct jclass *c, const char *descriptor)
{
    return jclass_get_method(c, "<init>", descriptor);
}

struct jmethod* jclass_lookup_method(struct jclass *c, const char *name, const char *descriptor)
{
    struct jmethod *method = jclass_get_method(c, name, descriptor);
    if (method != NULL) {
        return method;
    }

    // todo 在父类中查找
    if (c->super_class != NULL) {
        if ((method = jclass_lookup_method(c->super_class, name, descriptor)) != NULL)
            return method;
    }

    // todo 在父接口中查找
    for (int i = 0; i < c->interfaces_count; i++) {
        if ((method = jclass_lookup_method(c->interfaces[i], name, descriptor)) != NULL)
            return method;
    }

    // todo java.lang.NoSuchMethodError
    //    jvmAbort("java.lang.NoSuchMethodError. %s, %s, %s\n", this->className.c_str(), method_name.c_str(), method_descriptor.c_str());
    return NULL;
}

struct jmethod* jclass_lookup_static_method(struct jclass *c, const char *name, const char *descriptor)
{
    struct jmethod *m = jclass_lookup_method(c, name, descriptor);
    // todo m == nullptr
    if (!IS_STATIC(m->access_flags)) {
        // todo java.lang.IncompatibleClassChangeError
        jvm_abort("java.lang.IncompatibleClassChangeError");
    }
    return m;
}

struct jmethod* jclass_lookup_instance_method(struct jclass *c, const char *name, const char *descriptor)
{
    struct jmethod *m = jclass_lookup_method(c, name, descriptor);
    // todo m == nullptr
    if (IS_STATIC(m->access_flags)) {
        // todo java.lang.IncompatibleClassChangeError
        jvm_abort("java.lang.IncompatibleClassChangeError");
    }
    return m;
}

bool jclass_is_subclass_of(const struct jclass *c, const struct jclass *father)
{
    if (c == father)
        return true;

    if (c->super_class != NULL && jclass_is_subclass_of(c->super_class, father))
        return true;

    for (int i = 0; i < c->interfaces_count; i++) {
        if (jclass_is_subclass_of(c->interfaces[i], father))
            return true;
    }

    return false;
}

struct slot* copy_inited_instance_fields_values(const struct jclass *c)
{
    assert(c != NULL);
    VM_MALLOCS(struct slot, c->instance_fields_count, copy);
    memcpy(copy, c->inited_instance_fields_values, c->instance_fields_count * sizeof(struct slot));
    return copy;
}

void set_static_field_value_by_id(const struct jclass *c, int id, const struct slot *value)
{
    assert(c != NULL && value != NULL);
    assert(id >= 0 && id < c->static_fields_count);
    c->static_fields_values[id] = *value;
    if (slot_is_category_two(value)) {
        assert(id + 1 < c->static_fields_count);
        c->static_fields_values[id + 1] = phslot;
    }
}

void set_static_field_value_by_nt(const struct jclass *c,
                  const char *name, const char *descriptor, const struct slot *value)
{
    assert(c != NULL && name != NULL && descriptor != NULL && value != NULL);

    struct jfield *f = jclass_lookup_field(c, name, descriptor);
    if (f == NULL) {
        jvm_abort("error\n"); // todo
    }

    set_static_field_value_by_id(c, f->id, value);
}

const struct slot* get_static_field_value_by_id(const struct jclass *c, int id)
{
    assert(c != NULL);
    assert(id >= 0 && id < c->static_fields_count);
    return c->static_fields_values + id;
}

const struct slot* get_static_field_value_by_nt(const struct jclass *c, const char *name, const char *descriptor)
{
    assert(c != NULL && name != NULL && descriptor != NULL);

    struct jfield *f = jclass_lookup_field(c, name, descriptor);
    if (f == NULL) {
        jvm_abort("error\n"); // todo
    }

    return get_static_field_value_by_id(c, f->id);
}

char* get_arr_class_name(const char *class_name)
{
    assert(class_name != NULL);

    VM_MALLOCS(char, strlen(class_name) + 8 /* big enough */, array_class_name);

    if (class_name[0] == '[') {
        sprintf(array_class_name, "[%s\0", class_name);
        return array_class_name;
    }

    for (int i = 0; i < PRIMITIVE_TYPE_COUNT; i++) {
        if (strcmp(primitive_types[i].name, class_name) == 0) {
            return strcpy(array_class_name, primitive_types[i].array_class_name);
        }
    }

    sprintf(array_class_name, "[L%s;\0", class_name);
    return array_class_name;
}

//struct jclass* jclass_array_class(struct jclass *c)
//{
//    assert(c != NULL);
//
//    // todo 好像不对，没有考虑基本类型的类，多维怎么搞
//    char array_class_name[strlen(c->class_name) + 8]; // big enough
//    sprintf(array_class_name, "[L%s;\0", c->class_name);
//
//    if (c->class_name[0] == '[') {
//        sprintf(array_class_name, "[%s\0", c->class_name);
//        return classloader_load_class(c->loader, array_class_name);
//    }
//
//    for (int i = 0; i < PRIMITIVE_TYPE_COUNT; i++) {
//        if (strcmp(primitive_types[i].name, c->class_name) == 0) {
//            return classloader_load_class(c->loader, primitive_types[i].array_class_name);
//        }
//    }
//
//    sprintf(array_class_name, "[L%s;\0", c->class_name);
//    return classloader_load_class(c->loader, array_class_name);
//}

bool jclass_is_accessible_to(const struct jclass *c, const struct jclass *visitor)
{
    // todo 实现对不对
    assert(c != NULL && visitor != NULL); // todo

    if (c == visitor || IS_PUBLIC(c->access_flags))  // todo 对不对
        return true;

    if (IS_PRIVATE(c->access_flags)) {
        return false;
    }

    // 字段是 protected，则只有 子类 和 同一个包下的类 可以访问
    if (IS_PROTECTED(c->access_flags)) {
        return jclass_is_subclass_of(visitor, c) || strcmp(c->pkg_name, visitor->pkg_name) == 0;
    }

    // 字段有默认访问权限（非public，非protected，也非private），则只有同一个包下的类可以访问
    return strcmp(c->pkg_name, visitor->pkg_name) == 0;
}

char *jclass_to_string(const struct jclass *c)
{
    snprintf(global_buf, GLOBAL_BUF_LEN, "class: %s\0", c == NULL ? "NULL" : c->class_name);
    return global_buf;
}