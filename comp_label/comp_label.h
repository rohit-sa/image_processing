#ifndef COMP_LABEL_H
#define COMP_LABEL_H

#include <vector>

typedef unsigned char uchar;

struct node {
    node *parent;
    int i, rank;
};

class DisjointSet{
	int elements,sets;
	std::vector<node* > nodes;
	
	public:
	DisjointSet();
	node* find(node* i);
	void join(node* i, node* j);
	node* makeSet(int i);
		
};
DisjointSet::DisjointSet(){
	elements = 0;
	sets = 0;
}
node* DisjointSet::find(node* i){
	if(i->parent == i){
		return i;
	}
	else{
		i->parent = find(i->parent);
		return i->parent;
	}
}
void DisjointSet::join(node* i, node* j){
	node* irep = find(i);
	node* jrep = find(j);   
	int irank = irep->rank;
    int jrank = jrep->rank;

    if (irep == jrep)
        return;

    if (irank < jrank) {
        irep->parent = jrep;
    } 
    else if (jrank < irank) {
        jrep->parent = irep;
    }
    else {
        irep->parent = jrep;
        jrep->rank++;
    }
    sets--;
}

node* DisjointSet::makeSet(int i){
	if (elements + 1 > nodes.size()){
		 nodes.push_back(new node);
	}
    nodes[elements]->parent = nodes[elements];
    nodes[elements]->i = i;
    nodes[elements]->rank = 0;
 
    elements++;
    sets++;
 
    return nodes[elements-1];
}

#endif
