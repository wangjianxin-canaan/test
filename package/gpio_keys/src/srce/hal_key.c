/* Copyright 2022 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "hal_comm.h"
#include "hal_key.h"

int hal_key_open(const char *key, int mode)
{
	int ret = HAL_FAILURE;
    
    if (!key)
    {
        dbgmsg ("key is NULL\n");
        ret = HAL_FAILURE;
        goto EXIT;
    }

    ret = open(key, mode, 0777);
    if (0 > ret)
    {
        dbgmsg ("open %s failed: %s\n", key, strerror(errno));
        ret = HAL_FAILURE;
        goto EXIT;
    }

EXIT:
    return ret;
}

int hal_key_close(int *fd)
{
	int ret = HAL_FAILURE;
	
	if (!fd || *fd < 0)
	{
		dbgmsg("fd is NULL, or *fd < 0\n");
		ret = HAL_FAILURE;
		goto EXIT;
	}
	

    close (*fd);
    *fd = -1;
    ret = HAL_SUCCESS;
    
EXIT:
   	return ret;
}

int hal_key_read(int fd, key_result_t *result, callback_func_t cb)
{
	int ret = HAL_FAILURE;
	struct input_event ev;

	if (fd < 0)
	{
		dbgmsg("fd < 0\n");
		ret = HAL_FAILURE;
		goto EXIT;
	}

	if (!result)
	{
		dbgmsg("result is NULL\n");
		ret = HAL_FAILURE;
		goto EXIT;
	}
	
	memset(&ev, 0x00, sizeof(ev));
	ret = read(fd, &ev, sizeof(ev));
	if (ret != sizeof(ev))
	{
		dbgmsg("read length %d invalid\n", ret);
		ret = HAL_FAILURE;
		goto EXIT;
	}

	if (ev.type == EV_KEY)
	{
		result->code = ev.code;
		result->value = ev.value;
		result->time.tv_sec = ev.time.tv_sec;
		result->time.tv_usec = ev.time.tv_usec;

		if (cb)
			cb(result);
	}

	ret = sizeof(key_result_t);
EXIT:
	return ret;
}
