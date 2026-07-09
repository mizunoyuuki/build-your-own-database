#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
	char *name = "mizuno";
	char *name_space = malloc(7 * sizeof(char));
	memcpy(name_space, name, 7);

	printf("%s\n",name_space);

	return 0;
}

// man memcpy
//
// manページを見ると
// void *memcpy(void dest[restrict n], const void src[restrict n], size_t n);
//
// C言語のGCC拡張のパラメータ前方宣言
//
// void *memcpy(size_t n; void dest[restrict n], const void src[restrict n], size_t n);
//
// 今回の表記変更の裏には、コンピュータサイエンスの大きなテーマがある。
// 関数の「契約」をどこまで機械検出可能にするか。memcpy(dest, src. n)の正しい仕様には、「destはnバイト書き込み可能」、「srcはnbaいと読み取り可能」「両者は重ならない」という三つの事前条件(protection)がありますが、伝統的なCの型void*, void*, size_tは、これを一歳表現できない。
// 守るのはプログラマの責任で、破れば未定義動作　これがCのバッファオーバーフロー脆弱性が40年間絶えない根本原因
// manの新表記、dest[restrict n]は、この暗黙の契約を構文の上に浮かび上がらせる仕組み。この方向wクォ推し薄めた言語が、Rustで、&mut[u8]はスライス(ポインタ+ 長さの組)であり、restrict相当も型検査で強制できる。
// さらに先には、依存型を持つ言語があり、
//
//
