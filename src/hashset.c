#include "hashset.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// Начальная емкость по умолчанию, если передали 0
#define DEFAULT_CAPACITY 16

// Хеш-функция для структуры Point.
// Это критически важная часть. Плохая функция приведет к большому количеству
// коллизий и деградации производительности до O(n).
// Эта простая функция смешивает биты x и y.
static size_t hash_point(Point p, size_t capacity) {
  // Используем простые числа для лучшего распределения
  size_t hash = 17;
  hash = hash * 31 + p.x;
  hash = hash * 31 + p.y;
  return hash % capacity;
}

HashSet *hashset_create(size_t initial_capacity) {
  if (initial_capacity == 0) {
    initial_capacity = DEFAULT_CAPACITY;
  }

  HashSet *set = malloc(sizeof(HashSet));
  if (!set) {
    return NULL;
  }

  // calloc сразу заполнит память нулями, что эквивалентно NULL для указателей
  set->buckets = calloc(initial_capacity, sizeof(Node *));
  if (!set->buckets) {
    free(set);
    return NULL;
  }

  set->capacity = initial_capacity;
  set->size = 0;
  return set;
}
void hashset_destroy(HashSet *set) {
  if (!set) {
    return;
  }

  size_t i;
  for (i = 0; i < set->capacity; ++i) {
    Node *current = set->buckets[i];
    while (current != NULL) {
      Node *to_free = current;
      current = current->next;
      free(to_free);
    }
  }
  free(set->buckets);
  free(set);
}

bool hashset_add(HashSet *set, Point p) {
  if (!set)
    return false;

  size_t index = hash_point(p, set->capacity);

  // Сначала проверим, нет ли уже такого элемента
  Node *current = set->buckets[index];
  while (current != NULL) {
    if (current->point.x == p.x && current->point.y == p.y) {
      return false; // Элемент уже существует
    }
    current = current->next;
  }

  // Если мы здесь, элемента нет. Добавляем его в начало списка.
  Node *new_node = malloc(sizeof(Node));
  if (!new_node)
    return false; // Ошибка выделения памяти

  new_node->point = p;
  new_node->next = set->buckets[index];
  set->buckets[index] = new_node;
  set->size++;

  // Примечание: в "промышленной" реализации здесь бы проверялся
  // коэффициент загрузки (load factor) и при необходимости
  // хеш-таблица бы расширялась (rehashing). Для игры это излишне.

  return true;
}

bool hashset_contains(const HashSet *set, Point p) {
  if (!set)
    return false;

  size_t index = hash_point(p, set->capacity);
  Node *current = set->buckets[index];

  while (current != NULL) {
    if (current->point.x == p.x && current->point.y == p.y) {
      return true; // Нашли!
    }
    current = current->next;
  }

  return false; // Не нашли
}

bool hashset_remove(HashSet *set, Point p) {
  if (!set)
    return false;

  size_t index = hash_point(p, set->capacity);
  Node *current = set->buckets[index];
  Node *prev = NULL;

  while (current != NULL) {
    if (current->point.x == p.x && current->point.y == p.y) {
      // Элемент найден, удаляем его
      if (prev == NULL) {
        // Это первый элемент в списке
        set->buckets[index] = current->next;
      } else {
        // Элемент в середине или в конце списка
        prev->next = current->next;
      }
      free(current);
      set->size--;
      return true;
    }
    prev = current;
    current = current->next;
  }

  return false; // Элемент не найден
}
