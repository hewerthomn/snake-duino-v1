/**
*  Snake Duino v1
*
* Inspirated in Snake v1.0, Ouarrak Ayoub
* http://pastebin.com/iAVt9AGJ
*/

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

/* constants */
#define LEFT   1
#define RIGHT  2
#define UP     3
#define DOWN   4
#define MAX_WIDTH  84
#define MAX_HEIGHT 48

/* defaults */
#define ORIGINAL  true
#define SNAKE_LEN 10

/* lcd display */
Adafruit_PCD8544 lcd = Adafruit_PCD8544(7, 6, 5, 4, 3);

int speakerPin = 8;

int x = 5, y = 5;
    
/* snake face */    
int xC, yC;

/* position food */
int xFood = 0, yFood = 0;

int point = 0, points = 10;

boolean left  = false,
        right = true,
        up    = false,
        down  = false,
        pause = true;
        
int dr = 0,
    dc = 1,
    i;

// vetor containing the coordinates of the individual parts
// of the snake {cols[0], row[0]}, corresponding to the head
int snakeCols[260];

// Vector containing the coordinates of the individual parts 
// of the snake {cols [snake_lenght], row [snake_lenght]} correspond to the tail
int snakeRow[260];

int snakeLen = SNAKE_LEN;

int level = 0,
    time  = 20;
    
// sensors input
int btUp, btDown, btRight, btLeft;

void(* reset)(void) = 0;
/*
* setup
*/
void setup()
{
  Serial.begin(9600);
  randomSeed(analogRead(1));
  
  lcd.begin();
  lcd.setContrast(60);
  
  pinMode(speakerPin, OUTPUT);
  digitalWrite(speakerPin, LOW);
  
  //  pinMode(9,  OUTPUT);
//  pinMode(10, OUTPUT);
//  digitalWrite(9,  HIGH);
//  digitalWrite(10, HIGH);
  
  xFood = lcd.width()  / 2;
  yFood = lcd.height() / 2;
  
  intro();
}

/*
*  loop
*/
void loop()
{
  xC = snakeCols[0];
  yC = snakeRow[0];
  
  if(point == 0 || point == points)
  {
    level++;
    printLevel();
  }
  
  lcd.clearDisplay();
  
  btUp    = analogRead(A2);
  btLeft  = analogRead(A5);
  btDown  = analogRead(A4);
  btRight = analogRead(A3);
  
  moveSnake(btLeft, btRight, btUp, btDown);
  
  // the snake has eaten the food (right or left)
  for(int i=0; i < 3; i++)
  {
    // control the snake's head (x) with x-coordinates of the food
    if((xC+1 == xFood) or (xC == xFood+1))
    {
      // control the snake's head (y) with y-coordinates of the food
      if((yC == yFood) or (yC+1 == yFood) or (yC == yFood+1))
      {
        eatFood();
      }
    }
    
    // the snake has eaten the food (from above or from bellow)
    if((yC == yFood) or (yC == yFood+i))
    {
      if((xC == xFood) or (xC+i == xFood) or (xC == xFood+i))
      {
        eatFood();
      }
    }    
  }
  
  /* LEFT */
  if(left == true)
  {
    // snake touches the left wall
    if(xC == 1) gameover();
    if(xC  > 1) drawSnake();
  }
  
  /* RIGHT */
  if(right == true)
  {
    // snake touches the top wall
    if(xC == MAX_WIDTH-1) gameover();
    if(xC  < MAX_WIDTH-1) drawSnake();
  }
  
  /* UP */
  if(up == true)
  {
    // snake touches the above wall
    if(yC == 1) gameover();
    if(yC  > 1) drawSnake();
  }
  
  /* DOWN */
  if(down == true)
  {
    // snake touches the ground
    if(yC == MAX_HEIGHT-1) gameover();
    if(yC  < MAX_HEIGHT-1) drawSnake();
  }
  
  delay(time);
}

void eatFood()
{
  beep(2000, 10);
  
  // increase the point and snake lenght
  point++;
  snakeLen += 2;
  
  // new coordinates food randonly
  xFood = random(1, 80);
  yFood = random(1, 46);
  
  drawSnake();  
}

void drawSnake()
{
  lcd.drawRect(0, 0, MAX_WIDTH, MAX_HEIGHT, BLACK);
  
  for(int i = snakeLen; i > 0; i--)
  {
    lcd.drawCircle(snakeCols[i], snakeRow[i], 1, BLACK);
  }
  
  lcd.fillRect(xFood, yFood, 3, 3, BLACK);
  lcd.display();
  
  for(int i = snakeLen; i > 0; i--)
  {
    snakeRow[i]  = snakeRow[i - 1];
    snakeCols[i] = snakeCols[i - 1];
  }
  
  snakeRow[0]  += dr;
  snakeCols[0] += dc;
}

/*
 * moveSnake
 */
void moveSnake(int k, int l, int m, int j)
{
  const int me = 9;
  /* PAUSE */
  if(digitalRead(me) == HIGH)
  {
    Serial.println("Pause!");
//    showPause();    
  }
  
  /* LEFT */
  if(k > 900 and right == false)
  {
    if((xC > 0 or xC <= lcd.width() - xC))
      direc(LEFT);
  }
  
  /* RIGHT */
  if(l > 900 and left == false)
  {
    if((xC > 0 or xC <= lcd.width() - xC))
      direc(RIGHT);
  }
  
  /* UP */
  if(m > 900 and down == false)
  {
    if((yC > 0 or yC <= lcd.height() - yC))
      direc(UP);
  }
  
  /* DOWN */
  if(j > 900 and up == false)
  {
    if((yC > 0 or yC <= lcd.height() - yC));
      direc(DOWN);
  }
}

void showPause()
{
  lcd.clearDisplay();
  lcd.setTextSize(2);
  lcd.setTextColor(BLACK);
  lcd.println("Pause");
  lcd.println("");
  lcd.setTextSize(1);
  lcd.print("Level:  ");
  lcd.println(level);
  lcd.print("Points: ");
  lcd.println(point - 1);
  lcd.display();
  
  delay(2500);
}

/*
 * printLevel
 */
void printLevel()
{
  if(!ORIGINAL)
  {
    lcd.clearDisplay();
    lcd.setTextSize(2);
    lcd.println("");
    lcd.print("Level ");
    lcd.println(level);
    lcd.display();
    delay(2000);
  }
  
  upLevel();
  
  point   = 1;
  points += 10;
  
  if(!ORIGINAL)
  {
    resetGame();
  }
}

/*
*  upLevel
*/
void upLevel()
{
  if(level > 1)
  {
    time -= 4;
  }
}

/*
 * direc
 */
void direc(int d)
{
  switch(d)
  {
    case 1: { left=true ; right=false; up=false; down=false; dr = 0; dc = -1;} break;
    case 2: { left=false; right=true ; up=false; down=false; dr = 0; dc = +1;} break;
    case 3: { left=false; right=false; up=true ; down=false; dr = -1; dc = 0;} break;
    case 4: { left=false; right=false; up=false; down=true ; dr = +1; dc = 0;} break;
  }
}

/*
 * gameover
 */
void gameover()
{
  beep(1000, 100);
  
  lcd.clearDisplay();
  lcd.setTextSize(2);
  lcd.println("Game");
  lcd.print("   Over");
  lcd.setTextSize(1);
  lcd.print("Level:  ");
  lcd.println(level);
  lcd.print("Points: ");
  lcd.println(point -1);
  lcd.display();
  delay(2500);
  
  lcd.clearDisplay();
  lcd.setTextSize(2);
  lcd.println("");
  lcd.println("  Try");
  lcd.println(" Again!");
  lcd.display();  
  delay(1500);
  
  point  = 0;
  points = 10;
  level  = 0;
  
  resetGame();  
}

/*
 * beep
 */
void beep(int frequencyInHertz, long timeInMilliseconds)
{
  int x;
  long delayAmount = (long)(1000000/frequencyInHertz);
  long loopTime    = (long)((timeInMilliseconds*1000)/(delayAmount*2));
  for(x=0; x<loopTime; x++)
  {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(delayAmount);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(delayAmount);
  }
  
  delay(2);
  // a little delay to make all notes sound separate
}

/*
 * resetGame
 */
void resetGame()
{
 snakeLen = SNAKE_LEN;
 
 for(int i=0; i < (snakeLen-1); i++)
 {
   snakeCols[i] = i;
   snakeRow[i]  = (MAX_HEIGHT / 2);
 }
 
 up    = false; 
 right = true;
 down  = false;
 left  = false;
 dr    = 0;
 dc    = 1;
}

/*
* intro
*/
void intro()
{
  lcd.clearDisplay();
  lcd.setTextSize(1);
  lcd.println("");
  lcd.print("SnakeDuino ");
  lcd.setTextColor(WHITE, BLACK);
  lcd.println("v1");
  lcd.setTextColor(BLACK);
  lcd.println("");
  lcd.println("");
  lcd.println("By");
  lcd.println("hewertho.mn");
  lcd.display();
  delay(3400);
  
  lcd.clearDisplay();
  lcd.setTextSize(2);
  lcd.println("");
  lcd.println(" Enjoy!");
  lcd.display();
  lcd.setTextColor(BLACK);
  delay(2000);
}

