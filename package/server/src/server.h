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
#ifndef __SERVER_H
#define __SERVER_H

/******************Include header files*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <semaphore.h>
#include <pthread.h>

/**************************Macro define*/
#define TEST_PORT 9988
#define MTU_NUM 1400
#define COUNT_CYCLE 5
#define CLIENT_MAXNUM 8

/*************************Struct defien*/
typedef struct _thread_args {
	int client_sockfd;
	char *data_pointer;
	long packet_count;
	int thread_seq;
	pthread_t thread_id;
	struct _thread_args *next;
}thread_args;

/**************Declearation of function*/
char *malloc_packet(void);
int timer_cmp(struct timeval , struct timeval);
void *thread_response(void *arg);
void *thread_count(void *arg);
float calculate_speed(thread_args *);

#endif
