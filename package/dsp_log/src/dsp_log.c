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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>     //exit
#include <stdint.h>     //uint32_t
#include <stdbool.h>
#include <sys/mman.h>
#include <string.h>
#include <math.h>
#define MMAP_SIZE           0x4000

#define IOMUX_BASE_ADDR     0x80000000U   //SRAM
#define SHARE_MEMORY_LENGTH         (0x2000 - 0x8)
struct share_memory {
    uint8_t dsp_input_begin;
    uint8_t dsp_input_end;
    uint8_t reserved1[2];
    uint32_t dsp_input_length;
    uint8_t dsp_input_data[SHARE_MEMORY_LENGTH];
    uint8_t cpu_input_begin;
    uint8_t cpu_input_end;
    uint8_t reserved2[2];
    uint32_t cpu_input_length;
    uint8_t cpu_input_data[SHARE_MEMORY_LENGTH];
};

int main(void)
{
    int fd;
    volatile uint8_t *mmap_addr = NULL;
    volatile struct share_memory *fft_share_memory;
    float *result_addr;
    int i = 0;

    fd = open("/dev/mem", O_RDWR | O_NDELAY);
    if(fd < 0)
    {
        printf("open /dev/mem error\r\n");
        return -1;
    }

#if 0
    mmap_addr = (uint8_t*)mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, IOMUX_BASE_ADDR);
    //result_addr = (float *)(mmap_addr + 0x1000);
    if(mmap_addr == NULL)
    {
        printf("mmap_addr error\r\n");
        return -1;
    }

    while(*mmap_addr != 0xff)
    {
        sleep(1);
        printf("begin\r\n");
        i++;
        if(i > 20)
            break;
    }
    printf("begin calculate\r\n");
    i = 0;
    while(*(mmap_addr+1) != 0xff)
    {
        sleep(1);
        printf("begin\r\n");
        i++;
        if(i > 20)
            break;
    }
    printf("end calculate\r\n");
    printf("data length:%u\r\n", *(uint32_t*)(mmap_addr + 4));

    for(i = 0; i < 128; i++)
    {
        if(i%8 == 0)
            printf("\r\n");
        //printf("data[%2d]:%14.8f  ", i, result_addr[i]);
        printf("data[%3d]:%14.8f  ", i, *(float*)(mmap_addr + 8 + i*4));
    }

    memset(mmap_addr, 0, 0x4000);

    munmap(mmap_addr, MMAP_SIZE);
#endif

#if 1
    fft_share_memory = (struct share_memory*)mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, IOMUX_BASE_ADDR);
    //result_addr = (float *)(mmap_addr + 0x1000);
    if(fft_share_memory == NULL)
    {
        printf("fft_share_memory error\r\n");
        return -1;
    }

    while(1)
    {
        while(fft_share_memory->dsp_input_begin != 0xff)
        {
            sleep(1);
            // printf("begin\r\n");
            // i++;
            // if(i > 20)
            //     break;
        }
        printf("begin calculate\r\n");
        i = 0;
        while(fft_share_memory->dsp_input_end != 0xff)
        {
            sleep(1);
            // printf("end\r\n");
            // i++;
            // if(i > 20)
            //     break;
        }
        printf("end calculate\r\n");
        printf("data length:%u\r\n", fft_share_memory->dsp_input_length);

        for(i = 0; i < 128; i++)
        {
            if(i%8 == 0)
                printf("\r\n");
            //printf("data[%2d]:%14.8f  ", i, result_addr[i]);
            printf("data[%3d]:%14.8f  ", i, *(float *)(fft_share_memory->dsp_input_data + i*4));
        }

        memset(fft_share_memory, 0, 0x4000);
    }

    munmap(mmap_addr, MMAP_SIZE);
#endif

    return 0;
}
