#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "interface.h"

// Data structure for holding a single key-value pair
typedef struct {
    char *key;
    char *value;
} KeyValuePair;

// Data structure for holding multiple key-value pairs and metadata
typedef struct {
    KeyValuePair *pairs;
    int count;
    int capacity;
} KeyValueStore;

typedef struct {
    KeyValueStore *store;
    int start;
    int end;
    void (*map)(const struct mr_in_kv *);
} MapThreadArgs;

typedef struct {
    KeyValueStore *store;
    int start;
    int end;
    void (*reduce)(const struct mr_out_kv *);
} ReduceThreadArgs;

// Global storage for intermediate and final key-value pairs
KeyValueStore intermediate_store;
KeyValueStore final_store;

pthread_mutex_t intermediate_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t final_mutex = PTHREAD_MUTEX_INITIALIZER;

// Initialize a key-value store
void kv_store_init(KeyValueStore *store) {
    store->capacity = 10;
    store->count = 0;
    store->pairs = malloc(store->capacity * sizeof(KeyValuePair));
}

// Add a key-value pair to a key-value store
void kv_store_add(KeyValueStore *store, const char *key, const char *value) {
    if (store->count >= store->capacity) {
        store->capacity *= 2;
        store->pairs = realloc(store->pairs, store->capacity * sizeof(KeyValuePair));
    }
    store->pairs[store->count].key = strdup(key);
    store->pairs[store->count].value = strdup(value);
    store->count++;
}

// Sorting comparator function for key-value pairs
int kv_pair_compare(const void *a, const void *b) {
    return strcmp(((KeyValuePair *)a)->key, ((KeyValuePair *)b)->key);
}

// Sort key-value store by key
void kv_store_sort(KeyValueStore *store) {
    qsort(store->pairs, store->count, sizeof(KeyValuePair), kv_pair_compare);
}

// Map thread function
void *map_thread(void *args) {
    MapThreadArgs *mt_args = (MapThreadArgs *)args;
    for (int i = mt_args->start; i < mt_args->end; i++) {
        struct mr_in_kv kv;
        strncpy(kv.key, mt_args->store->pairs[i].key, MAX_KEY_SIZE);
        strncpy(kv.value, mt_args->store->pairs[i].value, MAX_VALUE_SIZE);
        mt_args->map(&kv);
    }
    return NULL;
}

// Reduce thread function
void *reduce_thread(void *args) {
    ReduceThreadArgs *rt_args = (ReduceThreadArgs *)args;
    for (int i = rt_args->start; i < rt_args->end; i++) {
        struct mr_out_kv grouped_kv = {0};
        strncpy(grouped_kv.key, rt_args->store->pairs[i].key, MAX_KEY_SIZE);

        int j = i;
        while (j < rt_args->end && strcmp(rt_args->store->pairs[i].key, rt_args->store->pairs[j].key) == 0) {
            strncpy(grouped_kv.value[grouped_kv.count++], rt_args->store->pairs[j].value, MAX_VALUE_SIZE);
            j++;
        }
        rt_args->reduce(&grouped_kv);
        i = j - 1;
    }
    return NULL;
}

// Emit an intermediate key-value pair
int mr_emit_i(const char *key, const char *value) {
    pthread_mutex_lock(&intermediate_mutex);
    kv_store_add(&intermediate_store, key, value);
    pthread_mutex_unlock(&intermediate_mutex);
    return 0;
}

// Emit a final key-value pair
int mr_emit_f(const char *key, const char *value) {
    pthread_mutex_lock(&final_mutex);
    kv_store_add(&final_store, key, value);
    pthread_mutex_unlock(&final_mutex);
    return 0;
}

// Execute MapReduce framework
int mr_exec(const struct mr_input *input, void (*map)(const struct mr_in_kv *),
            size_t mapper_count, void (*reduce)(const struct mr_out_kv *),
            size_t reducer_count, struct mr_output *output) {
    
    // Initialize intermediate and final stores
    kv_store_init(&intermediate_store);
    kv_store_init(&final_store);

    // Initialize input store
    KeyValueStore input_store;
    kv_store_init(&input_store);
    for (size_t i = 0; i < input->count; i++) {
        kv_store_add(&input_store, input->kv_lst[i].key, input->kv_lst[i].value);
    }

    // Create and start map threads
    pthread_t map_threads[mapper_count];
    size_t chunk_size = input->count / mapper_count;
    MapThreadArgs map_args[mapper_count];
    for (size_t i = 0; i < mapper_count; i++) {
        map_args[i].store = &input_store;
        map_args[i].start = i * chunk_size;
        map_args[i].end = (i == mapper_count - 1) ? input->count : (i + 1) * chunk_size;
        map_args[i].map = map;
        pthread_create(&map_threads[i], NULL, map_thread, &map_args[i]);
    }

    // Wait for map threads to complete
    for (size_t i = 0; i < mapper_count; i++) {
        pthread_join(map_threads[i], NULL);
    }

    // Sort and group intermediate key-value pairs
    kv_store_sort(&intermediate_store);

    // Create and start reduce threads
    pthread_t reduce_threads[reducer_count];
    chunk_size = intermediate_store.count / reducer_count;
    ReduceThreadArgs reduce_args[reducer_count];
    for (size_t i = 0; i < reducer_count; i++) {
        reduce_args[i].store = &intermediate_store;
        reduce_args[i].start = i * chunk_size;
        reduce_args[i].end = (i == reducer_count - 1) ? intermediate_store.count : (i + 1) * chunk_size;
        reduce_args[i].reduce = reduce;
        pthread_create(&reduce_threads[i], NULL, reduce_thread, &reduce_args[i]);
    }

    // Wait for reduce threads to complete
    for (size_t i = 0; i < reducer_count; i++) {
        pthread_join(reduce_threads[i], NULL);
    }

    // Transfer final store to output
    kv_store_sort(&final_store);
    output->kv_lst = malloc(final_store.count * sizeof(struct mr_out_kv));
    output->count = final_store.count;
    for (int i = 0; i < final_store.count; i++) {
        strncpy(output->kv_lst[i].key, final_store.pairs[i].key, MAX_KEY_SIZE);
        strncpy(output->kv_lst[i].value[0], final_store.pairs[i].value, MAX_VALUE_SIZE);
        output->kv_lst[i].count = 1;
    }

    // Clean up
    for (int i = 0; i < input_store.count; i++) {
        free(input_store.pairs[i].key);
        free(input_store.pairs[i].value);
    }
    free(input_store.pairs);

    for (int i = 0; i < intermediate_store.count; i++) {
        free(intermediate_store.pairs[i].key);
        free(intermediate_store.pairs[i].value);
    }
    free(intermediate_store.pairs);

    for (int i = 0; i < final_store.count; i++) {
        free(final_store.pairs[i].key);
        free(final_store.pairs[i].value);
    }
    free(final_store.pairs);

    return 0;
}