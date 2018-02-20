#pragma once

#include <iostream>
#include <stdio.h>
#include <cstdio>
#include "enemy.h"
//#include "utility.h"

struct nodeEnemy {
	enemy* enem;
	int degree;
	bool marked;

	nodeEnemy* prev;
	nodeEnemy* next;
	nodeEnemy* child;
	nodeEnemy* parent;

	nodeEnemy* getPrev() { return prev; }
	nodeEnemy* getNext() { return next; }
	nodeEnemy* getChild() { return child; }
	nodeEnemy* getParent() { return parent; }
	float getValue() { return enem->Priority; }
	bool isMarked() { return marked; }

	bool hasChildren() { return child != NULL; }
	bool hasParent() { return parent != NULL; }
};				


struct FibonacciHeap {

	nodeEnemy* heap;

	FibonacciHeap() {
		heap = empty();
	}


	nodeEnemy* singleton(enemy* enemer) 
	{
		nodeEnemy* n = new nodeEnemy;
		n->enem = enemer;
		n->prev = n->next = n;
		n->degree = 0;
		n->marked = false;
		n->child = NULL;
		n->parent = NULL;
		return n;
	}

	nodeEnemy* t_merge(nodeEnemy* a, nodeEnemy* b) 
	{
		if (a == NULL)return b;
		if (b == NULL)return a;
		if (a->enem->Priority>b->enem->Priority) {
			nodeEnemy* temp = a;
			a = b;
			b = temp;
		}
		nodeEnemy* an = a->next;
		nodeEnemy* bp = b->prev;
		a->next = b;
		b->prev = a;
		an->prev = bp;
		bp->next = an;

		return a;
	}

	nodeEnemy* merge(nodeEnemy* heap1, nodeEnemy* heap2) {
		heap = t_merge(heap1, heap2);
		heap2 = empty();
		return heap;
	}

	void insert(enemy* enemer) {
		nodeEnemy* ret = singleton(enemer);
		heap = merge(heap, ret);
		//return heap;
	}

	nodeEnemy* empty() {
		return NULL;
	}

	bool isEmpty() {
		return heap == NULL;
	}

	float getMinimum() {
		return heap->enem->Priority;
	}



	//changes in this function
	enemy* removeMinimum() {
		nodeEnemy* old = heap;
		heap = t_removeMinimum(heap);
		enemy* ret = old->enem;
		delete old;
		return ret;
	}



	void deleteAll(nodeEnemy* n) {
		if (n != NULL) {
			nodeEnemy* c = n;
			do {
				nodeEnemy* d = c;
				c = c->next;
				deleteAll(d->child);
				delete d;
			} while (c != n);
		}
	}

	void addChild(nodeEnemy* parent, nodeEnemy* child) {
		child->prev = child->next = child;
		child->parent = parent;
		parent->degree++;
		parent->child = t_merge(parent->child, child);
	}

	void unMarkAndUnParentAll(nodeEnemy* n) {
		if (n == NULL)return;
		nodeEnemy* c = n;
		do {
			c->marked = false;
			c->parent = NULL;
			c = c->next;
		} while (c != n);
	}

	nodeEnemy* t_removeMinimum(nodeEnemy* n) {
		unMarkAndUnParentAll(n->child);
		if (n->next == n) {
			n = n->child;
		}
		else {
			n->next->prev = n->prev;
			n->prev->next = n->next;
			n = t_merge(n->next, n->child);
		}
		if (n == NULL)return n;
		nodeEnemy* trees[64] = { NULL };

		while (true) {
			if (trees[n->degree] != NULL) {
				nodeEnemy* t = trees[n->degree];
				if (t == n)break;
				trees[n->degree] = NULL;
				if (n->enem->Priority<t->enem->Priority) {
					t->prev->next = t->next;
					t->next->prev = t->prev;
					addChild(n, t);
				}
				else {
					t->prev->next = t->next;
					t->next->prev = t->prev;
					if (n->next == n) {
						t->next = t->prev = t;
						addChild(t, n);
						n = t;
					}
					else {
						n->prev->next = t;
						n->next->prev = t;
						t->next = n->next;
						t->prev = n->prev;
						addChild(t, n);
						n = t;
					}
				}
				continue;
			}
			else {
				trees[n->degree] = n;
			}
			n = n->next;
		}
		nodeEnemy* min = n;
		do {
			if (n->enem->Priority<min->enem->Priority)min = n;
			n = n->next;
		} while (n != min);
		return min;
	}

	nodeEnemy* cut(nodeEnemy* heap, nodeEnemy* n) {
		if (n->next == n) {
			n->parent->child = NULL;
		}
		else {
			n->next->prev = n->prev;
			n->prev->next = n->next;
			n->parent->child = n->next;
		}
		n->next = n->prev = n;
		n->marked = false;
		return t_merge(heap, n);
	}

	nodeEnemy* decreaseKey(nodeEnemy* heap, nodeEnemy* n, float value) {
		if (n->enem->Priority<value)return heap;
		n->enem->Priority = value;
		if (n->enem->Priority < n->parent->enem->Priority) {
			heap = cut(heap, n);
			nodeEnemy* parent = n->parent;
			n->parent = NULL;
			while (parent != NULL && parent->marked) {
				heap = cut(heap, parent);
				n = parent;
				parent = n->parent;
				n->parent = NULL;
			}
			if (parent != NULL && parent->parent != NULL)parent->marked = true;
		}
		return heap;
	}

	nodeEnemy* find(nodeEnemy* heap, float value) {
		nodeEnemy* n = heap;
		if (n == NULL)return NULL;
		do {
			if (n->enem->Priority == value)return n;
			nodeEnemy* ret = find(n->child, value);
			if (ret)return ret;
			n = n->next;
		} while (n != heap);
		return NULL;
	}
};
