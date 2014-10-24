// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

#include <gmp.h>
#include <stdlib.h>
#include "stack.h"
#include "config.h"

// ## Simple stack

// Create a new stack.
void mpz_stack_init(mpz_stack **s)
{
	mpz_stack *n = malloc(sizeof(mpz_stack));
    n->top = NULL;
    n->size = 0;
    *s = n;
}

// Delete a stack and all its nodes.
void mpz_stack_clear(mpz_stack *s)
{
	mpz_stack_node *top = s->top;
    while (top != NULL) {
    	mpz_stack_node *next = top->next;
    	mpz_clear(top->val);
        free(top);
        top = next;
    }
    free(s);
}

// Pushes a value onto the stack,
// return 1 if element was pushed on successfully, 0 otherwise
int mpz_stack_push(mpz_stack *s, mpz_t val)
{
    if (s == NULL) return 0;
    mpz_stack_node *n = malloc(sizeof(mpz_stack_node));
    if (n == NULL) return 0;
    mpz_init_set(n->val, val);
    n->next = s->top;
    s->top = n;
    s->size += 1;
    return 1;
}

// Removes top element from stack,
// return 1 if an element was removed and 0 otherwise
// return value is passed through value reference
int mpz_stack_pop(mpz_stack *s, mpz_t val)
{
    if (s == NULL || s->top == NULL) return 0;
    mpz_stack_node *top = s->top;
    mpz_set(val, top->val);
    s->top = top->next;
    s->size -= 1;
    mpz_clear(top->val);
    free(top);
    return 1;
}

// Get the values of the top element
// return 1 if an element an top element exists
// return value is passed through value reference
int mpz_stack_top(mpz_stack *s, mpz_t val)
{
    if (s == NULL || s->top == NULL) return 0;
    mpz_set(val, s->top->val);
    return 1;
}

// ## (mpz, ui) pair stack

// Create a new pair stack.
void mpz_ui_pair_stack_init(mpz_ui_pair_stack **s)
{
	mpz_ui_pair_stack *n = malloc(sizeof(mpz_ui_pair_stack));
    n->top = NULL;
    n->size = 0;
    *s = n;
}

// Delete a stack and all its nodes
void mpz_ui_pair_stack_clear(mpz_ui_pair_stack *s)
{
	mpz_ui_pair_stack_node *top = s->top;
    while (top != NULL) {
    	mpz_ui_pair_stack_node *next = top->next;
    	mpz_clear(top->b);
        free(top);
        top = next;
    }
    free(s);
}

// Pushes a value onto the stack,
// return 1 if element was pushed on successfully, 0 otherwise
int mpz_ui_pair_stack_push(mpz_ui_pair_stack *s, size_t a, mpz_t b)
{
    if (s == NULL) return 0;
    mpz_ui_pair_stack_node *n = malloc(sizeof(mpz_ui_pair_stack_node));
    if (n == NULL) return 0;
    n->a = a;
    mpz_init_set(n->b, b);
    n->next = s->top;
    s->top = n;
    s->size += 1;
    return 1;
}

// Removes top element from stack,
// return 1 if an element was removed and 0 otherwise
// return value is passed through value reference
int mpz_ui_pair_stack_pop(mpz_ui_pair_stack *s, size_t *a, mpz_t b)
{
    if (s == NULL || s->top == NULL) return 0;
    mpz_ui_pair_stack_node *top = s->top;
    *a = top->a;
    mpz_set(b, top->b);
    s->top = top->next;
    s->size -= 1;
	mpz_clear(top->b);
    free(top);
    return 1;
}

// Get the values of the top element
// return 1 if an element an top element exists
// return value is passed through value reference
int mpz_ui_pair_stack_top(mpz_ui_pair_stack *s, size_t *a, mpz_t b)
{
    if (s == NULL || s->top == NULL) return 0;
    *a = s->top->a;
    mpz_set(b, s->top->b);
    return 1;
}

int mpz_ui_pair_stack_top_a(mpz_ui_pair_stack *s, size_t *a) {
    if (s == NULL || s->top == NULL) return 0;
    *a = s->top->a;
    return 1;
}
