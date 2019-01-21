#ifndef splaytree_h
#define splaytree_h

#include <iostream>
#include <stdexcept>
#include <algorithm>

template<class T>
class splaytree {
private:

	struct Nodo {
		Nodo(T const&e, Nodo *l, Nodo *r) : elem(e), left(l), right(r) {}
		Nodo(T const &e) : elem(e), left(nullptr), right(nullptr) {}
		T elem;
		Nodo *left;
		Nodo *right;
	};
	
	Nodo *raiz;

public:
	splaytree() { raiz = nullptr; }
	splaytree(T const&e) { raiz = new Nodo(e); }
	splaytree(Nodo *n) { raiz = n; }
	splaytree(T const&e, Nodo *l, Nodo *r) { raiz = new Nodo(e, l); }

	~splaytree() { raiz = nullptr; delete raiz; }

	T root() const {
		if (empty()) throw std::domain_error("No existe elemento en el arbol");
		return raiz->elem;
	}

	splaytree left() const {
		if (this->empty()) throw std::domain_error("Arbol vacio");
		return splaytree<T>(raiz->left);
	}

	splaytree right() const {
		if (this->empty()) throw std::domain_error("Arbol vacio");
		return splaytree<T>(raiz->right);
	}


	bool empty() const { return raiz == nullptr; }


	//Hace insercion y luego flota el elemento
	//coste peor O(n), coste amortizado O(log n)
	void insert(T const &e) {
		insertion(e);
		splay(e);
	}

	//Erase primero hace la funcion find para flotar o el elemento a borrar o el anterior en caso de no encontrarse
	//Despues pone como raiz el hijo izquierdo si el derecho no existe y viceversa, y si los dos existe flota a la raiz el mayor del hijo izquierdo
	//Coste del caso peor O(n), coste amortizado O(log n)
	void erase(T const &e) {
		if (empty()) return;
		if (find(e)) {
			//Caso de que el nodo a borrar sea el ultimo elemento del arbol
			if (raiz->left == nullptr && raiz->right == nullptr) raiz = nullptr;
			else if (raiz->left == nullptr) raiz = raiz->right;
			else if (raiz->right == nullptr) raiz = raiz->left;
			else {
				splaytree<T> sp = left();
				sp.flotamiento(left().max());
				Nodo *aux = raiz->right;
				raiz = sp.raiz;
				raiz->right = aux;
				aux = nullptr;
				delete aux;
			}
		}
	}


	//FIND PRIMERO LLAMA A LA FUNCION FINDING QUE ENCUENTRA EL ELEMENTO A FLOTAR DEL ARBOL, Y FIND DEVUELVE TRUE SI ESTE ES IGUAL AL QUE SE BUSCABA
	//coste peor O(n), caso amortizador O(log n)
	bool find(T const &e) {
		if (empty()) return false;
		T f = finding(e);
		splay(f);
		return f == e;
	}	

private:

	//Funcion recursiva para buscar si se encuentra el elemento o en su defecto el que se debe flotar
	T finding(T const &e) const {
		T r;
		if (raiz->elem == e) r = raiz->elem;

		//Si el elemento a buscar no existe en el arbol se devuelve el ultimo visitado, si no se busca en el subarbol correspondiente
		else if (raiz->elem < e) {
			if (right().empty()) r = raiz->elem;
			else r = right().finding(e);
		}
		else {
			if (left().empty()) r = raiz->elem;
			else r = left().finding(e);
		}
		return r;
	}

	void insertion(T const &e) {
		//El primero es el caso base en el caso que no exista ningun elemento en el arbol
		if (empty()) this->raiz = new Nodo(e);
		//Si en el subarbol en el que se debe buscar esta vacio, se inserta el nuevo nodo con el elemento, si no se busca en el subarbol correspondiente
		else if (this->raiz->elem > e) {
			if (this->left().empty()) this->raiz->left = new Nodo(e);
			else this->left().insertion(e);
		}
		else if (this->raiz->elem < e) {
			if (this->right().empty()) this->raiz->right = new Nodo(e);
			else this->right().insertion(e);
		}
		//Si el elemento se encuentra no hara nada la funcion ya que no se debe insertar un elemento ya existente y solo debe flotarse
	}

	//LA FUNCION HACE PRIMERO LAS FLOTACIONES CON ABUELO Y POR ULTIMO SI SE QUEDA EL ELEMENTO COMO HIJO DE RAIZ HACE EL ULTIMO REEQUILIBRIO
	void splay(T const &elem) {
		flotamiento(elem);
		if (raiz->elem != elem) {
			if (raiz->elem < elem) RR();
			else LL();
		}
	}

	//ESTA FUNCION HACE EL FLOTAMIENTO MIENTRAS EL ELEMENTO A FLOTAR TENGA ABUELO
	void flotamiento(T const &elem) {
		if (empty() || this->raiz->elem == elem) return;
		splaytree<T> h;
		if (this->raiz->elem < elem) {
			h = splaytree<T>(raiz->right);
			h.flotamiento(elem);
			this->raiz->right = new Nodo(*h.raiz);
			if (h.raiz->elem < elem && h.raiz->right != NULL) RRgc();
			else if(h.raiz->elem > elem && h.raiz->left != NULL) RL();
		}
		else {
			h = splaytree<T>(raiz->left);
			h.flotamiento(elem);
			this->raiz->left = new Nodo(*h.raiz);
			if (h.raiz->elem < elem && h.raiz->right != NULL) LR();
			else if(h.raiz->elem > elem && h.raiz->left != NULL) LLgc();
		}
	}

	//Todas las rotaciones son de coste constante O(1)
	void RRgc() {
		Nodo *b = raiz->right->left;
		Nodo *c = raiz->right->right->left;
		Nodo *gp = raiz;
		Nodo *p = raiz->right;
		raiz = p->right;
		raiz->left = p;
		p->right = c;
		p->left = gp;
		gp->right = b;
		b = nullptr; c = nullptr; gp = nullptr; p = nullptr;
		delete b; delete c; delete gp; delete p;
	}

	void LLgc() {
		Nodo *b = raiz->left->right;
		Nodo *c = raiz->left->left->right;
		Nodo *gp = raiz;
		Nodo *p = raiz->left;
		raiz = p->left;
		raiz->right = p;
		p->left = c;
		p->right = gp;
		gp->left = b;
		b = nullptr; c = nullptr; gp = nullptr; p = nullptr;
		delete b; delete c; delete gp; delete p;
	}

	void RR(){
		Nodo *b = raiz->right->left;
		Nodo *p = raiz;
		raiz = raiz->right;
		raiz->left = p;
		raiz->left->right = b;
		b = nullptr; p = nullptr;
		delete b; delete p;
	}

	void LL() {
		Nodo *b = raiz->left->right;
		Nodo *p = raiz;
		raiz = raiz->left;
		raiz->right = p;
		raiz->right->left = b;
		b = nullptr; p = nullptr;
		delete b; delete p;
	}

	void RL() {
		Nodo *gp = raiz;
		Nodo *p = raiz->right;
		Nodo *b = p->left->left;
		Nodo *c = p->left->right;
		raiz = p->left;
		gp->right = b;
		p->left = c;
		raiz->left = gp;
		raiz->right = p;
		gp = nullptr; p = nullptr; b = nullptr; c = nullptr;
		delete gp; delete p; delete b; delete c;
	}

	void LR() {
		Nodo *gp = raiz;
		Nodo *p = raiz->left;
		Nodo *b = p->right->left;
		Nodo *c = p->right->right;
		raiz = p->right;
		p->right = b;
		gp->left = c;
		raiz->left = p;
		raiz->right = gp;
		gp = nullptr; p = nullptr; b = nullptr; c = nullptr;
		delete gp; delete p; delete b; delete c;
	}

	//Funciones para buscar el maximo y minimo de un arbol, ambos funciones recursivas de coste O(n) caso peor y O(log n) en coste amortizado
	T max() const {
		T r;
		if (empty()) throw std::domain_error("Arbol vacio");
		else if (right().empty()) r = raiz->elem;
		else r = right().max();
		return r;
	}

	T min() const {
		T r;
		if (empty()) throw std::domain_error("Arbol vacio");
		else if (left().empty()) r = raiz->elem;
		else r = left().min();
		return r;
	}

public:

	void print(std::ostream &o) const {
		if (this->empty()) return;
		left().print(o);
		o << this->raiz->elem << " ";
		right().print(o);
	}

};

template<class T>
std::ostream& operator << (std::ostream &o, splaytree<T> spl) {
	spl.print(o);
	return o;
}

#endif