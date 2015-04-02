#include <stdio.h>
#include <stdlib.h>
 
struct Item
{
	int m_b;
	int m_ak;
};

int a(int m, int n){
	struct Item array[m];
	int i;
	int result;
 
	for(i=0; i<m; ++i) {
		array[i+1].m_b = -1;
		array[i+1].m_ak = 1;
	}
 
	array[0].m_b = -1;
	array[0].m_ak = 0;
 
	while(array[m].m_b != n) {
		array[0].m_b++;
		array[0].m_ak++;
		for(i=0; (i != m) && (array[i].m_b == array[i+1].m_ak); ++i) {
			array[i+1].m_b++;
			array[i+1].m_ak = array[i].m_ak;
		}
	}
	result = array[m].m_ak;
	return result;
}

int main(int argc, char **argv)
{
	printf("%d\n", a(atoi(argv[1]),atoi(argv[2])));
	return 0;
}
