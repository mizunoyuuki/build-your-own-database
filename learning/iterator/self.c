#include<stdio.h>
#include<stdbool.h>

typedef struct Iterator Iterator;
struct Iterator {
	void* state;
	bool (*next)(Iterator *self, long long* out);
};

/* フィボナッチ  */

typedef struct {
	long long a;
	long long b;
} FibState;

static bool fib_next(Iterator* self, long long* out) {
	FibState* s = self->state;
	*out = s->a;

	long long n = s->a + s->b;
	s->a = s->b;
	s->b = n;

	return true;
}
Iterator fib_iter(FibState* s) {
	s->a = 0;
	s->b = 1;

	return (Iterator){ .state = s, .next = fib_next };
}

/* filter */
typedef struct {
	Iterator src;
	bool (*pred)(long long);
} FilterState;
static bool filter_next(Iterator* self, long long* out){
	FilterState* s = self->state;
	long long x;
	while (s->src.next(&s->src, &x)) {
		if (s->pred(x)){
			*out = x;
			return true;
		}
	}
	return false;
}

Iterator filter_iter(FilterState* s, Iterator src, bool(*pred)(long long)) {
	s->src = src;
	s->pred = pred;
	return (Iterator) { .state = s, .next = filter_next };
}

/* map */
typedef struct {
	Iterator src;
	long long (*fn)(long long);
} MapState;
static bool map_next(Iterator* self, long long* out) {
	MapState* s = self->state;
	long long x;
	if (!s->src.next(&s->src, &x)) return false;

	*out = s->fn(x);
	return true;
}
Iterator map_iter(MapState* s, )
