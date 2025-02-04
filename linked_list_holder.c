#include <stdio.h>
#include <stdint.h>
#include "memory_manager.h"

typedef struct Node {
    uint16_t data;
    struct Node* next;
} Node;

void list_init(Node** head, size_t size) {
    *head = NULL;
    mem_init(size*2);
}

void list_insert(Node** head, int data) {
    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    new_node += sizeof(Node);
    if (new_node == NULL) {
        printf("Memory allocation failed\n");
        return;
    }
    new_node->data = data;
    new_node->next = NULL;
    printf("new_node next %p\n", new_node->next);

    // Adds the new node at the end
    if (*head == NULL) {
        *head = new_node;
        printf("empty head %p\n", *head);
    } else {
        Node* temp = *head;
        printf("temp next %p\n", temp->next);
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_node;
    }
}

void list_insert_after(Node* prev_node, int data) {
    if (prev_node == NULL) {
        printf("Previous node cannot be NULL\n");
        return;
    }
    printf("liist insert after\n");
    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    new_node += sizeof(Node);
    if (new_node == NULL) {
        printf("Memory allocation failed\n");
        return;
    }
    new_node->data = data;
    new_node->next = prev_node->next;
    prev_node->next = new_node;
}

void list_insert_before(Node** head, Node* next_node, int data) {
    if (next_node == NULL) {
        printf("Next node cannot be NULL\n");
        return;
    }
    
    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    new_node += sizeof(Node);
    if (new_node == NULL) {
        printf("Memory allocation failed\n");
        return;
    }
    new_node->data = data;

    if (*head == next_node) {
        new_node->next = *head;
        *head = new_node;
    } else {
        Node* temp = *head;
        while (temp != NULL && temp->next != next_node) {
            temp = temp->next;
        }
        if (temp == NULL) {
            printf("Next node not found in the list\n");
            mem_free(new_node);
            return;
        }
        new_node->next = next_node;
        temp->next = new_node;
    }
}

void list_delete(Node** head, int data) {
    if (*head == NULL) {
        printf("List is empty\n");
        return;
    }

    Node* temp = *head;
    Node* prev = NULL;

    if (temp != NULL && temp->data == data) {
        *head = temp->next;
        mem_free(temp);
        return;
    }

    while (temp != NULL && temp->data != data) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Node with data %d not found\n", data);
        return;
    }

    prev->next = temp->next;
    mem_free(temp);
}

Node* list_search(Node** head, int data) {
    Node* temp = *head;
    while (temp != NULL) {
        if (temp->data == data) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void list_display(Node** head) {
    Node* temp = *head;
    printf("[");
    while (temp != NULL) {
        printf("%d", temp->data);
        temp = temp->next;
        if (temp != NULL) {
            printf(", ");
        }
    }
    printf("]\n");
}

void list_display_range(Node** head, Node* start_node, Node* end_node) {
    Node* temp = *head;
    int start = 0, end = 0;

    if (start_node == NULL) {
        start = 1;
    }

    printf("[");
    while (temp != NULL) {
        if (temp == start_node) {
            start = 1;
        }
        if (start) {
            printf("%d", temp->data);
            if (temp->next != NULL && temp->next != end_node) {
                printf(", ");
            }
        }
        if (temp == end_node) {
            end = 1;
            break;
        }
        temp = temp->next;
    }
    printf("]\n");
}

int list_count_nodes(Node** head) {
    int count = 0;
    Node* temp = *head;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }
    return count;
}

void list_cleanup(Node** head) {
    Node* temp = *head;
    while (temp != NULL) {
        Node* next = temp->next;
        mem_free(temp);
        temp = next;
    }
    *head = NULL; // Set head to NULL after cleaning up the list
    mem_deinit(); // Deinitialize the memory manager
}