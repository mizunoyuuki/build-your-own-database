# include<stdio.h>
# include<string.h>

int main(){
	char *str = "hello world i am Mizuno";

	char *token;

	token = strtok(str, " ");

	while(token != NULL){
		printf("[%s]\n", token);
		token = strtok(str, " ");
	}

	return 0;
}
