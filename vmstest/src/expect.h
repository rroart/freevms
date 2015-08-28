#ifndef EXPECT_H
#define  EXPECT_H

#include <stdio.h>
#include <stsdef.h>

extern unsigned int passed;
extern unsigned int failed;

#define EXPECT_EQ(x, y)\
    {\
        long xx = x;\
        long yy = y;\
        if ((xx) == (yy))\
        {\
            passed++;\
        }\
        else\
        {\
            failed++;\
            printf("FAIL %s:%d\n\texpected " #x " == " #y "\n\tgot %ld (0x%lx) != %ld (0x%lx)\n",\
                    __FILE__, __LINE__, xx, (unsigned long)xx, yy, (unsigned long)yy);\
        }\
    }

#define EXPECT_NE(x, y)\
    {\
        long xx = x;\
        long yy = y;\
        if ((xx) != (yy))\
        {\
            passed++;\
        }\
        else\
        {\
            failed++;\
            printf("FAIL %s:%d\n\texpected " #x " != " #y "\n\tgot %ld (0x%lx)\n",\
                    __FILE__, __LINE__, xx, (unsigned long)xx);\
        }\
    }

#define EXPECT_GE(x, y)\
    {\
        long xx = x;\
        long yy = y;\
        if ((xx) >= (yy))\
        {\
            passed++;\
        }\
        else\
        {\
            failed++;\
            printf("FAIL %s:%d\n\texpected " #x " >= " #y "\n\tgot %ld (0x%lx) < %ld (0x%lx)\n",\
                    __FILE__, __LINE__, xx, (unsigned long)xx, yy, (unsigned long)yy);\
        }\
    }

#define EXPECT_GT(x, y)\
    {\
        long xx = x;\
        long yy = y;\
        if ((xx) > (yy))\
        {\
            passed++;\
        }\
        else\
        {\
            failed++;\
            printf("FAIL %s:%d\n\texpected " #x " > " #y "\n\tgot %ld (0x%lx) <= %ld (0x%lx)\n",\
                    __FILE__, __LINE__, xx, (unsigned long)xx, yy, (unsigned long)yy);\
        }\
    }

#define EXPECT_LE(x, y)\
    {\
        long xx = x;\
        long yy = y;\
        if ((xx) <= (yy))\
        {\
            passed++;\
        }\
        else\
        {\
            failed++;\
            printf("FAIL %s:%d\n\texpected " #x " <= " #y "\n\tgot %ld (0x%lx) > %ld (0x%lx)\n",\
                    __FILE__, __LINE__, xx, (unsigned long)xx, yy, (unsigned long)yy);\
        }\
    }

#define EXPECT_LT(x, y)\
    {\
        long xx = x;\
        long yy = y;\
        if ((xx) < (yy))\
        {\
            passed++;\
        }\
        else\
        {\
            failed++;\
            printf("FAIL %s:%d\n\texpected " #x " < " #y "\n\tgot %ld (0x%lx) >= %ld (0x%lx)\n",\
                    __FILE__, __LINE__, xx, (unsigned long)xx, yy, (unsigned long)yy);\
        }\
    }

#define EXPECT_SUCCESS(x)\
    {\
        int xx = x;\
        if ($VMS_STATUS_SUCCESS(xx))\
        {\
            passed++;\
        }\
        else\
        {\
            failed++;\
            printf("FAIL %s:%d\n\texpected " #x " == SUCCESS\n\tgot %d (0x%x)\n",\
                    __FILE__, __LINE__, xx, (unsigned int)xx);\
        }\
    }

#endif /* EXPECT_H */
