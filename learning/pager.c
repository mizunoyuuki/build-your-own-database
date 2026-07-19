//「ページャ」という抽象化が何を隠すのか。
//抽象化(abstruction)というのは、要するに呼ぶ側に面倒を見せない壁のこと。\
//上位のコード(Table)がやりたいのは「page x の中身が欲しい」だけ。
//そのページが今どこにあるのか。メモリにもう乗っているのか。まだディスクの上なのか。
//なんて知りたくない。
//そこでページャが間に立って、この２択を九州する。
//呼ぶ側から見た唯一のインターフェースは：
//```void *get_page(Pager *pager, uint32_t page_num){}```
//

// open -「ファイルとの接続」を作り、整理番号をもらう。
// O_RDONLY / O_WRONLY / O_RDWR + O_CREAT(なければ作る)、O_TRUNC(開いた時に中身を空にする)、O_APPEND(常に末尾に書く)
//
// write - バイト列をオフセット位置に書き、オフセットを進める。
// ssize_t n = wite(fd, buf, 100);
// fdの現在オフセット位置に書け。書けたバイト数を返す。
// 書いた分だけオフセットが自動で進む。
// ただ、成功して帰ってきても、まだディスクに届いたとは限らない。カーネルのページキャッシュに乗っただけで、確実に永続化させるには、fsync(fd)が必要。
// 
//
//
// lseek - カーネル内の「現在位置」を動かす/調べる
//
// off_t pos = lseek(fd, 4096, SEEK_SET);
//
// fdには、「次にread/writeする位置」という状態をカーネル内では持ってる。
// lseekはそれを動かし、移動後の絶対位置を返す。
// 基準は3種類
// SEEK_SET(先頭から)、SEEK_CUR(今の位置から相対)、SEEK_END(末尾から)。
// ここから二つの定番イディオムがある。
// lseek(fd, 0, SEEK_END) -> 末尾に移動。戻り値がファイルサイズ。
// lseek(fd, page_num * PAGE_SIZE, SEEK_SET); -> ページ番号指定でランダムアクセス
//

# include<stdio.h>
# include<string.h>

# include<fcntl.h>
# include<unistd.h>
# include<errno.h>

int main(int argc, char **argv){
	char *filename = argv[1];
	int fd = open(filename, O_RDWR | O_CREAT, 0644);

	printf("fd: %d\n",fd);

	char *write_char = " HELLO World!";

	off_t offset = lseek(fd, 0, SEEK_END);

	printf("offset: %ld\n", offset);

	ssize_t write_result = write(fd, write_char, 14);

	printf("%ld\n", write_result);

	int result = close(fd);

	printf("close result: %d\n", result);

	return 0;
}


