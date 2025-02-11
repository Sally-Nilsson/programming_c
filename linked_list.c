#include "memory_manager.h"
#include <stdio.h>
#include <stdint.h>

typedef struct Node {
    uint16_t data; // Data in the node
    struct Node* next; // Pointer to the next node
} Node;

void list_init(Node** head, size_t size) {
    mem_init(size);
    *head = NULL;
}

void list_insert(Node** head, uint16_t data) {
    if (!head) return;

    Node* new_node = (Node*) mem_alloc(sizeof(Node));
    if (!new_node) return;

    new_node->data = data;
    new_node->next = NULL;

    // Addes the new node to the end of the list
    if (*head == NULL) {
        *head = new_node;
    } else {
        Node* temp = *head;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = new_node;
    }
}

void list_insert_after(Node* prev_node, uint16_t data) {
    if (!prev_node) return;

    Node* new_node = (Node*) mem_alloc(sizeof(Node));
    if (!new_node) return;

    new_node->data = data;
    new_node->next = prev_node->next;
    prev_node->next = new_node;
}

void list_insert_before(Node** head, Node* next_node, uint16_t data) {
    if (!head || !*head || !next_node) return;

    Node* new_node = (Node*) mem_alloc(sizeof(Node));
    if (!new_node) return;

    new_node->data = data;
    
    if (*head == next_node) {
        new_node->next = *head;
        *head = new_node;
        return;
    }

    Node* temp = *head;
    while (temp->next && temp->next != next_node) {
        temp = temp->next;
    }

    if (temp->next == next_node) {
        new_node->next = next_node;
        temp->next = new_node;
    }
}

void list_delete(Node** head, uint16_t data) {
    if (!head || !*head) return;

    Node* temp = *head;
    Node* prev = NULL;

    while (temp && temp->data != data) {
        prev = temp;
        temp = temp->next;
    }

    if (!temp) return;

    if (prev) {
        prev->next = temp->next;
    } else {
        *head = temp->next;
    }

    mem_free(temp);
}

Node* list_search(Node** head, uint16_t data) {
    if (!head || !*head) return NULL;

    Node* temp = *head;
    while (temp) {
        if (temp->data == data) return temp;
        temp = temp->next;
    }
    return NULL;
}

void list_display(Node** head) {
    if (!head || !*head) return;

    Node* temp = *head;
    printf("[");
    while (temp) {
        printf("%d", temp->data);
        if (temp->next) printf(", ");
        temp = temp->next;
    }
    printf("]\n");
}

void list_display_range(Node** head, Node* start_node, Node* end_node) {
    if (!head || !*head) return;
    
    Node* temp = *head;
    // Find the start node
    if (start_node == NULL) {
        start_node = temp;
    }
    while (temp && temp != start_node) {
        temp = temp->next;
    }

    printf("[");
    // If no end node is provided, print the entire list
    if (end_node == NULL) {
        while (temp) {
        if (temp->next == NULL) {
            printf("%d", temp->data);
            break;
        }
        printf("%d", temp->data);
        if (temp->next && temp->next != end_node) printf(", ");
        temp = temp->next;
        }
    }
    else {
        // End node is provided, print the list until the end node
        while (temp) {
            if (temp == end_node) {
                printf("%d", temp->data);
                break;
            }
            printf("%d", temp->data);
            if (temp->next) printf(", ");
            temp = temp->next;
        }
    }
    printf("]");
}

int list_count_nodes(Node** head) {
    if (!head || !*head) return 0;

    int count = 0;
    Node* temp = *head;
    while (temp) {
        count++;
        temp = temp->next;
    }
    return count;
}

void list_cleanup(Node** head) {
    if (!head) return;

    *head = NULL;
    
    mem_deinit();
}

void list_print_details(Node** head) {
    if (!head || !*head) {
        printf("List is empty.\n");
        return;
    }

    Node* temp = *head;
    // Print the details of each node
    while (temp) {
        size_t node_size = sizeof(Node); // The size of each node
        printf("Node data: %d, Size: %zu bytes, Next node is %s\n", 
               temp->data, node_size, temp->next ? "not NULL" : "NULL");
        temp = temp->next;
    }
}
