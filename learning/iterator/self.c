// フィボナッチ数列から、偶数のものを、10個最初から取得する。
//
# include<stdio.h>
# include<stdlib.h>
# include<stdbool.h>

typedef struct Iterator Iterator;
struct Iterator {
	bool (*next)(Iterator *self, long long *out);
	void (*destroy)(Iterator *self);
};

typedef struct {
	Iterator base;
	long long a, b;
} FibIter;

static bool fib_next(Iterator *self, long long *out){
	FibIter *s = (FibIter *)self;
	*out = s->a;
	long long n = s->a + s->b;
	s->a = s->b;
	s->b = n;
	return true;
}

static void fib_destroy(Iterator *self) { free(self) };

void *fib_init(FibIter *s){
	s->base = fib_next;
	s->a = 0;
	s->b = 1;
}

typedef struct {
	Iterator basic;
	Iterator *src;
	bool (*pred)(long long);
} FilterIter;



int main(void){
	Iterator *it = take(filter(fib(), is_even), 10);

	// itはイテレータとして構造体を構築したので順番に実行する
	while(it->next(it)){
	}

}
