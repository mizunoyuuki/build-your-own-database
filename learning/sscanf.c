// scanfの理解

# include<stdio.h>

int main(){
	char *str = "insert 12 name";

	int num;
	char *column;
	char *name;
	int arr_assigned = sscanf(
			str,
			"insert %d %s %s",
			&num,
			column,
			name
			);
	//printf("%s\n",name);
	printf("%d\n", num);
	printf("arr_assigned: %d\n", arr_assigned);

	return 0;
}

///// 一番言われてる欠点
//
// %sのバッファオーバーフロー
//
// 一番よく言われるやつ
//
// char buf[10];
// scanf("%s", buf);
// %sは空白まで読むが、長さ制限がないので、gets() と同じ危険性を持つ。
// 回避策は、幅を明示すること
//
// scanf("%9s", buf); // NULL分を残して最大9文字
//
// 
/////// 欠陥2: 変換失敗時の挙動
//
// 実は%sより根が深いのがこっち。数値変換が失敗した時、問題の文字列をストリームに残したまま帰ってくる。
// 
// int a, b;
//
// scanf("%d", &a); // ユーザが"hello"と入力
// scanf("%d", &b); // 'h'がまだバッファに残っているので即失敗
//
//
//
