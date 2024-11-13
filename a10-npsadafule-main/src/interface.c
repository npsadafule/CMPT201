#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "interface.h"

// Constants for initial capacities
#define I_CAPACITY 16

// Structure for a key-value pair
typedef struct {
    char *key;
    char *value;
} KVPair;

// Structure for dynamic arr of key-value pair
typedef struct {
    KVPair *pair;
    size_t count;
    size_t size;
    pthread_mutex_t mutex;       // Mutex for synchronization
    pthread_cond_t var;     // Condition variable for synchronization
} KVArray;

// Function to initialize a KVArray
void kv_arr_init(KVArray *arr) {
    arr->size = I_CAPACITY;
    arr->count = 0;
    arr->pair = malloc(arr->size * sizeof(KVPair));
    if (!arr->pair) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_init(&arr->mutex, NULL);
    pthread_cond_init(&arr->var, NULL);
}

// Function to add a key-value pair to a KVArray
void kv_arr_add(KVArray *arr, const char *key, const char *value) {
    pthread_mutex_lock(&arr->mutex);

    // Resize the arr if necessary
    if (arr->count == arr->size) {
        arr->size *= 2;
        arr->pair = realloc(arr->pair, arr->size * sizeof(KVPair));
        if (!arr->pair) {
            perror("realloc failed");
            pthread_mutex_unlock(&arr->mutex);
            exit(EXIT_FAILURE);
        }
    }

    // Duplicate the key and value
    arr->pair[arr->count].key = strndup(key, MAX_KEY_SIZE - 1);
    arr->pair[arr->count].value = strndup(value, MAX_VALUE_SIZE - 1);
    if (!arr->pair[arr->count].key || !arr->pair[arr->count].value) {
        perror("strndup failed");
        pthread_mutex_unlock(&arr->mutex);
        exit(EXIT_FAILURE);
    }
    arr->count++;

    pthread_cond_signal(&arr->var); // Signal any waiting threads
    pthread_mutex_unlock(&arr->mutex);
}

// Function to free a KVArray
void kv_arr_free(KVArray *arr) {
    for (size_t i = 0; i < arr->count; i++) {
        free(arr->pair[i].key);
        free(arr->pair[i].value);
    }
    free(arr->pair);
    pthread_mutex_destroy(&arr->mutex);
    pthread_cond_destroy(&arr->var);
}

// Comparator function for qsort
int kv_compare(const void *a, const void *b) {
    const KVPair *kv1 = a;
    const KVPair *kv2 = b;
    return strcmp(kv1->key, kv2->key);
}

// Map thread arguments
typedef struct {
    const struct mr_input *input;
    size_t start;
    size_t end;
    void (*map)(const struct mr_in_kv *);
} MTArgs;

// Reduce thread arguments
typedef struct {
    size_t start;
    size_t end;
    void (*reduce)(const struct mr_out_kv *);
} RTArgs;

// Map thread function
void *map_thread(void *arg) {
    MTArgs *args = arg;
    for (size_t i = args->start; i < args->end; i++) {
        args->map(&args->input->kv_lst[i]);
    }
    return NULL;
}

// Global arrs for intermediate and final key-value pair
KVArray i_arr = {NULL, 0, 0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};
KVArray f_arr = {NULL, 0, 0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

// Reduce thread function
void *reduce_thread(void *arg) {
    RTArgs *args = arg;
    size_t i = args->start;

    while (i < args->end) {
        pthread_mutex_lock(&i_arr.mutex);

        // Wait if i_arr is empty
        while (i_arr.count == 0) {
            pthread_cond_wait(&i_arr.var, &i_arr.mutex);
        }

        // Group values for the same key
        struct mr_out_kv out_kv;
        strncpy(out_kv.key, i_arr.pair[i].key, MAX_KEY_SIZE - 1);
        out_kv.key[MAX_KEY_SIZE - 1] = '\0';

        size_t j = i + 1;
        while (j < i_arr.count &&
               strcmp(i_arr.pair[i].key, i_arr.pair[j].key) == 0) {
            j++;
        }

        size_t num_val = j - i; ////////////////////////////
        out_kv.count = num_val;
        out_kv.value = malloc(num_val * sizeof(char[MAX_VALUE_SIZE]));
        if (!out_kv.value) {
            perror("malloc failed");
            pthread_mutex_unlock(&i_arr.mutex);
            exit(EXIT_FAILURE);
        }

        for (size_t k = 0; k < num_val; k++) {
            strncpy(out_kv.value[k], i_arr.pair[i + k].value, MAX_VALUE_SIZE - 1);
            out_kv.value[k][MAX_VALUE_SIZE - 1] = '\0';
        }

        pthread_mutex_unlock(&i_arr.mutex);

        // Call the reduce function
        args->reduce(&out_kv);

        // Free allocated memory
        free(out_kv.value);

        i = j;
    }
    return NULL;
}

// Implementation of mr_emit_i
int mr_emit_i(const char *key, const char *value) {
    kv_arr_add(&i_arr, key, value);
    return 0;
}

// Implementation of mr_emit_f
int mr_emit_f(const char *key, const char *value) {
    kv_arr_add(&f_arr, key, value);
    return 0;
}

// Main MapReduce execution function
int mr_exec(const struct mr_input *input,
            void (*map)(const struct mr_in_kv *),
            size_t mapper_count,
            void (*reduce)(const struct mr_out_kv *),
            size_t reducer_count,
            struct mr_output *output) {

    // Initialize global arrs
    kv_arr_init(&i_arr);
    kv_arr_init(&f_arr);

    // Create mapper threads
    pthread_t *map_threads = malloc(mapper_count * sizeof(pthread_t));
    MTArgs *map_args = malloc(mapper_count * sizeof(MTArgs));
    if (!map_threads || !map_args) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    size_t records_per_map = input->count / mapper_count;
    size_t extra = input->count % mapper_count;
    size_t index = 0;

    for (size_t i = 0; i < mapper_count; i++) {
        size_t start = index;
        size_t end; /////////////////////////////////////////////
        if (i < extra) {
            end = start + records_per_map + 1;
        } else {
            end = start + records_per_map;
        }

        map_args[i].input = input;
        map_args[i].start = start;
        map_args[i].end = end;
        map_args[i].map = map;

        pthread_create(&map_threads[i], NULL, map_thread, &map_args[i]);

        index = end;
    }

    // Wait for mapper threads to finish
    for (size_t i = 0; i < mapper_count; i++) {
        pthread_join(map_threads[i], NULL);
    }
    free(map_threads);
    free(map_args);

    // Sort intermediate key-value pair
    pthread_mutex_lock(&i_arr.mutex);
    qsort(i_arr.pair, i_arr.count, sizeof(KVPair), kv_compare);
    pthread_mutex_unlock(&i_arr.mutex);

    // Create reducer threads
    pthread_t *reduce_threads = malloc(reducer_count * sizeof(pthread_t));
    RTArgs *reduce_args = malloc(reducer_count * sizeof(RTArgs));
    if (!reduce_threads || !reduce_args) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    // Calculate unique keys
    size_t totalkeys = 0;
    size_t i = 0;
    while (i < i_arr.count) {
        totalkeys++;
        size_t j = i + 1;
        while (j < i_arr.count &&
               strcmp(i_arr.pair[i].key, i_arr.pair[j].key) == 0) {
            j++;
        }
        i = j;
    }

    size_t keys_per_reducer = totalkeys / reducer_count;
    size_t extra_keys = totalkeys % reducer_count;
    i = 0;

    for (size_t l = 0; l < reducer_count; l++) {
        reduce_args[l].start = i;
        size_t keys_assigned = 0;
        size_t target_keys = keys_per_reducer + (l < extra_keys ? 1 : 0);

        while (i < i_arr.count && keys_assigned < target_keys) {
            size_t j = i + 1;
            while (j < i_arr.count &&
                   strcmp(i_arr.pair[i].key, i_arr.pair[j].key) == 0) {
                j++;
            }
            i = j;
            keys_assigned++;
        }
        reduce_args[l].end = i;
        reduce_args[l].reduce = reduce;

        pthread_create(&reduce_threads[l], NULL, reduce_thread
    , &reduce_args[l]);
    }

    // Wait for reducer threads to finish
    for (size_t i = 0; i < reducer_count; i++) {
        pthread_join(reduce_threads[i], NULL);
    }
    free(reduce_threads);
    free(reduce_args);

    // Sort final key-value pair
    pthread_mutex_lock(&f_arr.mutex);
    qsort(f_arr.pair, f_arr.count, sizeof(KVPair), kv_compare);
    pthread_mutex_unlock(&f_arr.mutex);

    // Build output
    output->kv_lst = malloc(f_arr.count * sizeof(struct mr_out_kv));
    if (!output->kv_lst) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    output->count = 0;
    i = 0;

    while (i < f_arr.count) {
        struct mr_out_kv *out_kv = &output->kv_lst[output->count];
        strncpy(out_kv->key, f_arr.pair[i].key, MAX_KEY_SIZE - 1);
        out_kv->key[MAX_KEY_SIZE - 1] = '\0';

        size_t j = i + 1;
        while (j < f_arr.count &&
               strcmp(f_arr.pair[i].key, f_arr.pair[j].key) == 0) {
            j++;
        }

        size_t num_val = j - i;
        out_kv->value = malloc(num_val * sizeof(char[MAX_VALUE_SIZE]));
        if (!out_kv->value) {
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }
        out_kv->count = num_val;

        for (size_t k = 0; k < num_val; k++) {
            strncpy(out_kv->value[k], f_arr.pair[i + k].value, MAX_VALUE_SIZE - 1);
            out_kv->value[k][MAX_VALUE_SIZE - 1] = '\0';
        }

        output->count++;
        i = j;
    }

    // Free global arrs
    kv_arr_free(&i_arr);
    kv_arr_free(&f_arr);

    return 0;
}