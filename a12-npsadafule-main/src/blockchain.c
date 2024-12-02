#include "blockchain.h"
#include <openssl/evp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

  // initializing nonce
  core->nonce = 0;
  unsigned char temp_hash[SHA256_DIGEST_LENGTH];
  int found = 0;

  // finding a nonce that makes the hash <= difficulty
  while (core->nonce <= UINT32_MAX) {
    hash_block_core(core, temp_hash);

    if (hash_meets_difficulty(temp_hash, bc->difficulty)) {
      found = 1;
      break; // valid hash found, no need to increment
    }

    core->nonce++;
  }

  if (!found) {
    // fail to find valid nonce with the nonce range
    return -1;
  }

  // set the block's hash
  memcpy(new_block->hash, temp_hash, SHA256_DIGEST_LENGTH);

  // increment blockchain count
  bc->count++;

  return (int)(bc->count - 1); // returning the index of the new block
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