#ifndef __EX23_H_
#define __EX23_H_

#define LAYS_8_OFFSET(offset)                   \
    LAYS_7_OFFSET(offset)                       \
        DUFFS_LAYOUT(offset, 0)

#define LAYS_7() LAYS_7_OFFSET(0)

#define LAYS_7_OFFSET(offset)                   \
    DUFFS_LAYOUT(offset, 7)                     \
        DUFFS_LAYOUT(offset, 6)                 \
        DUFFS_LAYOUT(offset, 5)                 \
        DUFFS_LAYOUT(offset, 4)                 \
        DUFFS_LAYOUT(offset, 3)                 \
        DUFFS_LAYOUT(offset, 2)                 \
        DUFFS_LAYOUT(offset, 1)                

#define DUFFS_LAYOUT(offset, remainder)         \
    case (8 * offset + remainder): *to++ = *from++;


#endif // __EX23_H_
