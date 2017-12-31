#include <stdio.h>
#include <time.h>
#include <ssdef.h>
#include <iodef.h>
#include <misc.h>
#include <descrip.h>
#include <lnmdef.h>

#define MBXBUFSIZ 1280
#define MBXBUFQUO 3840

int done=0;

int myast(int some)
{
    printf("myast\n");
    done=1;
}

main()
{
    struct _iosb myiosb;

    char mybuf1[MBXBUFSIZ]="test1";
    char mybuf2[MBXBUFSIZ]="test2";
    char mybuf3[MBXBUFSIZ]="test3";

    char * out1="hello";
    char * out2="world";
    char * out3="hi";

    void *p1, mbxast();
    char mbuffer[MBXBUFSIZ];
    unsigned short mbxchan1, mbxchan2, mbxchan3, mbxiosb;
    unsigned int status, outlen;
    unsigned int mbuflen=MBXBUFSIZ, bufquo=MBXBUFQUO, promsk=0;
    $DESCRIPTOR(mblognam1,"MAILBOX1");
    $DESCRIPTOR(mblognam2,"MAILBOX2");
    $DESCRIPTOR(mblognam3,"MAILBOX3");

    printf("this was originally in 1: %s\n",mybuf1);
    printf("this was originally in 2: %s\n",mybuf2);
    printf("this was originally in 3: %s\n",mybuf3);

    status = sys$crembx(0,&mbxchan1,mbuflen,bufquo,promsk,0,&mblognam1,0);
    printf("status %x mbxchan %x \n", status,mbxchan1);
    if ((status&1)==0) signal(status);
    status = sys$crembx(0,&mbxchan2,mbuflen,bufquo,promsk,0,&mblognam2,0);
    printf("status %x mbxchan %x \n", status,mbxchan2);
    if ((status&1)==0) signal(status);
    status = sys$crembx(0,&mbxchan3,mbuflen,bufquo,promsk,0,&mblognam3,0);
    printf("status %x mbxchan %x \n", status,mbxchan3);
    if ((status&1)==0) signal(status);

    printf("before qio %x\n",time(0));

    status=sys$qio(0,mbxchan1,IO$_READVBLK,0,0,0,mybuf1,512,0,0,0,0);

    printf("after qio %x\n",time(0));
    printf("status %x done %x \n", status,done);
    if ((status&1)==0) signal(status);

    printf("before qiow %x\n",time(0));
    status=sys$qio(0,mbxchan1,IO$_WRITEVBLK,0,0,0,out1,strlen(out1),0,0,0,0);
    printf("after qiow %x\n",time(0));
    printf("status %x done %x \n", status,done);
    if ((status&1)==0) signal(status);
    printf("before qiow %x\n",time(0));
    status=sys$qio(0,mbxchan1,IO$_WRITEVBLK,0,0,0,out2,strlen(out2),0,0,0,0);
    printf("after qiow %x\n",time(0));
    printf("status %x done %x \n", status,done);
    if ((status&1)==0) signal(status);

    printf("this was read from 1: %s\n",mybuf1);

    printf("Pausing...\n");
    sleep(2);





    printf("before qio %x\n",time(0));

    status=sys$qio(0,mbxchan2,IO$_READVBLK|IO$M_STREAM,0,0,0,mybuf2,512,0,0,0,0);

    printf("after qio %x\n",time(0));
    printf("status %x done %x \n", status,done);
    if ((status&1)==0) signal(status);

    printf("before qiow %x\n",time(0));
    status=sys$qio(0,mbxchan2,IO$_WRITEVBLK,0,0,0,out1,strlen(out1),0,0,0,0);
    printf("after qiow %x\n",time(0));
    printf("status %x done %x \n", status,done);
    if ((status&1)==0) signal(status);
    printf("before qiow %x\n",time(0));
    status=sys$qio(0,mbxchan2,IO$_WRITEVBLK,0,0,0,out2,strlen(out2),0,0,0,0);
    printf("after qiow %x\n",time(0));
    printf("status %x done %x \n", status,done);
    if ((status&1)==0) signal(status);

    printf("this was read from 2: %s\n",mybuf2);

    printf("Pausing...\n");
    sleep(2);




    printf("before qio %x\n",time(0));

    printf("before qiow %x\n",time(0));
    status=sys$qio(0,mbxchan3,IO$_WRITEVBLK,0,0,0,out1,strlen(out1),0,0,0,0);
    printf("after qiow %x\n",time(0));
    printf("status %x done %x \n", status,done);
    if ((status&1)==0) signal(status);
    printf("before qiow %x\n",time(0));
    status=sys$qio(0,mbxchan3,IO$_WRITEVBLK,0,0,0,out2,strlen(out2),0,0,0,0);
    printf("after qiow %x\n",time(0));
    printf("status %x done %x \n", status,done);
    if ((status&1)==0) signal(status);

    status=sys$qiow(0,mbxchan3,IO$_READVBLK|IO$M_STREAM,0,0,0,mybuf3,512,0,0,0,0);

    printf("after qio %x\n",time(0));
    printf("status %x done %x \n", status,done);
    if ((status&1)==0) signal(status);

    printf("this was read from 3: %s\n",mybuf3);

    sleep(2);


    {

        struct item_list_3
        {
            short buflen, item_code;
            void *bufaddr;
            void *retlenaddr;
        } itmlist[2];

        char buf[50];
        int retlen;

        itmlist[0].item_code=LNM$_STRING;
        itmlist[0].bufaddr=buf;
        itmlist[0].buflen=50;
        itmlist[0].retlenaddr=&retlen;
        itmlist[1].item_code=0;
        itmlist[1].buflen=0;
        itmlist[1].bufaddr=0;

        $DESCRIPTOR(tab,"LNM$TEMPORARY_MAILBOX");

        status = sys$trnlnm(0,
                            &tab,
                            &mblognam1, 0,
                            itmlist);
        if ((status&1)==0) signal(status);
        printf("mailbox1 translates to %s\n",buf);
    }
    return SS$_NORMAL;
}
