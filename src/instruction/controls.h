/*
 * Author: Jia Yang
 */

#ifndef JVM_CONTROLS_H
#define JVM_CONTROLS_H

#include "../interpreter/stack_frame.h"

static void __goto(struct stack_frame *frame)
{
    int offset = bcr_reads2(frame->reader);
    bcr_skip(frame->reader, offset - 3);  // 减3？减去本条指令自身的长度 todo
}

/*
 * 在Java 6之前，Oracle的Java编译器使用 jsr, jsr_w 和 ret 指令来实现 finally 子句。
 * 从Java 6开始，已经不再使用这些指令
 */

static void jsr(struct stack_frame *frame)
{
    jvm_abort("jsr doesn't support after jdk 6.");
}

static void ret(struct stack_frame *frame)
{
    // ret 有 wide 扩展
    jvm_abort("ret doesn't support after jdk 6.");
}


/*
 * todo 指令说明  好像是实现 switch 语句
 */
static void tableswitch(struct stack_frame *frame)
{
    struct bytecode_reader *reader = frame->reader;
    size_t pc = bcr_get_pc(reader) - 1; // 减去opcode的长度
    bcr_align4(reader);

    // 默认情况下执行跳转所需字节码的偏移量
    // 对应于 switch 中的 default 分支。
    s4 default_offset = bcr_reads4(reader);

    // low 和 height 标识了 case 的取值范围。
    s4 low = bcr_reads4(reader);
    s4 height = bcr_reads4(reader);

    // 跳转偏移量表，对应于各个 case 的情况
    s4 jump_offset_count = height - low + 1;
    s4 jump_offsets[jump_offset_count];
    bcr_reads4s(reader, jump_offset_count, jump_offsets);

    // 弹出要判断的值
    jint index = os_popi(frame->operand_stack);
    s4 offset;
    if (index < low || index > height) {
        offset = default_offset; // 没在 case 标识的范围内，跳转到 default 分支。
    } else {
        offset = jump_offsets[index - low]; // 找到对应的case了
    }

    // The target address that can be calculated from each jump table
    // offset, as well as the one that can be calculated from default,
    // must be the address of an opcode of an instruction within the method
    // that contains this tableswitch instruction.
    bcr_set_pc(reader, pc + offset);
}

/*
 * todo 指令说明  好像是实现 switch 语句
 */
static void lookupswitch(struct stack_frame *frame)
{
    struct bytecode_reader *reader = frame->reader;
    size_t pc = bcr_get_pc(reader) - 1; // 减去opcode的长度
    bcr_align4(reader);

    // 默认情况下执行跳转所需字节码的偏移量
    // 对应于 switch 中的 default 分支。
    s4 default_offset = bcr_reads4(reader);

    // case的个数
    s4 npairs = bcr_reads4(reader);
    assert(npairs >= 0); // The npairs must be greater than or equal to 0.

    // match_offsets 有点像 Map，它的 key 是 case 值，value 是跳转偏移量。
    s4 match_offsets[npairs * 2];
    bcr_reads4s(reader, npairs * 2, match_offsets);

    // 弹出要判断的值
    jint key = os_popi(frame->operand_stack);
    s4 offset = default_offset;
    for (int i = 0; i < npairs * 2; i += 2) {
        if (match_offsets[i] == key) { // 找到 case
            offset = match_offsets[i + 1];
            break;
        }
    }

    // The target address is calculated by adding the corresponding offset
    // to the address of the opcode of this lookupswitch instruction.
    bcr_set_pc(reader, pc + offset);
}


/*
 * todo
 * 要检查当前方法的返回类型
 * ireturn:
 The current method must have return type boolean, byte, short,
char, or int. The value must be of type int. If the current method
is a synchronized method, the monitor entered or reentered on
invocation of the method is updated and possibly exited as if by
execution of a monitorexit instruction (§monitorexit) in the current
thread. If no exception is thrown, value is popped from the operand
stack of the current frame (§2.6) and pushed onto the operand stack
of the frame of the invoker. Any other values on the operand stack
of the current method are discarded.
The interpreter then returns control to the invoker of the method,
reinstating the frame of the invoker.

 areturn:
 The objectref must be of type reference and must refer to an
object of a type that is assignment compatible (JLS §5.2) with the
type represented by the return descriptor (§4.3.3) of the current
method. If the current method is a synchronized method, the
monitor entered or reentered on invocation of the method is
updated and possibly exited as if by execution of a monitorexit
instruction (§monitorexit) in the current thread. If no exception is
thrown, objectref is popped from the operand stack of the current
frame (§2.6) and pushed onto the operand stack of the frame of
the invoker. Any other values on the operand stack of the current
method are discarded.
The interpreter then reinstates the frame of the invoker and returns
control to the invoker.
 */
#define TRETURN(func_name, T) \
static void func_name(struct stack_frame *frame) \
{ \
    assert(frame == jthread_top_frame(frame->thread)); \
    /* frame->method->reset(); */ \
    jthread_pop_frame(frame->thread); \
    \
    struct stack_frame *invoke_frame = jthread_top_frame(frame->thread); \
    /* todo invokeFrame 会不会为null */ \
    if (invoke_frame == NULL) { \
        jvm_abort("never goes here."); \
    } \
    os_push##T(invoke_frame->operand_stack, os_pop##T(frame->operand_stack)); \
    sf_exe_over(frame); \
}

TRETURN(ireturn, i)
TRETURN(lreturn, l)
TRETURN(freturn, f)
TRETURN(dreturn, d)
TRETURN(areturn, r)

static void __return(struct stack_frame *frame)
{
    /* frame->method->reset(); */
    jthread_pop_frame(frame->thread);
    sf_exe_over(frame);
}


#endif //JVM_CONTROLS_H
