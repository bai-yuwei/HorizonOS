#include "Hash_Table.h"


/**
 * @brief 在指定哈希表的特定桶中查找包含指定键的链表节点。
 *
 * 该函数会遍历给定桶中的双向链表，比较每个节点存储的键与传入的键是否相等。
 * 如果找到匹配的键，则返回对应的链表节点指针；如果未找到，则返回 NULL。
 *
 * @param table 指向哈希表的指针，包含要查找的桶信息。
 * @param bucket 指向要查找的双向链表（桶）的指针。
 * @param key 指向要查找的键的指针。
 * @return doubly_linked_list_node_t* 若找到匹配的键，返回对应的链表节点指针；否则返回 NULL。
 */
static doubly_linked_list_node_t* hash_Table_Bucket_Lookup(hash_table_t *table, doubly_linked_list_t* bucket, uint32 key)
{
    // 从桶的头节点开始遍历
    doubly_linked_list_node_t *node = bucket->head;
    // 当节点不为空时，继续遍历
    while (node != NULL)
    {
        // 将节点的数据指针转换为哈希表键值对指针
        hash_table_pair_t *pair = (hash_table_pair_t *)node->dataPtr;
        // 比较键值对中的键与传入的键是否相等
        if (pair->key == key)
        {
            // 若相等，返回当前节点指针
            return node;
        }
        // 移动到下一个节点
        node = node->next;
    }
    // 未找到匹配的键，返回 NULL
    return NULL;
}

static void hash_Table_Expand(hash_table_t* this)
{
    uint32 newBucketsNum = this->bucketsNum * 2;
    doubly_linked_list_t* newBuckets = (doubly_linked_list_t *)kmalloc(newBucketsNum * sizeof(doubly_linked_list_t), NOT_PAGE_ALIGNED);
    for (int32 i = 0; i < newBucketsNum; i++)
    {
        doubly_Linked_List_Init(&newBuckets[i]);
    }
    for (int32 i = 0; i < this->bucketsNum; i++)
    {
        doubly_linked_list_t* bucket = &this->buckets[i];
        doubly_linked_list_node_t *node = bucket->head;
        while (node != NULL)
        {
            doubly_linked_list_node_t *cur = node;
            node = node->next;
            hash_table_pair_t *pair = (hash_table_pair_t *)node->dataPtr;
            doubly_Linked_List_Remove(bucket, cur);
            uint32 key = pair->key;
            doubly_Linked_List_Append(&newBuckets[key % newBucketsNum], cur);
        }
    }
    kfree(this->buckets);
    this->buckets = newBuckets;
    this->bucketsNum = newBucketsNum;
}

void hash_Table_Init(hash_table_t *table)
{
    table->size = 0;
    table->bucketsNum = INITIAL_BUCKETS_NUM;
    table->buckets = (doubly_linked_list_t *)kmalloc(table->bucketsNum * sizeof(doubly_linked_list_t), NOT_PAGE_ALIGNED);
    for (int32 i = 0; i < INITIAL_BUCKETS_NUM; i++)
    {
        doubly_Linked_List_Init(&table->buckets[i]);
    }
}

hash_table_t create_Hash_Table(void)
{
    hash_table_t table;
    hash_Table_Init(&table);
    return table;
}

void hash_Table_Clear(hash_table_t *table)
{
    for (int32 i = 0; i < table->bucketsNum; i++)
    {
        doubly_linked_list_t bucket = table->buckets[i];
        doubly_linked_list_node_t *node = bucket.head;
        while (node != NULL)
        {
            doubly_linked_list_node_t *cur = node;
            node = node->next;
            hash_table_pair_t *pair = (hash_table_pair_t *)cur->dataPtr;
            kfree(pair->value);
            kfree(pair);
            doubly_Linked_List_Remove(&bucket, cur);
            kfree(cur);
        }
    }
    table->size = 0;
}

void hash_Table_Destroy(hash_table_t *table)
{
    hash_Table_Clear(table);
    kfree(table->buckets);
    table->size = 0;
}

void* hash_Table_Get(hash_table_t* this, uint32 key)
{
    doubly_linked_list_t* bucket = &this->buckets[key % this->bucketsNum];
    doubly_linked_list_node_t* node = hash_Table_Bucket_Lookup(this, bucket, key);
    if (node == NULL)
    {
        return NULL;
    }
    hash_table_pair_t* pair = (hash_table_pair_t*)node->dataPtr;
    return pair->value;
}

bool hash_Table_Contains(hash_table_t* this, uint32 key)
{
    doubly_linked_list_t* bucket = &this->buckets[key % this->bucketsNum];
    doubly_linked_list_node_t* node = hash_Table_Bucket_Lookup(this, bucket, key);
    if (node == NULL)
    {
        return false;
    }
    return true;
}

void* hash_Table_Put(hash_table_t* this, uint32 key, void* value)
{
    doubly_linked_list_t* bucket = &this->buckets[key % this->bucketsNum];
    doubly_linked_list_node_t* node = hash_Table_Bucket_Lookup(this, bucket, key);
    if (node != nullptr)
    {
        hash_table_pair_t* pair = (hash_table_pair_t*)node->dataPtr;
        void* oldValue = pair->value;
        pair->value = value;
        return oldValue;
    }
    hash_table_pair_t* newPair = (hash_table_pair_t*)kmalloc(sizeof(hash_table_pair_t), NOT_PAGE_ALIGNED);
    newPair->key = key;
    newPair->value = value;
    doubly_Linked_List_Append_Data(bucket, newPair);
    this->size++;
    if (this->size > this->bucketsNum * LOAD_FACTOR)
    {
        hash_Table_Expand(this);
    }
    return nullptr;
}

void* hash_Table_Remove(hash_table_t* this, uint32 key)
{
    doubly_linked_list_t* bucket = &this->buckets[key % this->bucketsNum];
    doubly_linked_list_node_t* node = hash_Table_Bucket_Lookup(this, bucket, key);
    if (node != nullptr) {
        doubly_Linked_List_Remove(bucket, node);
        hash_table_pair_t* pair = (hash_table_pair_t*)node->dataPtr;
        void* value = pair->value;
        kfree(pair);
        kfree(node);
        this->size--;

        // TODO: shrink buckets if needed?
        return value;
    }
    return nullptr;
}

hash_table_interator_t hash_Table_Iterator_Create(hash_table_t* this)
{
    hash_table_interator_t interator;
    interator.map = this;
    interator.bucketIndex = -1;
    interator.node = nullptr;
    interator.index = -1;
    return interator;
}

bool hash_Table_Iterator_Has_Next(hash_table_interator_t* iter)
{
    return iter->index < (int32)iter->map->size - 1;
}

hash_table_pair_t* hash_Table_Iterator_Next(hash_table_interator_t* iter)
{
    if (iter->index >= iter->map->size - 1) {
        iter->index = iter->map->size;
        iter->bucketIndex = iter->map->bucketsNum;
        iter->node = nullptr;
        return nullptr;
    }

    if (iter->node != nullptr && iter->node->next != nullptr) {
        iter->node = iter->node->next;
        iter->index++;
        return (hash_table_pair_t*)iter->node->dataPtr;
    }

    iter->bucketIndex++;
    while (iter->bucketIndex < iter->map->bucketsNum) {
        doubly_linked_list_t* bucket = &iter->map->buckets[iter->bucketIndex];
        doubly_linked_list_node_t* head = bucket->head;
        if (head != nullptr) {
            iter->node = head;
            iter->index++;
            return (hash_table_pair_t*)iter->node->dataPtr;
        }
        iter->bucketIndex++;
    }

    iter->node = nullptr;
    iter->index++;
}




