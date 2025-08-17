#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "Linked_List.h"
#include "Kheap.h"

#define INITIAL_BUCKETS_NUM 16
#define LOAD_FACTOR 0.75

struct hash_table_pair
{
    uint32 key;
    void *value;
};
typedef struct hash_table_pair hash_table_pair_t;

struct hash_table
{
    doubly_linked_list_t *buckets;
    int32 size;
    int32 bucketsNum;
};
typedef struct hash_table hash_table_t;

struct hash_table_interator
{
    hash_table_t* map;
    int32 bucketIndex;
    doubly_linked_list_node_t* node;
    int32 index;
};
typedef struct hash_table_interator hash_table_interator_t;


hash_table_pair_t* hash_Table_Iterator_Next(hash_table_interator_t* iter);
bool hash_Table_Iterator_Has_Next(hash_table_interator_t* iter);
hash_table_interator_t hash_Table_Iterator_Init(hash_table_t* map);
void* hash_Table_Remove(hash_table_t* this, uint32 key);
void hash_Table_Insert(hash_table_t* this, uint32 key, void* value);
void* hash_Table_Get(hash_table_t* this, uint32 key);
void hash_Table_Init(hash_table_t *table);
void hash_Table_Destroy(hash_table_t* this);


#endif // !HASH_TABLE_H