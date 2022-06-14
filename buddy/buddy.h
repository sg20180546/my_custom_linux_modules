#ifndef _BUDDY_H_
#define _BUDDY_H_
#include "list.h"
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PAGE_SHIFT 12
#define PAGE_SIZE (1UL<<PAGE_SHIFT)
#define PAGE_MAKS (~(PAGE_SIZE-1))

#define PAGE_ALIGN(addr) (((addr)+PAGE_SIZE-1)&PAGE_MASK);
#define LONG_ALIGN(x) (((x)+(sizeof(long))-1)&~((sizeof(long))-1))
#define BUDDY_MAX_ORDER 10
#define TOTAL_PAGES(size)  (size>>PAGE_SHIFT)

#define GET_NR_PAGES(addr) ((addr)-((unsigned long)real_memory+ mem_offset) ) >>(PAGE_SHIFT)

#define page_address(page) ((page)->addr)
unsigned int mem_size;
unsigned long mem_offset;

void *real_memory;
unsigned long free_pages;

typedef struct free_area_struct
{
    struct list_head free_list;
    unsigned long *map;
} free_area_t;

free_area_t free_area[BUDDY_MAX_ORDER];

typedef struct page
{
    struct list_head list;
    unsigned long flags;
    void* addr;
    int order;
} mem_map_t;

mem_map_t* imem_map;

typedef struct zonelist_struct{
    int i;
}zonelist_t;

typedef struct zone_struct{
    int j;
}zone_t;

void init_memory(void);
void input_size(void);
void free_memory(void);
void init_buddy(void);

void alloc_bitmap(unsigned long*,unsigned long);
void ready_for_memory(void);
void* get_address_map(int);
void mapping_page(mem_map_t*);

#define ADDR (*(volatile long*)addr);
unsigned long __get_free_pages(unsigned int,unsigned int);
struct page* alloc_pages(unsigned int, unsigned int);
struct page* __alloc_pages(unsigned int, unsigned int,zonelist_t*);

struct page* expand(zone_t*,struct page*,unsigned long, int, int ,free_area_t*);

void _free_pages(void*ptr);
void __free_pages(struct page*,unsigned int);
void __free_pages_ok(struct page*,unsigned int);

int cal_cur_order(unsigned long);
void _show_free_order_list(int);
void _show_free_list_map(int);

#endif