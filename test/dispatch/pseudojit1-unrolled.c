#include <stdio.h>	/* printf */
#include <sys/types.h>	/* mmap */
#include <sys/mman.h>	/* mmap */

typedef void (*Inst)(void);
static int a = 1;

int ins[] = { 1,2,4,3,2,3,1,2,4,3,2,3,1,2,2,3,4,4,3,1,4,3,2,1,3,4,2,1,1,4,2,3,5 };

void IOP1(void);
void IOP2(void);
void IOP3(void);
void IOP4(void);
void IOP5(void);

void
IOP1(void)
{
    a = a + 1;
}

void
IOP2(void)
{
    a = a - 1;
}

void
IOP3(void)
{
    a = a + 1;
}

void
IOP4(void)
{
    a = a - 1;
}

void
IOP5(void)
{
    return;
}

int
main(int argc, char *argv[])
{
    uint8_t *ex;
    Inst calls[5];
    int ip;
    int ol, il;

    ex = (uint8_t *)mmap(0, 65536, PROT_READ|PROT_WRITE|PROT_EXEC,
        MAP_ANON|MAP_PRIVATE, -1, 0);
    if (ex == MAP_FAILED) {
        printf("mmap failure\n");
        return (1);
    }

    calls[0] = IOP1;
    calls[1] = IOP2;
    calls[2] = IOP3;
    calls[3] = IOP4;
    calls[4] = IOP5;

    ip = 0;
    for (ol = 0; ol < 10; ++ol) {
    for (il = 0; il < sizeof(ins) / sizeof(int); ++il) {
#ifdef __x86_64__
        ex[ip++] = 0x48;		/* rex */
        ex[ip++] = 0xB8;		/* mov to rax */
        (*(uintptr_t *)&ex[ip]) = (uintptr_t)calls[ins[il]-1];
        ip += 8;
        ex[ip++] = 0xFF;		/* call *rax */
        ex[ip++] = 0xD0;
#else
        ex[ip++] = 0xB8;		/* mov to eax */
        (*(uintptr_t *)&ex[ip]) = (uintptr_t)calls[ins[il]-1];
        ip += 4;
        ex[ip++] = 0xFF;		/* call *eax */
        ex[ip++] = 0xD0;
#endif
    }
    }
    ex[ip] = 0xC3;

    for (ol = 0; ol < 10000000; ++ol)
        ((void (*)(void))ex)();

    printf("%d\n", a);

    return (0);
}
