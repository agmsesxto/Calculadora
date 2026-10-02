#include <iostream>
using namespace std;

int main() {
    // Declaración de variables
    float n1, n2, n3;
    int op;

    cout << "Introduce el primer número: ";
    cin >> n1;
    cout << '\n' << "Introduce el segundo número: ";
    cin >> n2;
    cout << '\n' << "Introduce 1 para sumar, 2 para restar, 3 para multiplicar y 4 para dividir: ";
    cin >> op;

    if(op == 1) {
        cout << '\n' << n1 + n2;
    } else if(op == 2) {
        cout << '\n' << n1 - n2;
    } else if(op == 3) {
        cout << '\n' << n1 * n2;
    } else if(op == 4) {
        cout << '\n' << n1 / n2;
    }

    return 0;
}