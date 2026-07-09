const int gloval_int_const = 10;
int gloval_int_data = 10;
int gloval_int_bss;
static int gloval_static_int = 10;

void func(){
	const int function_const_int = 10;
	static const int function_static_const_int = 10;

	return;
}

int main(){
	const int local_const_int = 10;
	static const int local_static_const_int = 10;
}
