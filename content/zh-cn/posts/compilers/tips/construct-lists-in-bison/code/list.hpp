#ifndef LIST_H
#define LIST_H

#include <iostream>

struct Node {
  Node *prev, *next;
  double data;

  Node() : Node(0xDEADBEEF) {}
  Node(double data) : data(data) {}
};

struct List {
  Node *head, *tail;

  List() : head(new Node{}), tail(new Node{}) {
    head->next = tail;
    tail->prev = head;
  }

  ~List() {
    auto p = head;
    while (p != nullptr) {
      auto q = p->next;
      delete p;
      p = q;
    }
  }

  auto insert_last(double data) -> void {
    auto nn = new Node{data};
    nn->prev = tail->prev;
    nn->next = tail;
    tail->prev->next = nn;
    tail->prev = nn;
  }

  auto insert_first(double data) -> void {
    auto nn = new Node{data};
    nn->prev = head;
    nn->next = head->next;
    head->next->prev = nn;
    head->next = nn;
  }

  auto empty() const -> bool { return head == tail; }
  auto is_last(const Node *n) const -> bool { return n->next == tail; }

  friend std::ostream &operator<<(std::ostream &out, const List &lst) {
    if (lst.empty()) {
      return (out << "nil");
    }
    auto p = lst.head->next;
    while (p != lst.tail) {
      out << p->data;
      if (!lst.is_last(p)) {
        out << " -> ";
      }
      p = p->next;
    }
    return out;
  }
};

#endif  // LIST_H
