#include <stdio.h>
#ifdef DEBUG
#include <assert.h>
#endif
#include <stdlib.h>
#include "avl.h"
#define MAX(x,y) ((x > y) ? x : y)
//#################################################################################
typedef struct node {
    struct node * parrent,
                * lSon,
                * rSon;
    void * elem;
    size_t dup_count;           // count indicates insertion of duplicate
    size_t count;               // count of left and right sons with duplicities
    size_t node_count;          // count of left and right sons without duplicities
    unsigned char height;       // as height being log of size_t, 8 bits is enought
} node_t;
//#################################################################################
node_t * find_node(node_t * root, const void * elem, comparator_t * comp){
    if(!root) return NULL;
    int res = comp(elem, root->elem);
    if(res < 0) return find_node(root->lSon, elem, comp);
    if(res > 0) return find_node(root->rSon, elem, comp);
    return root;
}
//---------------------------------------------------------------------------------
size_t avl_find(const avl_root * root, const void * elem){
    node_t * e = find_node(root->node, elem, root->avl_comp);
    return e ? 1 : 0;
}
//#################################################################################
void destroy_node(node_t * restrict n, destroyer_t * destroy){
#ifdef DEBUG
    printf("Destroying node: %p\n", (void*)n);
#endif
    if(destroy) destroy(n->elem);
    free(n);
}
//---------------------------------------------------------------------------------
void destroy_nodes(node_t * restrict n, destroyer_t * destroy){
    if(!n) return;
    destroy_nodes(n->rSon, destroy);
    destroy_nodes(n->lSon, destroy);
    destroy_node(n, destroy);
}
//---------------------------------------------------------------------------------
void avl_destroy(avl_root * root){
    if(!root) return;
    destroy_nodes(root->node, root->avl_destroy);
    free(root);
}
//#################################################################################
node_t * create_node(void * elem){
    node_t * node = (node_t*)malloc(sizeof(node_t));
    node->elem = elem;
    node->parrent = node->lSon = node->rSon = NULL;
    node->node_count = node->dup_count = 1;
    node->count = 0;
    node->height = 1;
    return node;
}
//---------------------------------------------------------------------------------
unsigned char getHeight(node_t * restrict cur){ return (cur) ? cur->height : 0; }
//---------------------------------------------------------------------------------
size_t getSonNodes(node_t * restrict sons){
    if(!sons) return 0;
    return sons->node_count;
}
//---------------------------------------------------------------------------------
size_t getSons(node_t * restrict sons){
    if(!sons) return 0;
    return sons->count + sons->dup_count;
}
//---------------------------------------------------------------------------------
char getBalanceFactor(node_t * restrict cur){ 
    if(!cur) return 0;
    return getHeight(cur->lSon) - getHeight(cur->rSon);
}
//---------------------------------------------------------------------------------
node_t * lRotate(node_t * cur){
#ifdef DEBUG
    printf("Left rotation!\n");
    printf("Right son: %p, left son: %p\n", (void*)cur->rSon, (void*)cur->lSon);
#endif
    node_t * rSon = cur->rSon,
           * rSon_lSon = rSon->lSon;
    rSon->lSon = cur;
    rSon->parrent = cur->parrent;
    cur->parrent = rSon;
    cur->rSon = rSon_lSon;
    if(rSon_lSon) rSon_lSon->parrent = cur;
    cur->height = 1 + MAX(getHeight(cur->lSon), getHeight(rSon_lSon));
    rSon->height = 1 + MAX(getHeight(rSon->lSon), getHeight(rSon->rSon));
    cur->count = getSons(rSon_lSon) + getSons(cur->lSon);
    cur->node_count = getSonNodes(rSon_lSon) + getSonNodes(cur->lSon) + 1;
    rSon->count = getSons(rSon->rSon) + cur->count + cur->dup_count;
    rSon->node_count = getSonNodes(rSon->rSon) + cur->node_count + 1;
    return rSon;
}
//---------------------------------------------------------------------------------
node_t * rRotate(node_t * cur){
#ifdef DEBUG
    printf("Right rotation!\n");
    printf("Right son: %p, left son: %p\n", (void*)cur->rSon, (void*)cur->lSon);
#endif
    node_t * lSon = cur->lSon,
           * lSon_rSon = lSon->rSon;
    lSon->rSon = cur;
    lSon->parrent = cur->parrent;
    cur->parrent = lSon;
    cur->lSon = lSon_rSon;
    if(lSon_rSon) lSon_rSon->parrent = cur;
    cur->height = 1 + MAX(getHeight(cur->rSon), getHeight(lSon_rSon));
    lSon->height = 1 + MAX(getHeight(lSon->lSon), getHeight(lSon->rSon));
    cur->count = getSons(lSon_rSon) + getSons(cur->rSon);
    cur->node_count = getSonNodes(lSon_rSon) + getSonNodes(cur->rSon) + 1;
    lSon->count = getSons(lSon->lSon) + cur->count + cur->dup_count;
    lSon->node_count = getSonNodes(lSon->lSon) + cur->node_count + 1;
    return lSon;
}
//---------------------------------------------------------------------------------
node_t * insert_node(node_t * root, void ** elem, avl_root * tree){
#ifdef DEBUG
    printf("Inside insert_node!\n");
    printf("Root: %p | Elem: %p\n",(void*)root,(void*)*elem);
#endif
    if(!root){
        tree->node_cnt++;
        return create_node(*elem);
    }
#ifdef DEBUG
    printf("Starting comparing!\n");
#endif
    int res = tree->avl_comp(*elem,root->elem);
#ifdef DEBUG
    printf("Compare done: %d.\n", res);
#endif
    if(res < 0) {
        root->count++;
        root->lSon = insert_node(root->lSon, elem, tree);
#ifdef DEBUG
        assert(root->lSon);
#endif
        root->lSon->parrent = root;
        root->node_count = getSonNodes(root->rSon) + root->lSon->node_count + 1;
    }
    if(res > 0) {
        root->count++;
        root->rSon = insert_node(root->rSon, elem, tree);
#ifdef DEBUG
        assert(root->rSon);
#endif
        root->rSon->parrent = root;
        root->node_count = getSonNodes(root->lSon) + root->rSon->node_count + 1;
    }
    if(res == 0) {
#if DEBUG > 0
        printf("Inserting same node\n");
#endif
        root->dup_count++;
        if(tree->avl_destroy) tree->avl_destroy(*elem);
        *elem = NULL;
        return root;
    }
#ifdef DEBUG
    printf("Assignment done!\n");
#endif
    if(!(*elem)) return root;
    root->height = 1 + MAX(getHeight(root->lSon), getHeight(root->rSon));
    signed char balanceFactor = getBalanceFactor(root);
#ifdef DEBUG
    printf("Checking balance factor!\n");
#endif
    if(balanceFactor < -1){
        int subRes = tree->avl_comp(*elem, root->rSon->elem);
#if DEBUG > 0
        printf("Ubalanced!\n");
        printf("Balance factor: %d, inside decision: %d\n", balanceFactor, subRes);
        printf("Affected element pointers: %p / %p.\n", 
                (void*)*elem, (void*)root->rSon->elem);
#endif
        if(subRes > 0){
#if DEBUG > 1
            printf("Atempting L rotation!\n");
#endif
            return lRotate(root);
        }
        if(subRes < 0){
#if DEBUG > 1
            printf("Atempting RL rotation!\n");
#endif
            root->rSon = rRotate(root->rSon);
            return lRotate(root);
        }
#ifdef DEBUG
        assert(subRes);
#endif
    }
    if(balanceFactor > 1){
        int subRes = tree->avl_comp(*elem, root->lSon->elem);
#if DEBUG > 0
        printf("Ubalanced!\n");
        printf("Balance factor: %d, inside decision: %d\n", balanceFactor, subRes);
        printf("Affected element pointers: %p / %p.\n", 
                (void*)*elem, (void*)root->lSon->elem);
#endif
        if(subRes > 0){
#if DEBUG > 1
            printf("Atempting LR rotation!\n");
#endif
            root->lSon = lRotate(root->lSon);
            return rRotate(root);
        }
        if(subRes < 0){
#if DEBUG > 1
            printf("Atempting R rotation!\n");
#endif
            return rRotate(root);
        }
#ifdef DEBUG
        assert(subRes);
#endif
    }
#ifdef DEBUG
    printf("Root exit\n");
#endif
    return root;
}
//---------------------------------------------------------------------------------
int avl_insert(avl_root * root, void * elem){
    ++(root->cnt);
    root->node = insert_node(root->node, &elem, root);
    return 0;
}
//#################################################################################
void print_node(const node_t * t, elemPrint_t * pf){
    printf("Elem info: %p\n\tParrent:\t%p\n\tLeft son:\t%p\n\tRight son:\t%p\n\t"
           "Height:\t%u\n\tCount:\t%lu\n\tCount of relative left sons: %ld\n\t"
           "Count of absolute left sons: %ld\n\tCount of relative right sons: %lu"
           "\n\tCount of absolute right sons: %ld\n\tValue: ",
           (void*) t, (void*)t->parrent, (void*)t->lSon, (void*)t->rSon, t->height, 
           t->dup_count, getSons(t->lSon), getSonNodes(t->lSon), getSons(t->rSon), 
           getSonNodes(t->rSon));
    if(pf) pf(t->elem);
    printf("\n"); 
}
//---------------------------------------------------------------------------------
void silent_print_node(const node_t * t, elemPrint_t * pf){
    if(pf) pf(t->elem);
}
//---------------------------------------------------------------------------------
void print_nodes(const node_t * t, elemPrint_t * pf, const char op){
    if(!t) return;
    if(((op >> 1) & 0x3) == 0){
        if((op & 0x1) == 0) print_node(t, pf);
        if((op & 0x1) == 1) silent_print_node(t, pf);
    }
    print_nodes(t->lSon, pf, op);
    if(((op >> 1) & 0x3) == 1){
        if((op & 0x1) == 0) print_node(t, pf);
        if((op & 0x1) == 1) silent_print_node(t, pf);
    }
    print_nodes(t->rSon, pf, op);
    if(((op >> 1) & 0x3) == 2){
        if((op & 0x1) == 0) print_node(t, pf);
        if((op & 0x1) == 1) silent_print_node(t, pf);
    }
}
//---------------------------------------------------------------------------------
void avl_print(const avl_root * root){
    printf("Avl tree summary:\nElem count: %lu\nNode count: %lu\n",
            root->cnt, root->node_cnt);
    print_nodes(root->node, root->avl_print, 0);
}
//#################################################################################
void avl_silent_print(const avl_root * root){
    print_nodes(root->node, root->avl_print, 1);
}
//#################################################################################
node_t * mostLeftNode(node_t * root, size_t * deleteFactor){
    if(!(root->lSon)){
        *deleteFactor = root->dup_count - 1;
        root->dup_count = 1;
        return root;
    }
    node_t * tmpRoot = root;
    root = mostLeftNode(root->lSon, deleteFactor);
    tmpRoot->count -= *deleteFactor;
    return root;
}
//---------------------------------------------------------------------------------
node_t * delete_node(node_t * root, const node_t * elem, avl_root * tree,
                     int * found){
#ifdef DEBUG
    printf("Inside delete_node!\n");
    printf("Root: %p | Elem: %p\n",(void*)root,(void*)elem);
#endif
    if(!root) return root;
#ifdef DEBUG
    printf("Starting comparing!\n");
#endif
    int res = tree->avl_comp(elem->elem,root->elem);
#ifdef DEBUG
    printf("Compare done: %d.\n", res);
#endif
    if(res < 0) root->lSon = delete_node(root->lSon, elem, tree, found);
    if(res > 0) root->rSon = delete_node(root->rSon, elem, tree, found);
    if(res == 0) {
#if DEBUG > 0
        printf("Node for deleting found.\n");
#endif
        *found = 1;
        root->dup_count--;
        if(root->dup_count) return root; 
                                // ^^ still has some viable undeleted candidates
#if DEBUG > 0
        printf("Node has empty count, whole node deletion required!\n");
#endif
        tree->node_cnt--;
        node_t * tmp; 
        if(!(root->lSon)){
#if DEBUG > 1
            printf("Replacing node with its right son.\n");
#endif
            tmp = root->rSon;
            if(tmp) tmp->parrent = root->parrent;
            destroy_node(root, tree->avl_destroy);
            return tmp;
        }
        if(!(root->rSon)){
#if DEBUG > 1
            printf("Replacing node with its left son.\n");
#endif
            tmp = root->lSon;
            if(tmp) tmp->parrent = root->parrent;
            destroy_node(root, tree->avl_destroy);
            return tmp;
        }
#if DEBUG > 0
        printf("Complicated manouver required!\n");
#endif
        ++(tree->node_cnt);
        int subFound = 0;
        size_t deleteFactor = 0;
        tmp = mostLeftNode(root->rSon,&deleteFactor);
        void * swp = root->elem;
        root->elem = tmp->elem;
        tmp->elem = swp;
        root->dup_count = deleteFactor + 1;
        root->rSon = delete_node(root->rSon, elem, tree, &subFound);
        root->count -= deleteFactor;
#if DEBUG > 0
        printf("Exiting complicated manouver!\n");
#endif
    }
    if(!(*found)){
#if DEBUG > 0
        printf("No candidate node for deletion found!\n");
#endif
        return root;
    }
    root->count--;
    root->node_count = getSonNodes(root->lSon) + getSonNodes(root->rSon) + 1;
    root->height = 1 + MAX(getHeight(root->lSon), getHeight(root->rSon));
    signed char balanceFactor = getBalanceFactor(root);
#ifdef DEBUG
    printf("Checking balance factor!\n");
#endif
    if(balanceFactor < -1){
#if DEBUG > 0
        printf("Ubalanced!\n");
#endif
        signed char subBF = getBalanceFactor(root->rSon);
        if(subBF > 0){
#if DEBUG > 1
            printf("Atempting RL rotation!\n");
#endif
            root->rSon = rRotate(root->rSon);
            return lRotate(root);
        }
        if(subBF <= 0){
#if DEBUG > 1
            printf("Atempting L rotation!\n");
#endif
            return lRotate(root);
        }
    }
    if(balanceFactor > 1){
#if DEBUG > 0
        printf("Ubalanced!\n");
#endif
        signed char subBF = getBalanceFactor(root->lSon);
        if(subBF < 0){
#if DEBUG > 1
            printf("Atempting LR rotation!\n");
#endif
            root->lSon = lRotate(root->lSon);
            return rRotate(root);
        }
        if(subBF >= 0){
#if DEBUG > 1
            printf("Atempting R rotation!\n");
#endif
            return rRotate(root);
        }
    }
#ifdef DEBUG
    printf("Root exit\n");
#endif
    return root;
}
//---------------------------------------------------------------------------------
int avl_delete(avl_root * root, const void * elem){
    node_t node;
    int found = 0;
    node.elem = (void*)elem;
    node.parrent = node.lSon = node.rSon = NULL;
    root->node = delete_node(root->node, &node, root, &found);
    if(!found) return 1;
    root->cnt--;
    return 0;
}
//#################################################################################
size_t avl_count(const avl_root * root){
    return root->cnt;
}
//#################################################################################
void * get_node(const node_t * root, size_t id){
    if(!root) return NULL;
    size_t tmp;
    if(root->lSon){
        tmp = getSons(root->lSon); 
        if(id < tmp) return get_node(root->lSon, id);
        id -= tmp;
    }
    if(id < root->dup_count) return root->elem;
    id -= root->dup_count;
    tmp = getSons(root->rSon);
    return id < tmp ? get_node(root->rSon, id) : NULL;
}
//---------------------------------------------------------------------------------
void * avl_get(const avl_root * root, const size_t id){
    return get_node(root->node, id);    
}
//#################################################################################
int avl_delete_id(avl_root * root, const size_t id){
    void * elem = get_node(root->node, id);
    return elem ? avl_delete(root, elem) : 1;
}
//#################################################################################
int avl_edit_id(avl_root * root, const size_t id, void * replacing){
    int res = avl_delete_id(root, id);
    if(res) return 1;
    avl_insert(root, replacing);
    return 0;
}
//#################################################################################
int avl_edit(avl_root * root, const void * elem, void * replacing){
    int res = avl_delete(root, elem);
    if(res) return 1;
    avl_insert(root, replacing);
    return 0;
}
//#################################################################################
avl_root * avl_init(comparator_t * comp, destroyer_t * des, elemPrint_t * pf){
    avl_root * r = (avl_root*) malloc (sizeof(avl_root));
    if(!r) return NULL;
    r->avl_destroy = des;
    r->avl_print = pf;
    r->avl_comp = comp;
    r->cnt = r->node_cnt = 0;
    r->node = NULL;
    return r;
}
//#################################################################################
size_t avl_count_id(const avl_root * root, const size_t id){
    node_t * e = find_node(root->node, get_node(root->node, id), root->avl_comp);
    return e ? e->dup_count : 0;
}
//#################################################################################
size_t avl_count_elem(const avl_root * root, const void * elem){
    node_t * e = find_node(root->node, elem, root->avl_comp);
    return e ? e->dup_count : 0;
}
//#################################################################################
size_t avl_node_count(const avl_root * root){
    return root->node_cnt;
}
//#################################################################################
void * avl_get_same(const avl_root * root, const void * elem){
    node_t * e = find_node(root->node, elem, root->avl_comp);
    return e ? e->elem : NULL;
}
//#################################################################################
void avl_ordered_print(const avl_root * root, const char order){
    if(order == 0) print_nodes(root->node, root->avl_print, 0x0);
    if(order == 1) print_nodes(root->node, root->avl_print, 0x2);
    if(order == 2) print_nodes(root->node, root->avl_print, 0x4);
}
//#################################################################################
void avl_ordered_silent_print(const avl_root *root, const char order){
    if(order == 0) print_nodes(root->node, root->avl_print, 0x1);
    if(order == 1) print_nodes(root->node, root->avl_print, 0x3);
    if(order == 2) print_nodes(root->node, root->avl_print, 0x5);
}
//#################################################################################
size_t avl_get_height(const avl_root * root){
    return root->node ? ((node_t*)root->node)->height : 0;
}
//#################################################################################
const node_t * find_absolute_node(const node_t * root, size_t id){
    if(!root) return NULL;
    size_t tmp;
    if(root->lSon){
        tmp = getSonNodes(root->lSon);
        if(id < tmp) return find_absolute_node(root->lSon, id);
        id -= tmp;
    }
    if(!id) return root;
    id--;
    tmp = getSonNodes(root->rSon);
    return id < tmp ? find_absolute_node(root->rSon,id) : NULL;
}
//---------------------------------------------------------------------------------
void * avl_get_node(const avl_root * root, const size_t id){
    const node_t * ret = find_absolute_node(root->node, id);
    return ret ? ret->elem : NULL;
}
//#################################################################################
int avl_delete_id_node(avl_root * root, const size_t id){
    const node_t * d = find_absolute_node(root->node, id);
    if(!d) return 1;
    return avl_delete(root, d->elem);
}
//#################################################################################
int avl_edit_id_node(avl_root * root, const size_t id, void * replacing){
    if(avl_delete_id_node(root, id)) return 1;
    return avl_insert(root, replacing);
}
//#################################################################################
size_t avl_count_id_node(const avl_root * root, const size_t id){
    const node_t * ret = find_absolute_node(root->node, id);
    return ret ? ret->dup_count : 0;
}
//#################################################################################