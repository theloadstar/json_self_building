#include <stdio.h>

void changenum(int a){
	a = 1;
	return;
}

int main(){
	int a = 0;
	changenum(a);
	printf("%d\n",a);
	return 0;
}