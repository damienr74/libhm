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

#include "hash.h"
#include <stdlib.h>

/*
 * HASHMAP_INIT_SIZE and HASHMAP_MAX_FILL_RATIO defined in
 * the lbrary header, can be redefined in your project's
 * Makefile, or before including the library header.
 */

int hm_add(hashmap *hm, pair *p)
{
	/* Grow hashmap if it is too full */
	if ((hm->nmemb / (double)hm->size) > HASHMAP_MAX_FILL_RATIO) {

		void *tmp = realloc(hm->pairs, hm->size * 2 * sizeof *p);
		if (!tmp)
			return HASHMAP_FAILURE;

		hm->pairs = tmp;
		hm->size *= 2;
		/* TODO Rehash */
	}

	size_t index = hm->khash(p) % hm->size;
	if (!hm->pairs[index]) {
		hm->pairs[index] = p;
	} else {
		pair *ptr = hm->pairs[index];

		while (!hm->isequal(ptr, p) && ptr->next)
			ptr = ptr->next;

		if (!ptr) {
			p->next = hm->pairs[index];
			hm->pairs[index]->next = p;
		} else {
			*ptr = *p;
			p->next = NULL;
		}
	}

	return HASHMAP_SUCCESS;
}

void *hm_get(hashmap *hm, pair *p)
{
	pair *ptr = hm->pairs[hm->khash(p) % hm->size];
	if (!ptr)
		return ptr;

	if (!ptr->next)
		return ptr->v;

	if (!hm->isequal)
		return NULL;

	while (!hm->isequal(p, ptr) && ptr->next)
		ptr = ptr->next;

	if (!ptr)
		return ptr;

	return ptr->v;
}

hashmap *hm_init(size_t (*khash)(const pair *),
		int (*isequal)(const pair *, const pair *))
{
	hashmap *hm = malloc(sizeof *hm);
	if (!hm)
		return hm;

	hm->pairs = malloc(HASHMAP_INIT_SIZE * sizeof *hm->pairs);
	if (!hm->pairs) {
		free(hm);
		return NULL;
	}

	hm->size = HASHMAP_INIT_SIZE;
	hm->nmemb = 0;
	hm->khash = khash;
	hm->isequal = isequal;

	for (size_t i = 0; i < hm->size; i++)
		hm->pairs[i] = NULL;

	return hm;
}

void hm_free(hashmap *hm)
{
	if (!hm)
		return;

	if (!hm->pairs)
		goto out;

	for (size_t i = 0; i < hm->size; i++) {
		if (hm->pairs[i] && hm->pairs[i]->next) {
			pair *p = hm->pairs[i];
			while (p->next) {
				hm->pairs[i] = p->next;
				free(p);
			}
			free(hm->pairs[i]);
		}
	}

	free(hm->pairs);
out:
	free(hm);
}

/* NOT TO BE USED ON STRUCTS DUE TO PADDING */
size_t hm_hash(const pair *p)
{
	size_t i = 0;
	size_t hash = 0;

	while (i != p->ksize) {
		hash += ((char *)p->k)[i++];
		hash += hash << 10;
		hash ^= hash >> 6;
	}

	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;

	return hash;
}
