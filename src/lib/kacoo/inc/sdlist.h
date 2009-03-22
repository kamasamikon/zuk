/* vim:set et sw=4 sts=4: */

#ifndef __SDLIST_H__
#define __SDLIST_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _K_dlist_entry
{
    struct _K_dlist_entry *next;
    struct _K_dlist_entry *prev;
} K_dlist_entry;

#define FIELD_TO_STRUCTURE(address, type, field) \
    ((type *)((char *)(address) - (char *)(&((type *)0)->field)))

void init_dlist_head(K_dlist_entry * list_head);
int is_dlist_empty(K_dlist_entry * list_head);
K_dlist_entry * remove_dlist_head_entry(K_dlist_entry * list_head);
K_dlist_entry * remove_dlist_tail_entry(K_dlist_entry * list_head);
void remove_dlist_entry(K_dlist_entry * list_entry);
void insert_dlist_tail_entry(K_dlist_entry * list_head, K_dlist_entry * list_entry);
void insert_dlist_head_entry(K_dlist_entry * list_head, K_dlist_entry * list_entry);
void replace_dlist_head(K_dlist_entry * old_list_head, K_dlist_entry * new_list_head);
void replace_dlist_entry(K_dlist_entry * old_list_entry, K_dlist_entry * new_list_entry);

// no include the head list
int inquire_dlist_number(K_dlist_entry * list_head);

void incorporate_dlist(K_dlist_entry * list_object_head, K_dlist_entry * list_old_head);
#ifdef __cplusplus
}
#endif

#endif /* __SDLIST_H__ */

