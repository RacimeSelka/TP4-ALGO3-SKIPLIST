#include <stdlib.h>
#include <assert.h>

#include "skiplist.h"
#include "rng.h"

typedef struct s_node Node;
typedef struct s_link Link;

struct s_node
{
	int key;
	int level;
	Link *links;
};

struct s_SkipList
{
	Node *sentinelle;
	int size;
	int maxLevel;
	RNG rng;
};

struct s_link
{
	Node *prev;
	Node *next;
};

struct s_SkipListIterator{
	SkipList* list;
	IteratorDirection direction;
	Node* curr;

};

SkipList *skiplist_create(int nblevels)
{

	SkipList *list = malloc(sizeof(SkipList) + sizeof(Node));
	list->rng = rng_initialize(0, nblevels);
	list->sentinelle = (Node *)(list + 1);
	list->size = 0;
	list->maxLevel = nblevels;
	list->sentinelle->level = nblevels;
	list->sentinelle->links = malloc(nblevels * sizeof(Link));
	for (int i = 0; i < nblevels; i++)
	{
		list->sentinelle->links[i].next = list->sentinelle->links[i].prev = list->sentinelle;
	}

	return list;
}

void skiplist_delete(SkipList **d)
{

	SkipList *list = *d;
	Node *curr = list->sentinelle->links[0].next;

	while (curr != list->sentinelle)
	{
		Node *next = curr->links[0].next;

		free(curr->links);
		free(curr);

		curr = next;
	}

	free(list->sentinelle->links);
	free(list->sentinelle);

	free(list);
}

unsigned int skiplist_size(const SkipList *d)
{
	return d->size;
}

int skiplist_at(const SkipList *d, unsigned int i)
{
	assert(i <= skiplist_size(d));
	int pos = i;
	Node *e = d->sentinelle->links[0].next;
	for (int i = 0; i < pos; i++)
	{
		e = e->links[0].next;
	}
	return e->key;
}

void skiplist_map(const SkipList *d, ScanOperator f, void *user_data)
{
	for (Node *e = d->sentinelle->links[0].next; e != d->sentinelle; e = e->links[0].next)
	{
		f(e->key, user_data);
	}
}

SkipList *skiplist_insert(SkipList *d, int value)
{
	Node *node = malloc(sizeof(Node));
	node->key = value;
	int level = rng_get_value(&(d->rng)) + 1;
	node->level = level;
	node->links = malloc(level * sizeof(Link));

	Node *trace[d->maxLevel];
	Node *x = d->sentinelle;

	for (int i = d->maxLevel - 1; i >= 0; i--)
	{
		while (x->links[i].next != d->sentinelle && x->links[i].next->key < value)
		{
			x = x->links[i].next;
		}
		trace[i] = x;
	}

	if (trace[0]->links[0].next != d->sentinelle && trace[0]->links[0].next->key == value)
	{
		free(node->links);
		free(node);
		return d;
	}

	for (int i = 0; i < level; i++)
	{

		node->links[i].next = trace[i]->links[i].next;
		node->links[i].prev = trace[i];

		trace[i]->links[i].next->links[i].prev = node;
		trace[i]->links[i].next = node;
	}

	d->size++;
	return d;
}

bool skiplist_search(const SkipList *d, int value, unsigned int *nb_operations)
{
	Node *x = d->sentinelle;

	for (int i = d->maxLevel - 1; i >= 0; i--)
	{
		while (x->links[i].next != d->sentinelle && x->links[i].next->key < value)
		{
			x = x->links[i].next;
			(*nb_operations)++;
		}
		if (x->links[i].next != d->sentinelle && x->links[i].next->key==value)
		{
			(*nb_operations)++;
			return true;
		}
	}
	
	return false;
}

SkipList *skiplist_remove(SkipList *d, int value)
{
	Node *trace[d->maxLevel];
	Node *x = d->sentinelle;

	
	for (int i = d->maxLevel - 1; i >= 0; i--)
	{
		while (x->links[i].next != d->sentinelle && x->links[i].next->key < value)
		{
			x = x->links[i].next;
		}
		trace[i] = x;
	}

	
	Node *node_to_remove = trace[0]->links[0].next;
	if (node_to_remove == d->sentinelle || node_to_remove->key != value)
	{
		
		return d;
	}

	
	for (int i = 0; i < node_to_remove->level; i++)
	{
		trace[i]->links[i].next = node_to_remove->links[i].next;
		node_to_remove->links[i].next->links[i].prev = trace[i];
	}

	
	free(node_to_remove->links);
	free(node_to_remove);

	d->size--;
	return d;
}



SkipListIterator* skiplist_iterator_create(SkipList* d, IteratorDirection w){
	SkipListIterator* iterator=malloc(sizeof(SkipListIterator));
	iterator->list=d;
	iterator->direction=w;
	return iterator;
}

void skiplist_iterator_delete(SkipListIterator** it){
	free(*it);
	*it=NULL;
}

SkipListIterator* skiplist_iterator_begin(SkipListIterator* it){
	
	if (it->direction==FORWARD_ITERATOR)
	{
		it->curr=it->list->sentinelle->links[0].next;
	}else
	{
		it->curr=it->list->sentinelle->links[0].prev;
	}
	
	
	return it;
}

bool skiplist_iterator_end(SkipListIterator* it){
	return  it->curr==it->list->sentinelle;
}

SkipListIterator* skiplist_iterator_next(SkipListIterator* it){
	
	if (it->direction==FORWARD_ITERATOR)
	{
		it->curr=it->curr->links[0].next;
	}else
	{
		it->curr=it->curr->links[0].prev;
	}
	
	return it;
}

int skiplist_iterator_value(SkipListIterator* it){
	return it->curr->key;
}
