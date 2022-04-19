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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <getopt.h>

#include "hal_comm.h"
#include "hal_key.h"

#define DEV_PATH_FMT	"/dev/input/event%d"
#define CLASS_PATH_FMT	"/sys/class/input/event%d/device/name"
#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])

static key_result_t g_result[2];

static int get_event_dev_id(const char *name);
static void key_result_callback(key_result_t *result);

int main(int argc, char **argv)
{
	int ret = -1;
	int id = -1;
	int fd = -1;
	key_result_t result;
	char key[128];
	
	id = get_event_dev_id("gpio-keys");
	if (id < 0)
	{
		ret = -1;
		goto EXIT;
	}

	memset(key, 0x00, sizeof(key));
	ret = snprintf(key, sizeof(key), DEV_PATH_FMT, id);
	if (ret >= sizeof(key) || ret < 0)
	{
		dbgmsg("snprintf failed");
		ret = -1;
		goto EXIT;
	}

	fd = hal_key_open(key, O_RDONLY);
	if (fd < 0)
	{
		ret = -1;
		goto EXIT;
	}

	memset(g_result, 0x00, sizeof(g_result));
	while (1)
	{
		memset(&result, 0x00, sizeof(result));
		ret = hal_key_read(fd, &result, key_result_callback);
		if (ret != sizeof(result))
		{
			ret = -1;
			goto EXIT;
		}
	}

EXIT:
	if (fd > 0)
		hal_key_close(&fd);
	return ret;
}



static void key_result_callback(key_result_t *result)
{
	int i = 0;
	printf(">>>>> key code: %d, action: %s <<<<<\n", result->code, result->value?"pressed":"released");

	for (i = 0; i < ARRAY_SIZE(g_result); i++)
	{
		if (!g_result[i].time.tv_sec)
		{
			memcpy(&g_result[i], result, sizeof(key_result_t));
			break;
		}
		else
		{
			if (g_result[i].code == result->code)
			{
				if (g_result[i].value == 1 || result->value == 0)
				{
					printf("---- keep %ld second ----\n", result->time.tv_sec - g_result[i].time.tv_sec);
					memcpy(&g_result[i], result, sizeof(key_result_t));
				}
				else if (g_result[i].value == 0 || result->value == 1)
				{
					memcpy(&g_result[i], result, sizeof(key_result_t));
				}
				break;
			}
		}
	}

}

static int get_event_dev_id(const char *name)
{
	int ret = -1;
	int try = 20;
	int id = 0;
	int fd = -1;
	
	char file[128];
	char buf[1024];

	if (!name)
	{
		printf("name is NULL\n");
		ret = -1;
		goto EXIT;
	}
	
	while (try--)
	{
		memset(file, 0x00, sizeof(file));
		ret = snprintf(file, sizeof(file), CLASS_PATH_FMT, id);
		if (ret >= sizeof(file) || ret < 0)
		{
			printf("snprintf failed");
			ret = -1;
			goto EXIT;
		}

		if (!access(file, F_OK))
		{
			fd = open(file, O_RDONLY);
			if (fd < 0)
			{
				printf("open %s failed: %s\n", file, strerror(errno));
				ret = -1;
				goto EXIT;
			}

			memset(buf, 0x00, sizeof(buf));
			ret = read(fd, buf, sizeof(buf));
			if (ret <= 0)
			{
				printf("read %s failed: %s\n", file, strerror(errno));
				ret = -1;
				goto EXIT;
			}

			close(fd);
			fd = -1;

			if (strstr(buf, name))
			{
				ret = id;
				goto EXIT;
			}
			
		}
		else
		{
			ret = id = -1;
			goto EXIT;
		}

		id++;
	}

	if (try <= 0)
		ret = id = -1;

EXIT:
	if (fd > 0)
		close(fd);
	return ret;
}
