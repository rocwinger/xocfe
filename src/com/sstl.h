/*@
Copyright (c) 2013-2014, Su Zhenyu steven.known@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Su Zhenyu nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
@*/
#ifndef __SSTL_H__
#define __SSTL_H__

#define NO_BASIC_MAT_DUMP //Default option
#define MAX_SHASH_BUCKET 97 //Default option

template <class T> class SVECTOR;

/*
Structure chain operations.
For easing implementation, there must be 2 fields declared by T
	1. T * next
	2. T * prev
*/
template <class T>
inline UINT cnt_list(T * t)
{
	UINT c = 0;
	while (t != NULL) { c++; t = t->next; }
	return c;
}


template <class T>
inline T * get_last(T * t)
{
	while (t != NULL && t->next != NULL) { t = t->next; }
	return t;
}


template <class T>
inline T * get_first(T * t)
{
	while (t != NULL && t->prev != NULL) { t = t->prev; }
	return t;
}


template <class T>
inline void add_next(T ** pheader, T * t)
{
	if (pheader == NULL || t == NULL) return ;
	T * p = NULL;
	t->prev = NULL;
	if ((*pheader) == NULL) {
		*pheader = t;
	} else {
		p = (*pheader)->next;
		IS_TRUE(t != *pheader, ("\n<add_next> : overlap list member\n"));
		if (p == NULL) {
			(*pheader)->next = t;
			t->prev = *pheader;
		} else {
			while (p->next != NULL) {
				p = p->next;
				IS_TRUE(p != t, ("\n<add_next> : overlap list member\n"));
			}
			p->next = t;
			t->prev = p;
		}
	}
}


template <class T>
inline void add_next(IN OUT T ** pheader, IN OUT T ** last, IN T * t)
{
	if (pheader == NULL || t == NULL) { return ; }

	t->prev = NULL;
	if ((*pheader) == NULL) {
		*pheader = t;
		while (t->next != NULL) { t = t->next; }
		*last = t;
	} else {
		IS_TRUE0(last != NULL && *last != NULL && (*last)->next == NULL);
		(*last)->next = t;
		t->prev = *last;
		while (t->next != NULL) { t = t->next; }
		*last = t;
	}
}


template <class T>
inline void replace(T ** pheader, T * olds, T * news)
{
	if (pheader == NULL || olds == NULL) return;
	if (olds == news) { return; }
	if (news == NULL) {
		remove(pheader, olds);
		return;
	}

	#ifdef _DEBUG_
	bool find = false;
	T * p = *pheader;
	while (p != NULL) {
		if (p == olds) {
			find = true;
			break;
		}
		p = p->next;
	}
	IS_TRUE(find, ("'olds' is not inside in pheader"));
	#endif

	news->prev = olds->prev;
	news->next = olds->next;
	if (olds->prev != NULL) {
		olds->prev->next = news;
	}
	if (olds->next != NULL) {
		olds->next->prev = news;
	}
	if (olds == *pheader) {
		*pheader = news;
	}
	olds->next = olds->prev = NULL;
}


template <class T>
inline T * removehead(T ** pheader)
{
	if (pheader == NULL || *pheader == NULL) return NULL;
	T * t = *pheader;
	*pheader = t->next;
	if (*pheader != NULL) {
		(*pheader)->prev = NULL;
	}
	t->next = t->prev = NULL;
	return t;
}


template <class T>
inline T * removetail(T ** pheader)
{
	if (pheader == NULL || *pheader == NULL) { return NULL; }
	T * t = *pheader;
	while (t->next != NULL) { t = t->next; }
	remove(pheader, t);
	return t;
}


template <class T>
inline void remove(T ** pheader, T * t)
{
	if (pheader == NULL || t == NULL) { return; }
	if (t == *pheader) {
		*pheader = t->next;
		if (*pheader != NULL) {
			(*pheader)->prev = NULL;
		}
		t->next = t->prev = NULL;
		return;
	}

	IS_TRUE(t->prev, ("t is not in list"));
	t->prev->next = t->next;
	if (t->next != NULL) {
		t->next->prev = t->prev;
	}
	t->next = t->prev = NULL;
}


//Insert one elem 't' before 'marker'.
template <class T>
inline void insertbefore_one(T ** head, T * marker, T * t)
{
	if (t == NULL) return;
	IS_TRUE(head, ("absent parameter"));
	if (t == marker) return;
	if (*head == NULL) {
		IS_TRUE(marker == NULL, ("marker must be NULL"));
		*head = t;
		return;
	}

	if (marker == *head) {
		//'marker' is head, and replace head.
		t->prev = NULL;
		t->next = marker;
		marker->prev = t;
		*head = t;
		return;
	}

	IS_TRUE(marker->prev != NULL, ("marker is head"));
	marker->prev->next = t;
	t->prev = marker->prev;
	t->next = marker;
	marker->prev = t;
}


/*
Insert a list that leading by 't' before 'marker'.

'head': function might modify the header of list.
't': the head element of list, that to be inserted.
*/
template <class T>
inline void insertbefore(T ** head, T * marker, T * t)
{
	if (t == NULL) { return; }
	IS_TRUE(head, ("absent parameter"));
	if (t == marker) { return; }
	if (*head == NULL) {
		IS_TRUE(marker == NULL, ("marker must be NULL"));
		*head = t;
		return;
	}

	if (marker == *head) {
		//'marker' is head, and replace head.
		IS_TRUE(t->prev == NULL, ("t is not the first element"));
		add_next(&t, *head);
		*head = t;
		return;
	}

	IS_TRUE(marker->prev != NULL, ("marker should not be head"));
	if (marker->prev != NULL) {
		marker->prev->next = t;
		t->prev = marker->prev;
	}

	t = get_last(t);
	t->next = marker;
	marker->prev = t;
}


/*
Insert t into list immediately that following 'marker'.
e.g: a->maker->b->c
	output is: a->maker->t->b->c
Return header in 'marker' if list is empty.
*/
template <class T>
inline void insertafter_one(T ** marker, T * t)
{
	if (marker == NULL || t == NULL) return;
	if (t == *marker) return;
	if (*marker == NULL) {
		*marker = t;
		return;
	}
	T * last = get_last(t);
	if ((*marker)->next != NULL) {
		(*marker)->next->prev = last;
		last->next = (*marker)->next;
	}
	(*marker)->next = t;
	t->prev = *marker;
}


/*
Insert t into marker's list as the subsequent element.
e.g: a->maker->b->c,  and t->x->y
	output is: a->maker->t->x->y->b->c
*/
template <class T>
inline void insertafter(T ** marker, T * t)
{
	if (marker == NULL || t == NULL) return;
	if (t == *marker) return;
	if (*marker == NULL) {
		*marker = t;
		return;
	}

	if ((*marker)->next != NULL) {
		T * last = get_last(t);
		(*marker)->next->prev = last;
		last->next = (*marker)->next;
	}
	t->prev = *marker;
	(*marker)->next = t;
}


//Reverse list, return the new list-head.
template <class T>
inline T * reverse_list(T * t)
{
	T * head = t;
	while (t != NULL) {
		T * tmp = t->prev;
		t->prev = t->next;
		t->next = tmp;
		head = t;
		t = t->prev;
	}
	return head;
}


//Double Chain Container.
#define C_val(c)    ((c)->val)
#define C_next(c)   ((c)->next)
#define C_prev(c)   ((c)->prev)
template <class T> class C {
public:
   C<T> * prev;
   C<T> * next;
   T val;
};


//Single Chain Container.
#define SC_val(c)    ((c)->v)
#define SC_next(c)   ((c)->next)
template <class T> class SC {
public:
   SC<T> * next;
   T v;
};



/*
FREE-LIST

T refer to basis element type.
	e.g: Suppose variable type is 'VAR*', then T is 'VAR'.

For easing implementation, there are 2 fields should be declared in T,
	struct T {
		T * next;
		T * prev;
		... //other field
	}
*/
template <class T>
class FREE_LIST {
protected:
	T * m_free_list;
	bool m_is_clean;
public:
	FREE_LIST();
	~FREE_LIST();
	inline void add_free_elem(T * t);
	inline T * get_free_elem();
	inline void set_clean(bool is_clean);
	inline void free_system_memory();
	inline void clean();
	inline UINT count_mem() const { return sizeof(FREE_LIST<T>); }
};


template <class T>
FREE_LIST<T>::FREE_LIST()
{
	m_is_clean = true;
	m_free_list = NULL;
}


template <class T>
FREE_LIST<T>::~FREE_LIST()
{
	m_free_list = NULL;
}


template <class T>
void FREE_LIST<T>::set_clean(bool is_clean)
{
	m_is_clean = is_clean;
}


template <class T>
void FREE_LIST<T>::add_free_elem(T * t)
{
	if (t == NULL) {return;}
	if (m_free_list == NULL) {
		m_free_list = t;
		m_free_list->next = m_free_list->prev = NULL;
		return;
	}
	t->prev = m_free_list;
	m_free_list->next = t;
	m_free_list = t;
	m_free_list->next = NULL;
}


template <class T>
T * FREE_LIST<T>::get_free_elem()
{
	T * t = NULL;
	if (m_free_list == NULL) { return NULL; }
	t = m_free_list;
	m_free_list = m_free_list->prev;
	if (m_free_list != NULL) {
		m_free_list->next = NULL;
	}

	m_is_clean ? memset(t, 0, sizeof(T)) : t->next = t->prev = NULL;
	return t;
}


template <class T>
void FREE_LIST<T>::clean()
{
	m_free_list = NULL;
}


//NOTE: the list elements should NOT be freed again by other functions.
template <class T>
void FREE_LIST<T>::free_system_memory()
{
	if (m_free_list == NULL) {
		return;
	}
	while (m_free_list != NULL) {
		T * t = m_free_list;
		m_free_list = m_free_list->prev;
		::free(t);
	}
	m_free_list = NULL;
}


/*
LIST

NOTICE:
	The following 3 operations are the key points which you should
	attention to:
	1.	If you REMOVE one element, its container will be collect by FREE-LIST.
		So if you need a new container, please check the FREE-LIST first,
		accordingly, you should first invoke 'get_free_list' which get free
		containers out from 'm_free_list'.
  	2.	If you want to invoke APPEND, please call 'newXXX' first to
		allocate a new container memory space, record your elements in
		container, then APPEND it at list.
		newXXX such as:
			T * newXXX(INT type)
			{
				T * t = get_free_T();
				if (t == 0) { t = (T*)malloc(sizeof(T)); }
				T_type(c) = type;
				return t;
			}
*/
template <class T> class LIST {
private:
	LIST(LIST const&) { IS_TRUE(0, ("Do not invoke copy-constructor.")); }
	void operator = (LIST const&)
	{ IS_TRUE(0, ("Do not invoke copy-constructor.")); }
protected:
	SMEM_POOL * m_free_list_pool;
	INT m_elem_count;
	C<T> * m_head;
	C<T> * m_tail;
	C<T> * m_cur;
	C<T> * m_start_pt; //starting pos of list that to be used in next searching
	FREE_LIST<C<T> > m_free_list; //Hold for available containers

	void * _xmalloc(ULONG size)
	{
		IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
		void * p = smpool_malloc_h(size, m_free_list_pool);
		IS_TRUE0(p != NULL);
		memset(p, 0, size);
		return p;
	}
public:
	LIST()
	{
		m_free_list_pool = NULL;
		init();
	}
	~LIST() { destroy(); }

	void init()
	{
		if (m_free_list_pool != NULL) return;
		m_elem_count = 0;
		m_head = m_tail = m_cur = m_start_pt = NULL;
		m_free_list_pool = smpool_create_handle(sizeof(C<T>), MEM_COMM);
		m_free_list.clean();
		m_free_list.set_clean(true);
	}

	void destroy()
	{
		if (m_free_list_pool == NULL) return;
		smpool_free_handle(m_free_list_pool);
		m_free_list_pool = NULL;
		m_elem_count = 0;
		m_head = m_tail = m_cur = m_start_pt = NULL;
	}
	inline C<T> * newc();
	inline C<T> * append_tail(T t);
	inline void append_tail(IN LIST<T> & list);
	inline C<T> * append_head(T t);
	inline void append_head(IN LIST<T> & list);
	inline void append_tail(IN C<T> * c);
	inline void append_head(IN C<T> * c);

	inline void clean();
	inline void copy(IN LIST<T> & src);
	inline UINT count_mem() const;

	inline C<T> * insert_before(T t, T marker);
	inline C<T> * insert_before(T t, IN C<T> * marker);
	inline void insert_before(IN C<T> * c, IN C<T> * marker);
	inline void insert_before(IN LIST<T> & list, T marker,
							  OUT C<T> ** list_head_ct,
							  OUT C<T> ** list_tail_ct);
	inline void insert_before(IN LIST<T> & list, IN C<T> * marker,
							  OUT C<T> ** list_head_ct,
							  OUT C<T> ** list_tail_ct);
	inline C<T> * insert_after(T t, T marker);
	inline C<T> * insert_after(T t, IN C<T> * marker);
	inline void insert_after(IN C<T> * c, IN C<T> * marker);
	inline void insert_after(IN LIST<T> & list, T marker,
							 OUT C<T> ** list_head_ct,
							 OUT C<T> ** list_tail_ct);
	inline void insert_after(IN LIST<T> & list, IN C<T> * marker,
							 OUT C<T> ** list_head_ct,
							 OUT C<T> ** list_tail_ct);

	UINT get_elem_count() const
	{
		IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
		return m_elem_count;
	}
	inline T get_tail(OUT C<T> ** holder = NULL); //update 'm_cur'
	inline T get_head(OUT C<T> ** holder = NULL); //update 'm_cur'

	//Readonly
	inline T get_tail_c(OUT C<T> ** holder = NULL) const; //not update 'm_cur'

	//Readonly
	inline T get_head_c(OUT C<T> ** holder = NULL) const; //not update 'm_cur'

	inline T get_cur() //not update 'm_cur'
	{
		IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
		if (m_cur == NULL) return T(0);
		return C_val(m_cur);
	}
	inline T get_cur(IN OUT C<T> ** holder); //not update 'm_cur'
	inline T get_next(); //update 'm_cur'
	inline T get_prev(); //update 'm_cur'

	//Readonly
	inline T get_next(IN OUT C<T> ** holder) const; //not update 'm_cur'

	//Readonly
	inline T get_prev(IN OUT C<T> ** holder) const; //not update 'm_cur'

	inline T get_tail_nth(INT n, IN OUT C<T> ** holder = NULL);
	inline T get_head_nth(INT n, IN OUT C<T> ** holder = NULL);
	inline bool find(T t, OUT C<T> ** holder = NULL);

	inline void reverse();
	inline T remove(T t);
	inline T remove(IN C<T> * holder);
	inline T remove_tail();
	inline T remove_head();
};


template <class T>
UINT LIST<T>::count_mem() const
{
	UINT count = smpool_get_pool_size_handle(m_free_list_pool);
	count += sizeof(m_elem_count);
	count += sizeof(m_head);
	count += sizeof(m_tail);
	count += sizeof(m_cur);
	count += sizeof(m_start_pt);
	count += m_free_list.count_mem();
	return count;
}


template <class T>
C<T> * LIST<T>::newc()
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	C<T> * c = m_free_list.get_free_elem();
	if (c == NULL) {
		c = (C<T>*)_xmalloc(sizeof(C<T>));
	}
	return c;
}


template <class T>
C<T> * LIST<T>::append_tail(T t)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	C<T> * c  = newc();
	IS_TRUE(c != NULL, ("newc return NULL"));
	C_val(c) = t;
	append_tail(c);
	return c;
}


template <class T>
void LIST<T>::append_tail(C<T> * c)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	if (m_head == NULL) {
		IS_TRUE(m_tail == NULL, ("tail should be NULL"));
		m_head = m_tail = c;
		C_next(m_head) = C_prev(m_head) =
		C_next(m_tail) = C_prev(m_tail) = NULL;
		m_elem_count++;
		return;
	}
	C_prev(c) = m_tail;
	C_next(m_tail) = c;
	m_tail = c;
	C_next(m_tail) = NULL;
	m_elem_count++;
	return;
}


template <class T>
void LIST<T>::append_tail(LIST<T> & list)
{
	IS_TRUE(m_free_list_pool != NULL && list.m_free_list_pool != NULL,
			("LIST not yet initialized."));
	for (T t = list.get_head(); t != T(0); t = list.get_next()) {
		append_tail(t);
	}
}


template <class T>
C<T> * LIST<T>::append_head(T t)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	C<T> * c  = newc();
	IS_TRUE(c != NULL, ("newc return NULL"));
	C_val(c) = t;
	append_head(c);
	return c;
}


template <class T>
void LIST<T>::append_head(C<T> * c)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	if (m_head == NULL) {
		IS_TRUE(m_tail == NULL, ("tail should be NULL"));
		m_head = m_tail = c;
		C_next(m_head) = C_prev(m_head) = NULL;
		m_elem_count++;
		return;
	}
	C_next(c) = m_head;
	C_prev(m_head) = c;
	m_head = c;
	C_prev(m_head) = NULL;
	m_elem_count++;
    return;
}


template <class T>
void LIST<T>::append_head(LIST<T> & list)
{
	IS_TRUE(m_free_list_pool != NULL && list.m_free_list_pool != NULL,
			("LIST not yet initialized."));
	for (T t = list.get_tail(); t != T(0); t = list.get_prev()) {
		append_head(t);
	}
}


template <class T>
bool LIST<T>::find(IN T t, OUT C<T> ** holder)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	C<T> * c = m_head;
	while (c != NULL) {
		if (C_val(c) == t) {
			if (holder  != NULL) {
				*holder = c;
			}
			return true;
		}
		c = C_next(c);
	}
	if (holder  != NULL) {
		*holder = NULL;
	}
	return false;
}


//Reverse list.
template <class T>
void LIST<T>::reverse()
{
	C<T> * next_ct;
	for (C<T> * ct = m_head; ct != NULL; ct = next_ct) {
		next_ct = ct->next;
		ct->next = ct->prev;
		ct->prev = next_ct;
	}
	next_ct = m_head;
	m_head = m_tail;
	m_tail = next_ct;
}


//Remove from list directly.
template <class T>
T LIST<T>::remove(C<T> * holder)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	IS_TRUE0(holder);
	if (holder == m_cur) {
		m_cur = m_cur->next;
	}
	IS_TRUE(m_head != NULL, ("list is empty"));
	if (m_head == holder) {
		return remove_head();
	}
	if (m_tail == holder) {
		return remove_tail();
	}
	IS_TRUE(C_prev(holder) != NULL &&
			C_next(holder) != NULL, ("illegal t in list"));
	m_start_pt = C_next(holder); //recording next search point
	C_next(C_prev(holder)) = C_next(holder);
	C_prev(C_next(holder)) = C_prev(holder);
	m_elem_count--;
	T t = C_val(holder);
	m_free_list.add_free_elem(holder);
	return t;
}


//Remove from list, and searching for 't' begin at head
template <class T>
T LIST<T>::remove(T t)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	if (m_head == NULL) return T(0);
	if (C_val(m_head) == t) {
		return remove_head();
	}
	if (C_val(m_tail) == t) {
		return remove_tail();
	}

	C<T> * c;
	if (m_start_pt != NULL) {
		//Searching for element where starting at the point be visited latest.
		c = m_start_pt;
		while (c != NULL) {
			if (C_val(c) == t) { break; }
			c = C_next(c);
		}
		if (c == NULL) {
			c = C_prev(m_start_pt);
			while (c != NULL) {
				if (C_val(c) == t) { break; }
				c = C_prev(c);
			}
		}
	} else {
		c = m_head;
		while (c != NULL) {
			if (C_val(c) == t) { break; }
			c = C_next(c);
		}
	}
	if (c == NULL) return T(0);
	return remove(c);
}


//Remove from tail
template <class T>
T LIST<T>::remove_tail()
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	C<T> * c = NULL;
	if (m_tail == NULL) { return T(0); }
	if (C_prev(m_tail) == NULL) {
		//list_tail is the only one
		IS_TRUE(m_tail == m_head && m_elem_count == 1,
				("illegal list-remove"));
		c = m_tail;
		m_head = m_tail = m_cur = NULL;
		goto FREE_COLLECT;
	}
	c = m_tail;
	if (m_cur == m_tail) {
		m_cur = C_prev(m_tail);
	}
	m_tail = C_prev(m_tail);
	C_next(m_tail) = NULL;

FREE_COLLECT:
	m_start_pt = m_head;
	T t = C_val(c);
	m_elem_count--;
	m_free_list.add_free_elem(c);
	return t;
}


//Remove from head
template <class T>
T LIST<T>::remove_head()
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	C<T> * c = NULL;
	if (m_head == NULL) { return T(0); }
	if (C_next(m_head) == NULL) {
		//list_head is the only one
		IS_TRUE(m_tail == m_head && m_elem_count == 1,
				("illegal list-remove"));
		c = m_head;
		m_head = m_tail = m_cur = NULL;
		goto FREE_COLLECT;
	}
	c = m_head;
	if (m_cur == m_head) {
		m_cur = C_next(m_head);
	}
	m_head = C_next(m_head);
	C_prev(m_head) = NULL;

FREE_COLLECT:
	m_start_pt = m_head;
	T t = C_val(c);
	m_free_list.add_free_elem(c);
	m_elem_count--;
	return t;
}


template <class T>
void LIST<T>::copy(IN LIST<T> & src)
{
	clean();
	T t = src.get_head();
	for (INT n = src.get_elem_count(); n > 0; n--) {
		append_tail(t);
		t = src.get_next();
	}
}


/*
Clean list, recycle element container, instead of
free memory.
*/
template <class T>
void LIST<T>::clean()
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	C<T> * c = m_head;
	while (c != NULL) {
		C<T> * next = C_next(c);
		m_free_list.add_free_elem(c);
		c = next;
	}
	m_elem_count = 0;
	m_head = m_tail = m_cur = m_start_pt = NULL;
}


template <class T>
C<T> * LIST<T>::insert_before(T t, T marker)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	IS_TRUE(t != marker,("element of list cannot be identical"));
	if (m_head == NULL || marker == C_val(m_head)) {
		append_head(t);
		return m_head;
	}
	C<T> * c = newc();
	IS_TRUE(c != NULL, ("newc return NULL"));
	C_val(c) = t;
	IS_TRUE(m_tail != NULL, ("head is not empty, but tail is NULL!"));
	if (marker == C_val(m_tail)) {
		if (C_prev(m_tail) != NULL) {
			C_next(C_prev(m_tail)) = c;
			C_prev(c) = C_prev(m_tail);
		}
		C_next(c) = m_tail;
		C_prev(m_tail) = c;
	} else {
		//find marker
		C<T> * mc = m_head;
		while (mc != NULL) {
			if (C_val(mc) == marker) {
				break;
			}
			mc = C_next(mc);
		}
		if (mc == NULL) return NULL;
		if (C_prev(mc) != NULL) {
			C_next(C_prev(mc)) = c;
			C_prev(c) = C_prev(mc);
		}
		C_next(c) = mc;
		C_prev(mc) = c;
	}
	m_elem_count++;
	return c;
}


//Insert 'c' into list before the 'marker'.
template <class T>
void LIST<T>::insert_before(IN C<T> * c, IN C<T> * marker)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	if (marker == NULL || c == NULL) {
		return;
	}
	if (c == marker) return;
	if (m_head == NULL || marker == m_head) {
		append_head(c);
		return;
	}
	IS_TRUE(m_tail, ("head is not empty, but tail is NULL!"));
	if (C_prev(marker) != NULL) {
		C_next(C_prev(marker)) = c;
		C_prev(c) = C_prev(marker);
	}
	C_next(c) = marker;
	C_prev(marker) = c;
	m_elem_count++;
}


//Insert 't' into list before the 'marker'.
template <class T>
C<T> * LIST<T>::insert_before(T t, IN C<T> * marker)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	C<T> * c = newc();
	IS_TRUE(c != NULL, ("newc return NULL"));
	C_val(c) = t;
	insert_before(c, marker);
	return c;
}


/*
Insert 'list' before 'marker', and return the CONTAINER
of list head and list tail.
*/
template <class T>
void LIST<T>::insert_before(IN LIST<T> & list,
							T marker,
							OUT C<T> ** list_head_ct,
							OUT C<T> ** list_tail_ct)
{
	C<T> * ct = NULL;
	find(marker, &ct);
	insert_before(list, ct, list_head_ct, list_tail_ct);
}


/*
Insert 'list' before 'marker', and return the CONTAINER
of list head and list tail.
*/
template <class T>
void LIST<T>::insert_before(IN LIST<T> & list,
							IN C<T> * marker,
							OUT C<T> ** list_head_ct,
							OUT C<T> ** list_tail_ct)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	IS_TRUE0(marker);
	UINT i = 0;
	C<T> * head_ct = NULL;
	C<T> * tail_ct = NULL;
	UINT n = list.get_elem_count();
	for (T t = list.get_tail(); i < n; i++, t = list.get_prev()) {
		marker = insert_before(t, marker);
		if (i == 0) {
			tail_ct = marker;
		} else if (i == n - 1) {
			head_ct = marker;
		}
	}
	if (list_head_ct != NULL) {
		*list_head_ct = head_ct;
	}
	if (list_tail_ct != NULL) {
		*list_tail_ct = tail_ct;
	}
}


template <class T>
C<T> * LIST<T>::insert_after(T t, T marker)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	IS_TRUE(t != marker,("element of list cannot be identical"));
	if (m_tail == NULL || marker == C_val(m_tail)) {
		append_tail(t);
		return m_tail;
	}
	IS_TRUE(m_head != NULL, ("Tail is non empty, but head is NULL!"));
	C<T> * c = newc();
	IS_TRUE(c != NULL, ("newc return NULL"));
	C_val(c) = t;
	if (marker == C_val(m_head)) {
		if (C_next(m_head) != NULL) {
			C_prev(C_next(m_head)) = c;
			C_next(c) = C_next(m_head);
		}
		C_prev(c) = m_head;
		C_next(m_head) = c;
	} else {
		//find marker
		C<T> * mc = m_head;
		while (mc != NULL) {
			if (C_val(mc) == marker) {
				break;
			}
			mc = C_next(mc);
		}
		if (mc == NULL) return NULL;
		if (C_next(mc) != NULL) {
			C_prev(C_next(mc)) = c;
			C_next(c) = C_next(mc);
		}
		C_prev(c) = mc;
		C_next(mc) = c;
	}
	m_elem_count++;
	return c;
}


//Insert 'c' into list after the 'marker'.
template <class T>
void LIST<T>::insert_after(IN C<T> * c, IN C<T> * marker)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	if (marker == NULL || c == NULL)
		return;
	if (c == marker)
		return;
	if (m_tail == NULL || marker == m_tail) {
		append_tail(c);
		return;
	}
	IS_TRUE(m_head != NULL, ("tail is not empty, but head is NULL!"));
	if (C_next(marker) != NULL) {
		C_prev(C_next(marker)) = c;
		C_next(c) = C_next(marker);
	}
	C_prev(c) = marker;
	C_next(marker) = c;
	m_elem_count++;
}


//Insert 't' into list after the 'marker'.
template <class T>
C<T> * LIST<T>::insert_after(T t, IN C<T> * marker)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	C<T> * c = newc();
	IS_TRUE(c != NULL, ("newc return NULL"));
	C_val(c) = t;
	insert_after(c, marker);
	return c;
}


/*
Insert 'list' after 'marker', and return the CONTAINER
of list head and list tail.
*/
template <class T>
void LIST<T>::insert_after(IN LIST<T> & list, T marker,
						   OUT C<T> ** list_head_ct,
						   OUT C<T> ** list_tail_ct)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	C<T> * ct = NULL;
	find(marker, &ct);
	insert_after(list, ct, list_head_ct, list_tail_ct);
}


/*
Insert 'list' after 'marker', and return the CONTAINER
of head and tail of members in 'list'.
*/
template <class T>
void LIST<T>::insert_after(IN LIST<T> & list, IN C<T> * marker,
						   OUT C<T> ** list_head_ct,
						   OUT C<T> ** list_tail_ct)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	IS_TRUE0(marker);
	UINT i = 0;
	C<T> * head_ct = NULL;
	C<T> * tail_ct = NULL;
	UINT n = list.get_elem_count();
	for (T t = list.get_head(); i < n;
		 i++, t = list.get_next()) {
		marker = insert_after(t, marker);
		if (i == 0) {
			head_ct = marker;
		} else if (i == n - 1) {
			tail_ct = marker;
		}
	}
	if (list_head_ct) {
		*list_head_ct = head_ct;
	}
	if (list_tail_ct) {
		*list_tail_ct = tail_ct;
	}
}


//Get tail of list, return the CONTAINER.
template <class T>
T LIST<T>::get_tail(OUT C<T> ** holder)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	if (m_head == NULL) {
		if (holder != NULL) {
			*holder = NULL;
		}
		return T(0);
	}
	m_cur = m_tail;
	if (holder != NULL) {
		*holder = m_tail;
	}
	return C_val(m_tail);
}


/*
Get tail of list, return the CONTAINER.
This function will not modify m_cur.
*/
template <class T>
T LIST<T>::get_tail_c(OUT C<T> ** holder) const
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	if (m_head == NULL) {
		if (holder != NULL) {
			*holder = NULL;
		}
		return T(0);
	}
	if (holder != NULL) {
		*holder = m_tail;
	}
	return C_val(m_tail);
}


/*
Get head of list, return the CONTAINER.
This function will not modify m_cur.
*/
template <class T>
T LIST<T>::get_head_c(OUT C<T> ** holder) const
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	if (m_head == NULL) {
		if (holder != NULL) {
			*holder = NULL;
		}
		return T(0);
	}
	if (holder != NULL) {
		*holder = m_head;
	}
	return C_val(m_head);
}


//Get head of list, return the CONTAINER.
template <class T>
T LIST<T>::get_head(OUT C<T> ** holder)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	if (m_head == NULL) {
		if (holder != NULL) {
			*holder = NULL;
		}
		return T(0);
	}
	m_cur = m_head;
	if (holder != NULL) {
		*holder = m_head;
	}
	return C_val(m_head);
}


template <class T>
T LIST<T>::get_next()
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	if (m_cur == NULL || m_cur->next == NULL) {
		m_cur = NULL;
		return T(0);
	}
	m_cur = m_cur->next;
	return C_val(m_cur);
}


template <class T>
T LIST<T>::get_prev()
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	if (m_cur == NULL || m_cur->prev == NULL) {
		m_cur = NULL;
		return T(0);
	}
	m_cur = m_cur->prev;
	return C_val(m_cur);
}


template <class T>
T LIST<T>::get_cur(IN OUT C<T> ** holder)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	if (m_cur == NULL) {
		*holder = NULL;
		return T(0);
	}
	IS_TRUE0(holder != NULL);
	*holder = m_cur;
	return C_val(m_cur);
}


/*
Return list member and update holder to next member.
NOTICE: 'm_cur' will not be updated.
*/
template <class T>
T LIST<T>::get_next(IN OUT C<T> ** holder) const
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	IS_TRUE0(holder != NULL);
	C<T> * c = *holder;
	if (c == NULL || C_next(c) == NULL) {
		*holder = NULL;
		return T(0);
	}
	c  = C_next(c);
	*holder = c;
	return C_val(c);
}


/*
Return list member and update holder to prev member.
NOTICE: 'm_cur' will not be updated.
*/
template <class T>
T LIST<T>::get_prev(IN OUT C<T> ** holder) const
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	C<T> * c = *holder;
	IS_TRUE0(holder != NULL);
	if (c == NULL || C_prev(c) == NULL) {
		*holder = NULL;
		return T(0);
	}
	c  = C_prev(c);
	*holder = c;
	return C_val(c);
}


/*
Get element for nth at tail.
'n': starting at 0.
*/
template <class T>
T LIST<T>::get_tail_nth(INT n, IN OUT C<T> ** holder)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	IS_TRUE(n >= 0 && n < m_elem_count,("Access beyond list"));
	m_cur = NULL;
	if (m_elem_count == 0) return T(0);
	C<T> * c;
	if (n <= (m_elem_count>>1)) {// n<floor(elem_count,2)
		c = m_tail;
		while (n > 0) {
			c = C_prev(c);
			n--;
		}
	} else {
		return get_head_nth(m_elem_count - n - 1);
	}
	m_cur = c;
	if (holder != NULL) {
		*holder = c;
	}
	return C_val(c);
}


/*
Get element for nth at head.
'n': getting start with zero.
*/
template <class T>
T LIST<T>::get_head_nth(INT n, IN OUT C<T> ** holder)
{
	IS_TRUE(m_free_list_pool != NULL, ("LIST not yet initialized."));
	IS_TRUE(n >= 0 && n < m_elem_count,("Access beyond list"));
	m_cur = NULL;
	if (m_head == NULL) {
		return T(0);
	}
	C<T> * c;
	if (n <= (m_elem_count>>1)) {// n<floor(elem_count,2)
		c = m_head;
		while (n > 0) {
			c = C_next(c);
			n--;
		}
	} else {
		return get_tail_nth(m_elem_count - n - 1);
	}
	m_cur = c;
	if (holder != NULL) {
		*holder = c;
	}
	return C_val(c);
}
//END LIST


/*
Single LIST Core
Encapsulate most operations for list.
*/
template <class T> class SLISTC {
private:
	SLISTC(SLISTC const&) { IS_TRUE(0, ("Do not invoke copy-constructor.")); }
	void operator = (SLISTC const&)
	{ IS_TRUE(0, ("Do not invoke copy-constructor.")); }
protected:
	INT m_elem_count;
	SC<T> * m_head;
	SC<T> * m_tail;

	void * _xmalloc(ULONG size, SMEM_POOL * pool)
	{
		IS_TRUE(pool != NULL, ("LIST not yet initialized."));
		void * p = smpool_malloc_h(size, pool);
		IS_TRUE0(p != NULL);
		memset(p, 0, size);
		return p;
	}

	bool in_list(SC<T> const* p) const
	{
		if (p == NULL) { return true; }
		SC<T> const* t = m_head;
		while (t != NULL) {
			if (t == p) { return true; }
			t = t->next;
		}
		return false;
	}
public:
	SLISTC()
	{
		m_elem_count = 0;
		m_head = m_tail = NULL;
	}
	~SLISTC() {}

	SC<T> * get_one_sc(SC<T> ** free_list)
	{
		if (free_list == NULL || *free_list == NULL) { return NULL; }
		SC<T> * t = *free_list;
		*free_list = (*free_list)->next;
		t->next = NULL;
		return t;
	}

	void free_sc(SC<T> * sc, SC<T> ** free_list)
	{
		IS_TRUE0(free_list);
		sc->next = *free_list;
		*free_list = sc;
	}

	SC<T> * newsc(SC<T> ** free_list, SMEM_POOL * pool)
	{
		SC<T> * c = get_one_sc(free_list);
		if (c == NULL) {
			c = (SC<T>*)_xmalloc(sizeof(SC<T>), pool);
		}
		return c;
	}

	SC<T> * append_tail(T t, SC<T> ** free_list, SMEM_POOL * pool)
	{
		SC<T> * c  = newsc(free_list, pool);
		IS_TRUE(c != NULL, ("newc return NULL"));
		SC_val(c) = t;
		append_tail(c);
		return c;
	}

	void append_tail(IN SC<T> * c)
	{
		if (m_head == NULL) {
			IS_TRUE(m_tail == NULL, ("tail should be NULL"));
			m_head = m_tail = c;
			SC_next(m_head) = NULL;
			m_elem_count++;
			return;
		}
		SC_next(m_tail) = c;
		m_tail = c;
		SC_next(m_tail) = NULL;
		m_elem_count++;
		return;
	}

	SC<T> * append_head(T t, SC<T> ** free_list, SMEM_POOL * pool)
	{
		SC<T> * c = newsc(free_list, pool);
		IS_TRUE(c != NULL, ("newc return NULL"));
		SC_val(c) = t;
		append_head(c);
		return c;
	}

	void append_head(IN SC<T> * c)
	{
		if (m_head == NULL) {
			IS_TRUE(m_tail == NULL, ("tail should be NULL"));
			IS_TRUE0(m_elem_count == 0);
			m_head = m_tail = c;
			m_head->next = NULL;
			m_elem_count++;
			return;
		}
		c->next = m_head;
		m_head = c;
		m_elem_count++;
		return;
	}

	void copy(IN SLISTC<T> & src, SC<T> ** free_list, SMEM_POOL * pool)
	{
		clean(free_list);
		SC<T> * sct;
		T t = src.get_head(&sct);
		for (INT n = src.get_elem_count(); n > 0; n--) {
			append_tail(t, free_list, pool);
			t = src.get_next(&sct);
		}
	}

	void clean(SC<T> ** free_list)
	{
		SC<T> * c = m_head;
		while (c != NULL) {
			SC<T> * next = c->next;
			c->next = NULL;
			free_sc(c, free_list);
			c = next;
		}
		m_elem_count = 0;
		m_head = m_tail = NULL;
	}

	UINT count_mem() const
	{
		UINT count = sizeof(m_elem_count);
		count += sizeof(m_head);
		count += sizeof(m_tail);
		//Do not count SC, they belong to input pool.
		return count;
	}

	//Insert 'c' into list after the 'marker'.
	void insert_after(IN SC<T> * c, IN SC<T> * marker)
	{
		if (marker == NULL || c == NULL) { return; }
		IS_TRUE0(in_list(marker));
		if (c == marker) { return; }
		if (m_tail == NULL || marker == m_tail) {
			append_tail(c);
			return;
		}
		c->next = marker->next;
		marker->next = c;
		m_elem_count++;
	}

	//Insert 't' into list after the 'marker'.
	SC<T> * insert_after(T t, IN SC<T> * marker, SC<T> ** free_list,
						 SMEM_POOL * pool)
	{
		if (marker == NULL) { return NULL; }
		IS_TRUE0(in_list(marker));
		SC<T> * c = newsc(free_list, pool);
		IS_TRUE(c != NULL, ("newc return NULL"));
		SC_val(c) = t;
		insert_after(c, marker);
		return c;
	}

	UINT get_elem_count() const
	{ return m_elem_count; }

	//Get tail of list, return the CONTAINER.
	T get_tail(OUT SC<T> ** holder) const
	{
		if (m_tail == NULL) {
			if (holder != NULL) {
				*holder = NULL;
			}
			return T(0);
		}
		if (holder != NULL) {
			*holder = m_tail;
		}
		return SC_val(m_tail);
	}

	//Get head of list, return the CONTAINER.
	T get_head(OUT SC<T> ** holder) const
	{
		if (m_head == NULL) {
			if (holder != NULL) {
				*holder = NULL;
			}
			return T(0);
		}
		if (holder != NULL) {
			*holder = m_head;
		}
		return SC_val(m_head);
	}

	//Return list member and update holder to next member.
	T get_next(IN OUT SC<T> ** holder) const
	{
		IS_TRUE0(holder != NULL && in_list(*holder));
		SC<T> * c = *holder;
		if (c == NULL || c->next == NULL) {
			*holder = NULL;
			return T(0);
		}
		IS_TRUE0(m_head || m_tail);
		c = c->next;
		*holder = c;
		return SC_val(c);
	}

	//Find 't' in list, return the container in 'holder' if 't' existed.
	//The function is regular list search, and has O(n) complexity.
	bool find(IN T t, OUT SC<T> ** holder = NULL) const
	{
		SC<T> * c = m_head;
		while (c != NULL) {
			if (SC_val(c) == t) {
				if (holder  != NULL) {
					*holder = c;
				}
				return true;
			}
			c = c->next;
		}
		if (holder != NULL) {
			*holder = NULL;
		}
		return false;
	}

	//Remove 't' out of list, return true if find t, otherwise return false.
	//Note that this is costly operation.
	bool remove(T t, SC<T> ** free_list)
	{
		if (m_head == NULL) { return false; }
		if (SC_val(m_head) == t) {
			remove_head(free_list);
			return true;
		}
		SC<T> * c = m_head->next;
		SC<T> * prev = m_head;
		while (c != NULL) {
			if (SC_val(c) == t) { break; }
			prev = c;
			c = c->next;
		}
		if (c == NULL) return false;
		remove(prev, c, free_list);
		return true;
	}

	//Return the element removed.
	//'prev': the previous one element of 'holder'.
	T remove(SC<T> * prev, SC<T> * holder, SC<T> ** free_list)
	{
		IS_TRUE0(holder);
		IS_TRUE(m_head != NULL, ("list is empty"));
		IS_TRUE0(in_list(prev));
		IS_TRUE0(in_list(holder));
		if (prev == NULL) {
			IS_TRUE0(holder == m_head);
			m_head = m_head->next;
			if (m_head == NULL) {
				IS_TRUE0(m_elem_count == 1);
				m_tail = NULL;
			}
		} else {
			IS_TRUE(prev->next == holder, ("not prev one"));
			prev->next = holder->next;
		}
		if (holder == m_tail) {
			m_tail = prev;
		}
		m_elem_count--;
		T t = SC_val(holder);
		free_sc(holder, free_list);
		return t;
	}

	//Return the element removed.
	T remove_head(SC<T> ** free_list)
	{
		if (m_head == NULL) { return T(0); }
		SC<T> * c = m_head;
		m_head = m_head->next;
		if (m_head == NULL) {
			m_tail = NULL;
		}
		T t = SC_val(c);
		free_sc(c, free_list);
		m_elem_count--;
		return t;
	}
};
//END SLISTC


/*
Single LIST

NOTICE:
	The following 3 operations are the key points which you should
	attention to:
	1.	If you REMOVE one element, its container will be collect by FREE-LIST.
		So if you need a new container, please check the FREE-LIST first,
		accordingly, you should first invoke 'get_free_list' which get free
		containers out from 'm_free_list'.
  	2.	If you want to invoke APPEND, please call 'newXXX' first to
		allocate a new container memory space, record your elements into
		the container, then APPEND it at list.
		newXXX such as:
			T * newXXX(INT type)
			{
				T * t = get_free_T();
				if (t == 0) { t = (T*)malloc(sizeof(T)); }
				T_type(c) = type;
				return t;
			}
*/
template <class T> class SLIST : public SLISTC<T> {
private:
	SLIST(SLIST const&) { IS_TRUE(0, ("Do not invoke copy-constructor.")); }
	void operator = (SLIST const&)
	{ IS_TRUE(0, ("Do not invoke copy-constructor.")); }
protected:
	SMEM_POOL * m_free_list_pool;
	SC<T> * m_free_list; //Hold for available containers
public:
	SLIST(SMEM_POOL * pool = NULL)
	{
		m_free_list_pool = pool;
		m_free_list = NULL;
	}

	~SLIST() {}

	void set_pool(SMEM_POOL * pool) { m_free_list_pool = pool; }
	SMEM_POOL * get_pool() { return m_free_list_pool; }

	SC<T> * append_tail(T t)
	{
		IS_TRUE0(m_free_list_pool);
		return SLISTC<T>::append_tail(t, &m_free_list, m_free_list_pool);
	}

	SC<T> * append_head(T t)
	{
		IS_TRUE0(m_free_list_pool);
		return SLISTC<T>::append_head(t, &m_free_list, m_free_list_pool);
	}

	void copy(IN SLIST<T> & src)
	{
		SLISTC<T>::clean(&m_free_list);
		SC<T> * sct;
		T t = src.get_head(&sct);
		for (INT n = src.get_elem_count(); n > 0; n--) {
			SLISTC<T>::append_tail(t, &m_free_list, m_free_list_pool);
			t = src.get_next(&sct);
		}
	}

	void clean() { SLISTC<T>::clean(&m_free_list); }

	UINT count_mem() const
	{
		UINT count = sizeof(m_free_list);
		count += sizeof(m_free_list_pool);
		count += SLISTC<T>::count_mem();
		//Do not count SC, they belong to input pool.
		return count;
	}

	//Insert 't' into list after the 'marker'.
	SC<T> * insert_after(T t, IN SC<T> * marker)
	{
		IS_TRUE0(m_free_list_pool);
		return SLISTC<T>::insert_after(t, marker, &m_free_list, m_free_list_pool);
	}

	//Remove 't' out of list, return true if find t, otherwise return false.
	//Note that this is costly operation.
	bool remove(T t)
	{
		IS_TRUE0(m_free_list_pool);
		return SLISTC<T>::remove(t, &m_free_list);
	}

	//Return element removed.
	//'prev': the previous one element of 'holder'.
	T remove(SC<T> * prev, SC<T> * holder)
	{
		IS_TRUE0(m_free_list_pool);
		return SLISTC<T>::remove(prev, holder, &m_free_list);
	}

	//Return element removed.
	T remove_head()
	{
		IS_TRUE0(m_free_list_pool);
		return SLISTC<T>::remove_head(&m_free_list);
	}
};
//END SLIST



/*
The Extended LIST

Add a hash-mapping table upon LIST in order to speed up the process
when inserting or removing an element if a 'marker' given.

NOTICE: User must define a mapping class bewteen.
*/
template <class T, class MAP_T2HOLDER> class ELIST : public LIST<T> {
protected:
	MAP_T2HOLDER m_t2holder_map; //map 't' to its LIST HOLDER.
public:
	ELIST(UINT bsize = MAX_SHASH_BUCKET) {}
	virtual ~ELIST() {} //MAP_T2HOLDER has virtual function.

	inline void copy(IN LIST<T> & src);
	void clean()
	{
		LIST<T>::clean();
		m_t2holder_map.clean();
	}
	inline UINT count_mem() const;

	C<T> * append_tail(T t)
	{
		C<T> * c = LIST<T>::append_tail(t);
		m_t2holder_map.aset(t, c);
		return c;
	}

	C<T> * append_head(T t)
	{
		C<T> * c = LIST<T>::append_head(t);
		m_t2holder_map.aset(t, c);
		return c;
	}

	inline void append_tail(IN LIST<T> & list);
	inline void append_head(IN LIST<T> & list);

	inline T remove(T t);
	T remove(C<T> * holder)
	{
		IS_TRUE0(m_t2holder_map.get(C_val(holder)) == holder);
		T t = LIST<T>::remove(holder);
		m_t2holder_map.aset(t, NULL);
		return t;
	}
	T remove_tail()
	{
		T t = LIST<T>::remove_tail();
		m_t2holder_map.aset(t, NULL);
		return t;
	}
	T remove_head()
	{
		T t = LIST<T>::remove_head();
		m_t2holder_map.aset(t, NULL);
		return t;
	}

	inline C<T> * insert_before(T t, T marker);
	inline C<T> * insert_before(T t, C<T> * marker);
	inline void insert_before(C<T> * c, C<T> * marker);
	inline C<T> * insert_after(T t, T marker);
	inline C<T> * insert_after(T t, C<T> * marker);
	inline void insert_after(C<T> * c, C<T> * marker);
	inline bool find(T t, C<T> ** holder = NULL);

	MAP_T2HOLDER * get_holder_map() { return &m_t2holder_map; }
	T get_cur() //Do NOT update 'm_cur'
	{ return LIST<T>::get_cur(); }
	T get_cur(IN OUT C<T> ** holder) //Do NOT update 'm_cur'
	{ return LIST<T>::get_cur(holder); }

	T get_next() //Update 'm_cur'
	{ return LIST<T>::get_next(); }
	T get_prev() //Update 'm_cur'
	{ return LIST<T>::get_prev(); }
	T get_next(IN OUT C<T> ** holder) //Do NOT update 'm_cur'
	{ return LIST<T>::get_next(holder); }
	T get_prev(IN OUT C<T> ** holder) //Do NOT update 'm_cur'
	{ return LIST<T>::get_prev(holder); }
	T get_next(T marker) //not update 'm_cur'
	{
		C<T> * holder;
		find(marker, &holder);
		IS_TRUE0(holder  != NULL);
		return LIST<T>::get_next(&holder);
	}
	T get_prev(T marker) //not update 'm_cur'
	{
		C<T> * holder;
		find(marker, &holder);
		IS_TRUE0(holder  != NULL);
		return LIST<T>::get_prev(&holder);
	}

	inline C<T> * replace(T oldt, T newt);
};


template <class T, class MAP_T2HOLDER>
void ELIST<T, MAP_T2HOLDER>::append_tail(IN LIST<T> & list)
{
	UINT i = 0;
	C<T> * c;
	for (T t = list.get_head(); i < list.get_elem_count();
		 i++, t = list.get_next()) {
		c = LIST<T>::append_tail(t);
		m_t2holder_map.aset(t, c);
	}
}


template <class T, class MAP_T2HOLDER>
void ELIST<T, MAP_T2HOLDER>::append_head(IN LIST<T> & list)
{
	UINT i = 0;
	C<T> * c;
	for (T t = list.get_tail(); i < list.get_elem_count();
		 i++, t = list.get_prev()) {
		c = LIST<T>::append_head(t);
		m_t2holder_map.aset(t, c);
	}
}


template <class T, class MAP_T2HOLDER>
bool ELIST<T, MAP_T2HOLDER>::find(T t, OUT C<T> ** holder)
{
	C<T> * c = m_t2holder_map.get(t);
	if (c == NULL) {
		return false;
	}
	if (holder != NULL) {
		*holder = c;
	}
	return true;
}


template <class T, class MAP_T2HOLDER>
T ELIST<T, MAP_T2HOLDER>::remove(T t)
{
	C<T> * c = m_t2holder_map.get(t);
	if (c == NULL) {
		return T(0);
	}
	T tt = LIST<T>::remove(c);
	m_t2holder_map.aset(t, NULL);
	return tt;
}


template <class T, class MAP_T2HOLDER>
void ELIST<T, MAP_T2HOLDER>::copy(IN LIST<T> & src)
{
	clean();
	T t = src.get_head();
	for (INT n = src.get_elem_count(); n > 0; n--) {
		append_tail(t);
		t = src.get_next();
	}
}


template <class T, class MAP_T2HOLDER>
UINT ELIST<T, MAP_T2HOLDER>::count_mem() const
{
	UINT count = m_t2holder_map.count_mem();
	count += ((LIST<T>*)this)->count_mem();
	return count;
}


template <class T, class MAP_T2HOLDER>
C<T> * ELIST<T, MAP_T2HOLDER>::insert_before(T t, T marker)
{
	C<T> * marker_holder = m_t2holder_map.get(marker);
	if (marker_holder == NULL) {
		IS_TRUE0(LIST<T>::get_elem_count() == 0);
		C<T> * t_holder = LIST<T>::append_head(t);
		m_t2holder_map.aset(t, t_holder);
		return t_holder;
	}
	C<T> * t_holder = LIST<T>::insert_before(t, marker_holder);
	m_t2holder_map.aset(t, t_holder);
	return t_holder;
}


//NOTICE: 'marker' should have been in the list.
template <class T, class MAP_T2HOLDER>
C<T> * ELIST<T, MAP_T2HOLDER>::insert_before(T t, IN C<T> * marker)
{
	IS_TRUE0(marker && m_t2holder_map.get(C_val(marker)) == marker);
	C<T> * t_holder = LIST<T>::insert_before(t, marker);
	m_t2holder_map.aset(t, t_holder);
	return t_holder;
}


//NOTICE: 'marker' should have been in the list.
template <class T, class MAP_T2HOLDER>
void ELIST<T, MAP_T2HOLDER>::insert_before(IN C<T> * c, IN C<T> * marker)
{
	IS_TRUE0(c && marker && m_t2holder_map.get(C_val(marker)) == marker);
	LIST<T>::insert_before(c, marker);
	m_t2holder_map.aset(C_val(c), c);
}


//NOTICE: 'marker' should have been in the list.
template <class T, class MAP_T2HOLDER>
C<T> * ELIST<T, MAP_T2HOLDER>::insert_after(T t, IN C<T> * marker)
{
	IS_TRUE0(marker && m_t2holder_map.get(C_val(marker)) == marker);
	C<T> * marker_holder = marker;
	C<T> * t_holder = LIST<T>::insert_after(t, marker_holder);
	m_t2holder_map.aset(t, t_holder);
	return t_holder;
}


//NOTICE: 'marker' should have been in the list.
template <class T, class MAP_T2HOLDER>
void ELIST<T, MAP_T2HOLDER>::insert_after(IN C<T> * c, IN C<T> * marker)
{
	IS_TRUE0(c && marker && m_t2holder_map.get(C_val(marker)) == marker);
	LIST<T>::insert_after(c, marker);
	m_t2holder_map.aset(C_val(c), c);
}


template <class T, class MAP_T2HOLDER>
C<T> * ELIST<T, MAP_T2HOLDER>::insert_after(T t, T marker)
{
	C<T> * marker_holder = m_t2holder_map.get(marker);
	if (marker_holder == NULL) {
		IS_TRUE0(LIST<T>::get_elem_count() == 0);
		C<T> * t_holder = LIST<T>::append_tail(t);
		m_t2holder_map.aset(t, t_holder);
		return t_holder;
	}
	C<T> * t_holder = LIST<T>::insert_after(t, marker_holder);
	m_t2holder_map.aset(t, t_holder);
	return t_holder;
}


//Return the container of 'newt'.
template <class T, class MAP_T2HOLDER>
C<T> * ELIST<T, MAP_T2HOLDER>::replace(T oldt, T newt)
{
	C<T> * old_holder = m_t2holder_map.get(oldt);
	IS_TRUE(old_holder != NULL, ("old elem not exist"));

	//add new one
	C<T> * new_holder = LIST<T>::insert_before(newt, old_holder);
	m_t2holder_map.aset(newt, new_holder);

	//remove old one
	m_t2holder_map.aset(oldt, NULL);
	LIST<T>::remove(old_holder);
	return new_holder;
}
//END ELIST



//STACK
template <class T> class SSTACK : public LIST<T> {
protected:
	bool m_is_init;
public:
	SSTACK();
	~SSTACK();
	void init();
	void destroy();
	inline void push(T t);
	inline UINT count_mem() const;
	inline T pop();
	inline T get_top();
	inline T get_bottom();
	inline T get_top_nth(INT n);
	inline T get_bottom_nth(INT n);
};


template <class T>
SSTACK<T>::SSTACK()
{
	m_is_init = false;
	init();
}


template <class T>
SSTACK<T>::~SSTACK()
{
	destroy();
}


template <class T>
void SSTACK<T>::init()
{
	if (m_is_init) return;
	m_is_init = true;
}


template <class T>
void SSTACK<T>::destroy()
{
	if (!m_is_init) return;
	m_is_init = false;
}


template <class T>
UINT SSTACK<T>::count_mem() const
{
	return ((LIST<T>*)this)->count_mem() +
			sizeof(m_is_init);
}


template <class T>
void SSTACK<T>::push(T t)
{
	LIST<T>::append_tail(t);
}


template <class T>
T SSTACK<T>::pop()
{
	return LIST<T>::remove_tail();
}


template <class T>
T SSTACK<T>::get_bottom()
{
	return LIST<T>::get_head();
}


template <class T>
T SSTACK<T>::get_top()
{
	return LIST<T>::get_tail();
}


template <class T>
T SSTACK<T>::get_top_nth(INT n)
{
	return LIST<T>::get_tail_nth(n);
}


template <class T>
T SSTACK<T>::get_bottom_nth(INT n)
{
	return LIST<T>::get_head_nth(n);
}
//END SSTACK



/*
SVECTOR

T refer to element type.
Zero is treated as the default NULL when we
determine the element existence.

Growth dynamic

*/
#define ELEM_GROW_SIZE 8
template <class T> class SVECTOR {
private:
	void operator = (SVECTOR const&)
	{ IS_TRUE(0, ("Do not invoke copy-constructor.")); }
protected:
	INT m_elem_num;	//The number of element in vector.
	INT m_last_idx;	//Last element idx

	//Always refers to free space to SVECTOR,
	//and the first free space position is '0'
	INT m_free_idx;

	//The number of element when vector is growing.
	INT m_grow_size:30;

	INT m_is_init:1;
public:
	T * m_vec;

	SVECTOR();
	explicit SVECTOR(INT size);
	SVECTOR(SVECTOR const& vec);
	~SVECTOR();
	void init();
	void destroy();
	inline void copy(LIST<T> & list);
	inline void copy(SVECTOR<T> const& vec);
	inline void clean();
	inline UINT count_mem() const;
	inline void set_grow_size(UINT i);
	inline void set(INT i, T elem);
	inline T get(INT i) const;
	inline const T operator[](INT i) const;
	inline T & operator[](INT i);
	inline INT get_size() const;
	inline INT get_last_idx() const;
	inline INT get_free_idx(T v = T(0));
	inline void add(T t);
	inline void grow(INT size);
};


template <class T>
SVECTOR<T>::SVECTOR()
{
	m_is_init = false;
	init();
}


template <class T>
SVECTOR<T>::SVECTOR(INT size)
{
	m_is_init = false;
	init();
	grow(size);
}


template <class T>
SVECTOR<T>::SVECTOR(SVECTOR const& vec)
{
	copy(vec);
}


template <class T>
SVECTOR<T>::~SVECTOR()
{
	destroy();
}


template <class T>
void SVECTOR<T>::init()
{
	if (m_is_init) return;
	m_elem_num = 0;
	m_grow_size = ELEM_GROW_SIZE;
	m_vec = NULL;
	m_last_idx = -1;
	m_free_idx = -1;
	m_is_init = true;
}


template <class T>
void SVECTOR<T>::destroy()
{
	if (!m_is_init) return;
	m_elem_num = 0;
	if (m_vec != NULL) {
		::free(m_vec);
	}
	m_vec = NULL;
	m_last_idx = -1;
	m_free_idx = -1;
	m_is_init = false;
}


template <class T>
void SVECTOR<T>::set_grow_size(UINT i)
{
	IS_TRUE(m_is_init, ("VECTOR not yet initialized."));
	m_grow_size = i;
}


template <class T>
T SVECTOR<T>::get(INT i) const
{
	IS_TRUE(m_is_init, ("VECTOR not yet initialized."));
	if (i < 0 || i >= m_elem_num) return T(0);
	return m_vec[i];
}


//Overloaded [] for const array reference create an rvalue.
//Be equal to 'get()'
template <class T>
T const SVECTOR<T>::operator[](INT i) const
{
	IS_TRUE(m_is_init, ("VECTOR not yet initialized."));
	IS_TRUE(i >= 0 && i < m_elem_num, ("array subscript over boundary."));
	return m_vec[i];
}


//Overloaded [] for non-const array reference create an lvalue.
//Be equal to 'set()'
template <class T>
T & SVECTOR<T>::operator[](INT i)
{
	IS_TRUE(m_is_init, ("VECTOR not yet initialized."));
	IS_TRUE(i >= 0, ("array subscript over boundary."));
	if (i >= m_elem_num) {
		set(i, T(0));
	}
	m_last_idx = MAX(i, m_last_idx);
	return m_vec[i];
}


/*
Copy each elements of 'list' into vector.

CAUTION: The default termination factor is '0'.
HINT: While we traversing elements of LIST one by one, or from head to
	tail or on opposition way, one can copy list into vector first and
	iterating the vector instead of travering list.
*/
template <class T>
void SVECTOR<T>::copy(LIST<T> & list)
{
	INT count = 0;
	set(list.get_elem_count() - 1, 0); //Alloc memory right away.
	for (T elem = list.get_head();
		 elem != T(0); elem = list.get_next(), count++) {
		set(count, elem);
	}
}


template <class T>
void SVECTOR<T>::copy(SVECTOR<T> const& vec)
{
	IS_TRUE0(vec.m_elem_num > 0 ||
			 (vec.m_elem_num == 0 && vec.m_last_idx == -1));
	if (m_elem_num < vec.m_elem_num) {
		destroy();
		init();
		grow(vec.m_elem_num);
	}
	if (vec.m_elem_num > 0) {
		memcpy(m_vec, vec.m_vec, sizeof(T) * vec.m_elem_num);
	}
	m_last_idx = vec.m_last_idx;
	m_free_idx = vec.m_free_idx;
}


template <class T>
UINT SVECTOR<T>::count_mem() const
{
	return m_elem_num + sizeof(SVECTOR<T>);
}


//Clean to zero(default) till 'last_idx'
template <class T>
void SVECTOR<T>::clean()
{
	IS_TRUE(m_is_init, ("VECTOR not yet initialized."));
	if (m_last_idx < 0) {
		return;
	}
	memset(m_vec, 0, sizeof(T) * (m_last_idx + 1));
	m_free_idx = 0;
	m_last_idx = -1; //Not any elements
}


//Return NULL if 'i' is illegal, otherwise return 'elem'.
template <class T>
void SVECTOR<T>::set(INT i, T elem)
{
	IS_TRUE(m_is_init, ("VECTOR not yet initialized."));
	if (i < 0) return;
	if (i >= m_elem_num) {
		UINT l = i;
		l = (l / m_grow_size + 1) * m_grow_size;
		grow(l);
	}
	m_last_idx = MAX(i, m_last_idx);
	m_vec[i] = elem;
	return;
}


/*
Growing vector by size, if 'm_size' is NULL , alloc vector.
Reallocate memory if necessory.
*/
template <class T>
void SVECTOR<T>::grow(INT size)
{
	IS_TRUE(m_is_init, ("VECTOR not yet initialized."));
	if (size <= 0) return;
	if (m_elem_num == 0) {
		IS_TRUE(m_vec == NULL, ("vector should be NULL if size is zero."));
		m_vec = (T*)::malloc(sizeof(T) * size);
		memset(m_vec, 0, sizeof(T) * size);
		m_elem_num = size;
		m_free_idx = 0;
		return;
	}
	T * tmp = (T*)::malloc((m_elem_num + size) * sizeof(T));
	memcpy(tmp, m_vec, m_elem_num * sizeof(T));
	memset(((CHAR*)tmp) + m_elem_num * sizeof(T), 0 , size * sizeof(T));
	::free(m_vec);
	m_vec = tmp;
	m_elem_num += size;
}


template <class T>
INT SVECTOR<T>::get_size() const
{
	IS_TRUE(m_is_init, ("VECTOR not yet initialized."));
	return m_elem_num;
}


template <class T>
void SVECTOR<T>::add(T t)
{
	IS_TRUE(m_is_init, ("VECTOR not yet initialized."));
	set(m_last_idx < 0 ? 0 : m_last_idx, t);
}


template <class T>
INT SVECTOR<T>::get_last_idx() const
{
	IS_TRUE(m_is_init, ("VECTOR not yet initialized."));
	IS_TRUE(m_last_idx < m_elem_num, ("Last index ran over SVECTOR's size."));
	return m_last_idx;
}


/*
Return index of free-slot into SVECTOR, and realloc memory if there are
not any free-slot
NOTICE:
	The condition that we considered a slot is free is that whether if the
	value of the slot equals v(means void).
*/
template <class T>
INT SVECTOR<T>::get_free_idx(T v)
{
	IS_TRUE(m_is_init, ("VECTOR not yet initialized."));
	if (m_free_idx < 0) {
		IS_TRUE(!m_elem_num && m_last_idx < 0 && !m_vec,
				("exception occur in SVECTOR"));
		grow(m_grow_size);

		//Free space is started at position '0',
		//so next free space is position '1'.
		m_free_idx = 1;
		return 0;
	} else {
		INT ret = m_free_idx;
		INT i;

		//Seaching in second-half SVECTOR of m_free_idx.
		for (i = m_free_idx + 1; i < m_elem_num; i++) {
			if (v == m_vec[i]) {
				m_free_idx = i;
				return ret;
			}
		}

		//Seaching in first-half SVECTOR of m_free_idx.
		for (i = 0; i < m_free_idx; i++) {
			if (v == m_vec[i]) {
				m_free_idx = i;
				return ret;
			}
		}
		m_free_idx = m_elem_num;
		grow(m_elem_num);
		return ret;
	}
	IS_TRUE(0,("Over here?!"));
}
//END SVECTOR



/*
SHASH

Hash element recorded not only in hash table but also in SVECTOR,
which is used in order to speed up accessing hashed elements.

NOTICE:
	1.T(0) is defined as default NULL in SHASH, so do not use T(0) as element.

	2.There are four hash function classes are given as default, and
	  if you are going to define you own hash function class, the
	  following member functions you should supply according to your needs.

		* Return hash-key deduced from 'val'.
			UINT get_hash_value(ULONG val) const

		* Return hash-key deduced from 't'.
			UINT get_hash_value(T * t) const

		* Compare t1, t2 when inserting a new element.
			bool compare(T * t1, T * t2) const

		* Compare t1, val when inserting a new element.
			bool compare(T * t1, ULONG val) const

	3.Use 'new'/'delete' operator to allocate/free the memory
	  of dynamic object and the virtual function pointers.
*/
#define HC_val(c)			(c)->val
#define HC_vec_idx(c)		(c)->vec_idx
#define HC_next(c)			(c)->next
#define HC_prev(c)			(c)->prev
template <class T> struct HC {
	HC<T> * prev;
	HC<T> * next;
	UINT vec_idx;
	T val;
};

#define SHB_member(hm)		(hm).hash_member
#define SHB_count(hm)		(hm).hash_member_count
class SHASH_BUCKET {
public:
    void * hash_member; //hash member list
	UINT hash_member_count; //
};


template <class T> class HASH_FUNC_BASE {
public:
	UINT get_hash_value(T t, UINT bucket_size) const
	{
		IS_TRUE0(bucket_size != 0);
		return ((UINT)(ULONG)t) % bucket_size;
	}

	UINT get_hash_value(ULONG val, UINT bucket_size) const
	{
		IS_TRUE0(bucket_size != 0);
		return ((UINT)val) % bucket_size;
	}

	bool compare(T t1, T t2) const
	{ return t1 == t2; }

	bool compare(T t1, ULONG val) const
	{ return t1 == (T)val; }
};


template <class T> class HASH_FUNC_BASE2 : public HASH_FUNC_BASE<T> {
public:
	UINT get_hash_value(T t, UINT bucket_size) const
	{
		IS_TRUE0(bucket_size != 0);
		IS_TRUE0(is_power_of_2(bucket_size));
		return hash32bit((UINT)(ULONG)t) & (bucket_size - 1);
	}

	UINT get_hash_value(ULONG val, UINT bucket_size) const
	{
		IS_TRUE0(bucket_size != 0);
		IS_TRUE0(is_power_of_2(bucket_size));
		return hash32bit((UINT)val) & (bucket_size - 1);
	}
};


class HASH_FUNC_STR {
public:
	UINT get_hash_value(CHAR const* s, UINT bucket_size) const
	{
		IS_TRUE0(bucket_size != 0);
		UINT v = 0;
		while (*s++) {
			v += (UINT)(*s);
		}
		return hash32bit(v) % bucket_size;
	}

	UINT get_hash_value(ULONG v, UINT bucket_size) const
	{
		IS_TRUE(sizeof(ULONG) == sizeof(CHAR*),
				("exception will taken place in type-cast"));
		return get_hash_value((CHAR const*)v, bucket_size);
	}

	bool compare(CHAR const* s1, CHAR const* s2) const
	{ return strcmp(s1, s2) == 0; }

	bool compare(CHAR const* s, ULONG val) const
	{
		IS_TRUE(sizeof(ULONG) == sizeof(CHAR const*),
				("exception will taken place in type-cast"));
		return (strcmp(s,  (CHAR const*)val) == 0);
	}
};


class HASH_FUNC_STR2 : public HASH_FUNC_STR {
public:
	UINT get_hash_value(CHAR const* s, UINT bucket_size) const
	{
		IS_TRUE0(bucket_size != 0);
		UINT v = 0;
		while (*s++) {
			v += (UINT)(*s);
		}
		IS_TRUE0(is_power_of_2(bucket_size));
		return hash32bit(v) & (bucket_size - 1);
	}
};


//'T': the element type.
//'HF': Hash function type.
template <class T, class HF = HASH_FUNC_BASE<T> > class SHASH {
protected:
	SMEM_POOL * m_free_list_pool;
	FREE_LIST<HC<T> > m_free_list; //Hold for available containers
	UINT m_bucket_size;
	SHASH_BUCKET * m_bucket;
	SVECTOR<T> m_elem_vector;
	UINT m_elem_count;

	inline HC<T> * newhc() //Allocate hash container.
	{
		IS_TRUE(m_bucket != NULL, ("HASH not yet initialized."));
		HC<T> * c = m_free_list.get_free_elem();
		if (c == NULL) {
			c = (HC<T>*)_xmalloc(sizeof(HC<T>));
		}
		return c;
	}

	void * _xmalloc(ULONG size)
	{
		IS_TRUE(m_bucket != NULL, ("HASH not yet initialized."));
		void * p = smpool_malloc_h(size, m_free_list_pool);
		IS_TRUE0(p != NULL);
		memset(p, 0, size);
		return p;
	}

	virtual T create(ULONG v)
	{
		IS_TRUE(0, ("Inherited class need to implement"));
		return T(0);
	}
public:
	SHASH(UINT bsize = MAX_SHASH_BUCKET)
	{
		m_bucket = NULL;
		m_bucket_size = 0;
		m_free_list_pool = NULL;
		m_elem_count = 0;
		init(bsize);
	}
	virtual ~SHASH() { destroy(); }

	T append(ULONG val, OUT HC<T> ** hct = NULL, bool * find = NULL);
	T append(T t, OUT HC<T> ** hct = NULL, bool * find = NULL);

	inline void clean();
	inline UINT count_mem() const;

	bool insert_t(IN OUT HC<T> ** bucket_entry, OUT HC<T> ** hc, IN T t);
	bool insert_v(IN OUT HC<T> ** bucket_entry, OUT HC<T> ** hc, ULONG val);
	inline void init(UINT bsize = MAX_SHASH_BUCKET);

	bool find(T t, T * ot);
	bool find(T t, HC<T> ** ct = NULL);
	T find(ULONG val);

	inline UINT get_bucket_size() const { return m_bucket_size; }
	inline SHASH_BUCKET const* get_bucket() const { return m_bucket; }
	inline T get_first(INT & cur);
	inline T get_next(INT & cur);
	inline T get_last(INT & cur);
	inline T get_prev(INT & cur);
	void grow(UINT bsize = 0);
	SMEM_POOL * get_free_list_pool() const
	{ return m_free_list_pool; };
	UINT get_elem_count() const { return m_elem_count; }

	inline void destroy();
	void dump_intersp(FILE * h);

	T removed(T t);
};


template <class T, class HF>
void SHASH<T, HF>::init(UINT bsize)
{
	if (m_bucket != NULL) return;
	if (bsize == 0) { return; }
	m_bucket = (SHASH_BUCKET*)::malloc(sizeof(SHASH_BUCKET) * bsize);
	memset(m_bucket, 0, sizeof(SHASH_BUCKET) * bsize);
	m_bucket_size = bsize;
	m_elem_count = 0;
	m_free_list_pool = smpool_create_handle(sizeof(HC<T>), MEM_COMM);
	m_free_list.clean();
	m_free_list.set_clean(true);
	m_elem_vector.init();
}


template <class T, class HF>
void SHASH<T, HF>::destroy()
{
	if (m_bucket == NULL) return;
	::free(m_bucket);
	m_bucket = NULL;
	m_bucket_size = 0;
	m_elem_count = 0;
	m_elem_vector.destroy();
	smpool_free_handle(m_free_list_pool);
	m_free_list_pool = NULL;
}


template <class T, class HF>
UINT SHASH<T, HF>::count_mem() const
{
	UINT count = smpool_get_pool_size_handle(m_free_list_pool);
	count += m_free_list.count_mem();
	count += sizeof(m_bucket_size);
	count += sizeof(m_bucket);
	count += m_bucket_size;
	count += m_elem_vector.count_mem();
	count += sizeof(m_elem_count);
	return count;
}


template <class T, class HF>
void SHASH<T, HF>::clean()
{
	if (m_bucket == NULL) return;
	memset(m_bucket, 0, sizeof(SHASH_BUCKET) * m_bucket_size);
	m_elem_count = 0;
	m_elem_vector.clean();
}


/*
Insert element into hash table.
Return true if 't' already exist.
*/
template <class T, class HF>
bool SHASH<T, HF>::insert_v(OUT HC<T> ** bucket_entry,
							OUT HC<T> ** hc, ULONG val)
{
	HC<T> * elemhc = *bucket_entry;
	HC<T> * prev = NULL;
	HF hf;
	while (elemhc != NULL) {
		IS_TRUE(HC_val(elemhc) != T(0),
				("Hash element has so far as to be overrided!"));
		if (hf.compare(HC_val(elemhc), val)) {
			*hc = elemhc;
			return true;
		}
		prev = elemhc;
		elemhc = HC_next(elemhc);
	} //end while
	elemhc = newhc();
	IS_TRUE(elemhc != NULL, ("newhc() return NULL"));
	HC_val(elemhc) = create(val);
	if (prev != NULL) {
		//Append on element-list
		HC_next(prev) = elemhc;
		HC_prev(elemhc) = prev;
	} else {
		*bucket_entry = elemhc;
	}
	*hc = elemhc;
	return false;
}


/*
Insert element into hash table.
Return true if 't' already exist.
*/
template <class T, class HF>
bool SHASH<T, HF>::insert_t(IN OUT HC<T> ** bucket_entry,
							OUT HC<T> ** hc, IN T t)
{
	HC<T> * prev = NULL;
	HC<T> * elemhc = *bucket_entry;
	HF hf;
	while (elemhc != NULL) {
		IS_TRUE(HC_val(elemhc) != T(0), ("Container is empty"));
		if (hf.compare(HC_val(elemhc), t)) {
			t = HC_val(elemhc);
			*hc = elemhc;
			return true;
		}
		prev = elemhc;
		elemhc = HC_next(elemhc);
	}
	elemhc = newhc();
	IS_TRUE(elemhc != NULL, ("newhc() return NULL"));
	HC_val(elemhc) = t;
	if (prev != NULL) {
		//Append on elem-list in the bucket.
		HC_next(prev) = elemhc;
		HC_prev(elemhc) = prev;
	} else {
		*bucket_entry = elemhc;
	}
	*hc = elemhc;
	return false;
}


/*
Append 't' into hash table and record its reference into
SVECTOR in order to walk through the table rapidly.
If 't' already exists, return the element immediately.
'find': set to true if 't' already exist.

NOTE:
	Do NOT append 0 to table.
	Maximum size of T equals sizeof(void*).
*/
template <class T, class HF>
T SHASH<T, HF>::append(T t, OUT HC<T> ** hct, bool * find)
{
	IS_TRUE(m_bucket != NULL, ("SHASH not yet initialized."));
	if (t == T(0)) return T(0);

	HF hf;
	UINT hashv = hf.get_hash_value(t, m_bucket_size);
	IS_TRUE(hashv < m_bucket_size,
			("hash value must less than bucket size"));

	HC<T> * elemhc = NULL;
	if (!insert_t((HC<T>**)&SHB_member(m_bucket[hashv]), &elemhc, t)) {
		SHB_count(m_bucket[hashv])++;
		m_elem_count++;

		//Get a free slot in elem-vector
		HC_vec_idx(elemhc) = m_elem_vector.get_free_idx();
		m_elem_vector.set(HC_vec_idx(elemhc), t);
		if (find != NULL) {
			*find = false;
		}
	} else if (find != NULL) {
		*find = true;
	}
	if (hct != NULL) {
		*hct = elemhc;
	}
	return t;
}


/*
Append 'val' into hash table.
More comment see above function.
'find': set to true if 't' already exist.

NOTE: Do NOT append T(0) to table.
*/
template <class T, class HF>
T SHASH<T, HF>::append(ULONG val, OUT HC<T> ** hct, bool * find)
{
	IS_TRUE(m_bucket != NULL, ("SHASH not yet initialized."));
	HF hf;
	UINT hashv = hf.get_hash_value(val, m_bucket_size);

	IS_TRUE(hashv < m_bucket_size, ("hash value must less than bucket size"));
	HC<T> * elemhc = NULL;
	if (!insert_v((HC<T>**)&SHB_member(m_bucket[hashv]), &elemhc, val)) {
		SHB_count(m_bucket[hashv])++;
		m_elem_count++;

		//Get a free slot in elem-vector
		HC_vec_idx(elemhc) = m_elem_vector.get_free_idx();
		m_elem_vector.set(HC_vec_idx(elemhc), HC_val(elemhc));
		if (find != NULL) {
			*find = false;
		}
	} else if (find != NULL) {
		*find = true;
	}
	if (hct != NULL) {
		*hct = elemhc;
	}
	return HC_val(elemhc);
}


/*
Do NOT change the order that elements in m_elem_vector and the value of m_cur.
Because it will impact the effect of get_first(), get_next(),
get_last() and get_prev().
*/
template <class T, class HF>
T SHASH<T, HF>::removed(T t)
{
	IS_TRUE(m_bucket != NULL, ("SHASH not yet initialized."));
	if (t == 0) return T(0);

	HF hf;
	UINT hashv = hf.get_hash_value(t, m_bucket_size);
	IS_TRUE(hashv < m_bucket_size, ("hash value must less than bucket size"));
	HC<T> * elemhc = (HC<T>*)SHB_member(m_bucket[hashv]);
	if (elemhc != NULL) {
 		while (elemhc != NULL) {
			IS_TRUE(HC_val(elemhc) != T(0),
					("Hash element has so far as to be overrided!"));
			if (hf.compare(HC_val(elemhc), t)) {
				break;
			}
			elemhc = HC_next(elemhc);
		}
		if (elemhc != NULL) {
			m_elem_vector.set(HC_vec_idx(elemhc), T(0));
			remove((HC<T>**)&SHB_member(m_bucket[hashv]), elemhc);
			m_free_list.add_free_elem(elemhc);
			SHB_count(m_bucket[hashv])--;
			m_elem_count--;
			return t;
		}
	}
	return T(0);
}


/*
Grow hash to 'bsize'.
The default grow size is twice as the current bucket size.
'bsize': expected bucket size.

NOTE: Grow is costly function.
*/
template <class T, class HF>
void SHASH<T, HF>::grow(UINT bsize)
{
	IS_TRUE(m_bucket != NULL, ("SHASH not yet initialized."));
	if (bsize != 0) {
		IS_TRUE0(bsize > m_bucket_size);
	} else {
		bsize = m_bucket_size * 2;
	}

	SHASH_BUCKET * new_bucket =
		(SHASH_BUCKET*)::malloc(sizeof(SHASH_BUCKET) * bsize);
	memset(new_bucket, 0, sizeof(SHASH_BUCKET) * bsize);
	if (m_elem_count == 0) {
		::free(m_bucket);
		m_bucket = new_bucket;
		m_bucket_size = bsize;
		return;
	}

	for (UINT i = 0; i < m_bucket_size; i++) {
		HC<T> * hc = NULL;
		while ((hc = removehead((HC<T>**)&SHB_member(m_bucket[i]))) != NULL) {
			m_free_list.add_free_elem(hc);
		}
	}

	::free(m_bucket);
	m_bucket = new_bucket;
	m_bucket_size = bsize;

	INT l = m_elem_vector.get_last_idx();
	HF hf;
	for (INT i = 0; i <= l; i++) {
		T t = m_elem_vector.get(i);
		if (t == T(0)) { continue; }

		UINT hashv = hf.get_hash_value(t, m_bucket_size);
		IS_TRUE(hashv < m_bucket_size,
				("hash value must less than bucket size"));
		HC<T> * elemhc = NULL;
		bool doit = insert_t((HC<T>**)&SHB_member(m_bucket[hashv]),
							 &elemhc, t);
		IS_TRUE0(!doit);
		HC_vec_idx(elemhc) = i;
		SHB_count(m_bucket[hashv])++;
	}
}


/*
You can implement your own find(), but do NOT
change the order that elements in m_elem_vector and the value of m_cur.
Because it will impact the effect of get_first(), get_next(),
get_last() and get_prev().
*/
template <class T, class HF>
T SHASH<T, HF>::find(ULONG val)
{
	IS_TRUE(m_bucket != NULL, ("SHASH not yet initialized."));
	HF hf;
 	UINT hashv = hf.get_hash_value(val, m_bucket_size);
	IS_TRUE(hashv < m_bucket_size, ("hash value must less than bucket size"));
	HC<T> * elemhc = (HC<T>*)SHB_member(m_bucket[hashv]);
	if (elemhc != NULL) {
		while (elemhc != NULL) {
			IS_TRUE(HC_val(elemhc) != T(0),
					("Hash element has so far as to be overrided!"));
			if (hf.compare(HC_val(elemhc), val)) {
				return HC_val(elemhc);
			}
			elemhc = HC_next(elemhc);
		} //end while
	}
	return T(0);
}


/*
You can implement your own find(), but do NOT
change the order that elements in m_elem_vector and the value of m_cur.
Because it will impact the effect of get_first(), get_next(),
get_last() and get_prev().
*/
template <class T, class HF>
bool SHASH<T, HF>::find(T t, HC<T> ** ct)
{
	IS_TRUE(m_bucket != NULL, ("SHASH not yet initialized."));
	if (t == T(0)) { return false; }

	HF hf;
	UINT hashv = hf.get_hash_value(t, m_bucket_size);
	IS_TRUE(hashv < m_bucket_size,
			("hash value must less than bucket size"));
	HC<T> * elemhc = (HC<T>*)SHB_member(m_bucket[hashv]);
	if (elemhc != NULL) {
		while (elemhc != NULL) {
			IS_TRUE(HC_val(elemhc) != T(0),
					("Hash element has so far as to be overrided!"));
			if (hf.compare(HC_val(elemhc), t)) {
				if (ct != NULL) {
					*ct = elemhc;
				}
				return true;
			}
			elemhc = HC_next(elemhc);
		} //end while
	}
	return false;
}


/*
You can implement your own find(), but do NOT
change the order that elements in m_elem_vector and the value of m_cur.
Because it will impact the effect of get_first(), get_next(),
get_last() and get_prev().

'ot': output the element if found it.
*/
template <class T, class HF>
bool SHASH<T, HF>::find(T t, T * ot)
{
	HC<T> * hc;
	if (find(t, &hc)) {
		IS_TRUE0(ot != NULL);
		*ot = HC_val(hc);
		return true;
	}
	return false;
}


//When T is type of integer, return zero may be fuzzy and ambiguous.
template <class T, class HF>
T SHASH<T, HF>::get_first(INT & cur)
{
	IS_TRUE(m_bucket != NULL, ("SHASH not yet initialized."));
	T t = T(0);
	cur = -1;
	if (m_elem_count <= 0) return T(0);
	INT l = m_elem_vector.get_last_idx();
	for (INT i = 0; i <= l; i++) {
		if ((t = m_elem_vector.get(i)) != T(0)) {
			cur = i;
			return t;
		}
	}
	return T(0);
}


template <class T, class HF>
T SHASH<T, HF>::get_next(INT & cur)
{
	IS_TRUE(m_bucket != NULL && cur >= -1, ("SHASH not yet initialized."));
	T t = T(0);
	if (m_elem_count <= 0) return T(0);
	INT l = m_elem_vector.get_last_idx();
	for (INT i = cur + 1; i <= l; i++) {
		if ((t = m_elem_vector.get(i)) != T(0)) {
			cur = i;
			return t;
		}
	}
	return T(0);
}


//When T is type of integer, return zero may be fuzzy and ambiguous.
template <class T, class HF>
T SHASH<T, HF>::get_last(INT & cur)
{
	IS_TRUE(m_bucket != NULL, ("SHASH not yet initialized."));
	T t = T(0);
	cur = -1;
	if (m_elem_count <= 0) return T(0);
	INT l = m_elem_vector.get_last_idx();
	for (INT i = l; i >= 0; i--) {
		if ((t = m_elem_vector.get(i)) != T(0)) {
			cur = i;
			return t;
		}
	}
	return T(0);
}


//When T is type of integer, return zero may be fuzzy and ambiguous.
template <class T, class HF>
T SHASH<T, HF>::get_prev(INT & cur)
{
	IS_TRUE(m_bucket != NULL, ("SHASH not yet initialized."));
	T t = T(0);
	if (m_elem_count <= 0) return T(0);
	for (INT i = cur - 1; i >= 0; i--) {
		if ((t = m_elem_vector.get(i)) != T(0)) {
			cur = i;
			return t;
		}
	}
	return T(0);
}


//Dump to see hash element interspersion.
template <class T, class HF>
void SHASH<T, HF>::dump_intersp(FILE * h)
{
	if (h == NULL) return;
	UINT bsize = get_bucket_size();
	SHASH_BUCKET const* bucket = get_bucket();
	fprintf(h, "\n=== SHASH ===");
	for (UINT i = 0; i < bsize; i++) {
		HC<T> * elemhc = (HC<T>*)bucket[i].hash_member;
		fprintf(h, "\nENTRY[%d]:", i);
		while (elemhc != NULL) {
			fprintf(h, "*");
			elemhc = HC_next(elemhc);
			if (elemhc != NULL) {
				//fprintf(g_tfile, ",");
			}
		}
	}
	fflush(h);
}
//END SHASH



//
//START RBTN
//
typedef enum _RBT_RED {
	RBT_NON = 0,
	RBRED = 1,
	RBBLACK = 2,
} RBCOL;

template <class T, class Ttgt>
class RBTN {
public:
	RBTN * parent;
	union {
		RBTN * lchild;
		RBTN * prev;
	};

	union {
		RBTN * rchild;
		RBTN * next;
	};
	T key;
	Ttgt mapped;
	RBCOL color;

	RBTN()
	{
		parent = NULL;
		lchild = NULL;
		rchild = NULL;
		key = T(0);
		mapped = Ttgt(0);
		color = RBBLACK;
	}
};


template <class T> class COMPARE_KEY_BASE {
public:
	bool is_less(T t1, T t2) const { return t1 < t2; }
	bool is_equ(T t1, T t2) const { return t1 == t2; }
};

template <class T, class Ttgt, class Compare_Key = COMPARE_KEY_BASE<T> >
class RBT {
protected:
	typedef RBTN<T, Ttgt> TN;
	UINT m_num_of_tn;
	TN * m_root;
	SMEM_POOL * m_pool;
	TN * m_free_list;

	void * _xmalloc(ULONG size)
	{
		void * p = smpool_malloc_h(size, m_pool);
		IS_TRUE0(p);
		memset(p, 0, size);
		return p;
	}

	void free_rbt(TN * t)
	{
		if (t == NULL) return;
		t->prev = t->next = t->parent = NULL;
		t->key = T(0);
		t->mapped = Ttgt(0);
		insertbefore_one(&m_free_list, m_free_list, t);
	}

	void rleft(TN * x)
	{
		IS_TRUE0(x->rchild != NULL);
		TN * y = x->rchild;
		y->parent = x->parent;
		x->parent = y;
		x->rchild = y->lchild;
		if (y->lchild != NULL) {
			y->lchild->parent = x;
		}
		y->lchild = x;
		if (y->parent == NULL) {
			m_root = y;
		} else if (y->parent->lchild == x) {
			y->parent->lchild = y;
		} else if (y->parent->rchild == x) {
			y->parent->rchild = y;
		}
	}

	void rright(TN * y)
	{
		IS_TRUE0(y->lchild != NULL);
		TN * x = y->lchild;
		x->parent = y->parent;
		y->parent = x;
		y->lchild = x->rchild;
		if (x->rchild != NULL) {
			x->rchild->parent = y;
		}
		x->rchild = y;
		if (x->parent == NULL) {
			m_root = x;
		} else if (x->parent->lchild == y) {
			x->parent->lchild = x;
		} else if (x->parent->rchild == y) {
			x->parent->rchild = x;
		}
	}

	inline bool is_lchild(TN const* z) const
	{
		IS_TRUE0(z && z->parent);
		return z == z->parent->lchild;
	}

	inline bool is_rchild(TN const* z) const
	{
		IS_TRUE0(z && z->parent);
		return z == z->parent->rchild;
	}

	void fixup(TN * z)
	{
		TN * y = NULL;
		while (z->parent != NULL && z->parent->color == RBRED) {
			if (is_lchild(z->parent)) {
				y =	z->parent->parent->rchild;
				if (y != NULL && y->color == RBRED) {
					z->parent->color = RBBLACK;
					z->parent->parent->color = RBRED;
					y->color = RBBLACK;
					z = z->parent->parent;
				} else if (is_rchild(z)) {
					z = z->parent;
					rleft(z);
				} else {
					IS_TRUE0(is_lchild(z));
					z->parent->color = RBBLACK;
					z->parent->parent->color = RBRED;
					rright(z->parent->parent);
				}
			} else {
				IS_TRUE0(is_rchild(z->parent));
				y =	z->parent->parent->lchild;
				if (y != NULL && y->color == RBRED) {
					z->parent->color = RBBLACK;
					z->parent->parent->color = RBRED;
					y->color = RBBLACK;
					z = z->parent->parent;
				} else if (is_lchild(z)) {
					z = z->parent;
					rright(z);
				} else {
					IS_TRUE0(is_rchild(z));
					z->parent->color = RBBLACK;
					z->parent->parent->color = RBRED;
					rleft(z->parent->parent);
				}
			}
		}
		m_root->color = RBBLACK;
	}
public:
	RBT()
	{
		m_pool = smpool_create_handle(sizeof(TN), MEM_COMM);
		m_root = NULL;
		m_num_of_tn = 0;
		m_free_list = NULL;
	}
	~RBT() { smpool_free_handle(m_pool); m_pool = NULL; }

	UINT count_mem() const
	{
		UINT c = sizeof(m_num_of_tn);
		c += sizeof(m_root);
		c += sizeof(m_pool);
		c += sizeof(m_free_list);
		c += smpool_get_pool_size_handle(m_pool);
		return c;
	}

	void clean()
	{
		LIST<TN*> wl;
		if (m_root != NULL) {
			wl.append_tail(m_root);
			m_root = NULL;
		}
		while (wl.get_elem_count() != 0) {
			TN * x = wl.remove_head();
			if (x->rchild != NULL) {
				wl.append_tail(x->rchild);
			}
			if (x->lchild != NULL) {
				wl.append_tail(x->lchild);
			}
			free_rbt(x);
		}
		m_num_of_tn = 0;
	}

	UINT get_elem_count() const { return m_num_of_tn; }
	SMEM_POOL * get_pool() { return m_pool; }
	TN const* get_root_c() const { return m_root; }
	TN * get_root() { return m_root; }

	TN * find_with_key(T keyt)
	{
		if (m_root == NULL) { return NULL; }
		Compare_Key ck;
		TN * x = m_root;
		while (x != NULL) {
			if (ck.is_equ(keyt, x->key)) {
				return x;
			} else if (ck.is_less(keyt, x->key)) {
				x = x->lchild;
			} else {
				x = x->rchild;
			}
		}
		return NULL;
	}

	inline TN * find_rbtn(T t)
	{
		if (m_root == NULL) { return NULL; }
		return find_with_key(t);
	}

	TN * insert(T t, bool * find = NULL)
	{
		if (m_root == NULL) {
			TN * z = new_tn(t, RBBLACK);
			m_num_of_tn++;
			m_root = z;
			if (find != NULL) {
				*find = false;
			}
			return z;
		}

		TN * mark = NULL;
		TN * x = m_root;
		Compare_Key ck;
		while (x != NULL) {
			mark = x;
			if (ck.is_equ(t, x->key)) {
				break;
			} else if (ck.is_less(t, x->key)) {
				x = x->lchild;
			} else {
				x = x->rchild;
			}
		}

		if (x != NULL) {
			IS_TRUE0(ck.is_equ(t, x->key));
			if (find != NULL) {
				*find = true;
			}
			return x;
		}

		if (find != NULL) {
			*find = false;
		}

		//Add new.
		TN * z = new_tn(t, RBRED);
		z->parent = mark;
		if (mark == NULL) {
			//The first node.
			m_root = z;
		} else {
			if (ck.is_less(t, mark->key)) {
				mark->lchild = z;
			} else {
				mark->rchild = z;
			}
		}
		IS_TRUE0(z->lchild == NULL && z->rchild == NULL);
		m_num_of_tn++;
		fixup(z);
		return z;
	}

	TN * min(TN * x)
	{
		IS_TRUE0(x);
		while (x->lchild != NULL) { x = x->lchild; }
		return x;
	}

	inline TN * new_tn(T t, RBCOL c)
	{
		TN * x = removehead(&m_free_list);
		if (x == NULL) {
			x = (TN*)_xmalloc(sizeof(TN));
		} else {
			x->lchild = NULL;
			x->rchild = NULL;
		}
		x->key = t;
		x->color = c;
		return x;
	}

	TN * succ(TN * x)
	{
		if (x->rchild != NULL) { return min(x->rchild); }
		TN * y = x->parent;
		while (y != NULL && x == y->rchild) {
			x = y;
			y = y->parent;
		}
		return y;
	}

	inline bool both_child_black(TN * x)
	{
		if (x->lchild != NULL && x->lchild->color == RBRED) {
			return false;
		}
		if (x->rchild != NULL && x->rchild->color == RBRED) {
			return false;
		}
		return true;
	}

	void rmfixup(TN * x)
	{
		IS_TRUE0(x);
		while (x != m_root && x->color == RBBLACK) {
			if (is_lchild(x)) {
				TN * bro = x->parent->rchild;
				IS_TRUE0(bro);
				if (bro->color == RBRED) {
					bro->color = RBBLACK;
					x->parent->color = RBRED;
					rleft(x->parent);
					bro = x->parent->rchild;
				}
				if (both_child_black(bro)) {
					bro->color = RBRED;
					x = x->parent;
					continue;
				} else if (bro->rchild == NULL ||
						   bro->rchild->color == RBBLACK) {
					IS_TRUE0(bro->lchild && bro->lchild->color == RBRED);
					bro->lchild->color = RBBLACK;
					bro->color = RBRED;
					rright(bro);
					bro = x->parent->rchild;
				}
				bro->color = x->parent->color;
				x->parent->color = RBBLACK;
				bro->rchild->color = RBBLACK;
				rleft(x->parent);
				x = m_root;
			} else {
				IS_TRUE0(is_rchild(x));
				TN * bro = x->parent->lchild;
				if (bro->color == RBRED) {
					bro->color = RBBLACK;
					x->parent->color = RBRED;
					rright(x->parent);
					bro = x->parent->lchild;
				}
				if (both_child_black(bro)) {
					bro->color = RBRED;
					x = x->parent;
					continue;
				} else if (bro->lchild == NULL ||
						   bro->lchild->color == RBBLACK) {
					IS_TRUE0(bro->rchild && bro->rchild->color == RBRED);
					bro->rchild->color = RBBLACK;
					bro->color = RBRED;
					rleft(bro);
					bro = x->parent->lchild;
				}
				bro->color = x->parent->color;
				x->parent->color = RBBLACK;
				bro->lchild->color = RBBLACK;
				rright(x->parent);
				x = m_root;
			}
		}
		x->color = RBBLACK;
	}

	void remove(T t)
	{
		TN * z = find_rbtn(t);
		if (z == NULL) { return; }
		remove(z);
	}

	void remove(TN * z)
	{
		if (z == NULL) { return; }
		if (m_num_of_tn == 1) {
			IS_TRUE(z == m_root, ("z is not the node of tree"));
			IS_TRUE(z->rchild == NULL && z->lchild == NULL,
					("z is the last node"));
			free_rbt(z);
			m_num_of_tn--;
			m_root = NULL;
			return;
		}
		TN * y;
		if (z->lchild == NULL || z->rchild == NULL) {
			y = z;
		} else {
			y = min(z->rchild);
		}

		TN * x = y->lchild != NULL ? y->lchild : y->rchild;
		TN holder;
		if (x != NULL) {
			x->parent = y->parent;
		} else {
			holder.parent = y->parent;
		}
		if (y->parent == NULL) {
			m_root = x;
		} else if (is_lchild(y)) {
			if (x != NULL) {
				y->parent->lchild = x;
			} else {
				y->parent->lchild = &holder;
			}
		} else {
			if (x != NULL) {
				y->parent->rchild = x;
			} else {
				y->parent->rchild = &holder;
			}
		}
		if (y != z) {
			z->key = y->key;
			z->mapped = y->mapped;
		}
		if (y->color == RBBLACK) {
			//Need to keep RB tree property: the number
			//of black must be same in all path.
			if (x != NULL) {
				rmfixup(x);
			} else {
				rmfixup(&holder);
				if (is_rchild(&holder)) {
					holder.parent->rchild = NULL;
				} else {
					IS_TRUE0(is_lchild(&holder));
					holder.parent->lchild = NULL;
				}
			}
		} else if (holder.parent != NULL) {
			if (is_rchild(&holder)) {
				holder.parent->rchild = NULL;
			} else {
				IS_TRUE0(is_lchild(&holder));
				holder.parent->lchild = NULL;
			}
		}
		free_rbt(y);
		m_num_of_tn--;
	}

	//iter should be clean by caller.
	T get_first(LIST<TN*> & iter, Ttgt * mapped = NULL)
	{
		if (m_root == NULL) {
			if (mapped != NULL) { *mapped = Ttgt(0); }
			return T(0);
		}
		iter.append_tail(m_root);
		if (mapped != NULL) { *mapped = m_root->mapped; }
		return m_root->key;
	}

	T get_next(LIST<TN*> & iter, Ttgt * mapped = NULL)
	{
		TN * x = iter.remove_head();
		if (x == NULL) {
			if (mapped != NULL) { *mapped = Ttgt(0); }
			return T(0);
		}
		if (x->rchild != NULL) {
			iter.append_tail(x->rchild);
		}
		if (x->lchild != NULL) {
			iter.append_tail(x->lchild);
		}
		x = iter.get_head();
		if (x == NULL) {
			if (mapped != NULL) { *mapped = Ttgt(0); }
			return T(0);
		}
		if (mapped != NULL) { *mapped = x->mapped; }
		return x->key;
	}
};
//END RBTN



/*
TMAP

Make a map between Tsrc and Ttgt.

'Tsrc': source type.
'Ttgt': target type.

Usage: Make a mapping from SRC* to TGT*.
	class SRC2TGT_MAP : public TMAP<SRC*, TGT*> {
	public:
	};

NOTICE:
	1. Tsrc(0) is defined as default NULL in TMAP, do NOT
	   use T(0) as element.
	2. Keep the key *UNIQUE* .
	3. Overload operator == and operator < if Tsrc is neither basic type
	   nor pointer type.
*/

//Iterator of TMAP.
template <class Tsrc, class Ttgt>
class TMAP_ITER : public LIST<RBTN<Tsrc, Ttgt>*> {};


template <class Tsrc, class Ttgt, class Compare_Key = COMPARE_KEY_BASE<Tsrc> >
class TMAP : public RBT<Tsrc, Ttgt, Compare_Key> {
public:
	typedef RBT<Tsrc, Ttgt, Compare_Key> BASE_TY;
	typedef RBTN<Tsrc, Ttgt> TN;
	TMAP() {}
	~TMAP() {}

	//Alway set new mapping even if it has done.
	void aset(Tsrc t, Ttgt mapped)
	{
		bool find;
		TN * z = BASE_TY::insert(t, &find);
		IS_TRUE0(z);
		z->mapped = mapped;
	}

	//Establishing mapping in between 't' and 'mapped'.
	void set(Tsrc t, Ttgt mapped)
	{
		bool find;
		TN * z = BASE_TY::insert(t, &find);
		IS_TRUE0(z);
		IS_TRUE(!find, ("already mapped"));
		z->mapped = mapped;
	}

	//Get mapped element of 't'. Set find to true if t is already be mapped.
	Ttgt get(Tsrc t, bool * f = NULL)
	{
		TN * z = BASE_TY::find_rbtn(t);
		if (z == NULL) {
			if (f != NULL) {
				*f = false;
			}
			return Ttgt(0);
		}

		if (f != NULL) {
			*f = true;
		}
		return z->mapped;
	}

	bool find(Tsrc t)
	{
		bool f;
		get(t, &f);
		return f;
	}
};
//END TMAP


/*
TTAB

NOTICE:
	1. T(0) is defined as default NULL in TTAB, do not use T(0) as element.
	2. Keep the key *UNIQUE*.
	3. Overload operator == and operator < if Tsrc is neither basic type
	   nor pointer type.

	e.g: Make a table to record OPND.
		class OPND_TAB : public TTAB<OPND*> {
		public:
		};
*/
template <class T>
class TAB_ITER : public LIST<RBTN<T, T>*> {};

template <class T, class Compare_Key = COMPARE_KEY_BASE<T> >
class TTAB : public TMAP<T, T, Compare_Key> {
public:
	typedef TMAP<T, T, Compare_Key> BASE_TTY;

	//Add element into table, if
	//Note: the element in the table should be unqiue.
	void append(T t)
	{
		IS_TRUE0(t != T(0));
		#ifdef _DEBUG_
		bool find = false;
		T mapped = BASE_TTY::get(t, &find);
		if (find) {
			IS_TRUE0(mapped == t);
		}
		#endif
		BASE_TTY::aset(t, t);
	}

	//Add element into table, if it is exist, return the exist one.
	T append_and_retrieve(T t)
	{
		IS_TRUE0(t != T(0));

		bool find = false;
		T mapped = BASE_TTY::get(t, &find);
		if (find) {
			return mapped;
		}

		BASE_TTY::aset(t, t);
		return t;
	}

	void remove(T t)
	{
		IS_TRUE0(t != T(0));
		BASE_TTY::remove(t);
	}
};
//END TTAB


/*
Unidirectional Hashed Map

Usage: Make a mapping from OPND to OPER.
	typedef HMAP<OPND*, OPER*, HASH_FUNC_BASE<OPND*> > OPND2OPER_MAP;

NOTICE:
	1. Tsrc(0) is defined as default NULL in HMAP, so do not use T(0) as element.
	2. The map is implemented base on SHASH, and one hash function class
	   have been given.

	3. Must use 'new'/'delete' operator to allocate/free the
	   memory of dynamic object of MAP, because the
	   virtual-function-pointers-table is needed.
*/
template <class Tsrc, class Ttgt, class HF = HASH_FUNC_BASE<Tsrc> >
class HMAP : public SHASH<Tsrc, HF> {
protected:
	SVECTOR<Ttgt> m_mapped_elem_table;

	//Find hash container
	HC<Tsrc> * findhc(Tsrc t) const
	{
		if (t == Tsrc(0)) { return NULL; }
		HF hf;
		UINT hashv = hf.get_hash_value(t, SHASH<Tsrc, HF>::m_bucket_size);
		IS_TRUE((hashv < SHASH<Tsrc, HF>::m_bucket_size),
				("hash value must less than bucket size"));
		HC<Tsrc> * elemhc =
			(HC<Tsrc>*)SHB_member((SHASH<Tsrc, HF>::m_bucket[hashv]));
		if (elemhc != NULL) {
			while (elemhc != NULL) {
				IS_TRUE(HC_val(elemhc) != Tsrc(0),
						("Hash element has so far as to be overrided!"));
				if (hf.compare(HC_val(elemhc), t)) {
					return elemhc;
				}
				elemhc = HC_next(elemhc);
			} //end while
		}
		return NULL;
	}
public:
	HMAP(UINT bsize = MAX_SHASH_BUCKET) : SHASH<Tsrc, HF>(bsize)
	{ m_mapped_elem_table.init(); }

	virtual ~HMAP() { destroy(); }

	//Alway set new mapping even if it has done.
	void aset(Tsrc t, Ttgt mapped)
	{
		IS_TRUE((SHASH<Tsrc, HF>::m_bucket != NULL), ("not yet initialize."));
		if (t == Tsrc(0)) { return; }
		HC<Tsrc> * elemhc;
		SHASH<Tsrc, HF>::append(t, &elemhc, NULL);
		IS_TRUE(elemhc != NULL, ("Element does not append into hash table."));
		m_mapped_elem_table.set(HC_vec_idx(elemhc), mapped);
	}

	SMEM_POOL * get_pool() { return SHASH<Tsrc, HF>::get_free_list_pool(); }

	//Get mapped pointer of 't'
	Ttgt get(Tsrc t, bool * find = NULL)
	{
		IS_TRUE((SHASH<Tsrc, HF>::m_bucket != NULL), ("not yet initialize."));
		HC<Tsrc> * elemhc = findhc(t);
		if (elemhc != NULL) {
			if (find != NULL) { *find = true; }
			return m_mapped_elem_table.get(HC_vec_idx(elemhc));
		}
		if (find != NULL) { *find = false; }
		return Ttgt(0);
	}

	//Readonly to get mapped pointer of 't'
	Ttgt cget(Tsrc t, bool * find = NULL) const
	{
		IS_TRUE((SHASH<Tsrc, HF>::m_bucket != NULL), ("not yet initialize."));
		HC<Tsrc> * elemhc = findhc(t);
		if (elemhc != NULL) {
			if (find != NULL) { *find = true; }
			return m_mapped_elem_table.get(HC_vec_idx(elemhc));
		}
		if (find != NULL) { *find = false; }
		return Ttgt(0);
	}

	SVECTOR<Ttgt> * get_tgt_elem_vec() { return &m_mapped_elem_table; }

	void clean()
	{
		IS_TRUE((SHASH<Tsrc, HF>::m_bucket != NULL), ("not yet initialize."));
		SHASH<Tsrc, HF>::clean();
		m_mapped_elem_table.clean();
	}

	UINT count_mem() const
	{
		UINT count = m_mapped_elem_table.count_mem();
		count += ((SHASH<Tsrc, HF>*)this)->count_mem();
		return count;
	}

	void init(UINT bsize = MAX_SHASH_BUCKET)
	{
		//Only do initialization while m_bucket is NULL.
		SHASH<Tsrc, HF>::init(bsize);
		m_mapped_elem_table.init();
	}

	void destroy()
	{
		SHASH<Tsrc, HF>::destroy();
		m_mapped_elem_table.destroy();
	}

	//Establishing mapping in between 't' and 'mapped'.
	void set(Tsrc t, Ttgt mapped)
	{
		IS_TRUE((SHASH<Tsrc, HF>::m_bucket != NULL), ("not yet initialize."));
		if (t == Tsrc(0)) { return; }

		HC<Tsrc> * elemhc;
		SHASH<Tsrc, HF>::append(t, &elemhc, NULL);

		IS_TRUE(elemhc != NULL,
				("Element does not append into hash table yet."));
		IS_TRUE(Ttgt(0) == m_mapped_elem_table.get(HC_vec_idx(elemhc)),
				("Already be mapped"));
		m_mapped_elem_table.set(HC_vec_idx(elemhc), mapped);
	}

	void setv(ULONG v, Ttgt mapped)
	{
		IS_TRUE((SHASH<Tsrc, HF>::m_bucket != NULL), ("not yet initialize."));
		if (v == 0) { return; }

		HC<Tsrc> * elemhc;
		SHASH<Tsrc, HF>::append(v, &elemhc, NULL);
		IS_TRUE(elemhc != NULL,
				("Element does not append into hash table yet."));
		IS_TRUE(Ttgt(0) == m_mapped_elem_table.get(HC_vec_idx(elemhc)),
				("Already be mapped"));
		m_mapped_elem_table.set(HC_vec_idx(elemhc), mapped);
	}
};
//END MAP



/*
Dual directional Map

MAP_Tsrc2Ttgt: class derive from HMAP<Tsrc, Ttgt>
MAP_Ttgt2Tsrc: class derive from HMAP<Ttgt, Tsrc>

Usage: Mapping OPND to corresponding OPER.
	class MAP1 : public HMAP<OPND*, OPER*> {
	public:
	};

	class MAP2 : public HMAP<OPER*, OPND*>{
	public:
	};

	DMAP<OPND*, OPER*, MAP1, MAP2> opnd2oper_dmap;

NOTICE:
	1. Tsrc(0) is defined as default NULL in DMAP, so do not use T(0) as element.
	2. DMAP Object's memory can be allocated by malloc() dynamically.
*/
template <class Tsrc, class Ttgt, class MAP_Tsrc2Ttgt, class MAP_Ttgt2Tsrc>
class DMAP {
protected:
	MAP_Tsrc2Ttgt m_src2tgt_map;
	MAP_Ttgt2Tsrc m_tgt2src_map;
public:
	DMAP() {}
	~DMAP() {}

	//Alway overlap the old value by new 'mapped' value.
	void aset(Tsrc t, Ttgt mapped)
	{
		if (t == Tsrc(0)) { return; }
		m_src2tgt_map.aset(t, mapped);
		m_tgt2src_map.aset(mapped, t);
	}

	UINT count_mem() const
	{
		UINT count = m_src2tgt_map.count_mem();
		count += m_tgt2src_map.count_mem();
		return count;
	}

	//Establishing mapping in between 't' and 'mapped'.
	void set(Tsrc t, Ttgt mapped)
	{
		if (t == Tsrc(0)) { return; }
		m_src2tgt_map.set(t, mapped);
		m_tgt2src_map.set(mapped, t);
	}

	//Get mapped pointer of 't'
	Ttgt get(Tsrc t)
	{ return m_src2tgt_map.get(t); }

	//Inverse mapping
	Tsrc geti(Ttgt t)
	{ return m_tgt2src_map.get(t); }
};
//END DMAP



/*
Multiple Target Map
Map src with type Tsrc to many tgt elements with type Ttgt.

'TAB_Ttgt': records tgt elements.
	e.g: We implement TAB_Ttgt via deriving from SHASH.

	class TAB_Ttgt: public SHASH<Ttgt, HASH_FUNC_BASE<Ttgt> > {
	public:
	};

	or deriving from LIST, typedef LIST<Ttgt> TAB_Ttgt;

NOTICE:
	1. Tsrc(0) is defined as default NULL in MMAP, do not use T(0) as element.

	2. MMAP allocate memory for 'TAB_Ttgt' and return 'TAB_Ttgt *'
		when get(Tsrc) be invoked. DO NOT free these objects yourself.

	3. TAB_Ttgt should be pointer type.
		e.g: Given type of tgt's table is a class that
			OP_HASH : public SHASH<OPER*>,
			then type MMAP<OPND*, OPER*, OP_HASH> is ok, but
			type MMAP<OPND*, OPER*, OP_HASH*> is not expected.

	4. Do not use DMAP directly, please overload following functions optionally:
		* create hash-element container.
			T * create(ULONG v)

		e.g: Mapping one OPND to a number of OPERs.
			class OPND2OPER_MMAP : public MMAP<OPND*, OPER*, OP_TAB> {
			public:
				virtual T create(ULONG id)
				{
					//Allocate OPND from heap or pool, here we only do
					//allocation from process heap and call constructor.
					return new OPND(id);
				}
			};

	4. Please use 'new'/'delete' operator to allocate/free
	   the memory of dynamic object of MMAP, because the
	   virtual-function-pointers-table is needed.
*/
template <class Tsrc, class Ttgt, class TAB_Ttgt,
		  class HF = HASH_FUNC_BASE<Tsrc> >
class MMAP : public HMAP<Tsrc, TAB_Ttgt*, HF> {
protected:
	bool m_is_init;
public:
	MMAP()
	{
		m_is_init = false;
		init();
	}
	virtual ~MMAP() { destroy(); }

	UINT count_mem() const
	{
		return ((HMAP<Tsrc, TAB_Ttgt*, HF>*)this)->count_mem() +
			   sizeof(m_is_init);
	}

	//Establishing mapping in between 't' and 'mapped'.
	void set(Tsrc t, Ttgt mapped)
	{
		if (t == Tsrc(0)) return;
		TAB_Ttgt * tgttab = HMAP<Tsrc, TAB_Ttgt*, HF>::get(t);
		if (tgttab == NULL) {
			/*
			Do NOT use SHASH::_xmalloc(sizeof(TAB_Ttgt)) to allocate memory,
			because __vfptr is NULL. __vfptr points to TAB_Ttgt::vftable.
			*/
			tgttab = new TAB_Ttgt;
			HMAP<Tsrc, TAB_Ttgt*, HF>::set(t, tgttab);
		}
		tgttab->append_node(mapped);
	}

	//Get mapped elements of 't'
	TAB_Ttgt * get(Tsrc t)
	{ return HMAP<Tsrc, TAB_Ttgt*, HF>::get(t); }

	void init()
	{
		if (m_is_init) { return; }
		HMAP<Tsrc, TAB_Ttgt*, HF>::init();
		m_is_init = true;
	}

	void destroy()
	{
		if (!m_is_init) { return; }
		TAB_Ttgt * tgttab = get(HMAP<Tsrc, TAB_Ttgt*, HF>::get_first());
		UINT n = HMAP<Tsrc, TAB_Ttgt*, HF>::get_elem_count();
		for (UINT i = 0; i < n; i++) {
			IS_TRUE0(tgttab);
			delete tgttab;
			tgttab = HMAP<Tsrc, TAB_Ttgt*, HF>::
							get(HMAP<Tsrc, TAB_Ttgt*, HF>::get_next());
		}

		HMAP<Tsrc, TAB_Ttgt*, HF>::destroy();
		m_is_init = false;
	}
};
#endif
