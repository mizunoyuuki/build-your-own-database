//## Cにはジェネレータがない
//# というのは実は、「Cにはコールスタックをヒープに逃す言語機能がない」という話。
//#
//# 用語の整理
//# ・イテレータ(iterator)	「次の要素をくれ」と聞けるオブジェクト
//# ・ジェネレータ(generator)	「イテレータを作るための言語機能」関数を書くだけでイテレータが手に入る。
//# ・yield: ジェネレータの中で「1個返して、ここで一旦止まる」ためのキーワード
//#
//# yieldの本質: 関数が「途中で止まる」
//# 普通の関数はreturnしたら終わり。
//
//# def count():
//# 	i = 0
//# 	while True:
//# 		i += 1
//# 		yield i  # 1を返してここで止まる。
//#
//# g = count()
//# next(g) # 1
//# next(g) # 2
//# next(g) # 3
//#
//# なぜこんなことができるのか。関数のフレーム(ローカル変数・実行位置)がどこに置かれてるのかが違うから。
//#
//# 外部イテレータ と 内部イテレータ
//#
//# ループを回すのは誰か。という違い
//#
//#
//###################### rubyで書くと ##########################
//#
//### 内部イテレータ
//# 
//# [1, 2, 3].each do |x|
//# 	puts x
//# end
//#
//# class Array
//# 	def each
//# 		i = 0
//# 		while i < sie
//# 			yield self[i]  // 利用側のブロックを呼ぶ
//# 			i += 1
//# 		end
//# 	end
//# end
//#
//### 外部イテレータ
//#
//# it = [1, 2, 3].each
//#
//# loop do
//# 	puts it.next
//# end
//#
//# 決定的な違い: プログラムカウンタを誰が持つか。
//# ここが本質: 「今どこまで進んだか」という情報がどこで持っているかを考えよう。
//#
//# 内部イテレータの場合、「今3番目まで処理した」という情報は、eachメソッドのスタックフレームにあるローカル変数iと、CPUのプログラムカウンタが持ってる。
//#
//# 外部イテレータの場合、eachに相当する処理は毎回、next()で抜けてしまうので、スタック不yレームから消えてしまう。だから「今3番目」という
//#
//# 内部/外部イテレータは「制御の所有権」の問題で、その正体は継続である。
//# 
//# 内部 / 外部イテレータの違いを、最も高い視点から一言でいうと、「制御フローを誰が所有するか」です。
//# 内部イテレータは制御の逆転　の最小例。利用者は「呼ぶ側」から「呼ばれる側」に降格する。
//# Hollywood Principle(Dont call us, we'll call you)そのもので、GUIのイベントループ、Webフレームワークのハンドラ、DIコンテナ、コールバック地獄、非同期のawait 
//

//
//
// for文に直書き 

// これでいいやん。しかも完璧に遅延している。
// では何が問題なのか、「フィボナッチを生成する」というロジックが、このmainの中から一歩も外に出せない。別の場所で「フィボナッチの偶数だけ合計したい」となったらコピペするしかない。
//

int main(void) {
	unsigned long long a = 0, b = 1;
	while(a <= 1000) {
		unsigned long long next = a + b;
		a = b;
		b = next;
	}

	printf("%llu\n", a);
	return 0;
}


// Cで手書きイテレータ
# include<stdio.h>

typedef struct {
	unsigned long long a;
	unsigned long long b;
} FibIter;

void fib_init(FibIter* it){
	it->a = 0;
	it->b = 1;
}

unsigned long long fib_next(FibIter *it){
	unsigned long long cur = it->a;
	unsigned long long next = it->a + it->b;
	it->a = it->b;
	it->b = next;
	return cur;
}

// 使う側

int main(void){
	FibIter it;
	fib_init(&it);

	unsigned long long x;
	while ((x = fib_next(&it)) <= 1000) {
		/* 何か処理*/
	}

	printf("%llu\n", x);

	return 0;
}


// yieldが裏でやっていること
// 「ローカル変数をstructに外だしし、中断位置を状態として持つ構造体」を、コンパイラが自動生成してくれる機能
//

typedef struct Iterator Iterator;
struct Iterator {
	void* state;
	bool (*next)(void* state, void* out);
};

void print_all(Iterator *it){
	unsigned long long x;
	while(it->next(it->state, &x)) printf("%llu\n", x);
}
