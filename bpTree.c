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

void split_bp(bp_tree* node){
    bp_tree* left = (bp_tree*)malloc(sizeof(bp_tree));
    bp_tree* right = (bp_tree*)malloc(sizeof(bp_tree));

    left->m = node->m;
    right->m = node->m;

    if(node->leaf_bool == 1){
        node->leaf_bool = 0;
        left->leaf_bool = 1;
        right->leaf_bool = 1;
        
        left->key_len = node->key_len/2;
        right->key_len = node->key_len - left->key_len;

        left->prev = node->prev;
        left->next = right;
        if(node->prev != NULL) node->prev->next = left;
        node->prev = NULL;
        
        right->prev = left;
        right->next = node->next;
        if(node->next !=NULL) node->next->prev = right;
        node->next = NULL;

        int i=0;
        for(i; i<left->key_len; i++)    left->index[i] = node->index[i];
        for(int j=0; i<node->key_len; j++,i++)  right->index[j] = node->index[i];

        node->index[0] = node->index[node->key_len/2];
        node->key_len = 1;
        
        node->ptr[0] =  left;
        node->ptr[1] = right;
        node->ptr_len = 2;
    }
    else{

        left->key_len = node->key_len/2;
        right->key_len = node->key_len - left->key_len - 1;
        
        // 1. 왼쪽 노드로 키와 포인터 이동
        int i = 0;
        for (i = 0; i < left->key_len; i++) {
            left->index[i] = node->index[i];
            left->ptr[i] = node->ptr[i];
        }
        left->ptr[i] = node->ptr[i]; // 마지막 포인터 추가

        // 2. 오른쪽 노드로 키와 포인터 이동
        int j = 0;
        for (i = left->key_len + 1; i < node->key_len; i++, j++) {
            right->index[j] = node->index[i];
            right->ptr[j] = node->ptr[i];
        }
        right->ptr[j] = node->ptr[i];

        node->index[0] = node->index[node->key_len/2];
        node->key_len = 1;

        for(int k=0; k<node->m; k++)    node->ptr[k] = NULL;

        node->ptr[0] = left;
        node->ptr[1] = right;
        node->ptr_len = 2;
    }

    return 0;
}

void merge_bp(){

}

void check_bp(){

}

bp_tree* insert_bp(double num, bp_tree* node){

    //현재 노드가 leaf_node일 경우 숫자 삽입
    if(node->leaf_bool == 1){
        double tmp = 0;

        for(int i=node->key_len; i>0; i--){
            if(node->index[i-1] > num) node->index[i] = node->index[i-1];
            else node->index[i] = num;
        }
        node->key_len++;

        if(node->key_len == node->m-1)    split_bp(node);
    }
    //현재 노드가 leaf_node가 아닐 경우
    else{
        int i=0;
        for (i = 0; i < node->key_len; i++) {
            if (node->index[i] > num) {
                break;
            }
        }

        insert_bp(num, node->ptr[i]);

        if(check(node, node->ptr[i])){
            
            for(int j=node->key_len; j > i; j++){
                node->index[j] = node->index[j-1];
                
            }
            node->index[i] = node->ptr[i]->index[0];
            node->ptr[i] = node->ptr[i]->ptr[0];
            node->ptr[i+1] = node->ptr[i]->ptr[1];

            free(node->ptr[i]);

            node->key_len++;
            if(node->key_len == node->m-1)    split_bp(node);
        }
        
    }

    return NULL;
}
