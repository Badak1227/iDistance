#include <stdio.h>
#include <stdlib.h>

typedef struct value{
    double iDist;
    double pos[2];
}value;

typedef struct bp_tree{
    int m;

    int leaf_bool; // 리프노드 판단
    int key_len;
    value* key;
    struct bp_tree** child;
    
    struct bp_tree* prev;
    struct bp_tree* next;
}bp_tree;

bp_tree* get_bp(int m){
    bp_tree* tmp = (bp_tree*)malloc(sizeof(bp_tree));
    tmp->m = m;
    tmp->leaf_bool = 1;
    tmp->key_len = 0;
    tmp->key = (value*)malloc((m-1) * sizeof(value));
    tmp->child = (bp_tree**)malloc(m * sizeof(bp_tree*));
    
    for(int i=0; i<m; i++){
        tmp->child[i] = NULL;
    }

    tmp->prev = NULL;
    tmp->next = NULL;
    return tmp;
}


void split_bp(bp_tree* node){
    bp_tree* left = get_bp(node->m);
    bp_tree* right = get_bp(node->m);

    if(node->leaf_bool == 1){
        //현재 노드는 idx노드로 승격
        node->leaf_bool = 0;
        left->leaf_bool = 1;
        right->leaf_bool = 1;
        
        //좌우 노드를 절반씩 분할
        left->key_len = node->key_len/2;
        right->key_len = node->key_len - left->key_len;
        
        //key값 좌우 노드로 분배
        int i=0;
        for(i; i<left->key_len; i++)    left->key[i] = node->key[i];
        for(int j=0; i<node->key_len; j++,i++)  right->key[j] = node->key[i];

        //leaf노드끼리 연결
        left->prev = node->prev;
        left->next = right;
        if(node->prev != NULL) node->prev->next = left;
        node->prev = NULL;
        
        right->prev = left;
        right->next = node->next;
        if(node->next !=NULL) node->next->prev = right;
        node->next = NULL;
    }
    else{
        //자식 노드도 idx노드
        left->leaf_bool = 0;
        right->leaf_bool = 0;

        //좌우 노드를 절반씩 분할
        left->key_len = node->key_len/2;
        right->key_len = node->key_len - left->key_len - 1;
        
        // 왼쪽 노드로 키와 포인터 이동
        int i = 0;
        for (i = 0; i < left->key_len; i++) {
            left->key[i] = node->key[i];
            left->child[i] = node->child[i];
        }
        left->child[i] = node->child[i];
        i++;

        // 오른쪽 노드로 키와 포인터 이동
        int j=0;
        for (j = 0; i < node->key_len; i++, j++) {
            right->key[j] = node->key[i];
            right->child[j] = node->child[i];
        }
        right->child[j] = node->child[i];
    }
    //부모 노드에 중앙값 하나만 남김
    node->key[0] = node->key[node->key_len/2];
    node->key_len = 1;

    node->child[0] = left;
    node->child[1] = right;

    printf("split to - p: %.0f, l: %.0f, r: %.0f\n", node->key[0].iDist, node->child[0]->key[0].iDist, node->child[1]->key[0].iDist);
}

void search_bp(double iDist, bp_tree* node){

}

void insert_bp(value num, bp_tree* node){
    //현재 노드가 leaf_node일 경우 숫자 삽입
    if(node->leaf_bool == 1){
        int i = node->key_len;
        for(i; i>0; i--){
            if(node->key[i-1].iDist > num.iDist) node->key[i] = node->key[i-1];
            else break;
        }

        node->key[i] = num;

        node->key_len++;

        if(node->key_len == node->m-1)    split_bp(node);
    }
    //현재 노드가 leaf_node가 아닐 경우
    else{

        //적정 탐색 지점 찾기
        int i=0;
        for (i = 0; i < node->key_len; i++) {
            if (node->key[i].iDist > num.iDist) {
                break;
            }
        }

        insert_bp(num, node->child[i]);

        //적정지점 자식노드가 split이 진행 되었을 경우
        if(node->child[i]->key_len == 1){
            bp_tree* tmp = node->child[i];

            for(int j = node->key_len; j > i; j--){
                node->key[j] = node->key[j-1];
                node->child[j+1] = node->child[j];
            }

            node->key[i] = tmp->key[0];
            node->child[i] = tmp->child[0];
            node->child[i+1] = tmp->child[1];

            free(tmp->key);
            free(tmp->child);
            free(tmp);

            node->key_len++;

            printf("merge - p: %.0f, l: %.0f, r: %.0f\n", node->key[0].iDist, node->child[i]->key[0].iDist, node->child[i+1]->key[0].iDist);
            
            printf("\n");
            if(node->key_len == node->m-1)    split_bp(node);
        }
    }
}

void free_bp(bp_tree* node){
    bp_tree* tmp = node;

    for(int i=0; i <= node->key_len; i++){
        if (node->child[i] != NULL) {
            free_bp(node->child[i]);
        }
    }
    
    free(tmp->key);
    free(tmp->child);
    free(tmp);
}