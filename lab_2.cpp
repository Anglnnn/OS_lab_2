#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#define div 1

#define MAX_PAGES 500 / div
#define MAX_RAM 250 / div

#define TIME 1000 / div
#define TAU 300 / div
#define MEASUREMENT_TIME 200 / div

typedef struct Item {
  int page_id;
  int hit;
  long time;
}
Item;

typedef struct Node Node;

typedef struct Node {
  Item item;
  Node * next;
  Node * prev;
}
Node;

typedef struct List {
  int size;
  Node * front;
}
List;

void List_Init(List * list) {
  list -> size = 0;
  Node * head = (Node * ) malloc(sizeof(Node));
  list -> front = head;
  head -> next = list -> front;
  head -> prev = list -> front;
}

int List_Size(List * pList) {
  return pList -> size;
}

int List_IsEmpty(List * pList) {
  return pList -> size == 0;
}

Node * List_Search(List * list, int x) {
  Node * aux = list -> front -> next;
  while (aux != list -> front) {
    if (aux -> item.page_id == x) {
      return aux;
    }
    aux = aux -> next;
  }
  return NULL;
}

int List_Insert(List * list, Node * p, Item x) {
  if (List_Size(list) == MAX_RAM) {
    return 0;
  } else {
    Node * newNode = (Node * ) malloc(sizeof(Node));
    newNode -> item = x;
    newNode -> next = p;
    newNode -> prev = p -> prev;
    p -> prev -> next = newNode;
    p -> prev = newNode;
    list -> size++;
    return 1;
  }
}

int List_Remove(List * list, Node * p, Item * x) {
  if (List_IsEmpty(list)) {
    return 0;
  } else {
    p -> prev -> next = p -> next;
    p -> next -> prev = p -> prev;
    * x = p -> item;
    free(p);
    list -> size--;
    return 1;
  }
}

void List_Print(List * list) {
  Node * aux = list -> front -> next;
  while (aux != list -> front) {
    printf("page_id: %d\t", aux -> item.page_id);
    printf("page_hit: %d\t", aux -> item.hit);
    printf("time_access: %ld\n", aux -> item.time);
    aux = aux -> next;
  }
}

void RandomPage(Item * x, int clock) {
  x -> page_id = rand() % MAX_PAGES;
  x -> hit = 1;
  x -> time = clock;
}

void ResetAllHits(List * list) {
  Node * aux = list -> front -> next;
  while (aux != list -> front) {
    aux -> item.hit = 0;
    aux = aux -> next;
  }
}

Node * SubstitutePage(List * list, long clk) {
  Node * aux = list -> front -> next;
  Node * max_time;
  long t = 9999999;
  while (aux != list -> front) {
    if (aux -> item.hit == 1) {
      aux -> item.hit = 0;
    } else if (aux -> item.hit == 0) {
      if ((clk - aux -> item.time) > TAU) {
        return aux;
      } else if (aux -> item.time < t) {
        t = aux -> item.time;
        max_time = aux;
      }
    }
    aux = aux -> next;
  }
  return max_time;
}

void WSClock(List * list) {
  int i, count_hit = 0, page_miss = 0, cont = 1;
  long clock = 0;
  Item x;
  Node * position = list -> front -> next;
  Node * aux;
  for (i = 0; i < TIME; i++) {
    clock++;
    RandomPage( & x, clock);
    aux = List_Search(list, x.page_id);
    if (aux == NULL) {
      if (List_Size(list) < MAX_RAM) {
        if (List_Insert(list, position, x)) {
          printf("nº %d \t Inserted_id: %d\tclock: %ld\n", cont, x.page_id, clock);
          cont++;
        } else {
          printf("ERROR_0\n");
        }
      } else {
        Node * page_out;
        Item xx;
        page_out = SubstitutePage(list, clock);
        if (List_Remove(list, page_out, & xx)) {
          if (List_Insert(list, position, x)) {
            printf("Removed_id: %d and Inserted_id: %d \tclock: %ld\n", xx.page_id, x.page_id, clock);
          } else {
            printf("ERROR_1\n");
          }
        } else {
          printf("ERROR_2\n");
        }
      }
      page_miss++;
    } else {
      aux -> item.hit = 1;
      aux -> item.time = clock;
      count_hit++;
    }
    if (clock >= MEASUREMENT_TIME) {
      ResetAllHits(list);
      clock = 0;
      printf("\t\t\t\t\tRESET CLOCK\n");
    }
  }
  printf("count hit: %d\n", count_hit);
  printf("page miss: %d\n", page_miss);
}

int main() {
  List list;
  List_Init( & list);
  WSClock( & list);
  // List_Print(&list);
  return 0;
}