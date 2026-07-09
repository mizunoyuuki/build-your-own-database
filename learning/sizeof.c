# include<stdio.h>

typedef struct Row Row;
struct Row {
	char *id;
	char a;
	char *name;
	char *email;
};

int main(){
	Row *row;
	printf("sizeof(row->id), %ld\n", sizeof(row->id));
	printf("sizeof(row->a), %ld\n", sizeof(row->a));
	printf("sizeof(row->name): %ld\n", sizeof(row->name));
	printf("sizeof(row->email): %ld\n", sizeof(row->email));
	printf("sizeof(row): %ld\n", sizeof(Row));

	return 0;
}
