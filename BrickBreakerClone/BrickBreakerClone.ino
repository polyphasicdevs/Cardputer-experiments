#include "M5Cardputer.h"

#define TFT_BLACK 0x0000
#define PADDLE_COLOR 0xFFFF // White
#define BALL_COLOR 0xF800   // Red
#define BRICK_COLORS 6
const uint16_t BRICK_COLOR[BRICK_COLORS] = {0xF800, 0xFFE0, 0x07FF, 0x07E0, 0x001F, 0xF81F}; // Red, Yellow, Cyan, Green, Blue, Magenta
#define PADDLE_WIDTH 30
#define PADDLE_HEIGHT 5
#define BALL_SIZE 4
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 135
#define BRICK_ROWS 4
#define BRICK_COLUMNS 8
#define BRICK_WIDTH (SCREEN_WIDTH / BRICK_COLUMNS)
#define BRICK_HEIGHT 8
#define SCORE_X 10
#define SCORE_Y 10

int paddleX = SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2;
int ballX = paddleX + PADDLE_WIDTH / 2 - BALL_SIZE / 2;
int ballY = SCREEN_HEIGHT - PADDLE_HEIGHT - BALL_SIZE - 10;
int ballSpeedX = 1;
int ballSpeedY = -1;
int paddleSpeed = 2;
bool bricks[BRICK_ROWS][BRICK_COLUMNS];
int score = 0;
int lives = 3;
bool soundEnabled = true;
bool levelCleared = false; // Flag to indicate if the level is cleared

// Add these variables to track last ball and paddle positions
int lastPaddleX;
int lastBallX;
int lastBallY;

void setup() {
    M5Cardputer.begin();
    M5Cardputer.Display.setBrightness(70);
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setTextSize(1);

    initializeBricks();

    // Initialize last positions
    lastPaddleX = paddleX;
    lastBallX = ballX;
    lastBallY = ballY;
}

void loop() {
    M5Cardputer.update();

    if (lives <= 0 || levelCleared) {
        return; // Skip the rest of the loop if the game is over or level is cleared
    }

    if (M5Cardputer.Keyboard.isKeyPressed('m')) {
        soundEnabled = !soundEnabled;
    }

    if (M5Cardputer.Keyboard.isKeyPressed('r')) {
        restartGame();
        return; // Exit the loop to start a new game
    }

    if (M5Cardputer.Keyboard.isKeyPressed('a')) {
        paddleX -= paddleSpeed;
        paddleX = max(paddleX, 0);
    }
    if (M5Cardputer.Keyboard.isKeyPressed('l')) {
        paddleX += paddleSpeed;
        paddleX = min(paddleX, SCREEN_WIDTH - PADDLE_WIDTH);
    }

    moveBall();
    redrawElements();

    if (areAllBricksCleared() && !levelCleared) {
        displayLevelCleared();
        levelCleared = true;
        delay(2000); // Wait for 2 seconds before starting next level
        initializeBricks(); // Initialize the next level
        resetBallAndPaddle();
        levelCleared = false; // Reset the flag for the next level
    }

    delay(10); // Control the game speed
}

bool areAllBricksCleared() {
    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLUMNS; j++) {
            if (bricks[i][j]) {
                return false;
            }
        }
    }
    return true;
}

void displayLevelCleared() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setCursor((SCREEN_WIDTH - M5Cardputer.Display.textWidth("LEVEL CLEARED!")) / 2, SCREEN_HEIGHT / 2);
    M5Cardputer.Display.println("LEVEL CLEARED!");
    delay(2000);
    M5Cardputer.Display.fillScreen(TFT_BLACK);
}

void moveBall() {
    ballX += ballSpeedX;
    ballY += ballSpeedY;

    if (ballX <= 0 || ballX >= SCREEN_WIDTH - BALL_SIZE) {
        ballSpeedX *= -1;
    }
    if (ballY <= 0) {
        ballSpeedY *= -1;
    }

    if (ballX >= paddleX && ballX <= paddleX + PADDLE_WIDTH && ballY >= SCREEN_HEIGHT - PADDLE_HEIGHT - BALL_SIZE) {
        ballSpeedY *= -1;
        ballY = SCREEN_HEIGHT - PADDLE_HEIGHT - BALL_SIZE - 1;
        playSound(1000, 10);
    }

    checkBrickCollision();

    if (ballY > SCREEN_HEIGHT) {
        lives--;
        if (lives <= 0) {
            gameOver();
        } else {
            resetBallAndPaddle();
        }
    }
}

void checkBrickCollision() {
    int brickRow = ballY / BRICK_HEIGHT;
    int brickColumn = ballX / BRICK_WIDTH;
    if (brickRow >= 0 && brickRow < BRICK_ROWS && brickColumn >= 0 && brickColumn < BRICK_COLUMNS) {
        if (bricks[brickRow][brickColumn]) {
            bricks[brickRow][brickColumn] = false;
            ballSpeedY *= -1;
            score += 10;
            playSound(500, 10);
            redrawBrick(brickRow, brickColumn);
            drawScore();
        }
    }
}

void playSound(int frequency, int duration) {
    if (soundEnabled) {
        M5Cardputer.Speaker.tone(frequency, duration);
    }
}

void gameOver() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setCursor((SCREEN_WIDTH - M5Cardputer.Display.textWidth("GAME OVER")) / 2, SCREEN_HEIGHT / 2);
    M5Cardputer.Display.println("GAME OVER");
    delay(3000);
    lives = 0; // Prevent re-entering this section
M5Cardputer.Display.fillScreen(TFT_BLACK);
    initializeBricks();
    resetBallAndPaddle();
    drawScore();


}

void restartGame() {
    score = 0;
    lives = 3;
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    initializeBricks();
    resetBallAndPaddle();
    drawScore();
}

void initializeBricks() {
    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLUMNS; j++) {
            bricks[i][j] = random(0, 2); // Randomize brick presence for new level
        }
    }
    drawBricks();
}

void resetBallAndPaddle() {
    paddleX = SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2;
    ballX = paddleX + PADDLE_WIDTH / 2 - BALL_SIZE / 2;
    ballY = SCREEN_HEIGHT - PADDLE_HEIGHT - BALL_SIZE - 10;
    ballSpeedY = -1;
    drawPaddle();
    drawBall();
}

void redrawElements() {
    if (paddleX != lastPaddleX) {
        M5Cardputer.Display.fillRect(lastPaddleX, SCREEN_HEIGHT - PADDLE_HEIGHT, PADDLE_WIDTH, PADDLE_HEIGHT, TFT_BLACK);
        drawPaddle();
        lastPaddleX = paddleX;
    }

    if (ballX != lastBallX || ballY != lastBallY) {
        M5Cardputer.Display.fillRect(lastBallX, lastBallY, BALL_SIZE, BALL_SIZE, TFT_BLACK);
        drawBall();
        lastBallX = ballX;
        lastBallY = ballY;
    }
}

void drawPaddle() {
    M5Cardputer.Display.fillRect(paddleX, SCREEN_HEIGHT - PADDLE_HEIGHT, PADDLE_WIDTH, PADDLE_HEIGHT, PADDLE_COLOR);
}

void drawBall() {
    M5Cardputer.Display.fillRect(ballX, ballY, BALL_SIZE, BALL_SIZE, BALL_COLOR);
}

void drawBricks() {
    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLUMNS; j++) {
            if (bricks[i][j]) {
                M5Cardputer.Display.fillRect(j * BRICK_WIDTH, i * BRICK_HEIGHT, BRICK_WIDTH - 1, BRICK_HEIGHT - 1, BRICK_COLOR[i % BRICK_COLORS]);
            }
        }
    }
}

void redrawBrick(int row, int col) {
    M5Cardputer.Display.fillRect(col * BRICK_WIDTH, row * BRICK_HEIGHT, BRICK_WIDTH - 1, BRICK_HEIGHT - 1, TFT_BLACK);
}

void drawScore() {
    M5Cardputer.Display.fillRect(SCORE_X - 10, SCORE_Y - 10, 50, 20, TFT_BLACK);
    M5Cardputer.Display.setCursor(SCORE_X, SCORE_Y);
    M5Cardputer.Display.print("Score: ");
    M5Cardputer.Display.print(score);
}
