#ifndef __LINUX_NETFILTER_H
#define __LINUX_NETFILTER_H

#ifdef __KERNEL__
#include <linux/init.h>
#include <linux/types.h>
#include <linux/skbuff.h>
#include <linux/net.h>
#include <linux/if.h>
#include <linux/wait.h>
#endif

/* Responses from hook functions. */
#define NF_DROP 0
#define NF_ACCEPT 1
#define NF_STOLEN 2
#define NF_QUEUE 3
#define NF_REPEAT 4
#define NF_MAX_VERDICT NF_REPEAT

/* Generic cache responses from hook functions. */
#define NFC_ALTERED 0x8000
#define NFC_UNKNOWN 0x4000

#ifdef __KERNEL__
#define NF_HOOK(pf, hook, skb, indev, outdev, okfn) (okfn)(skb)

/* From arch/i386/kernel/smp.c:
 *
 *  Why isn't this somewhere standard ??
 *
 * Maybe because this procedure is horribly buggy, and does
 * not deserve to live.  Think about signedness issues for five
 * seconds to see why.      - Linus
 */

/* Two signed, return a signed. */
#define SMAX(a,b) ((ssize_t)(a)>(ssize_t)(b) ? (ssize_t)(a) : (ssize_t)(b))
#define SMIN(a,b) ((ssize_t)(a)<(ssize_t)(b) ? (ssize_t)(a) : (ssize_t)(b))

/* Two unsigned, return an unsigned. */
#define UMAX(a,b) ((size_t)(a)>(size_t)(b) ? (size_t)(a) : (size_t)(b))
#define UMIN(a,b) ((size_t)(a)<(size_t)(b) ? (size_t)(a) : (size_t)(b))

/* Two unsigned, return a signed. */
#define SUMAX(a,b) ((size_t)(a)>(size_t)(b) ? (ssize_t)(a) : (ssize_t)(b))
#define SUMIN(a,b) ((size_t)(a)<(size_t)(b) ? (ssize_t)(a) : (ssize_t)(b))
#endif /*__KERNEL__*/

#endif /*__LINUX_NETFILTER_H*/
