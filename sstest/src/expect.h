#ifndef EXPECT_H
#define  EXPECT_H

#include <stsdef.h>

extern unsigned int passed;
extern unsigned int failed;

#define EXPECT_EQ(x, y)\
    {\
        int xx = x;\
        int yy = y;\
        if ((xx) == (yy))\
        {\
            passed++;\
        }\
        else\
        {\
            failed++;\
            printf("FAIL %s:%d\n\texpected " #x " == " #y "\n\tgot %d (0x%x) != %d (0x%x)\n",\
                    __FILE__, __LINE__, xx, (unsigned int)xx, yy, (unsigned int)yy);\
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