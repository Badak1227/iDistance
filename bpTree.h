
#ifndef __BPTREE_H__
#define __BPTREE_H__
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

bp_tree* get_bp(int m);

void search_bp(value num, bp_tree* node);

void split_bp(bp_tree* node);

void insert_bp(value num, bp_tree* node);

void free_bp(bp_tree* node);
#endif