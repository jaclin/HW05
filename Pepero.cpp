#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdio.h>
#include <vector>
#include <cstdlib>
#include <cmath>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

SDL_Window* gWindow = nullptr;
SDL_Renderer* renderer = nullptr;

// Class Declaration
class Ball
{
 public:

  // Ball dimensions
  int bRadius = 30;
  int bMass = 10;// in grams
  int bPosX, bPosY;
  int velX, velY;

  // Unique ID
  const int uid;
  int hitByID = 0;

  Ball();
  void checkBallBoundaries(Ball& );
  void collisionDetection(Ball& );
  void determineCollisionPoint(Ball& );
  double distance( int , int , int , int  );
  void render();
  void move();
  ~Ball();

  private:
     // uid hack
    static int newUID;
    // Ball Boundaries Flag
    bool overlappedBoundary;
    // Collision Points
    int collisionX;
    int collisionY;
    // Collision Velocity Holder
    int newVelX = 0;
    int newVelY = 0;
    // colorsssss
    int red = rand() % 255;
    int green = rand() % 255;
    int blue = rand() % 255;
};

// Ball UID hack
int Ball::newUID = 0;

class BallGroup
{
  public:
   static const int ballAmount = 10;
   std::vector<Ball> BallGroupContainer;

   BallGroup();
   void render();
   void move();
   void collisionDetection();
   ~BallGroup();
};

// Ball Class Implementation and methods

Ball::Ball(): uid(newUID++)
{
// Starting position
  bPosX = rand() % int(SCREEN_WIDTH/1.5) + bRadius;
  bPosY = rand() % int(SCREEN_HEIGHT/1.5) + bRadius;

// Velocity
  velX = rand() % 9 + 1;
  velY = rand() % 9 + 1;
}

void Ball::checkBallBoundaries(Ball& otherBall)
{
 if (this->bPosX + this->bRadius + otherBall.bRadius > otherBall.bPosX
  && this->bPosX < otherBall.bPosX + this->bRadius + otherBall.bRadius
  && this->bPosY + this->bRadius + otherBall.bRadius > otherBall.bPosY
  && this->bPosY < otherBall.bPosY + this->bRadius + otherBall.bRadius)
 {
     this->overlappedBoundary = true;
     printf("%d", this->overlappedBoundary);
     printf("\n");
 }
 else
 {
     this->overlappedBoundary = false;
     printf("%d", this->overlappedBoundary);
     printf("\n");
 }
}

void Ball::collisionDetection(Ball& otherBall)
{
  this->checkBallBoundaries(otherBall);

  if (this->overlappedBoundary && this->uid != otherBall.uid)
  {
   // Reset flag
   this->overlappedBoundary = false;

   int totalRadius = this->bRadius + otherBall.bRadius;

   if(distance(this->bPosX, this->bPosY, otherBall.bPosX, otherBall.bPosY) < totalRadius)
   {
     this->determineCollisionPoint(otherBall);

     // Compute for vector of ball collision
     double vn_x = this->bPosX - otherBall.bPosX;
     double vn_y = this->bPosY - otherBall.bPosY;
     double vn_mag = std::sqrt(vn_x * vn_x + vn_y * vn_y);

     // Compute for unit vector
     double uv_x = vn_x / vn_mag;
     double uv_y = vn_y / vn_mag;

     // Compute for unit tangent
     double ut_x = -vn_y / vn_mag;
     double ut_y = vn_x / vn_mag;

     // Compute scalar projections of velocities unto uv and ut
      // Normal vector
     double v1n_x = uv_x * this->velX;
     double v1n_y = uv_y * this->velY;
     double v2n_x = uv_x * otherBall.velX;
     double v2n_y = uv_y * otherBall.velY;
      // Tangential Vector
     double v1t_x = ut_x * this->velX;
     double v1t_y = ut_y * this->velY;
     double v2t_x = ut_x * otherBall.velX;
     double v2t_y = ut_y * otherBall.velY;

     // Compute new tangential velocity
     double v1tPrime_x = v1t_x;
     double v1tPrime_y = v1t_y;
     double v2tPrime_x = v2t_x;
     double v2tPrime_y = v2t_y;

     // Compute new normal velocities using elasticity equations in normal direction
     double v1nPrime_x = (v1n_x * (this->bMass - otherBall.bMass) + 2. * otherBall.bMass * v2n_x) / (this->bMass * otherBall.bMass);
     double v1nPrime_y = (v1n_y * (this->bMass - otherBall.bMass) + 2. * otherBall.bMass * v2n_y) / (this->bMass * otherBall.bMass);
     double v2nPrime_x = (v2n_x * (otherBall.bMass - this->bMass) + 2. * this->bMass * v1n_x) / (this->bMass * otherBall.bMass);
     double v2nPrime_y = (v2n_y * (otherBall.bMass - this->bMass) + 2. * this->bMass * v1n_y) / (this->bMass * otherBall.bMass);

     // Compute new normal and tangential velocity vectors
     double v_v1nPrime_x = v1nPrime_x * uv_x;
     double v_v1nPrime_y = v1nPrime_y * uv_y;
     double v_v2nPrime_x = v2nPrime_x * uv_x;
     double v_v2nPrime_y = v2nPrime_y * uv_y;

     double v_v1tPrime_x = v1tPrime_x * ut_x;
     double v_v1tPrime_y = v1tPrime_y * ut_y;
     double v_v2tPrime_x = v2tPrime_x * ut_x;
     double v_v2tPrime_y = v2tPrime_y * ut_y;

     // Set new velocities
     this->velX = v_v1nPrime_x + v_v1tPrime_x;
     this->velY = v_v1nPrime_y + v_v1tPrime_y;
     otherBall.velX = v_v2nPrime_x + v_v2tPrime_x;
     otherBall.velY = v_v2nPrime_y + v_v2tPrime_y;

     // Adjust positions
     this->bPosX += this->velX;
     this->bPosY += this->velY;
     otherBall.bPosX += otherBall.velX;
     otherBall.bPosY += otherBall.velY;

     // Determine new velocities
//     this->newVelX = (this->velX * (this->bMass - otherBall.bMass) + (2 * otherBall.bMass * otherBall.velX)) / (this->bMass + otherBall.bMass);
//     this->newVelY = (this->velY * (this->bMass - otherBall.bMass) + (2 * otherBall.bMass * otherBall.velY)) / (this->bMass + otherBall.bMass);
//     otherBall.newVelX = (otherBall.velX * (otherBall.bMass - this->bMass) + (2 * this->bMass * this->velX)) / (this->bMass + otherBall.bMass);
//     otherBall.newVelY = (otherBall.velY * (otherBall.bMass - this->bMass) + (2 * this->bMass * this->velY)) / (this->bMass + otherBall.bMass);
//her
//     // Adjust ball positions
//     this->bPosX += this->newVelX;
//     this->bPosY += this->newVelY;
//     otherBall.bPosX += otherBall.newVelX;
//     otherBall.bPosY += otherBall.newVelY;
//
//     // Adjust speed
//     this->velX = this->newVelX;
//     this->velY = this->newVelY;
//     otherBall.velX = otherBall.newVelX;
//     otherBall.velY = otherBall.newVelY;
//
//     // Reset velocity holders
//     this->newVelX = 0;
//     this->newVelY = 0;
//     otherBall.newVelX = 0;
//     otherBall.newVelY = 0;
   }
  }
}

void Ball::determineCollisionPoint(Ball& otherBall)
{
 this->collisionX =
 ((this->bPosX * otherBall.bRadius) + (otherBall.bPosX * this->bRadius))
 / (this->bRadius + otherBall.bRadius);

 this->collisionY =
 ((this->bPosY * otherBall.bRadius) + (otherBall.bPosY * this->bRadius))
 / (this->bRadius + otherBall.bRadius);


}

void Ball::move(){

  if (bPosX <= 0+bRadius)
  {
    this->bPosX = bRadius;
    velX *= -1;
  }
  if  (bPosX >= SCREEN_WIDTH-bRadius)
  {
    this->bPosX = SCREEN_WIDTH-bRadius;
    velX *= -1;
  }

  if (bPosY <= 0+bRadius)
  {
    this->bPosY = bRadius;
    velY *= -1;
  }
  if (bPosY >= SCREEN_HEIGHT-bRadius)
  {
    this->bPosY = SCREEN_HEIGHT-bRadius;
    velY *= -1;
  }
  bPosX += velX;
  bPosY += velY;
}

double Ball::distance( int x1, int y1, int x2, int y2 )
{
  int distance = sqrt(
            ((x1 - x2) * (x1 - x2))
          + ((y1 - y2) * (y1 - y2))
           );
  return distance;
}

void Ball::render()
{
  filledCircleRGBA(renderer, bPosX, bPosY, bRadius, red, green, blue, 255);
}

Ball::~Ball(){}

// BallGroup Class Implementation and methods
BallGroup::BallGroup(){
 for (int i = 0; i < ballAmount; i++){
  BallGroupContainer.push_back(Ball());
 }
}

void BallGroup::move(){
 for (auto &x : BallGroupContainer){
  x.move();
 }
}

void BallGroup::render(){
 for (auto &x : BallGroupContainer){
  x.render();
 }
}

void BallGroup::collisionDetection(){
    for (auto &pCircle : BallGroupContainer){
      for(auto &tCircle : BallGroupContainer)
            pCircle.collisionDetection(tCircle);
    }
}

BallGroup::~BallGroup(){}

// SDL init(), load(), and close() from LazyFoo Productions
bool init()
{
  bool success = true;

  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    success = false;
  }
  else
  {
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
    {
      printf("Warning: Linear texture filtering not enabled!");
    }

    gWindow = SDL_CreateWindow("Pepero", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if(gWindow == NULL)
    {
      printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
      success = false;
    }
    else
    {
      renderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

      if(renderer == NULL)
      {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        success = false;
      }
      else
      {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      }
    }
  }
  return success;
}

void close()
{
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;
  renderer = NULL;

  SDL_Quit();
}

int main(int argc, char* args[])
{
  bool quit = false;
  BallGroup lol;

  if(!init())
  {
    printf("Failed to initialize!\n");
  }
  else
  {
    SDL_Event e;
    while(!quit)
    {
      uint capTimer = SDL_GetTicks();
      lol.move();
      lol.collisionDetection();
      while(SDL_PollEvent(&e) != 0)
      {
        if(e.type == SDL_QUIT)
        {
          quit = true;
        }
      }
      // Clear Screen
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderClear(renderer);
      // Render Objects
      lol.render();
      // Render other objects
      SDL_RenderPresent(renderer);
      SDL_Delay(200);
    }
  }
  close();
  return 0;
}
