#ifndef REG_DUMP_TEST_H
#define REG_DUMP_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

void test_reg_dump() {

    __asm__ volatile (
        "movl $0x01, %eax\n"
        "movl $0x02, %ebx\n"
        "movl $0x03, %ecx\n"
        "movl $0x04, %edx\n"
        "movl $0x05, %esi\n"
        "movl $0x06, %edi\n"
        "movl $0x07, %ebp\n"
        "ud2"
    );
}

#ifdef __cplusplus
}
#endif

#endif // REG_DUMP_TEST_H
