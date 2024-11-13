#include "interface.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// constant for initial capacity
#define I_CAPACITY 16

// struct for kv pair
typedef struct {
  char *key;
  char *value;
} KVPair;

// struct for dynamic arr of kv pairs
typedef struct {
  KVPair *pair;
  size_t count;
  size_t size;
  // Mutex for sychronization
  pthread_mutex_t mutex;
  // condition var for synchronization
  pthread_cond_t var;
} KVArray;

// func to initialize KVarray
void kv_arr_init(KVArray *arr) {
  arr->size = I_CAPACITY;
  arr->count = 0;
  arr->pair = malloc(arr->size * sizeof(KVPair));
  if (!arr->pair) {
    perror("malloc failed in kv_arr_init");
    exit(EXIT_FAILURE);
  }
  pthread_mutex_init(&arr->mutex, NULL);
  pthread_cond_init(&arr->var, NULL);
}

// freeing a Kvarray
void kv_arr_free(KVArray *arr) {
  for (size_t i = 0; i < arr->count; i++) {
    free(arr->pair[i].key);
    free(arr->pair[i].value);
  }
  free(arr->pair);
  pthread_mutex_destroy(&arr->mutex);
  pthread_cond_destroy(&arr->var);
}

// comparing function for qsort
int kv_compare(const void *a, const void *b) {
  const KVPair *kv1 = a;
  const KVPair *kv2 = b;
  return strcmp(kv1->key, kv2->key);
}

// map thread args
typedef struct {
  const struct mr_input *input;
  size_t start;
  size_t end;
  void (*map)(const struct mr_in_kv *);
} MTArgs;

// reduce thread args
typedef struct {
  size_t start;
  size_t end;
  void (*reduce)(const struct mr_out_kv *);
} RTArgs;

// map thread function
void *map_thread(void *arg) {
  MTArgs *args = arg;
  for (size_t i = args->start; i < args->end; i++) {
    args->map(&args->input->kv_lst[i]);
  }
  return NULL;
}

// global array for intermediate and final kv pairs
KVArray i_arr = {NULL, 0, 0, PTHREAD_MUTEX_INITIALIZER,
                 PTHREAD_COND_INITIALIZER}; // intermediate
                                            // array
KVArray f_arr = {NULL, 0, 0, PTHREAD_MUTEX_INITIALIZER,
                 PTHREAD_COND_INITIALIZER}; // final array

// reduce thread function
void *reduce_thread(void *arg) {
  RTArgs *args = arg;
  size_t i = args->start;

  while (i < args->end) {
    pthread_mutex_lock(&i_arr.mutex);

    while (i_arr.count == 0) {
      // waiting if intial array is empty
      pthread_cond_wait(&i_arr.var, &i_arr.mutex);
    }

    // grouping values for the same key
    struct mr_out_kv out_kv;
    strncpy(out_kv.key, i_arr.pair[i].key, MAX_KEY_SIZE - 1);
    out_kv.key[MAX_KEY_SIZE - 1] = '\0';

    size_t j = i + 1;
    while (j < i_arr.count &&
           strcmp(i_arr.pair[i].key, i_arr.pair[j].key) == 0) {
      j++;
    }

    size_t num_val = j - i;
    out_kv.count = num_val;
    out_kv.value = malloc(num_val * sizeof(char[MAX_VALUE_SIZE]));
    if (!out_kv.value) {
      perror("malloc failed in RT function");
      pthread_mutex_unlock(&i_arr.mutex);
      exit(EXIT_FAILURE);
    }

    for (size_t k = 0; k < num_val; k++) {
      strncpy(out_kv.value[k], i_arr.pair[i + k].value, MAX_VALUE_SIZE - 1);
      out_kv.value[k][MAX_VALUE_SIZE - 1] = '\0';
    }

    pthread_mutex_unlock(&i_arr.mutex);

    args->reduce(&out_kv); // calling reduce
    free(out_kv.value);    // freeing memory that was allocated

    i = j;
  }
  return NULL;
}

int mr_emit_i(const char *key, const char *value) {
  pthread_mutex_lock(&i_arr.mutex);

  // increasing size of array if needed
  if (i_arr.count == i_arr.size) {
    i_arr.size *= 2;
    i_arr.pair = realloc(i_arr.pair, i_arr.size * sizeof(KVPair));
    if (!i_arr.pair) {
      perror("realloc in mr_emit_i failed");
      pthread_mutex_unlock(&i_arr.mutex);
      exit(EXIT_FAILURE);
    }
  }

  // duplicating kvs
  i_arr.pair[i_arr.count].key = strndup(key, MAX_KEY_SIZE - 1);
  i_arr.pair[i_arr.count].value = strndup(value, MAX_VALUE_SIZE - 1);
  if (!i_arr.pair[i_arr.count].key || !i_arr.pair[i_arr.count].value) {
    perror("strndup failed in mr_emit_i");
    pthread_mutex_unlock(&i_arr.mutex);
    exit(EXIT_FAILURE);
  }
  i_arr.count += 1;

  pthread_cond_signal(&i_arr.var);
  pthread_mutex_unlock(&i_arr.mutex);
  return 0;
}

int mr_emit_f(const char *key, const char *value) {
  pthread_mutex_lock(&f_arr.mutex);

  if (f_arr.count == f_arr.size) {
    f_arr.size *= 2;
    f_arr.pair = realloc(f_arr.pair, f_arr.size * sizeof(KVPair));
    if (!f_arr.pair) {
      perror("realloc fail in mr_emit_f");
      pthread_mutex_unlock(&f_arr.mutex);
      exit(EXIT_FAILURE);
    }
  }

  f_arr.pair[f_arr.count].key = strndup(key, MAX_KEY_SIZE - 1);
  f_arr.pair[f_arr.count].value = strndup(value, MAX_VALUE_SIZE - 1);
  if (!f_arr.pair[f_arr.count].key || !f_arr.pair[f_arr.count].value) {
    perror("strndup failed in mr_emit_f");
    pthread_mutex_unlock(&f_arr.mutex);
    exit(EXIT_FAILURE);
  }
  f_arr.count += 1;
  pthread_cond_signal(&f_arr.var);
  pthread_mutex_unlock(&f_arr.mutex);
  return 0;
}

// mapreduce execution function
int mr_exec(const struct mr_input *input, void (*map)(const struct mr_in_kv *),
            size_t mapper_count, void (*reduce)(const struct mr_out_kv *),
            size_t reducer_count, struct mr_output *output) {

  // initializing global arrays
  kv_arr_init(&i_arr);
  kv_arr_init(&f_arr);

  // creating map threads
  pthread_t *map_threads = malloc(mapper_count * sizeof(pthread_t));
  MTArgs *map_args = malloc(mapper_count * sizeof(MTArgs));
  if (!map_threads || !map_args) {
    perror("malloc failed in mr_exec mapping threads");
    exit(EXIT_FAILURE);
  }

  size_t records_per_map = input->count / mapper_count;
  size_t extra = input->count % mapper_count;
  size_t index = 0;

  for (size_t i = 0; i < mapper_count; i++) {
    size_t start = index, end;
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

  for (size_t i = 0; i < mapper_count; i++) {
    pthread_join(map_threads[i], NULL); // waiting for mapper threads to finish
  }
  free(map_threads);
  free(map_args);

  // sorting intermediate kv pairs
  pthread_mutex_lock(&i_arr.mutex);
  qsort(i_arr.pair, i_arr.count, sizeof(KVPair), kv_compare);
  pthread_mutex_unlock(&i_arr.mutex);

  // creating reducer threads
  pthread_t *reduce_threads = malloc(reducer_count * sizeof(pthread_t));
  RTArgs *reduce_args = malloc(reducer_count * sizeof(RTArgs));
  if (!reduce_threads || !reduce_args) {
    perror("malloc failed in mr_exec reducer threads");
    exit(EXIT_FAILURE);
  }

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
    size_t keys_assigned = 0, target_keys;
    if (l < extra_keys) {
      target_keys = keys_per_reducer + 1;
    } else {
      target_keys = keys_per_reducer;
    }

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

    pthread_create(&reduce_threads[l], NULL, reduce_thread, &reduce_args[l]);
  }

  for (size_t i = 0; i < reducer_count; i++) {
    pthread_join(reduce_threads[i],
                 NULL); // waiting for reducer threads to finish
  }
  free(reduce_threads);
  free(reduce_args);

  // sorting final kv pair
  pthread_mutex_lock(&f_arr.mutex);
  qsort(f_arr.pair, f_arr.count, sizeof(KVPair), kv_compare);
  pthread_mutex_unlock(&f_arr.mutex);

  output->kv_lst = malloc(f_arr.count * sizeof(struct mr_out_kv));
  if (!output->kv_lst) {
    perror("malloc failed in building output");
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
      perror("malloc failed in output");
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

  // freeing global arrays
  kv_arr_free(&i_arr);
  kv_arr_free(&f_arr);

  return 0;
}