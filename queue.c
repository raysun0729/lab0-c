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
    if (!head || q_size(head) == 0)
        return NULL;

    element_t *node = list_entry(head->next, element_t, list);
    list_del(head->next);
    if (sp) {
        // If sp is non-NULL, copy the value of removed element to *sp.
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return node;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || q_size(head) == 0)
        return NULL;

    element_t *node = list_entry(head->prev, element_t, list);

    if (sp) {
        // If sp is non-NULL, copy the value of removed element to *sp.
        strncpy(sp, node->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    list_del(head->prev);
    return node;
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
    struct list_head *node;
    int size = 0;
    list_for_each (node, head)
        ++size;
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
    if (!head || list_empty(head))
        return false;

    element_t *cur, *safe;
    list_for_each_entry_safe (cur, safe, head, list) {
        if (&safe->list != head && !strcmp(cur->value, safe->value)) {
            list_del(&cur->list);
            q_release_element(cur);
        }
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    struct list_head *prev, *cur;
    list_for_each_safe (prev, cur, head) {
        list_move_tail(cur, prev);
        cur = prev->next;
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
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    // split
    struct list_head *slow, *fast;
    for (slow = head->next, fast = head->next->next;
         fast != head && fast->next != head;
         slow = slow->next, fast = fast->next->next)
        ;

    LIST_HEAD(left);
    LIST_HEAD(right);
    list_cut_position(&left, head, slow);
    // check node is odd or even
    fast = fast != head ? fast : fast->prev;
    list_cut_position(&right, head, fast);
    q_sort(&left);
    q_sort(&right);
    // Merge
    while (!list_empty(&left) && !list_empty(&right)) {
        struct list_head *l = left.next, *r = right.next;
        element_t *ele_l = container_of(l, element_t, list);
        element_t *ele_r = container_of(r, element_t, list);
        // strcmp  1: str1 > str2, 0: equal, -1: str1 < str2
        if (strcmp(ele_l->value, ele_r->value) <= 0) {
            list_del(l);
            list_add_tail(l, head);
        } else {
            list_del(r);
            list_add_tail(r, head);
        }
    }
    if (!list_empty(&left))
        list_splice_tail(&left, head);
    if (!list_empty(&right))
        list_splice_tail(&right, head);
}
