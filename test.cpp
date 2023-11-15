# include <bits/stdc++.h>
using namespace std;
int b = 3;
void fun(){
	static int a = b;
	cout << a << '\n';
}
int main(){
	fun();
	b++;
	fun();
	b++;
	fun();
	return 0;
}