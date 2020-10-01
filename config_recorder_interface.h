/*
 * config_recorder_interface.h
 *
 *  Created on: Aug 28, 2020
 *      Author: ning
 */

#ifndef SERVER_CONFIG_CONFIG_RECORDER_INTERFACE_H_
#define SERVER_CONFIG_CONFIG_RECORDER_INTERFACE_H_

/*
 * header
 */
#include "../../manager/global_interface.h"
/*
 * define
 */
#define		CONFIG_RECORDER_PROFILE				0

/*
 * structure
 */
//system info
typedef struct recorder_profile_config_t {
	unsigned int	max_length;			//in seconds
} recorder_profile_config_t;

typedef struct recorder_config_t {
	int	status;
	recorder_profile_config_t	profile;
} recorder_config_t;
/*
 * function
 */


#endif /* SERVER_CONFIG_CONFIG_RECORDER_INTERFACE_H_ */
