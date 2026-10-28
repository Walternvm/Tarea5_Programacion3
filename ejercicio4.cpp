#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <memory>
#include <stdexcept>
using namespace std;

class Node {
    friend class Graph;
    friend class BFSIterator;
    friend class DFSIterator;
    int id;
    vector<int> vecinos;
public:
    Node(int id): id(id) {}

    int getId() const {
        return id;
    }
};

class Graph {
    friend class BFSIterator;
    friend class DFSIterator;
    map<int, Node> nodos;
public:
    void addNode(int id) {
        nodos.emplace(id, Node(id));
    }

    void addEdge(int origen, int destino) {
        addNode(origen);
        addNode(destino);

        auto& vecinos = nodos.at(origen).vecinos;

        for (int vecino: vecinos) {
            if (vecino == destino)
                return;
        }

        vecinos.push_back(destino);
    }
};

class GraphIterator {
public:
    virtual bool hasNext() = 0;
    virtual int next() = 0;
    virtual ~GraphIterator() = default;
};

class BFSIterator: public GraphIterator {
    const Graph& grafo;
    queue<int> pendientes;
    set<int> visitados;
public:
    BFSIterator(const Graph& grafo, int inicio): grafo(grafo) {
        if (grafo.nodos.find(inicio) != grafo.nodos.end()) {
            pendientes.push(inicio);
            visitados.insert(inicio);
        }
    }

    bool hasNext() override {
        return !pendientes.empty();
    }

    int next() override {
        if (!hasNext())
            throw out_of_range("No hay mas nodos");

        int actual = pendientes.front();
        pendientes.pop();

        for (int vecino: grafo.nodos.at(actual).vecinos) {
            if (visitados.find(vecino) == visitados.end()) {
                pendientes.push(vecino);
                visitados.insert(vecino);
            }
        }

        return actual;
    }
};

class DFSIterator: public GraphIterator {
    const Graph& grafo;
    stack<int> pendientes;
    set<int> visitados;
    void descartarVisitados() {
        while (!pendientes.empty()) {
            int actual = pendientes.top();

            if (visitados.find(actual) == visitados.end())
                break;

            pendientes.pop();
        }
    }
public:
    DFSIterator(const Graph& grafo, int inicio): grafo(grafo) {
        if (grafo.nodos.find(inicio) != grafo.nodos.end())
            pendientes.push(inicio);
    }

    bool hasNext() override {
        descartarVisitados();
        return !pendientes.empty();
    }

    int next() override {
        if (!hasNext())
            throw out_of_range("No hay mas nodos");

        int actual = pendientes.top();
        pendientes.pop();
        visitados.insert(actual);

        const auto& vecinos = grafo.nodos.at(actual).vecinos;

        for (int i = static_cast<int>(vecinos.size()) - 1; i >= 0; i--) {
            int vecino = vecinos[i];

            if (visitados.find(vecino) == visitados.end())
                pendientes.push(vecino);
        }

        return actual;
    }
};

int main() {
    Graph g;

    g.addEdge(1, 2);
    g.addEdge(1, 3);
    g.addEdge(2, 4);
    g.addEdge(4, 1);

    unique_ptr<GraphIterator> it = make_unique<BFSIterator>(g, 1);

    cout << "BFS: ";

    while (it->hasNext())
        cout << it->next() << " ";

    cout << endl;

    it = make_unique<DFSIterator>(g, 1);

    cout << "DFS: ";

    while (it->hasNext())
        cout << it->next() << " ";

    cout << endl;
}
