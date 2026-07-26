/* ============================================================================
 *  iterator-cheat.c  —  C で外部イテレータ + アダプタを手書きする（yield の中身）
 *
 *  コンパイル: gcc -Wall -O2 learning/iterator-cheat.c && ./a.out
 *
 *  【全体像】
 *    ソース      : 値を生み出す              (fib, range)
 *    アダプタ    : Iterator -> Iterator      (filter, map, take)  ← 何段でも積める
 *    コンシューマ: Iterator -> 値            (print_all, sum_all, find_first)
 *
 *    print_all ──next()──▶ take ──next()──▶ filter ──next()──▶ fib
 *      (消費)              (5個で打切)      (偶数だけ通す)      (無限生成)
 *
 *    呼び出しは「下流 → 上流」へ潜り、値はその return で「上流 → 下流」へ戻る。
 *    これが pull 型ストリーム。だから take(3) は上流を 3 回しか引かない。
 *
 *  【要点メモ】
 *   1. Iterator = 「状態(state)」+「次を1個くれる関数(next)」だけ。
 *      get / advance / is_end を 1 関数に畳んだ形
 *      （out 引数で値を返し、戻り値 bool で継続可否を返す）。
 *   2. アダプタを呼んだ時点では計算ゼロ。パイプラインを組み立てているだけ。
 *      コンシューマが next() を叩いた瞬間に初めて計算が走る（遅延評価）。
 *   3. yield の正体 = ローカル変数を struct に外出しし、
 *      中断位置(次に実行すべき処理)を関数ポインタとして値にしたもの。
 *      Python/Ruby/C#/Rust はこの変換をコンパイラが自動生成してくれるだけ。
 *
 *  【ハマりどころ】
 *   A. s->src.next(&s->src, &x) の & はなぜ必要か
 *        FilterState は Iterator を「ポインタでなく実体」で持っている。
 *          s          : FilterState*
 *          s->src     : Iterator      (実体)
 *          &s->src    : Iterator*     ← next() が要求する型
 *        ルール:  `.` で呼ぶなら & が必要 /  `->` で呼ぶなら不要
 *          a.next(&a, &x)     ← a は実体
 *          p->next(p,  &x)    ← p は既にポインタ
 *        なお &s->src は &(s->src) の意味（-> は & より優先順位が高い）。
 *
 *   B. next が self を受け取るのは必須
 *        next() は「呼ぶたび自分が変化する」破壊的操作。値コピーを渡すと
 *        変更が捨てられ、同じ値を返し続ける。C++ の this、Python の self、
 *        Rust の &mut self がすべて同じ理由で存在する。
 *
 *   C. アダプタは src を「値コピー」で取り込む (s->src = src)
 *        取り込んだ後に元の Iterator 変数を触ると、状態が二重管理になり
 *        別々に進む 2 本のイテレータになる。包んだら元の変数は触らない。
 *        値で持つ利点は所有関係が木になり寿命の心配が消えること
 *        （Rust の Take<Map<Filter<..>>> と同じ戦略）。
 * ========================================================================== */

# include <stdio.h>
# include <stdbool.h>

/* ---------- 1. インターフェース ------------------------------------------- */
typedef struct Iterator Iterator;

struct Iterator {
	void* state;					/* 各イテレータ固有のデータ   */
	bool (*next)(Iterator* self, long long* out);	/* 次を1つ。なければ false */
};

/* 遅延が効いていることを可視化するカウンタ */
static long fib_calls = 0;

/* ---------- 2. ソース: 無限フィボナッチ ----------------------------------- */
typedef struct {
	long long a;
	long long b;
} FibState;

static bool fib_next(Iterator* self, long long* out) {
	FibState* s = self->state;
	fib_calls++;

	*out = s->a;
	long long n = s->a + s->b;
	s->a = s->b;
	s->b = n;
	return true;					/* 無限列なので終わらない */
}

Iterator fib_iter(FibState* s) {
	s->a = 0;
	s->b = 1;
	return (Iterator) { .state = s, .next = fib_next };
}

/* ---------- 3. ソース: 有限レンジ [begin, end) ---------------------------- */
typedef struct {
	long long cur;
	long long end;
} RangeState;

static bool range_next(Iterator* self, long long* out) {
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

/* ==========================================================================
 *  4. アダプタ (adapter)  —  Iterator を受け取り Iterator を返す
 * ======================================================================== */

/* filter: 条件を満たすものだけ通す */
typedef struct {
	Iterator src;
	bool (*pred)(long long);
} FilterState;

static bool filter_next(Iterator* self, long long* out) {
	FilterState* s = self->state;
	long long x;

	while (s->src.next(&s->src, &x)) {		/* 通るまで上流を引く */
		if (s->pred(x)) { *out = x; return true; }
	}
	return false;
}

Iterator filter_iter(FilterState* s, Iterator src, bool (*pred)(long long)) {
	s->src  = src;
	s->pred = pred;
	return (Iterator) { .state = s, .next = filter_next };
}

/* map: 各要素を変換する */
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

Iterator map_iter(MapState* s, Iterator src, long long (*fn)(long long)) {
	s->src = src;
	s->fn  = fn;
	return (Iterator) { .state = s, .next = map_next };
}

/* take: 先頭 n 個だけ。無限列を有限に切る「ハサミ」 */
typedef struct {
	Iterator src;
	int remaining;
} TakeState;

static bool take_next(Iterator* self, long long* out) {
	TakeState* s = self->state;
	if (s->remaining <= 0) return false;
	if (!s->src.next(&s->src, out)) return false;
	s->remaining--;
	return true;
}

Iterator take_iter(TakeState* s, Iterator src, int n) {
	s->src = src;
	s->remaining = n;
	return (Iterator) { .state = s, .next = take_next };
}

/* ---------- 5. コンシューマ (ここで初めて計算が走る) ---------------------- */
void print_all(Iterator* it) {
	long long x;
	while (it->next(it, &x)) printf("%lld ", x);
	printf("\n");
}

long long sum_all(Iterator* it) {
	long long x, acc = 0;
	while (it->next(it, &x)) acc += x;
	return acc;
}

/* 「何個必要か事前にわからない」の典型。無限列 + 早期終了 */
bool find_first(Iterator* it, bool (*pred)(long long), long long* out) {
	long long x;
	while (it->next(it, &x)) {
		if (pred(x)) { *out = x; return true; }
	}
	return false;
}

/* ---------- 6. 述語 / 変換 ------------------------------------------------ */
static bool      is_even(long long x)   { return x % 2 == 0; }
static bool      over_1000(long long x) { return x > 1000; }
static long long square(long long x)    { return x * x; }

/* ---------- 7. デモ ------------------------------------------------------- */
/*  期待出力:
 *    even fib x5   : 0 2 8 34 144
 *    sum of squares: 385
 *    first > 1000  : 1597 (fib_next calls = 18)
 *    take(3)       : fib_next calls = 3
 */
int main(void) {
	/* (1) 無限fib -> 偶数だけ -> 先頭5個 */
	{
		FibState    fs;  Iterator a0 = fib_iter(&fs);
		FilterState ffs; Iterator a1 = filter_iter(&ffs, a0, is_even);
		TakeState   ts;  Iterator a2 = take_iter(&ts, a1, 5);
		printf("even fib x5   : ");
		print_all(&a2);
	}

	/* (2) range(1,11) -> 二乗 -> 合計 (= 385) */
	{
		RangeState rs;  Iterator b0 = range_iter(&rs, 1, 11);
		MapState   ms;  Iterator b1 = map_iter(&ms, b0, square);
		printf("sum of squares: %lld\n", sum_all(&b1));
	}

	/* (3) 無限fib から最初に1000を超えるもの。
	 *     リストを1個も作らず、必要な回数だけ計算して止まる。 */
	{
		fib_calls = 0;
		FibState fs; Iterator c0 = fib_iter(&fs);
		long long v;
		if (find_first(&c0, over_1000, &v))
			printf("first > 1000  : %lld (fib_next calls = %ld)\n", v, fib_calls);
	}

	/* (4) 遅延の証拠: take(3) は上流を3回しか引かない */
	{
		fib_calls = 0;
		FibState  fs; Iterator d0 = fib_iter(&fs);
		TakeState ts; Iterator d1 = take_iter(&ts, d0, 3);
		long long x;
		while (d1.next(&d1, &x)) { /* 値は捨てる。呼ばれた回数だけ見たい */ }
		printf("take(3)       : fib_next calls = %ld\n", fib_calls);
	}

	return 0;
}

/* ==========================================================================
 *  【この設計が実世界のどこに出てくるか】
 *
 *  ・Linux カーネル  struct file_operations
 *      read/write/llseek の関数ポインタ表。ext4 も procfs も /dev/null も
 *      同じインターフェースで扱える = 「すべてはファイルである」の実装。
 *
 *  ・C++ の vtable
 *      仮想関数呼び出し obj->f() は obj->vptr[i](obj) に展開される。
 *      今回は関数ポインタを構造体に直接埋めた fat pointer 方式。
 *      Rust の dyn Trait / Go の interface 値は (data, vtable) の 2 ワード。
 *
 *  ・RDBMS の Volcano / Iterator モデル (Graefe, 1994)
 *      全演算子が open()/next()/close() を実装し、木になる。
 *      SELECT ... WHERE ... LIMIT 5 は Limit -> Filter -> SeqScan の順に
 *      next() を呼ぶので、テーブル全体を読まずに済む。
 *      take_iter がまさに Limit 演算子の最小実装。
 *      ※ 現代の分析系DB (DuckDB/ClickHouse) は 1 行ずつの関数呼び出しコストを
 *        避けるため、1 回の next で ~1024 行返すベクトル化実行に移行。
 *        骨格は同じ pull 型で、粒度を行からバッチに変えただけ。
 *
 *  【Pull 型 と Push 型 は圏論的に双対】
 *      Pull (Iterator) : 受け手が next() を呼ぶ。主導権 = 消費者。例: for
 *      Push (Observer) : 送り手が onNext() を呼ぶ。主導権 = 生産者。例: イベント
 *      Iterable/Iterator の矢印を全部逆向きにすると Observable/Observer になる。
 *      Push 型は止めるのに購読解除が要り、backpressure 問題が生まれる。
 *      Reactive Streams の request(n) は push に pull を後付けした解。
 * ======================================================================== */
