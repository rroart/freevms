#ifndef _LINUX_ELEVATOR_H
#define _LINUX_ELEVATOR_H

typedef int (elevator_merge_fn) (request_queue_t *, struct request **, struct list_head *,
                                 struct buffer_head *, int, int);

typedef void (elevator_merge_cleanup_fn) (request_queue_t *, struct request *, int);

typedef void (elevator_merge_req_fn) (struct request *, struct request *);

struct elevator_s
{
    int read_latency;
    int write_latency;

    elevator_merge_fn *elevator_merge_fn;
    elevator_merge_cleanup_fn *elevator_merge_cleanup_fn;
    elevator_merge_req_fn *elevator_merge_req_fn;

    unsigned int queue_ID;
};

typedef struct blkelv_ioctl_arg_s
{
    int queue_ID;
    int read_latency;
    int write_latency;
    int max_bomb_segments;
} blkelv_ioctl_arg_t;

#define BLKELVGET   _IOR(0x12,106,sizeof(blkelv_ioctl_arg_t))
#define BLKELVSET   _IOW(0x12,107,sizeof(blkelv_ioctl_arg_t))

/*
 * Return values from elevator merger
 */
#define ELEVATOR_NO_MERGE   0
#define ELEVATOR_FRONT_MERGE    1
#define ELEVATOR_BACK_MERGE 2

#endif
