#include "M5Cardputer.h"

// Game Constants
#define PLAYER_WIDTH 8
#define PLAYER_HEIGHT 8
#define INVADER_WIDTH 8
#define INVADER_HEIGHT 8
#define BULLET_WIDTH 2
#define BULLET_HEIGHT 4
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 135
#define INVADERS_ROW 4
#define INVADERS_COLUMN 6
#define INVADER_SPACING 12

// Player Variables
int playerX = SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2;
int playerY = SCREEN_HEIGHT - 20;

// Invaders Variables
int invaderX[INVADERS_ROW][INVADERS_COLUMN];
int invaderY[INVADERS_ROW][INVADERS_COLUMN];
bool invaderAlive[INVADERS_ROW][INVADERS_COLUMN];
int invaderSpeed = 1;
bool moveDown = false;

// Bullet Variables
int bulletX = -1;
int bulletY = -1;
bool bulletFired = false;

void setup() {
  M5Cardputer.begin();
  M5Cardputer.Display.setBrightness(70);
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.fillScreen(TFT_BLACK);

  // Initialize Invaders
  for (int i = 0; i < INVADERS_ROW; i++) {
    for (int j = 0; j < INVADERS_COLUMN; j++) {
      invaderX[i][j] = j * (INVADER_WIDTH + INVADER_SPACING);
      invaderY[i][j] = i * (INVADER_HEIGHT + INVADER_SPACING) + 20;
      invaderAlive[i][j] = true;
    }
  }
}

void loop() {
  M5Cardputer.update();

  // Player Movement
  if (M5Cardputer.Keyboard.isKeyPressed('a')) { // Move Left
    playerX = max(0, playerX - 5);
  }
  if (M5Cardputer.Keyboard.isKeyPressed('d')) { // Move Right
    playerX = min(SCREEN_WIDTH - PLAYER_WIDTH, playerX + 5);
  }
  if (M5Cardputer.Keyboard.isKeyPressed('s') && !bulletFired) { // Shoot
    bulletX = playerX + PLAYER_WIDTH / 2;
    bulletY = playerY;
    bulletFired = true;
  }

  // Bullet Movement
  if (bulletFired) {
    bulletY -= 5;
    if (bulletY < 0) bulletFired = false; // Reset bullet when it goes off screen
  }

  // Invaders Movement
  for (int i = 0; i < INVADERS_ROW; i++) {
    for (int j = 0; j < INVADERS_COLUMN; j++) {
      if (moveDown) {
        invaderY[i][j] += INVADER_HEIGHT / 2;
      } else {
        invaderX[i][j] += invaderSpeed;
      }
      // Check for screen edge
      if (invaderX[i][j] >= SCREEN_WIDTH - INVADER_WIDTH || invaderX[i][j] <= 0) {
        invaderSpeed *= -1; // Change direction
        moveDown = true;
      }
    }
  }
  if (moveDown) moveDown = false; // Reset move down

  // Collision Detection
  for (int i = 0; i < INVADERS_ROW; i++) {
    for (int j = 0; j < INVADERS_COLUMN; j++) {
      if (bulletFired && invaderAlive[i][j] &&
          bulletX >= invaderX[i][j] && bulletX <= invaderX[i][j] + INVADER_WIDTH &&
          bulletY >= invaderY[i][j] && bulletY <= invaderY[i][j] + INVADER_HEIGHT) {
        invaderAlive[i][j] = false; // Invader is hit
        bulletFired = false; // Reset bullet
      }
    }
  }

  // Drawing
  M5Cardputer.Display.fillScreen(TFT_BLACK); // Clear screen
  drawPlayer();
  drawBullet();
  drawInvaders();

  delay(30); // Game speed
}

void drawPlayer() {
  M5Cardputer.Display.fillRect(playerX, playerY, PLAYER_WIDTH, PLAYER_HEIGHT, TFT_WHITE);
}

void drawBullet() {
  if (bulletFired) {
    M5Cardputer.Display.fillRect(bulletX - BULLET_WIDTH / 2, bulletY, BULLET_WIDTH, BULLET_HEIGHT, TFT_WHITE);
  }
}

void drawInvaders() {
  for (int i = 0; i < INVADERS_ROW; i++) {
    for (int j = 0; j < INVADERS_COLUMN; j++) {
      if (invaderAlive[i][j]) {
        M5Cardputer.Display.fillRect(invaderX[i][j], invaderY[i][j], INVADER_WIDTH, INVADER_HEIGHT, TFT_WHITE);
      }
    }
  }
}
