/*
 * Licenced under MIT LICENCE.
 *
 * This is a hashmap implementation that comes with a
 * default hashing algorithm (see hm_hash) for integer
 * types, strings, and arrays.
 *
 * DO NOT USE `hm_hash` FOR STRUCTS. Since there can be
 * padding and it will be UNDEFINED BEHAVIOR. To hash a
 * structure, make your own hashing method. Please refer
 * to examples.c for an example of struct hashing.
 *
 * A pair is a key-value pair that holds four elements:
 *   size_t ksize: the sizeof the key to hash
 *   void *k: a pointer to the key
 *   void *v: a pointer to the value
 *   struct _pair *next: a pointer to the next collision
 *
 * A hashmap is a data strucutre that maps pairs to indexes
 * in a list of pairs with an expected time of O(1).
 */

#ifndef HASHMAP_H
#define HASHMAP_H

/*
 * HASHMAP_INIT_SIZE and HASHMAP_MAX_FILL_RATIO can
 * be redefined in your project's makefile, or before
 * including the library header.
 */
#ifndef HASHMAP_INIT_SIZE
#define HASHMAP_INIT_SIZE (101)
#endif

#ifndef HASHMAP_MAX_FILL_RATIO
#define HASHMAP_MAX_FILL_RATIO (0.5)
#endif

#include <stddef.h>

enum {
	HASHMAP_FAILURE,
	HASHMAP_SUCCESS,
};

typedef struct _pair {
	size_t ksize;
	const void *k;
	void *v;
	struct _pair *next;
} pair;

typedef struct {
	pair **pairs;
	size_t nmemb;
	size_t size;
	size_t (*khash)(const pair *);
	int (*isequal)(const pair *, const pair *);
} hashmap;


int hm_add(hashmap *hm, pair *p);
void *hm_get(hashmap *hm, pair *p);
hashmap *hm_init(size_t (*khash)(const pair *),
		int (*isequal)(const pair *, const pair *));
void hm_free(hashmap *hm);

/*
 * Default hash function for integer types and arrays see
 * example.c for an example of how to hash another type.
 */
size_t hm_hash(const pair *p);

#endif
