#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "libs/avl.h"

int compare(const void * a, const void * b){
    int aTmp = *(int*)a,
        bTmp = *(int*)b;
    if(aTmp < bTmp) return -1;
    return aTmp > bTmp ? 1 : 0;
}

void printing(const void * e){
    printf("%d\n", *(int*) e);
}

int main(){
    clock_t start = clock();
    avl_root * testTree = avl_init(&compare, &free, &printing);
    clock_t a = start, b;
    //                        351238537, 1546906
    for(int i = 0, j = 0; i < 15469060; ++i){
        int * p = (int*) malloc(sizeof(int));
        *p = rand() % 100000;
        avl_insert(testTree, p);
        if(!(i % 1000000)){
            b = clock();
            printf("%d. inserting of 1000000 took %lf s.\nTree height is: %ld\n",
                   j++,(double)(b - a) / CLOCKS_PER_SEC, avl_get_height(testTree));
            a = b;
        }
    }
    clock_t end = clock();
    printf("Inserting done in %lf s.\n", (double)(end - start) / CLOCKS_PER_SEC);
/*    avl_ordered_silent_print(testTree,1);
    printf("Next print.\n");
    avl_ordered_silent_print(testTree,2);
    printf("Printing done.\n");
*/    for(int i = 0, j = 0; i < 15469060; ++i){
        int p =  rand() % 100000;
        avl_delete(testTree, &p);
        if(!(i % 1000000)){
            b = clock();
            printf("%d. deleting of 1000000 took %lf s.\nTree height is: %ld\n",
                   j++,(double)(b - a) / CLOCKS_PER_SEC, avl_get_height(testTree));
            a = b;
        }
    }
    printf("Deleting done.\n");
    for(int i = 0; i < 10; ++i){
        int * e = (int*)avl_get(testTree, i);
        int * f = (int*)avl_get_node(testTree, i);
        printf("Element at id %d pointing to %p\n"
                "Element at absolute id %d pointing to %p\n",i,(void*)e,i,(void*)f);
        if(e) printf("At id %d got element: %d.\n",i,*e); 
        else printf("Element at id %d does not exists!\n",i);
        if(f) printf("At absolute id %d got element: %d.\n",i,*f); 
        else printf("Element at absolute id %d does not exists!\n",i);
        
    }
    int * p = (int*) malloc(sizeof(int));
    *p = 11;
    if(!avl_edit_id(testTree,5,p))
        printf("Replacing: Ok\n");
    else {
        printf("Replacing: NOk\n");
        free(p);
    }
    p = (int*) malloc(sizeof(int));
    *p = 11;
    avl_insert(testTree, p);
    int e = 11;
    printf("Count of elem %d is: %ld\n", e , avl_count_elem(testTree, &e));
    printf("Number of elements: %ld\nNumber of nodes: %lu\n",
           avl_count(testTree), avl_node_count(testTree));
    avl_destroy(testTree);
    return 0;
}
