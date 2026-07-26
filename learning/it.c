// インターフェースを決める

# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>

typedef struct Iterator Iterator;

struct Iterator {
	void *state;					// 各イテレータ固有のデータ
	bool (*next)(Iterator *self, long long *out);	// 次を一つ。なければfalse
};

typedef struct { long long a, b; } FibState;

static bool fib_next(Iterator *self, long long *out){
	FibState *s = self->state;
	long long next = s->a + s->b;
	s->a = s->b;
	s->b = next;

	*out = next;

	return true;
}

Iterator *fib_iter(FibState *s){
	s->a = 0; s->b = 1;
	Iterator *it = malloc(sizeof(Iterator));
	it->state = s;
	it->next = fib_next;

	return it;
}

int main(void) {
	FibState *f = malloc(sizeof(FibState));
	Iterator *it = fib_iter(f);
	long long out;

	for (int i = 0; i < 10; i++){
		it->next(it, &out);
	}

	printf("%lld\n", out);

	return 0;
}
