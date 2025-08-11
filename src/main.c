#include "../include/directfb/directfb.h"
#include "hashset.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static IDirectFB *dfb = NULL;
static IDirectFBSurface *primary = NULL;
static IDirectFBInputDevice *keyboard = NULL;
static IDirectFBEventBuffer *events = NULL;
static int screen_width = 0;
static int screen_height = 0;

#define DFBCHECK(x...)                                                         \
  {                                                                            \
    DFBResult err = x;                                                         \
    if (err != DFB_OK) {                                                       \
      fprintf(stderr, "%s <%d>:\n\t", __FILE__, __LINE__);                     \
      DirectFBErrorFatal(#x, err);                                             \
    }                                                                          \
  }

struct SnakeNode {
  Point p;
  struct SnakeNode *next;
};

typedef struct SnakeNode SnakeNode;

typedef struct {
  Point v;
  SnakeNode *head;
  char isAlive;
} Snake;

void handleKeyboardInput(Snake *p, Snake *bot) {
  DFBEvent evt;
  if (events->GetEvent(events, &evt) == DFB_OK &&
      evt.input.type == DIET_KEYPRESS) {

    switch (evt.input.key_symbol) {
    case DIKS_ESCAPE:
      p->isAlive = 0;
      bot->isAlive = 0;
    case DIKS_2:
      if (p->v.y == 0) {
        p->v.x = 0;
        p->v.y = -1;
      }
      break;
    case DIKS_4:
      if (p->v.x == 0) {
        p->v.y = 0;
        p->v.x = -1;
      }
      break;
    case DIKS_6:
      if (p->v.x == 0) {
        p->v.y = 0;
        p->v.x = 1;
      }
      break;
    case DIKS_8:
      if (p->v.y == 0) {
        p->v.x = 0;
        p->v.y = 1;
      }
      break;
    }
  }
}

void removeSnakeFromHashset(SnakeNode *node, HashSet *busyCells) {
  if (node == NULL)
    return;

  hashset_remove(busyCells, node->p);
  removeSnakeFromHashset(node->next, busyCells);
}

void moveSnake(Snake *p, Point **apple, int *points, HashSet *busyCells) {
  SnakeNode *temp = p->head;

  int oldX = temp->p.x;
  int oldY = temp->p.y;

  if (hashset_contains(busyCells,
                       (Point){oldX + p->v.x * 5, oldY + p->v.y * 5})) {
    p->isAlive = 0;
    removeSnakeFromHashset(p->head, busyCells);

    DFBCHECK(primary->SetColor(primary, 0xFF, 0x00, 0x00, 0xFF));
    DFBCHECK(
        primary->FillRectangle(primary, 0, 0, screen_width, screen_height));

    return;
  }

  temp->p.x += p->v.x * 5;
  temp->p.y += p->v.y * 5;

  if (temp->p.x > screen_width - 5) {
    temp->p.x = 0;
  } else if (temp->p.x < 0) {
    temp->p.x = screen_width - 5;
  }

  if (temp->p.y > screen_height - 5) {
    temp->p.y = 0;
  } else if (temp->p.y < 0) {
    temp->p.y = screen_height - 5;
  }
  hashset_add(busyCells, temp->p);

  if (*apple != NULL && temp->p.x == (*apple)->x && temp->p.y == (*apple)->y) {
    (*points)++;

    free(*apple);
    *apple = NULL;

    SnakeNode *curr = temp;
    while (curr->next != NULL)
      curr = curr->next;

    SnakeNode *tail = malloc(sizeof(SnakeNode));
    tail->p.x = curr->p.x;
    tail->p.y = curr->p.y;
    tail->next = NULL;

    curr->next = tail;
  }

  temp = temp->next;
  while (temp != NULL) {
    int nextX = temp->p.x;
    int nextY = temp->p.y;

    temp->p.x = oldX;
    temp->p.y = oldY;

    oldX = nextX;
    oldY = nextY;

    temp = temp->next;
  }

  Point tail = {oldX, oldY};
  hashset_remove(busyCells, tail);
}

void freeSnake(SnakeNode *node) {
  if (node == NULL)
    return;

  freeSnake(node->next);

  free(node);
}

void drawSnake(SnakeNode *node) {
  if (node == NULL)
    return;

  DFBCHECK(primary->FillRectangle(primary, node->p.x, node->p.y, 5, 5));

  drawSnake(node->next);
}

void drawApple(Point *apple) {
  DFBCHECK(primary->SetColor(primary, 0xFF, 0x00, 0x00, 0xFF));
  DFBCHECK(primary->FillRectangle(primary, apple->x, apple->y, 5, 5));
}

void drawPoints(int playerPoints, int botPoints, int botAlive) {
  DFBCHECK(primary->SetColor(primary, 0xFF, 0xFF, 0xFF, 0xFF));

  char pointsStr[512];
  sprintf(pointsStr, "%d", playerPoints);

  DFBCHECK(primary->DrawString(primary, pointsStr, strlen(pointsStr), 0, 0,
                               DSTF_TOPLEFT));

  if (botAlive) {
    DFBCHECK(primary->SetColor(primary, 0x32, 0x35, 0x37, 0xFF));

    char botPointsStr[512];
    sprintf(botPointsStr, "%d", botPoints);

    DFBCHECK(primary->DrawString(primary, botPointsStr, strlen(botPointsStr),
                                 screen_width, 0, DSTF_TOPRIGHT));
  }
}

Snake *newSnake(HashSet *busyCells) {
  Snake *sn = malloc(sizeof(Snake));
  sn->v.x = 0;
  sn->v.y = 0;
  sn->isAlive = 0;

  SnakeNode *head = malloc(sizeof(SnakeNode));
  head->p.x = 5 + rand() % screen_width;
  head->p.y = 5 + rand() % screen_height;

  head->p.x = head->p.x - head->p.x % 5;
  head->p.y = head->p.y - head->p.y % 5;

  SnakeNode *next = malloc(sizeof(SnakeNode));
  next->p.x = head->p.x;
  next->p.y = head->p.y;
  next->next = NULL;

  head->next = next;
  sn->head = head;

  hashset_add(busyCells, head->p);

  return sn;
}

void botThink(Snake *bot, Point *apple, HashSet *busyCells, float missChance) {
  if (apple == NULL)
    return;

  float miss = (float)rand() / RAND_MAX;
  if (missChance > miss)
    return;

  Point currPos = bot->head->p;
  currPos.x += 5;
  if (!hashset_contains(busyCells, currPos)) {
    bot->v.x = 1;
    bot->v.y = 0;
    if (bot->head->p.x < apple->x)
      return;
  }
  currPos.x -= 5;
  currPos.y += 5;
  if (!hashset_contains(busyCells, currPos)) {
    bot->v.x = 0;
    bot->v.y = 1;
    if (bot->head->p.y < apple->y)
      return;
  }
  currPos.x -= 5;
  currPos.y -= 5;
  if (!hashset_contains(busyCells, currPos)) {
    bot->v.x = -1;
    bot->v.y = 0;
    if (bot->head->p.x > apple->x)
      return;
  }
  currPos.x += 5;
  currPos.y -= 5;
  if (!hashset_contains(busyCells, currPos)) {
    bot->v.y = -1;
    bot->v.x = 0;
  }
}

int snakeLen(SnakeNode *node) {
  if (node == NULL)
    return 0;

  return 1 + snakeLen(node->next);
}

void clearScreen() {
  DFBCHECK(primary->SetColor(primary, 0x00, 0x00, 0x00, 0xFF));
  DFBCHECK(primary->FillRectangle(primary, 0, 0, screen_width, screen_height));
}

int main(int argc, char **argv) {
  srand(time(NULL));

  DFBCHECK(DirectFBInit(&argc, &argv));
  DFBCHECK(DirectFBCreate(&dfb));

  dfb->SetCooperativeLevel(dfb, DFSCL_FULLSCREEN);

  DFBSurfaceDescription dsc;
  dsc.flags = DSDESC_CAPS;
  dsc.caps = DSCAPS_PRIMARY | DSCAPS_FLIPPING;
  DFBCHECK(dfb->CreateSurface(dfb, &dsc, &primary));

  DFBFontDescription fdsc;
  fdsc.flags = DFDESC_HEIGHT;
  fdsc.height = 20;

  IDirectFBFont *font = NULL;
  DFBCHECK(dfb->CreateFont(dfb, "./CozetteVector.ttf", &fdsc, &font));
  DFBCHECK(primary->SetFont(primary, font));

  DFBCHECK(primary->GetSize(primary, &screen_width, &screen_height));

  DFBCHECK(dfb->GetInputDevice(dfb, DIDID_KEYBOARD, &keyboard));
  DFBCHECK(keyboard->CreateEventBuffer(keyboard, &events));

  int gameRunning = 1;
  while (gameRunning) {

    HashSet *busyCells = hashset_create(512);
    Snake *player = newSnake(busyCells);
    Snake *bot = newSnake(busyCells);

    int botDiffMenu = 0;
    while (1) {
      DFBEvent evt;
      if (events->GetEvent(events, &evt) == DFB_OK &&
          evt.input.type == DIET_KEYPRESS) {

        if (evt.input.key_symbol == DIKS_1) {
          player->isAlive = 1;
          break;
        } else if (evt.input.key_symbol == DIKS_2) {
          player->isAlive = 1;
          bot->isAlive = 1;
          botDiffMenu = 1;
          break;
        } else if (evt.input.key_symbol == DIKS_ESCAPE) {
          gameRunning = 0;
          break;
        }
      }
      usleep(80000);
      clearScreen();

      DFBCHECK(primary->SetColor(primary, 0xFF, 0xFF, 0xFF, 0xFF));
      DFBCHECK(primary->DrawString(primary, "1) Play alone", 13, 0, 0,
                                   DSTF_TOPLEFT));
      DFBCHECK(primary->DrawString(primary, "2) Play with bot", 16, 0, 25,
                                   DSTF_TOPLEFT));

      DFBCHECK(primary->Flip(primary, NULL, DSFLIP_WAITFORSYNC));
    }

    float botMissChance = 0;
    while (botDiffMenu) {
      DFBEvent evt;
      if (events->GetEvent(events, &evt) == DFB_OK &&
          evt.input.type == DIET_KEYPRESS) {

        if (evt.input.key_symbol == DIKS_1) {
          botMissChance = 0.7;
          botDiffMenu = 0;
          break;
        } else if (evt.input.key_symbol == DIKS_2) {
          botMissChance = 0.2;
          botDiffMenu = 0;
          break;
        } else if (evt.input.key_symbol == DIKS_3) {
          botDiffMenu = 0;
          break;
        } else if (evt.input.key_symbol == DIKS_ESCAPE) {
          botDiffMenu = 0;
          player->isAlive = 0;
          bot->isAlive = 0;
          break;
        }
      }
      usleep(80000);
      clearScreen();

      DFBCHECK(primary->SetColor(primary, 0x00, 0xFF, 0x00, 0xFF));
      DFBCHECK(
          primary->DrawString(primary, "1) Easy bot", 11, 0, 0, DSTF_TOPLEFT));

      DFBCHECK(primary->SetColor(primary, 0x00, 0xFF, 0xFF, 0xFF));
      DFBCHECK(primary->DrawString(primary, "2) Normal bot", 13, 0, 25,
                                   DSTF_TOPLEFT));

      DFBCHECK(primary->SetColor(primary, 0xFF, 0x00, 0x00, 0xFF));
      DFBCHECK(
          primary->DrawString(primary, "3) Hard bot", 11, 0, 50, DSTF_TOPLEFT));

      DFBCHECK(primary->Flip(primary, NULL, DSFLIP_WAITFORSYNC));
    }

    Point *apple = NULL;

    int playerPoints = 0;
    int botPoints = 0;

    bot->v.x = 1;
    while (player->isAlive || bot->isAlive) {
      handleKeyboardInput(player, bot);
      usleep(80000);
      clearScreen();

      if (player->v.x != 0 || player->v.y != 0) {
        if (player->isAlive)
          moveSnake(player, &apple, &playerPoints, busyCells);

        if (bot->isAlive) {
          botThink(bot, apple, busyCells, botMissChance);
          moveSnake(bot, &apple, &botPoints, busyCells);
        }
      }

      if (player->isAlive) {
        DFBCHECK(primary->SetColor(primary, 0xFF, 0xFF, 0xFF, 0xFF));
        drawSnake(player->head);
      }

      if (bot->isAlive) {
        DFBCHECK(primary->SetColor(primary, 0x32, 0x35, 0x37, 0xFF));
        drawSnake(bot->head);
      }

      if (apple == NULL) {
        apple = malloc(sizeof(Point));
        apple->x = rand() % (screen_width - 5);
        apple->y = rand() % (screen_height - 5);

        apple->x = apple->x - apple->x % 5;
        apple->y = apple->y - apple->y % 5;
      }
      drawApple(apple);

      drawPoints(playerPoints, botPoints, bot->isAlive);

      DFBCHECK(primary->Flip(primary, NULL, DSFLIP_WAITFORSYNC));
    }

    freeSnake(bot->head);
    free(bot);

    freeSnake(player->head);
    free(player);

    hashset_destroy(busyCells);
  }

  events->Release(events);
  keyboard->Release(keyboard);
  primary->Release(primary);
  dfb->Release(dfb);

  return 0;
}

// posapi.play_sound()!!!
