#ifndef __LIBAVL__
#define __LIBAVL__
typedef int (comparator_t)(const void *, const void *);
typedef void (destroyer_t)(void *);
typedef void (elemPrint_t)(const void *);
typedef struct {
    comparator_t * avl_comp;
    destroyer_t * avl_destroy;
    elemPrint_t * avl_print;
    size_t cnt, node_cnt;
    void * node;    
} avl_root;
int avl_insert(avl_root *, void *);
int avl_delete(avl_root *, const void *);
int avl_delete_id(avl_root *, const size_t);
int avl_delete_id_node(avl_root *, const size_t);
int avl_edit_id(avl_root *, const size_t, void *);
int avl_edit_id_node(avl_root *, const size_t, void *);
int avl_edit(avl_root *, const void *, void *);
size_t avl_find(const avl_root *, const void *);
size_t avl_count(const avl_root *);
size_t avl_count_id(const avl_root *, const size_t);
size_t avl_count_id_node(const avl_root *, const size_t);
size_t avl_count_elem(const avl_root *, const void *);
size_t avl_node_count(const avl_root *);
void * avl_get(const avl_root *, const size_t);
void * avl_get_node(const avl_root *, const size_t);
void * avl_get_same(const avl_root *, const void *);
void avl_print(const avl_root *);
void avl_ordered_print(const avl_root *, const char);
void avl_silent_print(const avl_root *);
void avl_ordered_silent_print(const avl_root *, const char);
avl_root * avl_init(comparator_t *, destroyer_t *, elemPrint_t *);
void avl_destroy(avl_root *);
size_t avl_get_height(const avl_root *);
#endif