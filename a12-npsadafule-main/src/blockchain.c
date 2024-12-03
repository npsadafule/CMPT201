#include "blockchain.h"
#include <openssl/evp.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Structure to pass args to threads
struct thread_args {
  const struct block_core *core_template; // Read-only template of block_core
  struct block_core *core;                // pointer to the block_core to update
  const unsigned char *difficulty;
  uint32_t nonce_start;
  uint32_t nonce_end;
  int *found;
  pthread_mutex_t *found_mutex;
};

// helper function to hash_block core
static void hash_block_core(const struct block_core *core,
                            unsigned char *hash) {
  // creating a new EVP_MD context
  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  if (!ctx) {
    fprintf(stderr, "Failed to create EVP_MD_CTX\n");
    exit(EXIT_FAILURE);
  }

  // initialize context for SHA256
  if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1 ||
      EVP_DigestUpdate(ctx, core, sizeof(struct block_core)) != 1 ||
      EVP_DigestFinal_ex(ctx, hash, NULL) != 1) {
    fprintf(stderr, "sha256 compute fail\n");
    EVP_MD_CTX_free(ctx);
    exit(EXIT_FAILURE);
  }

  // free context
  EVP_MD_CTX_free(ctx);
}

// helper function to check is hash meets difficulty
static int hash_meets_difficulty(const unsigned char *hash,
                                 const unsigned char *difficulty) {
  return memcmp(hash, difficulty, SHA256_DIGEST_LENGTH) <= 0;
}

void *nonce_search_thread(void *arg) {
  struct thread_args *args = (struct thread_args *)arg;
  struct block_core local_core = *(args->core_template); // copying template
  unsigned char temp_hash[SHA256_DIGEST_LENGTH];

  for (uint32_t nonce = args->nonce_start; nonce <= args->nonce_end; nonce++) {
    // cheching if vlaid nonce has been found already
    pthread_mutex_lock(args->found_mutex);
    if (*(args->found)) {
      pthread_mutex_unlock(args->found_mutex);
      break; // exiting if another thread finds a valid nonce
    }
    pthread_mutex_unlock(args->found_mutex);

    local_core.nonce = nonce;
    hash_block_core(&local_core, temp_hash);

    if (hash_meets_difficulty(temp_hash, args->difficulty)) {
      // valid nonce found, updating shared core and found flag
      pthread_mutex_lock(args->found_mutex);
      if (!*(args->found)) {
        *(args->found) = 1;
        *(args->core) = local_core; // updating shared core
      }
      pthread_mutex_unlock(args->found_mutex);
      break;
    }
  }

  return NULL;
}

// Function to initialize the blockchain
int bc_init(struct blockchain *bc,
            unsigned char difficulty[SHA256_DIGEST_LENGTH]) {
  if (!bc)
    return -1;

  memset(bc, 0, sizeof(struct blockchain)); // initializing the blockchain
  if (difficulty) {
    memcpy(bc->difficulty, difficulty, SHA256_DIGEST_LENGTH);
  } else {
    memset(bc->difficulty, 0xFF,
           SHA256_DIGEST_LENGTH); // no difficulty constraint
  }

  return 0;
}

// function to add a block to the blockchain
int bc_add_block(struct blockchain *bc, const unsigned char data[DATA_SIZE]) {
  if (!bc || bc->count >= BLOCKCHAIN_SIZE)
    return -1;

  struct block *new_block = &bc->blocks[bc->count];
  struct block_core *core = &new_block->core;

  // zero-initializing block_core to avoid uninitialzed padding bytes
  memset(core, 0, sizeof(struct block_core));

  // set block_core fields
  core->index = bc->count;

  if (clock_gettime(CLOCK_REALTIME, &core->timestamp) != 0) {
    perror("error in clock_gettime");
    return -1;
  }

  memcpy(core->data, data, DATA_SIZE);

  if (core->index == 0) {
    memset(core->p_hash, 0, SHA256_DIGEST_LENGTH);
  } else {
    memcpy(core->p_hash, bc->blocks[core->index - 1].hash,
           SHA256_DIGEST_LENGTH);
  }

  unsigned char temp_hash[SHA256_DIGEST_LENGTH];
  int found = 0;
  pthread_mutex_t found_mutex = PTHREAD_MUTEX_INITIALIZER;

  // number of threads (using 8 as this container has 8 cores)
  int num_threads = 8;

  pthread_t threads[num_threads];
  struct thread_args args[num_threads];

  // calculating nonce range for every thread
  uint32_t nonce_per_thread = UINT32_MAX / num_threads;
  uint32_t nonce_start = 0;

  // preparing a read only template of core
  struct block_core core_template = *core;

  // thread creation
  for (int i = 0; i < num_threads; i++) {
    args[i].core_template = &core_template;
    args[i].core = core;
    args[i].difficulty = bc->difficulty;
    args[i].nonce_start = nonce_start;
    if (i == num_threads - 1) {
      args[i].nonce_end = UINT32_MAX;
    } else {
      args[i].nonce_end = nonce_start + nonce_per_thread - 1;
    }
    args[i].found = &found;
    args[i].found_mutex = &found_mutex;

    if (pthread_create(&threads[i], NULL, nonce_search_thread, &args[i]) != 0) {
      perror("error in pthread_create");
      // clean up previous created threads
      for (int j = 0; j < i; j++) {
        pthread_join(threads[j], NULL);
      }
      pthread_mutex_destroy(&found_mutex);
      return -1;
    }

    nonce_start += nonce_per_thread;
  }

  // waiting for threads to finish
  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  pthread_mutex_destroy(&found_mutex);

  if (!found) {
    // failed to find valid nonce with the nonce_range
    return -1;
  }

  // computing hash with found nonce
  hash_block_core(core, temp_hash);

  // set block's hash
  memcpy(new_block->hash, temp_hash, SHA256_DIGEST_LENGTH);

  // incrementing blockchain count
  bc->count++;

  return 0; // read the piazza posts
}

// function to verify blockchain integrity
int bc_verify(struct blockchain *bc) {
  if (!bc)
    return -1;

  for (size_t i = 0; i < bc->count; i++) {
    struct block *block = &bc->blocks[i];
    unsigned char calculated_hash[SHA256_DIGEST_LENGTH];

    // recalculating the hash of the block_core
    hash_block_core(&block->core, calculated_hash);

    // checking if hash matches the stored hash
    if (memcmp(calculated_hash, block->hash, SHA256_DIGEST_LENGTH) != 0) {
      return -1; // hash mismatch
    }

    // checking if hash meets the difficulty
    if (!hash_meets_difficulty(block->hash, bc->difficulty)) {
      return -1; // difficultynot met
    }

    // check p_hash
    if (i > 0) {
      struct block *previous_block = &bc->blocks[i - 1];
      if (memcmp(block->core.p_hash, previous_block->hash,
                 SHA256_DIGEST_LENGTH) != 0) {
        return -1; // p_hash mismatch
      }
    } else {
      // for genesis block, p_hash should be all zero
      unsigned char zeros[SHA256_DIGEST_LENGTH];
      memset(zeros, 0, SHA256_DIGEST_LENGTH);
      if (memcmp(block->core.p_hash, zeros, SHA256_DIGEST_LENGTH) != 0) {
        return -1; // genesis block p_hash is not zero
      }
    }
  }

  return 0;
}