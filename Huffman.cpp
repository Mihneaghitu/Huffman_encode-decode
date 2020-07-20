#include <fstream>
#include <cstring>
#include <algorithm>
#include <utility>
#include <map>
#include <string>

using namespace std;

ifstream fin("huff.in");
ofstream fout("huff.out");

char text[101],ctext[101], code[501] ;
int  cnt = 0;
map<char, char*> mymap;
char codl[101][101];

struct HeapNode
{
    char data;
    int frecv;
    HeapNode *tata = NULL, *urm = NULL, *prec = NULL, *fstg = NULL, *fdr = NULL;
};
HeapNode hn[105], *p , *radacina;

void fill_pq(char letter, int pointer);
bool rule(HeapNode h1, HeapNode h2);
HeapNode * huff_tree();
void make_list();
void addNode(HeapNode h);
void encode(HeapNode * rad, char sir[101], bool dir);
void decode(HeapNode * rad , int index);
pair< pair<char,int>, pair<HeapNode *, HeapNode *> > extractMin();
char * findCode(char letter);
void assignCode(char letter, char sir[101]);

int main()
{
    fin.get(text, 101);
    strcpy(ctext, text);
    for(unsigned int i = 0 ; i < strlen(text) ; i++)
        if(text[i] != 8) //ascii 8 is backspace
            fill_pq(text[i], i);
    sort(hn + 1, hn + cnt + 1, rule);
    make_list();
    radacina = new HeapNode;
    char sir[101] = {0};
    radacina = huff_tree();
    cnt = 0;
    //encode andd print code
    encode(radacina, sir, true);
    for(int i = 0 ; i < strlen(ctext) ; i++)
        if(ctext[i] != 8) //ascii 8 is backspace
            strcat(code, findCode(ctext[i]));
    for(int i = 0 ; i < strlen(code) ; i++)
        fout<< code[i];
    fout<<'\n';
    //decode and print text
    decode(radacina , 0);
    return 0;
}

void decode(HeapNode * rad, int index)
{
    int frecv = rad->frecv;
    char data = rad->data;
    if(rad->data != '#')
    {
        fout<<rad->data;
        decode(radacina, index);
    }
    else
    {
        if(code[index] == '0' && rad->fstg) decode(rad->fstg , index + 1);
        if(code[index] == '1' && rad->fdr) decode(rad->fdr , index + 1);
    }
}

void assignCode(char letter, char sir[101])
{
    cnt++;
    codl[cnt][0] = letter;
    for(int i = 0 ; i < strlen(sir) ; i++)
        codl[cnt][i+1] = sir[i];
}

char * findCode(char letter)
{
    char sir[101] = {0};
    for(int i = 1 ; i <= cnt ; i++)
        if(codl[i][0] == letter)
        {
            strcpy(sir,codl[i]+1);
            return sir;
        }
}

void encode(HeapNode * rad, char bin[101], bool dir)
{
    if(!rad->tata) {}
    else if(!dir) strcat(bin, "0");
    else strcat(bin, "1");

    if(rad->data != '#')
        assignCode(rad->data, bin);
    else
    {
        if(rad->fstg)
        {
            encode(rad->fstg, bin, false);
            bin[strlen(bin)-1] = 0;
        }
        if(rad->fdr)
        {
            encode(rad->fdr, bin, true);
            bin[strlen(bin)-1] = 0;
        }
    }
}

pair< pair<char,int>, pair<HeapNode *, HeapNode *> > extractMin()
{
    HeapNode *q   = new HeapNode;
    HeapNode *r   = new HeapNode;
    HeapNode *aux = new HeapNode;

    //doar unul ramas in lista
    if(!p->prec && !p->urm)
    {
        aux = p ;
        return make_pair(make_pair(aux->data, aux->frecv), make_pair(aux->fstg, aux->fdr));
    }

    q = p;
    aux = q;
    int mini = q->frecv;
    while(q->urm)
    {
        q = q->urm;
        // merge si cu mai mic strict dar trb pusa cond la encode
        if(q->frecv < mini) mini = q->frecv, aux = q;
    }

    if(aux == p)
    {
        p = p->urm;
        p->prec = NULL;
    }
    else if(aux->urm == NULL)
    {
        q = aux->prec;
        q->urm = NULL;
    }
    else
    {
        q = aux->prec;
        r = aux->urm ;
        q->urm  = r;
        r->prec = q;
    }
    pair< pair<char, int>, pair<HeapNode*, HeapNode*> > p;
    p = make_pair(make_pair(aux->data, aux->frecv), make_pair(aux->fstg, aux->fdr));
    return p;
}

HeapNode * huff_tree()
{
    HeapNode *F   = new HeapNode;
    while(cnt > 1)
    {
        HeapNode *q   = new HeapNode;
        HeapNode *n1 = new HeapNode;
        HeapNode *n2 = new HeapNode;
        HeapNode *f  = new HeapNode;
        pair< pair<char, int>, pair<HeapNode*, HeapNode*> > p1 = extractMin();
        pair< pair<char, int>, pair<HeapNode*, HeapNode*> > p2 = extractMin();
        n1->data = p1.first.first, n2->data = p2.first.first;
        n1->frecv = p1.first.second, n2->frecv = p2.first.second;
        n1->fstg = p1.second.first, n1->fdr = p1.second.second;
        n2->fstg = p2.second.first, n2->fdr = p2.second.second;
        n1->tata = f, n2->tata = f;
        f->fstg = n1, f->fdr = n2;
        f->frecv = p1.first.second + p2.first.second;
        f->data = '#';
        q = p;
        while(q->urm) q = q->urm;
        q->urm  = f;
        f->prec = q;
        f->urm  = NULL;
        cnt--;
        F = f;
    }
    return F;
}

void addNode(HeapNode h)
{
    HeapNode *q;
    if(!p->urm)
    {
        q = new HeapNode;
        p->urm   = q;
        q->urm   = NULL;
        q->prec  = p;
        q->data  = h.data;
        q->frecv = h.frecv;
    }
    else
    {
        q = p;
        while(q->urm) q = q->urm;
        HeapNode *aux = new HeapNode;
        q->urm     = aux;
        aux->urm   = NULL;
        aux->prec  = q;
        aux->data  = h.data;
        aux->frecv = h.frecv;
    }
}

void make_list()
{
    p = new HeapNode;
    p->data  = hn[1].data;
    p->frecv = hn[1].frecv;
    for(int i = 2 ; i <= cnt ; i++)
        addNode(hn[i]);
}

bool rule(HeapNode h1, HeapNode h2)
{
    return h1.frecv < h2.frecv ;
}

void fill_pq(char letter, int pointer)
{
    cnt ++ ;
    for(unsigned int i = pointer ; i < strlen(text) ; i++)
        if(text[i] == letter)
        {
            hn[cnt].data = letter;
            hn[cnt].frecv++;
            text[i] = 8; //ascii 8 is backspace
        }
}
