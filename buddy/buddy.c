#include "buddy.h"
#ifndef NULL
#define NULL 0
#endif

#define STRUCT_DATA (1*1024*1024)
// mem_map_t : physical page map
//|real_memory
// struct page(points physical page)  // bitmap // physical page //
void read_for_memory(void){
    // real memory : start point of physical memory
    real_memory=mmap(0,mem_size+STRUCT_DATA,
    PROT_READ|PROT_WRITE|PROT_EXEC,MAP_ANON|MAP_PRIVATE,-1,0);
    printf("mem ready, add: %lx\n",(unsigned long)real_memory);
}
void* get_address_map(int size)
{
    char* addr;
    addr=(char*)((char*)real_memory+mem_offset);
    memset(addr,(int)0,size);
    mem_offset+=size;
    return addr;
}

void mapping_page(mem_map_t* mem_map){
    unsigned long temp=mem_offset;
    while(mem_offset<=mem_size+STRUCT_DATA){
        mem_map->addr=(unsigned long*)((char*)real_memory+mem_offset);
        mem_offset+=PAGE_SIZE;
        mem_map++;
    }
    mem_offset=temp;
}

int cat_cur_order(unsigned long mem){
    int i=BUDDY_MAX_ORDER-1;
    while(i>=0){
        if((mem)==(PAGE_SIZE<<i)){
            return i;
        }
        i--;
    }
    if(mem==(PAGE_SIZE<<(BUDDY_MAX_ORDER-1) ) ) return BUDDY_MAX_ORDER;
    return -1;
}   

static __inline__ int constant_test_bit(int nr,const volatile void* addr){
    return ((1UL<<(nr&31))&
            (((const volatile unsigned int*)addr)[nr>>5]))!=0;

}

static __inline__ void __change_bit(int nr,volatile void *addr)
{
    if(constant_test_bit(nr,addr)==1){
        (((volatile unsigned int*)addr)[nr>>5])&=(0xFFFFFFFF^(1UL<<(nr&31)));
    }else{
        (((volatile unsigned int*)addr)[nr>>5])|=(1UL<<(nr&31));
    }
}

static __inline__ int __test_and_change_bit(int nr,volatile void* addr){
    int oldbit;
    if((oldbit=constant_test_bit(nr,addr))==1 ){
        (((volatile unsigned int*)addr)[nr>>5])&=(0xFFFFFFFF^(1UL<<(nr&31)));
    }else{
        (((volatile unsigned int*)addr)[nr>>5])|=(1UL<<(nr&31));
    }
    return oldbit;
}

#define MARK_USED(index, order, area) \
                    __change_bit((index)>>(1+(order)),(area)->map) 

void init_memory(void){
    int i;
    unsigned long cur_size=0;
    if((mem_size<=0) ||(mem_size%PAGE_SIZE)!=0 )
    {
        printf("allcoate size %d bytes not permited\t\n",mem_size);
        _exit(-1);
    }
    read_for_memory();
    printf("allaction memory, size %d bytes \t\n",mem_size);
    free_pages=TOTAL_PAGES(mem_size);
    printf("total number of page : %ld\n",free_pages);
     // to use at get_address_map, mem_offset : after struct page. struct page points to physcial memory
    mem_offset=sizeof(struct page)*TOTAL_PAGES(mem_size);
    imem_map=(struct page*)real_memory;
    for(int i=0;i<BUDDY_MAX_ORDER;i++){
        unsigned long bitmap_size;
        INIT_LIST_HEAD(&free_area[i].free_list);
        bitmap_size=(mem_size-1)>>(i+4); // long : 8byte : 64bit : 2^6,??
        bitmap_size=LONG_ALIGN(bitmap_size+1);

        free_area[i].map=(unsigned long*)get_address_map(bitmap_size);
        *(free_area[i].map)=0;
    }
    mem_offset=STRUCT_DATA;
    mapping_page(imem_map); // construct physcial page map
    init_buddy();
}

void init_buddy(void){
    unsigned long nr_next,nr_prev;
    int cur_order=BUDDY_MAX_ORDER-1;
    unsigned long total_page=free_pages;
    unsigned long top_buddy_size=PAGE_SIZE<<cur_order;
    free_area_t* area=&free_area[cur_order];
    if((top_buddy_size*2)>=(mem_size) ){
        cur_order=cat_cur_order(mem_size);
        area=&free_area[--cur_order];
    }
    top_buddy_size=PAGE_SIZE<<cur_order;
    unsigned long order_page=TOTAL_PAGES(top_buddy_size);

    list_add(&(imem_map[0].list),&(area)->free_list ); //??
    nr_prev=0;nr_next=0;

    while (1)
    {
        // if order ==11, nr_prev=0 nr_next=2048
        nr_prev=nr_next;
        nr_next=nr_prev+(1UL<<cur_order);
        // 2048+2^2
        if(nr_next+order_page>=total_page){
            list_add(&(imem_map[nr_next].list),&(area)->free_list);
            MARK_USED(nr_prev,cur_order,area);
            break;
        }
        while((total_page-nr_next)<=order_page ){
            if(cur_order==0) break;
            cur_order--;
            area--;
            order_page=1<<cur_order;
        }
        nr_prev=nr_next;
        list_add(&(imem_map[nr_prev]).list,&(area)->free_list);
        MARK_USED(nr_prev,cur_order,area);
    }
    
}

unsigned long __get_free_pages(unsigned int gfp_mask,unsigned int order)
{
    struct page * page;
    page= alloc_pages(gfp_mask,order);
    if(!page) return 0;
    return (unsigned long) page_address(page);
}

struct page* alloc_pages(unsigned int gfp_mask,unsigned int order){
    return __alloc_pages(gfp_mask,order,NULL);
}
struct page* __alloc_pages(unsigned int gfp_mask,unsigned int order,zonelist_t *zonelist){
    struct page * page;
    unsigned int curr_order=order;
    free_area_t* area=&free_area[order];
    // hard//////////////////////////////////////
    struct list_head* head,* curr;
    do{
        head=&area->free_list;
        curr=head->next;
        if(curr!=head){ // if there is PAGE
            unsigned long index;
            page=list_entry(curr,struct page,list);
            printf("page : %p\n",page);
            list_del(curr);
            index=GET_NR_PAGES((unsigned long)page->addr);
            if(curr_order!=BUDDY_MAX_ORDER-1) MARK_USED(index,curr_order,area);
            free_pages-=1UL<<order;

            page=expand(NULL,page,index,order,curr_order,area);
            page->order=order;
            return page;
        }
        //check free page low to high
        curr_order++;
        area++;
    }while(curr_order<BUDDY_MAX_ORDER);
    return NULL;
}

struct page* expand(zone_t* zone,struct page* page,unsigned long index, int low,int high,free_area_t*area)
{
    unsigned long size=1<<(high);
    // mark bitmap high to low
    while(high>low)
    {
        area--;
        high--;
        size>>=1;
        list_add(&(page)->list,&(area)->free_list);
        MARK_USED(index,high,area);
        index+=size;
        page+=size;
    }
    return page;
}

// ptr is page->addr
void _free_pages(void* ptr)
{
    int i;
    i=(((char*)ptr-(char*)imem_map[0].addr)>>PAGE_SHIFT);
    __free_pages(&imem_map[i],imem_map[i].order);
}

void __free_pages(struct page* page,unsigned int order)
{
    __free_pages_ok(page,order);
}
// page and order of page belong to
void __free_pages_ok(struct page* page,unsigned int order){
    unsigned long index,page_idx,mask;
    free_area_t * area;
    struct page* base;

    mask=(~0UL)<<order; // ~0UL == -1
    base=imem_map;
    page_idx=GET_NR_PAGES((unsigned long)page->addr);
    
    index=page_idx>>(1+order); // index : bitmap index

    area=&free_area[order];
    free_pages-=mask;
    /// harrrrd
    while (mask+(1<<(BUDDY_MAX_ORDER-1) ))
    {
        struct page *buddy1,*buddy2;
        if(area>=free_area+BUDDY_MAX_ORDER){
            printf("over free_area boundory\n");
            break;
        }
        if(!__test_and_change_bit(index,area->map)){
            break;
        }
        buddy1=&imem_map[(page_idx^-mask)];
        buddy2=&imem_map[page_idx];
        list_del(&buddy1->list);
        mask<<=1;
        area++;
        index>>=1;
        page_idx&=mask;

    }
    list_add(&imem_map[page_idx].list,&area->free_list);    
}

void _show_free_order_list(int order)
{
    free_area_t* area=&free_area[order];
    struct page *p,*q;
    int i=0;
    p=(struct page*)(area)->free_list.next;
    q=(struct page*)&(area)->free_list;
    printf("----------------- order %d-----------------\n",order);
    while(p!=q){
        printf("%ld \t",GET_NR_PAGES((unsigned long)p->addr));
        p=(struct page*)p->list.next;
        if(++i==7){
            printf("\n"); i=0;
        }
       
    }
    printf("\n-------------------------------------------\n");
}

void free_memory(void)
{
    munmap(real_memory,mem_size);
    printf("free allocated memory\n");
}

void input_size(void){
    int size;
    printf("TOTAL MEMORY SIZE? (KB)");
    scanf("%d",&size);
    mem_size=size<<10; // mem size :KB? B?
}