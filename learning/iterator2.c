# include <stdio.h>
# include <stdbool.h>

typedef struct Iterator Iterator;

struct Iterator {
	void* state;
	bool (*next)(Iterator *self, long long *out);
};

// 遅延が効いていることを可視化するカウンタ
static long fib_calls = 0;

typedef struct {
	long long a;
	long long b;
} FibState;

static bool fib_next(Iterator* self, long long *out) {
	FibState *s = self->state;
	fib_calls++;

	*out = s->a;
	long long n = s->a + s->b;
	s->a = s->b;
	s->b = n;
	return true;
}

Iterator fib_iter(FibState* s) {
	s->a = 0;
	s->b = 1;
	return (Iterator) { .state = s, .next = fib_next };
}

/* 有限レンジ [begin, end) */
typedef struct {
	long long cur;
	long long end;
} RangeState;

static bool range_next(Iterator* self, long long* out){
	RangeState* s = self->state;
	if (s->cur >= s->end) return false;
	*out = s->cur++;
	return true;
}

Iterator range_iter(RangeState* s, long long begin, long long end) {
	s->cur = begin;
	s->end = end;
	return (Iterator) { .state = s, .next = range_next };
}

/******************************************************************
 * アダプタ(adapter)
 * 	Iteratorを受け取り、Iteratorを返す。だから何段でも詰める
 **************************************************************** */

/* filter: 条件を満たすものだけを通す */
typedef struct {
	Iterator src;
	bool (*pred)(long long);
} FilterState;

static bool filter_next(Iterator* self, long long* out){
	FilterState* s = self->state;
	long long x;

	while(s->src.next(&s->src, &x)) {
		if (s->pred(x)) { *out = x; return true; }
	}

	return false;
}

Iterator filter_iter(FilterState *s, Iterator src, bool (*pred)(long long)) {
	s->src = src;
	s->pred = pred;
	return (Iterator) { .state = s, .next = filter_next };
}

/* map: 各要素を変換する */

typedef struct {
	Iterator src;
	long long (*fn)(long long);
} MapState;

static bool map_next(Iterator* self, long long *out) {
	MapState *s = self->state;
	long long x;

	if (!s->src.next(&s->src, &x)) return false;
	*out = s->fn(x);
	return true;
}

Iterator map_iter(MapState* s, Iterator src, long long (*fn)(long long)) {
	s->src = src;
	s->fn = fn;
	return (Iterator) { .state = s, .next = map_next };
}


/* take: 先頭n個だけ */

typedef struct {
	Iterator src;
	int remaining;
} TakeState;

static bool take_next(Iterator *self, long long *out){
	TakeState *s = self->state;
	if (s->remaining <= 0) return false;
	if (!s->src.next(&s->src, out)) return false;
	s->remaining--;

	return true;
}

Iterator take_iter(TakeState *s, Iterator src, int n) {
	s->src = src;
	s->remaining = n;
	return (Iterator) { .state = s, .next = take_next };
}

void print_all(Iterator *it) {
	long long x;
	while(it->next(it, &x)) printf("%lld ", x);
	printf("\n");
}

long long sum_all(Iterator *it) {
	long long x, acc = 0;
	while (it->next(it, &x)) acc += x;
	return acc;
}

static bool is_even(long long x) { return x % 2 == 0; }
static long long square(long long x) { return x * x; }

int main(void) {
	FibState fs;  	Iterator a0 = fib_iter(&fs);
	FilterState ffs;Iterator a1 = filter_iter(&ffs, a0, is_even);
	TakeState ts;	Iterator a2 = take_iter(&ts, a1, 5);
	print_all(&a2);


}
