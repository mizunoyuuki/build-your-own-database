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
