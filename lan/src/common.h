/*
 * common.h
 *
 *  Created on: Jan 1, 2016
 *      Author: guido
 */

#ifndef LAN_COMMON_H_
#define LAN_COMMON_H_

#include <vms_types.h>

void config_in_dev(struct in_device ** in);

int register_inetaddr_notifier(struct notifier_block *nb);
int unregister_inetaddr_notifier(struct notifier_block *nb);

UINT32 in_aton(const char *str);

#endif /* COMMON_H_ */
