#include <iostream>
#include <limits>

using namespace std;

int main() {
    double num1, num2, resultado;
    char operacion;
    char continuar;

    cout << "===============================" << endl;
    cout << "   Calculadora sencilla en C++" << endl;
    cout << "===============================" << endl;

    do {
        cout << "\nIngresa el primer numero: ";
        cin >> num1;

        cout << "Ingresa el segundo numero: ";
        cin >> num2;

        cout << "\nElige una operacion:" << endl;
        cout << "[+] Suma" << endl;
        cout << "[-] Resta" << endl;
        cout << "[*] Multiplicacion" << endl;
        cout << "[/] Division" << endl;
        cout << "Opcion: ";
        cin >> operacion;

        switch (operacion) {
            case '+':
                resultado = num1 + num2;
                cout << "\nResultado: " << num1 << " + " << num2 << " = " << resultado << endl;
                break;

            case '-':
                resultado = num1 - num2;
                cout << "\nResultado: " << num1 << " - " << num2 << " = " << resultado << endl;
                break;

            case '*':
                resultado = num1 * num2;
                cout << "\nResultado: " << num1 << " * " << num2 << " = " << resultado << endl;
                break;

            case '/':
                if (num2 != 0) {
                    resultado = num1 / num2;
                    cout << "\nResultado: " << num1 << " / " << num2 << " = " << resultado << endl;
                } else {
                    cout << "\nError: No se puede dividir entre cero." << endl;
                }
                break;

            default:
                cout << "\nError: Operacion no valida." << endl;
                break;
        }

        cout << "\n¿Deseas realizar otra operacion? (s/n): ";
        cin >> continuar;

        // Limpia el buffer por si el usuario introduce algo inesperado
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

    } while (continuar == 's' || continuar == 'S');

    cout << "\nGracias por usar la calculadora. ¡Hasta luego!" << endl;

    return 0;
}
