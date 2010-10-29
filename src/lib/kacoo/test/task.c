/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

#include <stdio.h>

#include <ktmr.h>
#include <ktsk.h>

int OnMsg_12345678(kbean a_tsk, kuint msg, kvoid *ur0, kvoid *ur1, kvoid *ur2, kvoid *ur3, kint rsn)
{
    static int cnt = 0;
    printf("[%d](OnMsg_12345678) : %x : %x, %x, %x, %x\n", cnt++, msg, ur0, ur1, ur2, ur3);
    kmsg_post(ur0, msg, ur0, 0x67, 0, 0x8768);

    return 0;
}

int OnMsg_34567890(kbean a_tsk, kuint msg, kvoid *ur0, kvoid *ur1, kvoid *ur2, kvoid *ur3, kint rsn)
{
    static int cnt = 0;
    printf("[%d](OnMsg_34567890) : %x : %x, %x, %x, %x\n", cnt++, msg, ur0, ur1, ur2, ur3);
    kmsg_post(ur0, msg, ur0, 0x67, 0, 0x8768);
    kmsg_post(ur0, 0x55555555, 3453, 0x637, 0, 0x8768);
    return 0;
}

int OnMsg_55555555(kbean a_tsk, kuint msg, kvoid *ur0, kvoid *ur1, kvoid *ur2, kvoid *ur3, kint rsn)
{
    static int cnt = 0;
    printf("[%d](OnMsg_55555555) : %x : %x, %x, %x, %x\n", cnt++, msg, ur0, ur1, ur2, ur3);
    if (ur1 == 0x11111111) {
        kmsg_send(ur0, KMSG_QUIT, 1, 2, 2, 2);
    }
    return 0;
}

int OnMsg_Timer(kbean a_tsk, kuint msg, kvoid *ur0, kvoid *ur1, kvoid *ur2, kvoid *ur3, kint rsn)
{
    static int cnt = 0;
    printf("[%d](OnMsg_Timer) : %x : %x, %x, %x, %x\n", cnt++, msg, ur0, ur1, ur2, ur3);
    return 0;
}

kbean sema;
#define CMD_GET            0x00000002
#define CMD_REL            0x00000003

int om_get(kbean a_tsk, kuint msg, kvoid *ur0, kvoid *ur1, kvoid *ur2, kvoid *ur3, kint rsn)
{
    kchar *name = (kchar*)ur3;
    printf("om_get: hey %s\n", name);
    ksyn_sem_get(sema, -1);
    printf("om_get: bye %s\n\n", name);
}

int om_rel(kbean a_tsk, kuint msg, kvoid *ur0, kvoid *ur1, kvoid *ur2, kvoid *ur3, kint rsn)
{
    kchar *name = (kchar*)ur3;
    printf("\nom_get:\n");
    ksyn_sem_rel(sema);
}


int main()
{
    kint ch;

    // release
    kbean tsk_rel = ktsk_new(knil, knil, 0, 0, 1, 2, 3, "tsk_rel");

    // get
    kbean tsk_get1 = ktsk_new(knil, knil, 0, 0, 1, 2, 3, "tsk_get1");
    kbean tsk_get2 = ktsk_new(knil, knil, 0, 0, 1, 2, 3, "tsk_get2");
    kbean tsk_get3 = ktsk_new(knil, knil, 0, 0, 1, 2, 3, "tsk_get3");
    kbean tsk_get4 = ktsk_new(knil, knil, 0, 0, 1, 2, 3, "tsk_get4");

    sema = ksyn_sem_new(0);

    kmsg_slot_set(tsk_rel, CMD_REL, om_rel);

    kmsg_slot_set(tsk_get1, CMD_GET, om_get);
    kmsg_slot_set(tsk_get2, CMD_GET, om_get);
    kmsg_slot_set(tsk_get3, CMD_GET, om_get);
    kmsg_slot_set(tsk_get4, CMD_GET, om_get);

    for (;;) {
        ch = getch();
        switch (ch) {
        case 'r':
            kmsg_post(tsk_rel, CMD_REL, 0, 0, 0, 0);
            break;
        case 'g':
            kmsg_post(tsk_get1, CMD_GET, 0, 0, 0, "tsk_get1");
            kmsg_post(tsk_get2, CMD_GET, 0, 0, 0, "tsk_get2");
            kmsg_post(tsk_get3, CMD_GET, 0, 0, 0, "tsk_get3");
            kmsg_post(tsk_get4, CMD_GET, 0, 0, 0, "tsk_get4");
            break;
        case 'q':
            return 0;
        default:
            break;
        }
    }
}

int main2()
{
    kchar cmd[2048];
    kchar opt[2048];

    kbean tsk = ktsk_new(knil, knil, 0, 0, 1, 2, 3, 4);
    ktmr_init();
    kmsg_slot_set(tsk, 0x12345678, OnMsg_12345678);
    kmsg_slot_set(tsk, 0x34567890, OnMsg_34567890);

    kmsg_slot_set(tsk, KMSG_TIMER, OnMsg_Timer);

    for (;;) {
        scanf("%s %s", cmd, opt);
        printf("your input:cmd:[%s], opt:[%s]\n", cmd, opt);
        if (!strcmp(cmd, "t")) {
            ktmr_set(tsk, 5000, ktrue, knil, 2, 2, 2, 2);
        }
        if (!strcmp(cmd, "q")) {
            kmsg_send(tsk, KMSG_QUIT, 0, 0, 0, 0);
            printf("post quit done, break for loop\n");
            break;
        }
        if (!strcmp(cmd, "s1")) {
            kmsg_send(tsk, 0x12345678, tsk, 0x67, 0, 0x8768);
        }
        if (!strcmp(cmd, "p1")) {
            kmsg_post(tsk, 0x12345678, tsk, 0x67, 0, 0x8768);
        }

        if (!strcmp(cmd, "s3")) {
            kmsg_send(tsk, 0x34567890, tsk, 0x637, 0, 0x8768);
        }
        if (!strcmp(cmd, "p3")) {
            kmsg_post(tsk, 0x34567890, tsk, 0x637, 0, 0x8768);
        }

        if (!strcmp(cmd, "s5")) {
            kmsg_send(tsk, 0x55555555, tsk, 0x637, 0, 0x8768);
        }
        if (!strcmp(cmd, "p5")) {
            kmsg_post(tsk, 0x55555555, tsk, 0x637, 0, 0x8768);
        }

        if (!strcmp(cmd, "a5")) {
            kmsg_slot_set(tsk, 0x55555555, OnMsg_55555555);
        }
        if (!strcmp(cmd, "d5")) {
            kmsg_slot_del(tsk, 0x55555555);
        }
        if (!strcmp(cmd, "k")) {
            kmsg_post(tsk, 0x55555555, tsk, 0x11111111, 0, 0x8768);
        }
        if (!strcmp(cmd, "all")) {
            kmsg_slot_set(tsk, 0x55555555, OnMsg_55555555);
            kmsg_post(tsk, 0x12345678, tsk, 0x67, 0, 0x8768);
            kmsg_post(tsk, 0x34567890, tsk, 0x637, 0, 0x8768);
        }
    }
    printf("1 before slot del\n");
    kmsg_slot_del(tsk, 0x12345678);
    printf("2 before slot del\n");
    kmsg_slot_del(tsk, 0x34567890);
    printf("after slot del\n");

    printf("before kmsg_send KMSG_QUIT\n");
    kmsg_send(tsk, KMSG_QUIT, 0, 0, 0, 0);
    printf("after kmsg_send KMSG_QUIT\n");
    ktmr_final();

    printf("hit any key to quit...\n");
    getchar();
    getchar();
    getchar();
    getchar();
    getchar();
    getchar();
    getchar();
    getchar();
    return 0;
}


