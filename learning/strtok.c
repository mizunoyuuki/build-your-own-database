// 「空白で区切ってトークン列に変換する」の作業はコンパイラ理論で言う字句解析"lexing/tokenizer"の最小系
// あらゆる言語処理系/ GCCmおPythonも、そして本物のSQLiteも「文字列->トークン列->構文木->実行」と言うパイプラインで動く
// sscanfは字句解析と構文解析と値の変換を一行に混ぜていたのに対し、strtokは「トークン(strtok)-> 検証(NULLチェック・strlen)->  変換(atoi) -> 格納(strcpy)と段階を分ける
// この分離こそが本質的な進捗で、各段階が独立に検査可能になったからこそ、「長さ検査」を挿入できるようになった。



// ## sscanf と strtok　とは

//sscanf => 書式付き入力変換関数(stdio系)
//strtok => 文字列分離関数(string系)

# include<stdio.h>
# include<string.h>

int main(){
	char str[] = "insert 1 user1 foo@gmail.com";

	char *token = strtok(str, " ");
	while(token != NULL){
		printf("[%s]\n", token);
		printf("%s\n", str
		token = strtok(NULL, " ");
	}

	return 0;
}

// strtokは元の文字列を破壊する
//
// mallocは一切しない、なのでメモリ管理も不要だけど、元の文字列はもう使えなくなる。
// これが「破壊的(destructive)」と言われる理由
//
// 文字リテラルに対しては使えない。
//
// 文字リテラルに対しては使えない。
//
// rodataは書き換えできないから。PageTabelで書き込みが禁止された領域にマッピングされてる。
//
// ちなみに、str[]であれば、配列なので文字列リテラルを代入すると配列としてコピーされたデータが作成されるが、
// char *str = "hello"のように書くと、直接文字列リテラルを持つため、書き込み不可でSegment faultになる
//
//
// strtokは内部にstaticな状態を持っているため、「2回目以降にNULLを渡すと続きから読める」
//
// これが原因でスレッドセーフではなくなる。
// static変数をプロセス内で共有するので、マルチスレッドで使うと競合する。
//
// これは、「strtok」が呼び出し側の状態をライブラリ内部のstatic変数1個に保存している」ことが原因
//
// 内部実装のイメージ
//
// glibcの実装を単純化するとこうなってる。
//
// char *strtok(char *str, const char *delim){
// 	static char *saved; // プロセスで一つだけ
//
// 	if (str == NULL) 
// 		str == saved;
// }
