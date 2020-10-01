/*
 * config_recorder.c
 *
 *  Created on: Aug 16, 2020
 *      Author: ning
 */

/*
 * header
 */
//system header
#include <pthread.h>
#include <stdio.h>
//program header
#include "../../tools/log.h"
#include "../../manager/manager_interface.h"
//server header
#include "config.h"
#include "config_recorder.h"
#include "config_recorder_interface.h"
#include "rwio.h"

/*
 * static
 */
//variable
static pthread_rwlock_t			lock;
static int						dirty;
static recorder_config_t		recorder_config;
static config_map_t recorder_config_map[] = {
	{"max_length",      &(recorder_config.profile.max_length),      cfg_u32, 600,0,0,3600,  	},
    {NULL,},
};
//function
static int recorder_config_save(void);

/*
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 */



/*
 * interface
 */
static int recorder_config_save(void)
{
	int ret = 0;
	message_t msg;
	ret = pthread_rwlock_wrlock(&lock);
	if(ret)	{
		log_err("add lock fail, ret = %d\n", ret);
		return ret;
	}
	if( misc_get_bit(dirty, CONFIG_RECORDER_PROFILE) ) {
		ret = write_config_file(&recorder_config_map, CONFIG_RECORDER_PROFILE_PATH);
		if(!ret)
			misc_set_bit(&dirty, CONFIG_RECORDER_PROFILE, 0);
	}
	if( !dirty ) {
		/********message body********/
		msg_init(&msg);
		msg.message = MSG_MANAGER_TIMER_REMOVE;
		msg.arg_in.handler = recorder_config_save;
		/****************************/
		manager_message(&msg);
	}
	ret = pthread_rwlock_unlock(&lock);
	if (ret)
		log_err("add unlock fail, ret = %d\n", ret);

	return ret;
}

int config_recorder_read(void)
{
	int ret,ret1=0;
	pthread_rwlock_init(&lock, NULL);
	ret = pthread_rwlock_wrlock(&lock);
	if(ret)	{
		log_err("add lock fail, ret = %d\n", ret);
		return ret;
	}
	ret = read_config_file(&recorder_config_map, CONFIG_RECORDER_PROFILE_PATH);
	if(!ret)
		misc_set_bit(&recorder_config.status, CONFIG_RECORDER_PROFILE,1);
	else
		misc_set_bit(&recorder_config.status, CONFIG_RECORDER_PROFILE,0);
	ret1 |= ret;
	ret = pthread_rwlock_unlock(&lock);
	if (ret)
		log_err("add unlock fail, ret = %d\n", ret);
	ret1 |= ret;
	return ret1;
}

int config_recorder_get(void **t, int *size)
{
	int ret = 0;
	ret = pthread_rwlock_wrlock(&lock);
	if(ret)	{
		log_err("add lock fail, ret = %d\n", ret);
		return ret;
	}
	*t = calloc( sizeof(recorder_config_t), 1);
	if( *t == NULL ) {
		log_err("memory allocation failed");
		return -1;
	}
	memcpy( (recorder_config_t*)(*t), &recorder_config, sizeof(recorder_config_t));
	*size = sizeof(recorder_config_t);
	ret = pthread_rwlock_unlock(&lock);
	if (ret)
		log_err("add unlock fail, ret = %d\n", ret);
	return ret;
}

int config_recorder_set(int module, void *arg)
{
	int ret = 0;
	ret = pthread_rwlock_wrlock(&lock);
	if(ret)	{
		log_err("add lock fail, ret = %d\n", ret);
		return ret;
	}
	if(dirty==0) {
		message_t msg;
	    /********message body********/
		msg_init(&msg);
		msg.message = MSG_MANAGER_TIMER_ADD;
		msg.sender = SERVER_CONFIG;
		msg.arg_in.cat = 60000;	//1min
		msg.arg_in.dog = 0;
		msg.arg_in.duck = 0;
		msg.arg_in.handler = &recorder_config_save;
		/****************************/
		manager_message(&msg);
	}
	misc_set_bit(&dirty, module, 1);
	if( module == CONFIG_RECORDER_PROFILE) {
		memcpy( (recorder_profile_config_t*)(&recorder_config.profile), arg, sizeof(recorder_profile_config_t));
	}
	ret = pthread_rwlock_unlock(&lock);
	if (ret)
		log_err("add unlock fail, ret = %d\n", ret);
	return ret;
}

int config_recorder_get_config_status(int module)
{
	int st,ret=0;
	ret = pthread_rwlock_wrlock(&lock);
	if(ret)	{
		log_err("add lock fail, ret = %d\n", ret);
		return ret;
	}
	if(module==-1)
		st = recorder_config.status;
	else
		st = misc_get_bit(recorder_config.status, module);
	ret = pthread_rwlock_unlock(&lock);
	if (ret)
		log_err("add unlock fail, ret = %d\n", ret);
	return st;
}
