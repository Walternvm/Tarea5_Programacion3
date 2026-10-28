#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <stdexcept>
#include <cmath>
using namespace std;

class Account {
    friend class TransactionManager;
    string id;
    double balance;
    mutable mutex mtx;
public:
    Account(string id, double balance): id(id), balance(balance) {
        if (!isfinite(balance) || balance < 0)
            throw invalid_argument("Saldo inicial invalido");
    }

    double getBalance() const {
        lock_guard<mutex> lock(mtx);
        return balance;
    }
};

class Transaction {
    string cuenta;
    string tipo;
    double monto;
public:
    Transaction(string cuenta, string tipo, double monto): cuenta(cuenta), tipo(tipo), monto(monto) {}

    void print() const {
        cout << cuenta << " - " << tipo << ": " << monto << endl;
    }
};

class TransactionManager {
    vector<Transaction> historial;
    mutex mtx;
    TransactionManager() {}
    void validar(Account* cuenta, double monto) {
        if (cuenta == nullptr)
            throw invalid_argument("Cuenta nula");

        if (!isfinite(monto) || monto <= 0)
            throw invalid_argument("Monto invalido");
    }
public:
    TransactionManager(const TransactionManager&) = delete;
    TransactionManager& operator=(const TransactionManager&) = delete;

    static TransactionManager& getInstance() {
        static TransactionManager instancia;
        return instancia;
    }

    void deposit(Account* cuenta, double monto) {
        validar(cuenta, monto);

        lock_guard<mutex> lock1(mtx);
        lock_guard<mutex> lock2(cuenta->mtx);

        double nuevoSaldo = cuenta->balance + monto;

        if (!isfinite(nuevoSaldo))
            throw overflow_error("Saldo fuera de rango");

        historial.emplace_back(cuenta->id, "Deposito", monto);
        cuenta->balance = nuevoSaldo;
    }

    void withdraw(Account* cuenta, double monto) {
        validar(cuenta, monto);

        lock_guard<mutex> lock1(mtx);
        lock_guard<mutex> lock2(cuenta->mtx);

        if (monto > cuenta->balance)
            throw runtime_error("Fondos insuficientes");

        historial.emplace_back(cuenta->id, "Retiro", monto);
        cuenta->balance -= monto;
    }

    void mostrarHistorial() {
        lock_guard<mutex> lock(mtx);

        for (const Transaction& transaccion: historial)
            transaccion.print();
    }
};

int main() {
    auto& manager = TransactionManager::getInstance();

    Account account("A1", 1000);
    vector<thread> threads;

    for (int i = 0; i < 10; i++) {
        threads.emplace_back([&]() {
            manager.deposit(&account, 100);
            manager.withdraw(&account, 50);
        });
    }

    for (auto& t: threads)
        t.join();

    cout << "Balance final: " << account.getBalance() << endl;
    cout << "Direccion manager: " << &TransactionManager::getInstance() << endl;
    manager.mostrarHistorial();
}
