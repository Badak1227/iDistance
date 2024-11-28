#include <stdlib.h>

typedef struct bp_tree{
    int m;

    int leaf_bool; // 리프노드 판단
    int key_len;
    int ptr_len;
    double* index;
    bp_tree** ptr;
    
    bp_tree* prev;
    bp_tree* next;
}bp_tree;

typedef struct rec_bp{
    double num;

    bp_tree* right_bp;
}rec_bp;

bp_tree* get_bp_root(int m){
    bp_tree* tmp = (bp_tree*)malloc(sizeof(bp_tree));
    tmp->m = 8;
    tmp->leaf_bool = 1;
    tmp->key_len = 0;
    tmp->ptr_len = 0;
    tmp->index = (double*)malloc((m-1) * sizeof(double));
    tmp->ptr = (bp_tree**)malloc(m * sizeof(bp_tree*));

    tmp->prev = NULL;
    tmp->next = NULL;
    return tmp;
}

bp_tree* split_bp(){

}

void merge_bp(){

}

bp_tree* insert_bp(double num, bp_tree* node){

    //현재 노드가 leaf_node일 경우 숫자 삽입
    if(node->leaf_bool == 1){
        double tmp = 0;

        for(int i=0; i<node->m; i++){
            if(node)
        }
        node->index[node->key_len] = num;
        node->key_len++;

        if(node->key_len == node->m)    return split_bp();
    }
    //현재 노드가 leaf_node가 아닐 경우
    else{
        if(node->index[0] > num)    insert_bp(num, node->ptr[0]);
        else{
            for(int i=1; i<node->key_len; i++){
                
                if(node->index[0] >= num){
                    insert_bp(num, node->ptr[i+1]);
                }
            }
        }
    }

    return NULL;
}
