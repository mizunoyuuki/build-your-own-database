typedef struct InputBuffer InputBuffer;
struct InputBuffer {
	char* buffer;
	size_t buffer_length;
	ssize_t input_length;
};

InputBuffer* new_input_buffer() {
	InputBuffer* input_buffer = (InputBuffer*) malloc(sizeof(InputBuffer));
	input_buffer->buffer = NULL;
	input_buffer->buffer_length = 0;
	input_buffer->input_length = 0;

	return input_buffer;
}

void print_prompt() { printf("db > "); }

int main(int argc, char *argv[]){
	InputBuffer* input_buffer = new_input_buffer();
	while(true){
		print_prompt();
		read_input();
		
		if(strcmp(input_buffer->bufer, ".exit") == 0){
			close_input_buffer(input_buffer);
			exit(EXIT_SUCCESS);
		} else {
			printf("Unrecognized command '%s'. \n", input_buffer->buffer);
		}
	}
}
