// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// Implementation of "Factoring into coprimes in essentially linear time".

// # algorithm
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <gmp.h>
#include "copri.h"
#include "config.h"
#if USE_OPENMP
#include <omp.h>
#endif



// ###Compute a^2^n.

// The result is stored in the parameter `mpz_t rot`.
// This parameter has to be **initialized** on invocation.
// The function operates on a single (input & output) integer.
//
// Algorithm 10.1 [PDF page 13](http://cr.yp.to/lineartime/dcba-20040404.pdf)
//
// See [twopower test](test-twopower.html) for basic usage.
void two_power(mpz_t rot, unsigned long long n) {
	while(n > 0) {
		mpz_mul(rot, rot, rot);
		n--;
	}
}

// ### Compute gcd, ppi and ppo

//     gcd = gcd(a,b)  greatest common divisor
//     ppi = ppi(a,b)  powers in a of primes inside b
//     ppo = ppo(a,b)  powers in a of primes outside b
//
//
//
// Algorithm 11.3 [PDF page 14](http://cr.yp.to/lineartime/dcba-20040404.pdf)
//
// See [gcdppippo test](test-gcdppippo.html) for basic usage.
void gcd_ppi_ppo(mpz_t gcd, mpz_t ppi, mpz_t ppo, const mpz_t a, const mpz_t b) {
	mpz_t g;
	mpz_init(g);
	mpz_gcd(ppi, a, b);
	mpz_set(gcd, ppi);
	mpz_fdiv_q(ppo, a, ppi);
	while(1) {
		mpz_gcd(g, ppi, ppo);
		if (mpz_cmp_ui(g, 1) == 0) {
			mpz_clear(g);
			return;
		}
		mpz_mul(ppi, ppi, g);
		mpz_fdiv_q(ppo, ppo, g);
	}
}

// #### Shortcuts

// Compute ppi and ppo. Ingore gcd.
void ppi_ppo(mpz_t ppi, mpz_t ppo, const mpz_t a, const mpz_t c) {
	mpz_t gcd;
	mpz_init(gcd);
	gcd_ppi_ppo(gcd, ppi, ppo, a, c);
	mpz_clear(gcd);
}

// Compute ppi. Ingore gcd and ppo.
void ppi(mpz_t ppi, const mpz_t a, const mpz_t c) {
	mpz_t gcd;
	mpz_t ppo;
	mpz_init(gcd);
	mpz_init(ppo);
	gcd_ppi_ppo(gcd, ppi, ppo, a, c);
	mpz_clear(gcd);
	mpz_clear(ppo);
}

// ### Compute gcd, ppg and pple

//     gcd  =  gcd(a,b)  greatest common divisor
//     ppg  =  ppg(a,b)  prime powers in a greater than those in b
//     pple = pple(a,b)  prime powers in a less than or equal to those in b
//
//
//
// Algorithm 11.4 [PDF page 14](http://cr.yp.to/lineartime/dcba-20040404.pdf)
//
// See [gcdppgpple test](test-gcdppgpple.html) for basic usage.
void gcd_ppg_pple(mpz_t gcd, mpz_t ppg, mpz_t pple, const mpz_t a, const mpz_t b) {
	mpz_t g;
	mpz_init(g);
	mpz_gcd(pple, a, b);
	mpz_set(gcd, pple);
	mpz_fdiv_q(ppg, a, pple);
	while(1) {
		mpz_gcd(g, ppg, pple);
		if (mpz_cmp_ui(g, 1) == 0) {
			mpz_clear(g);
			return;
		}
		mpz_mul(ppg, ppg, g);
		mpz_fdiv_q(pple, pple, g);
	}
}

// ### Adds cb{a,b} to the array.

// Algorithm 13.2 [PDF page 17](http://cr.yp.to/lineartime/dcba-20040404.pdf)
//
// See [appendcb test](test-appendcb.html) for basic usage.
void append_cb(mpz_array *out, const mpz_t a, const mpz_t b) {

	mpz_t r, g, h, c, c0, x, y, d, b1, b2, a1;
	unsigned long long n;

	/* gmp_printf("enter append_cb(%Zd, %Zd)\n", a, b); */

	// **Step 1**
	//
	// If `b == 1` and `a == 1` add `a` to the array. If `b == 1` return.
	// This is the break condition of the recursive function.
	if (mpz_cmp_ui(b, 1) == 0) {
		if (mpz_cmp_ui(a, 1) != 0)  {
			array_add(out, a);
		}
		return;
	}

	mpz_init(r);
	mpz_init(g);
	mpz_init(a1);

	// **Step 2**
	//
	// Store ppi in `a1` and ppo in `r`. Use `a1` and not `a` to keep the input `const`
	// and handle the case that one parameter is used twice.
	ppi_ppo(a1, r, a, b);

	// **Step 3**
	//
	// If `r` (ppo) is **not** one add it to the array.
	if (mpz_cmp_ui(r, 1) != 0) {
		array_add(out, r);
	}

	mpz_init(h);
	mpz_init(c);

	// **Step 4**
	//
	// Caluclate the gcd, ppg and pple.
	gcd_ppg_pple(g, h, c, a1, b);

	// **Step 5**
	//
	// Store pple in `c0` and `x`.
	mpz_init_set(c0, c);
	mpz_init_set(x, c0);

	// **Step 6**
	//
	// Set `n` to one.
	n = 1;

	mpz_init(b1);
	mpz_init(b2);
	mpz_init(d);
	mpz_init(y);

	// Start while loop to be able to return to step 7.
	while(1) {
		// **Step 7**
		//
		// Compute (g, h, c) ← (gcd,ppg,pple)(h,g^2)
		mpz_mul(b1, g, g);
		mpz_set(b2, h);
		gcd_ppg_pple(g, h, c, b2, b1);

		// **Step 8**
		//
		// Compute d ← gcd(c, b)
		mpz_gcd(d, c, b);

		// **Step 9**
		//
		// Set x ← xd
		mpz_mul(x, x, d);

		// **Sep 10**
		//
		// Compute y ← d^2^	n−1.
		mpz_set(y, d);
		two_power(y, n - 1);

		// **Sep 11**
		//
		// Recursively apply (c/y,d).
		mpz_fdiv_q(b1, c, y);

		/* gmp_printf("rec call append_cb(%Zd, %Zd)\n", b1, d); */

		append_cb(out, b1, d);

		// **Sep 12**
		//
		// If h is not 1: Set n ← n+1. Return to Step 7.
		if (mpz_cmp_ui(h, 1) == 0) break;
		n = n + 1;
	}

	// **Step 13**
	//
	// Recursively apply (b/x, c0).
	mpz_fdiv_q(b1, b, x);
	append_cb(out, b1, c0);

	// Free the memory.
	mpz_clear(r);
	mpz_clear(g);
	mpz_clear(h);
	mpz_clear(c);
	mpz_clear(c0);
	mpz_clear(x);
	mpz_clear(y);
	mpz_clear(d);
	mpz_clear(b1);
	mpz_clear(b2);
	mpz_clear(a1);
}


// ###Compute the product of an array.

// Compute the product of the values in `mpz_t array` and store it in `mpz_t rot`.
// `size_t from` is the start index and `size_t to` is the end index.
// 
// This function expects initialized mpz integers in all array fields between `from` and `to`.
//
// Algorithm 14.1 [PDF page 19](http://cr.yp.to/lineartime/dcba-20040404.pdf)
//
// See [prod test](test-prod.html) for basic usage.
void prod(mpz_t rot, mpz_t * array, size_t from, size_t to) {
	size_t n = to - from;
	mpz_t x, y;
	
	//  If #S = 1: Find a ∈ S. Print a. Stop.
	if (n == 0) {
		mpz_set(rot, array[from]);
		return;
	}
	
	// Select T ⊆ S with #T = b#S/2c.
	//
	// Compute X ← prod(T).
	mpz_init(x);
	prod(x, array, from, to - n/2 - 1);

	// Compute Y ← prod(S−T).
	mpz_init(y);
	prod(y, array, to - n/2, to);

	// Print XY.
	mpz_mul(rot, x, y);

	// Free the memory.
	mpz_clear(x);
	mpz_clear(y);
}

// #### array verison
// Compute product of an `mpz_array` and store it in `mpz_t rot`.
void array_prod(mpz_array *a, mpz_t rot) {
	if (a->used > 0)
		prod(rot, a->array, 0, a->used-1);
	else {
		mpz_set_ui(rot, 1);
	}
}


// ### fast algorithm to compute split(a,P).

// This function expects initialized mpz integers in all array fields between `from` and `to`.
//
// Algorithm 15.3 [PDF page 20](http://cr.yp.to/lineartime/dcba-20040404.pdf)
//
// See [split test](test-split.html) for basic usage.
void split(mpz_array *ret, const mpz_t a, mpz_t *p, size_t from, size_t to) {
	mpz_t b, x;
	size_t n = to - from;

	// **Sep 2**
	//
	//  Compute b ← ppi(a,prodP)
	mpz_init(x);
	mpz_init(b);
	prod(x, p, from, to);
	ppi(b, a, x);
	mpz_clear(x);

	// **Sep 2**
	//
	//  If #P = 1: find p ∈ P, print (p,b), and stop
	if (n == 0) {
		array_add(ret, b);
		mpz_clear(b);
		return;
	}

	// **Sep 3**
	//
	//  Select Q ⊆ P with #Q = b#P/2c.
	split(ret, b, p, from, to - n/2 - 1);
	split(ret, b, p, to - n/2, to);

	// Free the memory.
	mpz_clear(b);
}

// #### array verison
void array_split(mpz_array *ret, const mpz_t a, mpz_array *p) {
	if (p->used > 0)
		split(ret, a, p->array, 0, p->used-1);
	else
		fprintf(stderr, "array_split on empty array\n");
}


// ### Extending a coprime base

// This algorithm finds cb(P∪{b}) when P is coprime.
//
// Algorithm 16.2  [PDF page 21](http://cr.yp.to/lineartime/dcba-20040404.pdf)
//
// See [cbextend test](test-cbextend.html) for basic usage.
void cbextend(mpz_array *ret, mpz_array *p, const mpz_t b) {
	size_t i;
	mpz_t x, a, r;
	mpz_array s;

	// **Sep 1**
	//
	//  If P = {}: Print b if b != 1. Stop.
	if (!p->used) {
		if (mpz_cmp_ui(b, 1) != 0) {
			array_add(ret, b);
		}
	}

	// **Sep 2**
	//
	//  Compute x ← prod P
	mpz_init(x);
	array_prod(p, x);

	// **Sep 3**
	//
	//   Compute (a,r) ← (ppi,ppo)(b, x) b
	mpz_init(a);
	mpz_init(r);
	ppi_ppo(a, r, b, x);

	// **Sep 4**
	//
	//   Print r if r != 1.
	if (mpz_cmp_ui(r, 1) != 0) {
		array_add(ret, r);
	}

	// **Sep 5**
	//
	//   Compute S ← split(a,P)
	array_init(&s, p->used);
	array_split(&s, a, p);

	// **Sep 6**
	//
	//   For each (p, c) ∈ S: Apply append_cb(p, c).
	if (p->used != s.used) {
		fprintf(stderr, "logic error in cbextend: p.used != s.used");
	} else {
		for (i = 0; i < p->used; i++) {
			append_cb(ret, p->array[i], s.array[i]);
		}
	}

	// Free the memory.
	array_clear(&s);
	mpz_clear(a);
	mpz_clear(r);
	mpz_clear(x);
}


// #### bit test util

// see [PDF page 22](http://cr.yp.to/lineartime/dcba-20040404.pdf)
int bit(size_t i, size_t k) {
	if (k & (1 << i)) return 1;
	return 0;
}


// ### Merging coprime bases

// This algorithm finds cb(P ∪ Q) if P is coprime and Q is coprime
//
// Algorithm 17.3  [PDF page 23](http://cr.yp.to/lineartime/dcba-20040404.pdf)
//
// See [cbmerge test](test-cbmerge.html) for basic usage.
void cbmerge(mpz_array *s, mpz_array *p, mpz_array *q) {
	mpz_array t; // T
	mpz_array r; // buffer for q_k : bit_i k = 0 and q_k : bit_i k = 1
	size_t n = q->used;
	size_t b = 0;
	size_t i = 0;
	size_t k = 0;
	mpz_t x; // buffer
	mpz_init(x);

	// Find the smallest b ≥ 1 with 2^b.
	do {
		b++;
		mpz_ui_pow_ui(x, 2, b);
	} while(mpz_cmp_ui(x, n) < 0);

	// Set S ← P.
	array_copy(s, p);

	while(1) {
		// If i = b: Print S. Stop.
		if (i == b) {
			mpz_clear(x);
			return;
		}
		// Find R ← {qk : bit(k) = 1}
		array_init(&r, n);
		for(k=0; k<n; k++) {
			if (!bit(i,k)) array_add(&r, q->array[k]);
		}

		// Compute x ← prod{R}
		array_prod(&r, x);

		// Compute T ← cbextend(S ∪ {x})
		array_init(&t, s->size);
		cbextend(&t, s, x);

		// Find R ← {qk : bit(k) = 1}
		array_clear(&r);
		array_init(&r, n);
		for(k=0; k<n; k++) {
			if (bit(i,k)) array_add(&r, q->array[k]);
		}

		// Compute x ← prod{R}
		array_prod(&r, x);

		// Compute S ← cbextend(T ∪ {x})
		array_clear(s);
		array_init(s, s->size);
		cbextend(s, &t, x);

		// Free the memory.
		array_clear(&r);
		array_clear(&t);
		i++;
	}
}

// ### Computing a coprime base for a finite set

// This algorithm computes the natural coprime base for any finite subset of a free coid.
// It uses `cbmerge` to merge coprime bases for halves of the set.
//
// Algorithm 18.1 [PDF page 24](http://cr.yp.to/lineartime/dcba-20040404.pdf)
//
// See [cb test](test-cb.html) for basic usage.
void cb(mpz_array *ret, mpz_t *s, size_t from, size_t to) {
	size_t n = to - from;
	mpz_array p, q;

	// If #S = 1: Find a ∈ S. Print a if a != 1. Stop.
	if (n == 0) {
		if (mpz_cmp_ui(s[from], 0) == 0) {
			fprintf(stderr, "warning adding 0 in cb\n");
		} else {
			if (mpz_cmp_ui(s[from], 1) != 0) {
				array_add(ret, s[from]);
			}
		}
		return;
	}

// ## OpenMP multithreading
// Execute both recrusive `cb` calls in parallel.
//
// `export OMP_NUM_THREADS=4` to set the maximal thread number.
	array_init(&p, n);
	array_init(&q, n);
#if USE_OPENMP
#pragma omp parallel sections
{
 #pragma omp section
 {
	cb(&p, s, from, to - n/2 - 1);
 }
 #pragma omp section
 {
	cb(&q, s, to - n/2, to);
 }
}
#else
	cb(&p, s, from, to - n/2 - 1);
	cb(&q, s, to - n/2, to);
#endif
	// Print cbmerge(P∪Q)
	if (q.used && p.used) {
		cbmerge(ret, &p, &q);
	} else if(!q.used && p.used) {
		array_copy(ret, &p);
		fprintf(stderr, "warning: q is empty in cb\n");
	} else if(q.used && !p.used) {
		array_copy(ret, &q);
		fprintf(stderr, "warning: p is empty in cb\n");
	} else {
		fprintf(stderr, "warning: p an q are empty in cb\n");
	}

	// Free the memory.
	array_clear(&p);
	array_clear(&q);
}

// #### array verison
void array_cb(mpz_array *ret, mpz_array *s) {
	if (s->used > 0)
		cb(ret, s->array, 0, s->used-1);
	else
		fprintf(stderr, "array_cb on empty array\n");
}


// ### The reduce function

// Algorithm 19.2  [PDF page 24](http://cr.yp.to/lineartime/dcba-20040404.pdf)
//
// See [reduce test](test-reduce.html) for basic usage.
void reduce(mpz_t i, mpz_t pai, const mpz_t p, const mpz_t a) {
	mpz_t r, j, b, p2, a2;

	// **Sep 1**
	//
	//  If p does not divide a: Print (0,a) and stop.
	mpz_init(r);
	mpz_fdiv_r(r, a, p);
	if (mpz_cmp_ui(r, 0) != 0) {
		mpz_clear(r);
		mpz_set_ui(i, 0);
		mpz_set(pai, a);
		return;
	}

	// **Sep 2**
	//
	//  Compute (j,b) ← reduce(p^2 ,a/p)
	mpz_init(j);
	mpz_init(b);
	mpz_init(p2);
	mpz_init(a2);
	mpz_mul(p2, p, p);
	mpz_fdiv_q(a2, a, p);
	reduce(j, b, p2, a2);
	mpz_clear(p2);
	mpz_clear(a2);

	// **Sep 3**
	//
	//  If p divides b: Print (2 j +2,b/p) and stop.
	mpz_fdiv_r(r, b, p);
	if (mpz_cmp_ui(r, 0) == 0) {
		mpz_mul_ui(j, j, 2);
		mpz_add_ui(j, j, 2);
		mpz_set(i, j);

		mpz_fdiv_q(b, b, p);
		mpz_set(pai, b);

		mpz_clear(r);
		mpz_clear(b);
		mpz_clear(j);
		return;
	}
	mpz_clear(r);

	// **Sep 4**
	//
	//  Print (2 j +1,b).
	mpz_mul_ui(j, j, 2);
	mpz_add_ui(j, j, 1);
	mpz_set(i, j);
	mpz_set(pai, b);

	// Free the memory.
	mpz_clear(b);
	mpz_clear(j);
}

// ### Factoring over a coprime base

// This algorithm factors a as a product of powers of elements of P if possible; otherwise it
// proclaims failure.
//
// Algorithm 20.1  [PDF page 25](http://cr.yp.to/lineartime/dcba-20040404.pdf)
//
// See [findfactor test](test-findfactor.html) for basic usage.
int find_factor(mpz_array *out, const mpz_t a0, const mpz_t a, mpz_t *p, size_t from, size_t to) {
	mpz_t m, c, y, b, c2;
	size_t n = to - from;
	unsigned int r = 1;

	// If #P = 1: Find p ∈ P. Compute (n, c) ← reduce(p,a) by Algorithm 19.2. If
	// c != 1, proclaim failure and stop. Otherwise print (p,n) and stop
	if (n == 0) {
		mpz_init(m);
		mpz_init(c);
		reduce(m, c, p[from], a);
		if (mpz_cmp_ui(c, 1) != 0) {
			r = 0;
		} else {
			if (mpz_cmp(a0, p[from]) != 0) {
				mpz_init(y);
				mpz_fdiv_q(y, a0, p[from]);
				array_add(out, a0);
				array_add(out, p[from]);
				array_add(out, y);
				mpz_clear(y);
				r = 0;
			}
		}
		mpz_clear(m);
		mpz_clear(c);
		return r;
	}
	// Select Q ⊆ P with #Q = b#P/2c.

	// Compute y ← prod Q
	mpz_init(y);
	prod(y, p, from, to - n/2 - 1);

	// Compute (b, c) ← (ppi,ppo)(a, y)
	mpz_init(b);
	mpz_init(c2);
	ppi_ppo(b, c2, a, y);

	// Apply Algorithm 20.1 to (b,Q) recursively. If Algorithm 20.1 fails, proclaim
	// failure and stop.
	if (!find_factor(out, a0, b, p, from, to - n/2 - 1)) {
		r = 0;
	// Apply Algorithm 20.1 to (c,P−Q) recursively. If Algorithm 20.1 fails, proclaim
	// failure and stop.
	} else if (!find_factor(out, a0, c2, p, to - n/2, to)) {
		r = 0;
	}

	// Free the memory.
	mpz_clear(y);
	mpz_clear(b);
	mpz_clear(c2);
	return r;
}

// #### array verison
int array_find_factor(mpz_array *out, const mpz_t a, mpz_array *p) {
	if (p->used > 0)
		return find_factor(out, a, a, p->array, 0, p->used-1);
	else {
		fprintf(stderr, "array_printfactors on empty array\n");
		return 0;
	}
}


// ### Factoring a set over a coprime base

// This algorithm factors each element a ∈ S over P if P is a base for S; otherwise it proclaims failure.
//
// Algorithm 21.2  [PDF page 27](http://cr.yp.to/lineartime/dcba-20040404.pdf)
//
// See [findfactors test](test-findfactors.html) for basic usage.
void find_factors(mpz_array *out, mpz_t *s, size_t from, size_t to, mpz_array *p) {
	mpz_t x, y, z;
	mpz_array d, q;
	size_t i, n = to - from;

	mpz_init(x);
	array_prod(p, x);

	mpz_init(y);
	prod(y, s, from, to);

	mpz_init(z);
	ppi(z, x, y);

	array_init(&d, p->size);
	array_split(&d, z, p);

	array_init(&q, p->size);
	for (i = 0; i < p->used; i++) {
		if (mpz_cmp(d.array[i], p->array[i]) == 0)
			array_add(&q, p->array[i]);
	}

	if (n == 0) {
		array_find_factor(out, y, &q);
	} else {
		find_factors(out, s, from, to - n/2 - 1, &q);
		find_factors(out, s, to - n/2, to, &q);
	}

	mpz_clear(x);
	mpz_clear(y);
	mpz_clear(z);
	array_clear(&d);
	array_clear(&q);
}

// #### array verison
void array_find_factors(mpz_array *out, mpz_array *s, mpz_array *p) {
	if (s->used > 0)
		find_factors(out, s->array, 0, s->used-1, p);
	else
		fprintf(stderr, "array_printfactors_set on empty array\n");
}
