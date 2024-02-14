#include "M5Cardputer.h"

#define TFT_GREEN 0x07E0
#define TFT_BLACK 0x0000
#define PADDLE_WIDTH 5
#define PADDLE_HEIGHT 30
#define BALL_SIZE 4
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 135
#define SCORE_LIMIT 20

// Game variables
int ballX = SCREEN_WIDTH / 2 - BALL_SIZE / 2;
int ballY = SCREEN_HEIGHT / 2 - BALL_SIZE / 2;
int ballSpeedX = 2;
int ballSpeedY = 2;
int paddle1Y = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
int paddle2Y = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
int paddleSpeed = 3;
int score1 = 0;
int score2 = 0;
bool gameActive = true;

void setup() {
  M5Cardputer.begin();
  M5Cardputer.Display.setBrightness(70);
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);

  // Splash screen
  showSplashScreen();
  drawScores(); // Draw initial scores
  drawPaddlesAndBall(); // Draw initial positions of paddles and ball
}

void loop() {
  if (gameActive) {
    M5Cardputer.update();

    // Check for control inputs
    handleControls();

    // Update ball position
    updateBall();

    // Check for collisions
    checkCollisions();

    // Update the screen with new positions
    updateDisplay();

    // Check for scoring
    checkScoring();
  } else {
    // Check if escape key is pressed to reset the match
    if (M5Cardputer.Keyboard.isKeyPressed('`')) {
      resetMatch();
    }
  }
}

void showSplashScreen() {
  String splashText[] = {"Polyphasic", "Developers", "Ltd.", "Pong", "polyphasicdevs.com"};
  for (int i = 0; i < 4; i++) {
    int textWidth = M5Cardputer.Display.textWidth(splashText[i]);
    int x = (SCREEN_WIDTH - textWidth) / 2;
    int y = 40 + i * 20; // Adjust y position for each line
    M5Cardputer.Display.setCursor(x, y);
    M5Cardputer.Display.println(splashText[i]);
  }
  delay(3000); // Show splash screen for 3 seconds
  M5Cardputer.Display.fillScreen(TFT_BLACK);
}

void drawScores() {
  M5Cardputer.Display.setCursor(SCREEN_WIDTH / 2 - 20, 10);
  M5Cardputer.Display.print(score1);
  M5Cardputer.Display.print(" | ");
  M5Cardputer.Display.print(score2);
}

void drawPaddlesAndBall() {
  M5Cardputer.Display.fillRect(0, paddle1Y, PADDLE_WIDTH, PADDLE_HEIGHT, TFT_GREEN);
  M5Cardputer.Display.fillRect(SCREEN_WIDTH - PADDLE_WIDTH, paddle2Y, PADDLE_WIDTH, PADDLE_HEIGHT, TFT_GREEN);
  M5Cardputer.Display.fillRect(ballX, ballY, BALL_SIZE, BALL_SIZE, TFT_GREEN);
}

void handleControls() {
  if (M5Cardputer.Keyboard.isKeyPressed('1')) paddle1Y -= paddleSpeed;
  if (M5Cardputer.Keyboard.isKeyPressed('q')) paddle1Y += paddleSpeed;
  if (M5Cardputer.Keyboard.isKeyPressed('0')) paddle2Y -= paddleSpeed;
  if (M5Cardputer.Keyboard.isKeyPressed('p')) paddle2Y += paddleSpeed;
  
  paddle1Y = constrain(paddle1Y, 0, SCREEN_HEIGHT - PADDLE_HEIGHT);
  paddle2Y = constrain(paddle2Y, 0, SCREEN_HEIGHT - PADDLE_HEIGHT);
}

void updateBall() {
  ballX += ballSpeedX;
  ballY += ballSpeedY;
}

void checkCollisions() {
  if (ballY <= 0 || ballY >= SCREEN_HEIGHT - BALL_SIZE) ballSpeedY *= -1;
  if ((ballX <= PADDLE_WIDTH && ballY >= paddle1Y && ballY <= paddle1Y + PADDLE_HEIGHT) ||
      (ballX >= SCREEN_WIDTH - PADDLE_WIDTH - BALL_SIZE && ballY >= paddle2Y && ballY <= paddle2Y + PADDLE_HEIGHT)) {
    ballSpeedX *= -1;
    // Play the paddle hit sound
    M5Cardputer.Speaker.tone(1000, 10);
  }
}

void updateDisplay() {
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  drawScores();
  drawPaddlesAndBall();
}

void checkScoring() {
  if (ballX <= 0) {
    score2++;
    resetBall();
    if (score2 >= SCORE_LIMIT) endGame(2);
  } else if (ballX >= SCREEN_WIDTH - BALL_SIZE) {
    score1++;
    resetBall();
    if (score1 >= SCORE_LIMIT) endGame(1);
  }
}

void resetBall() {
  ballX = SCREEN_WIDTH / 2 - BALL_SIZE / 2;
  ballY = SCREEN_HEIGHT / 2 - BALL_SIZE / 2;
  ballSpeedX = -ballSpeedX; // Change direction
  ballSpeedY = random(-2, 3);
  // Play the scoring sound
  M5Cardputer.Speaker.tone(500, 500);
}

void endGame(int winner) {
  gameActive = false;
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  M5Cardputer.Display.setCursor((SCREEN_WIDTH - M5Cardputer.Display.textWidth("PLAYER  WINS!")) / 2, SCREEN_HEIGHT / 2);
  M5Cardputer.Display.print("PLAYER ");
  M5Cardputer.Display.print(winner);
  M5Cardputer.Display.println(" WINS!");
  // Play the win sound
  M5Cardputer.Speaker.tone(2000, 1000);
}

void resetMatch() {
  score1 = 0;
  score2 = 0;
  gameActive = true;
  resetBall();
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  drawScores();
  drawPaddlesAndBall();
}
