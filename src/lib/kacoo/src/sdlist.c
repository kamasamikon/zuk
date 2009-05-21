/* vim:set et sw=4 sts=4 ff=unix: */

#include <sdlist.h>

void init_dlist_head(K_dlist_entry * list_head)
{
    list_head->next = list_head->prev = list_head;
}

int is_dlist_empty(K_dlist_entry * list_head)
{
    return (list_head->next == list_head);
}

K_dlist_entry * remove_dlist_head_entry(K_dlist_entry * list_head)
{
    K_dlist_entry *list_entry;

    list_entry = list_head->next;
    remove_dlist_entry(list_head->next);

    return list_entry;
}

K_dlist_entry * remove_dlist_tail_entry(K_dlist_entry * list_head)
{
    K_dlist_entry *list_entry;

    list_entry = list_head->prev;
    remove_dlist_entry(list_head->prev);

    return list_entry;
}

void remove_dlist_entry(K_dlist_entry * list_entry)
{
    K_dlist_entry * _ex_prev;
    K_dlist_entry * _ex_next;

    _ex_next = list_entry->next;
    _ex_prev = list_entry->prev;
    _ex_prev->next = _ex_next;
    _ex_next->prev = _ex_prev;
}

void insert_dlist_tail_entry(K_dlist_entry * list_head, K_dlist_entry * list_entry)
{
    K_dlist_entry * _ex_prev;
    K_dlist_entry * _ex_list_head;

    _ex_list_head = list_head;
    _ex_prev = _ex_list_head->prev;
    list_entry->next = _ex_list_head;
    list_entry->prev = _ex_prev;
    _ex_prev->next = list_entry;
    _ex_list_head->prev = list_entry;
}

void insert_dlist_head_entry(K_dlist_entry * list_head, K_dlist_entry * list_entry)
{
    K_dlist_entry * _ex_next;
    K_dlist_entry * _ex_list_head;

    _ex_list_head = list_head;
    _ex_next = _ex_list_head->next;
    list_entry->next = _ex_next;
    list_entry->prev = _ex_list_head;
    _ex_next->prev = list_entry;
    _ex_list_head->next = list_entry;
}


void incorporate_dlist(K_dlist_entry * list_object_head, K_dlist_entry * list_old_head)
{
    K_dlist_entry  * new_head;
    K_dlist_entry  * new_tail;

    K_dlist_entry  * old_tail;
    K_dlist_entry  * old_head;


    new_head=list_object_head;
    new_tail=list_old_head->prev;

    old_tail=list_object_head->prev;
    old_head=list_old_head;


    old_tail->next=old_head;
    old_head->prev=old_tail;

    new_head->prev=new_tail;
    new_tail->next=new_head;


}


// no include the head list
int inquire_dlist_number(K_dlist_entry * list_head)
{
    int l_iNumber=0;

    K_dlist_entry *list_entry;

    if(!is_dlist_empty(list_head))
    {
        list_entry=list_head->next;

        while(list_entry!=list_head)
        {
            l_iNumber++;

            list_entry=list_entry->next;
        }


    }

    return l_iNumber;
}
