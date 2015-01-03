#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "divide_conquer.h"
#include "stack.h"

// This algorithm will iterative simulate a recursive divide and conquer algorithm.
size_t array_divide_conquer(mpz_pool *pool, mpz_t ret, mpz_array *in, void(*merge)(mpz_pool *, mpz_t, const mpz_t, const mpz_t)) {
	size_t n = 1, index = 2, length = in->used, stack_len = 0, n_top;
	mpz_t a, b, x;
	mpz_ui_pair_stack *stack;

	// At least two elements are required.
	if (length < 2) {
		return 0;
	} else if (length == 2) {
		// No bootstrap shortcut.
		merge(pool, ret, in->array[0], in->array[1]);
	} else {
		// Bootstrap.
		mpz_ui_pair_stack_init(&stack);
		mpz_init_set(a, in->array[0]);
		mpz_init_set(b, in->array[1]);
		mpz_init(x);

		// Start looping.
		while (1) {
			// Call the merge function.
			merge(pool, x, a, b);

			// Store the stack length and the n value of the top
			// element in the stack.
			stack_len = stack->size;
			if (stack_len) mpz_ui_pair_stack_top_a(stack, &n_top);
			// If the stack top is a left neighbor go up.
			if(stack_len && n_top == n) {
				mpz_ui_pair_stack_pop(stack, &n, a);
				mpz_set(b, x);
				n++;
			// Else if we can jump down do it.
			} else if(index + 1 < length) {
				mpz_ui_pair_stack_push(stack, n, x);
				n = 1;
				mpz_set(a, in->array[index]);
				mpz_set(b, in->array[index+1]);
				index = index + 2;
			// Jump right if only one leaf is left.
			} else if(index < length) {
				mpz_set(a, x);
				mpz_set(b, in->array[index]);
				index++;
			// If we can't use the left neighbor, jump down or right
			// jump up to the first element in the stack.
			} else if (stack_len) {
				mpz_ui_pair_stack_pop(stack, &n, a);
				mpz_set(b, x);
			// If the stack is empty and the index is at length stop.
			} else if (stack_len == 0 && index == length) {
				break;
			// Should never happen.
			} else {
				printf("error in array_divide_conquer\n");
				break;
			}
		}

		mpz_set(ret, x);

		mpz_clear(a);
		mpz_clear(b);
		mpz_clear(x);
		mpz_ui_pair_stack_clear(stack);
	}

	return n;
}
