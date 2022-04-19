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
#ifndef __HAL_COMM__
#define __HAL_COMM__

#ifdef _cplusplus
extern "C" {
#endif

#include <pthread.h>

#define dbgmsg(fmt, ...) printf("%s:%s[%d]: " fmt, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define free_mem(p) do { if (NULL != p) {free (p); p = NULL;} } while (0)

typedef enum 
{
    HAL_FAILURE = -1,
    HAL_SUCCESS = 0,
    HAL_RET_END
} result_e;

typedef pthread_mutex_t mutex_t;
#define mutex_init(x)   pthread_mutex_init(&x, NULL)
#define mutex_deinit(x) pthread_mutex_destroy(&x)
#define mutex_lock(x)   pthread_mutex_lock(&x)
#define mutex_unlock(x) pthread_mutex_unlock(&x)

#ifdef _cplusplus
}
#endif

#endif /*__HAL_WIFI__*/