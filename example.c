#include "hash.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define MAKE_PAIR(x) ((pair){ .k = (x), .ksize = strlen(x) })

struct student {
	char *university;
	char *student_id;
};

static size_t khash(const pair *p)
{
	const struct student *st = p->k;
	pair student = MAKE_PAIR(st->university);
	size_t uni_hash = hm_hash(&student);

	student = MAKE_PAIR(st->student_id);
	size_t sid_hash = hm_hash(&student);

	return ((uni_hash + sid_hash) << 5) ^ (uni_hash + sid_hash);
}

static int student_isequal(const pair *p1, const pair *p2)
{
	const struct student *s1 = p1->k;
	const struct student *s2 = p2->k;

	return !strcmp(s1->university, s2->university) &&
		!strcmp(s1->student_id, s2->student_id);
}

static int isequal(const pair *p1, const pair *p2)
{
	return !strcmp(p1->k, p2->k);
}

int main(void)
{
	size_t hash;
	char k[] = "blah blah"; /* key for test entry */
	int v1 = 0xDEADBEEF; /* value for test entry */

	/* Create new hashmap */
	hashmap *hm = hm_init(hm_hash, isequal);

	/* Insert test values in hashmap */
	pair *p = malloc(sizeof *p);
	p->k = k;
	p->ksize = sizeof k;
	p->v = &v1;

	if (!hm_add(hm, p)) { /* same as  == HASHMAP_FAILURE */
		perror("Could not insert into HashMap\n");
		return EXIT_FAILURE;
	}

	printf("%s: %x\n", k, *(int *)hm_get(hm, p));

	/* test that we can find what we inserted */
	hash = hm_hash(p);
	assert(hm->pairs[hash % hm->size]->v == hm_get(hm, p));
	free(p);

	/* free hashmap memory */
	hm_free(hm);

	/* init another hashmap */
	hm = hm_init(khash, student_isequal);

	/* init a student struct for the key */
	struct student s = {
		.university = "Dalhousie",
		.student_id = "BXCD43243",
	};

	/* put struct student in a pair */
	p = malloc(sizeof *p);
	p->k = &s;
	p->v = "Damien Robichaud";

	/* add the pair to the hashmap */
	if (!hm_add(hm, p)) {
		perror("Could not insert into HashMap\n");
		return EXIT_FAILURE;
	}

	/* reinitialize a pair with the same key and no value */
	free(p);
	p = malloc(sizeof *p);
	p->k = &s;

	printf("%s-%s: %s\n", s.university, s.student_id, (char *)hm_get(hm, p));

	free(p);
	hm_free(hm);

	return 0;
}
