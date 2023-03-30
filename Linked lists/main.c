#include <stdio.h>
#include <stdlib.h>

typedef struct linked_list {
    int info;
    struct linked_list* next_p;
} l_list;

l_list* l_list_init(int item) {
    l_list* l_p = malloc(sizeof(l_list));
    l_p->info = item;
    l_p->next_p = NULL;
    printf("ptr=%p info=%d next=%p\n", (void*)l_p, l_p->info, (void*)l_p->next_p);
    return l_p;
}

l_list* add_list(l_list* l_p, int item) {
    l_list* node_p = malloc(sizeof(l_list));
    node_p->info = item;
    node_p->next_p = l_p;
    printf("ptr=%p info=%d next=%p\n", (void*)node_p, node_p->info, (void*)node_p->next_p);
    return node_p;
}

int main() {
    l_list* list_p = l_list_init(10);
    list_p = add_list(list_p, 9);
    list_p = add_list(list_p, 8);
    list_p = add_list(list_p, 7);
    list_p = add_list(list_p, 6);
    list_p = add_list(list_p, 5);
    list_p = add_list(list_p, 4);
    list_p = add_list(list_p, 3);
    list_p = add_list(list_p, 2);
    list_p = add_list(list_p, 1);

    int x;
    printf("Enter shift amount:");
    scanf("%d", &x);
    if (x > 10) {
        printf("x is bigger than 10\n");
        return 0;
    }
    else if (x < 0) {
        printf("x is smaller than 0\n");
        return 0;
    }

    l_list *this_p = list_p;
    l_list *prev_p = NULL;
    for (int i = 0; i < x; i++) {
        while (this_p->next_p != NULL) {
            prev_p = this_p;
            this_p = this_p->next_p;
        }
        this_p->next_p = list_p;
        prev_p->next_p = NULL;
        list_p = this_p;
        this_p = list_p;
        prev_p = NULL;
    }

    this_p = list_p;
    while (this_p != NULL) {
        printf("ptr=%p info=%d next=%p\n", (void*)this_p, this_p->info, (void*)this_p->next_p);
        this_p = this_p->next_p;
    }

    return 0;
}
