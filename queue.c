#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *q = (struct list_head *) malloc(sizeof(struct list_head));
    if (!q) {
        return NULL;
    }
    INIT_LIST_HEAD(q);
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return; /* queue is NULL */
    element_t *cur, *next;
    /* del, the element to be free. next, the next element of del */
    list_for_each_entry_safe (cur, next, l, list) {
        list_del(&cur->list);
        q_release_element(cur);
    }
    free(l);
    return;
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false; /* queue is NULL */
    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return NULL; /* could not allocate space */
    int len = strlen(s) + 1;
    node->value = malloc(len);
    if (!node->value) {
        free(node);
        return NULL; /* could not allocate space */
    }
    memcpy(node->value, s, len);
    INIT_LIST_HEAD(&node->list);
    list_add(&node->list, head);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false; /* queue is NULL */
    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return NULL; /* could not allocate space */
    int len = strlen(s) + 1;
    node->value = malloc(len);
    if (!node->value) {
        free(node);
        return NULL; /* could not allocate space */
    }
    memcpy(node->value, s, len);
    INIT_LIST_HEAD(&node->list);
    list_add_tail(&node->list, head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head) || !sp)
        return NULL;
    element_t *delete = list_first_entry(head, element_t, list);
    list_del(head->next);
    int len = strlen(delete->value);
    if (len > (bufsize - 1)) {
        strncpy(sp, delete->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    } else {
        strncpy(sp, delete->value, len);
        sp[len] = '\0';
    }
    return delete;
}

/*參考Chao-Shun-Cheng同學的code*/

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head) || !sp)
        return NULL;
    element_t *remove = list_last_entry(head, element_t, list);
    list_del(head->prev);
    int len = strlen(remove->value);
    if (len > (bufsize - 1)) {
        strncpy(sp, remove->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    } else {
        strncpy(sp, remove->value, len);
        sp[len] = '\0';
    }
    return remove;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    struct list_head *node = NULL;
    int size = 0;
    list_for_each (node, head) {
        size++;
    }
    return size;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    struct list_head *front = head->next;
    struct list_head *back = head;

    while (front != back) {
        back = back->prev;
        if (front == back)
            break;
        front = front->next;
    }

    list_del(front);
    q_release_element(list_entry(front, element_t, list));
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head) {
        return false;
    }
    struct list_head *node, *next;
    bool next_same_val = false;
    for (node = (head)->next; node != (head);) {
        next = node->next;
        // cppcheck-suppress nullPointer
        element_t *del = list_entry(node, element_t, list);
        if (next != head &&
            strcmp(del->value,
                   // cppcheck-suppress nullPointer
                   list_entry(next, element_t, list)->value) == 0) {
            next_same_val = true;
            node->prev->next = next;
            next->prev = node->prev;
            q_release_element(del);
        } else if (next_same_val) {
            next_same_val = false;
            node->prev->next = next;
            next->prev = node->prev;
            q_release_element(del);
        }
        node = next;
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    if (!head || list_is_singular(head))
        return;
    struct list_head *node = head->next, *front = NULL, *back = NULL, *next = NULL;
    while ((node != head) && (node->next != head)) {
        next = node->next;
        front = node->prev;
        back = node->next->next;
        front->next = next;
        next->prev = front;
        next->next = node;
        node->prev = next;
        node->next = back;
        back->prev = node;
        node = back;
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *cur = head;
    struct list_head *next = cur->next;
    do {
        cur->next = cur->prev;
        cur->prev = next;
        cur = next;
        next = cur->next;
    } while (cur != head);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void mergetwolist(struct list_head *head1,struct list_head *head2,struct list_head *head)
{
    if (!head || !head1 || !head2 || (list_empty(head1) && list_empty(head2)))
        return;
    if (list_empty(head1)) {
        list_splice_tail_init(head2, head);
        return;
    }
    if (list_empty(head2)) {
        list_splice_tail_init(head1, head);
        return;
    }
    struct list_head *temp = NULL, *l1 = head1->next, *l2 = head2->next;
    while (l1 != head1 && l2 != head2) {
        if (strcmp(list_entry(l1, element_t, list)->value,
                   list_entry(l2, element_t, list)->value) > 0) {
            temp = l2;
            l2 = l2->next;
            list_move_tail(temp, head);
        } else {
            temp = l1;
            l1 = l1->next;
            list_move_tail(temp, head);
        }
    }
    list_splice_tail_init(head1, head);
    list_splice_tail_init(head2, head);
}

void mergesort(struct list_head *head, int size)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    LIST_HEAD(head1);
    LIST_HEAD(head2);
    int mid = size >> 1, count = 0;
    struct list_head *node = NULL, *safe = NULL;
    list_for_each_safe (node, safe, head) {
        count++;
        if (count == mid) {
            list_cut_position(&head1, head, node);
            list_splice_tail_init(head, &head2);
            break;
        }
    }
    mergesort(&head1, mid);
    mergesort(&head2, size - mid);
    mergetwolist(&head1, &head2, head);
}

void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    mergesort(head, q_size(head));
}
