// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

#ifndef STACK_H_
#define STACK_H_

typedef struct mpz_stack_node_ {
    mpz_t val;
    struct mpz_stack_node_* next;
} mpz_stack_node;

typedef struct mpz_stack_ {
	mpz_stack_node *top;
    size_t size;
} mpz_stack;

void mpz_stack_init(mpz_stack **s);

void mpz_stack_clear(mpz_stack *s);

int mpz_stack_push(mpz_stack *s, mpz_t val);

int mpz_stack_pop(mpz_stack *s, mpz_t val);

int mpz_stack_top(mpz_stack *s, mpz_t val);

typedef struct mpz_ui_pair_stack_node_ {
    size_t a;
    mpz_t b;
    struct mpz_ui_pair_stack_node_* next;
} mpz_ui_pair_stack_node;

typedef struct mpz_ui_pair_stack_ {
	mpz_ui_pair_stack_node *top;
    size_t size;
} mpz_ui_pair_stack;

void mpz_ui_pair_stack_init(mpz_ui_pair_stack **s);

void mpz_ui_pair_stack_clear(mpz_ui_pair_stack *s);

int mpz_ui_pair_stack_push(mpz_ui_pair_stack *s, size_t a, mpz_t b);

int mpz_ui_pair_stack_pop(mpz_ui_pair_stack *s, size_t *a, mpz_t b);

int mpz_ui_pair_stack_top(mpz_ui_pair_stack *s, size_t *a, mpz_t b);

int mpz_ui_pair_stack_top_a(mpz_ui_pair_stack *s, size_t *a);

#endif /* STACK_H_ */
