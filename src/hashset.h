#ifndef HASHSET_H
#define HASHSET_H

#include <stdbool.h> // Для bool, true, false
#include <stddef.h>  // Для size_t

// Структура для хранения координат точки (наши данные)
typedef struct {
  int x;
  int y;
} Point;

// Узел связанного списка для обработки коллизий
typedef struct Node {
  Point point;
  struct Node *next;
} Node;

// Сама структура хеш-множества
typedef struct {
  Node **buckets;  // Массив указателей на узлы (наши "корзины")
  size_t capacity; // Размер массива buckets
  size_t size;     // Количество элементов в множестве
} HashSet;

// Создает и инициализирует новое хеш-множество
HashSet *hashset_create(size_t initial_capacity);

// Уничтожает хеш-множество и освобождает всю память
void hashset_destroy(HashSet *set);

// Добавляет точку в множество. Возвращает true, если элемент был добавлен,
// и false, если он уже существовал.
bool hashset_add(HashSet *set, Point p);

// Проверяет, содержится ли точка в множестве
bool hashset_contains(const HashSet *set, Point p);

// Удаляет точку из множества. Возвращает true, если элемент был найден и
// удален.
bool hashset_remove(HashSet *set, Point p);

#endif // HASHSET_H
