#include "buddy.h"

int main(void){
    input_size();

    int i=0;
    struct page* page;
    struct page* page1,*page2;
    init_memory();
    // page=alloc_pages(0,0);
    page=alloc_pages(0,1);

    page1=alloc_pages(0,2);
    page2=alloc_pages(0,3);
    _free_pages(page->addr);
    _free_pages(page1->addr);
    _free_pages(page2->addr);
    for(i=0;i<=9;i++) _show_free_order_list(i);
    printf("\n\n");
    free_memory();
    return 0;
}