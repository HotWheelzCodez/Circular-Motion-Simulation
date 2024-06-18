#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include "../include/raylib/raylib.h"

#define RAYGUI_IMPLEMENTATION 
#include "../include/raylib/raygui.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define RADIUS 150

enum TireTypes
{
  TIRE_COMFORT_HARD = 0,
  TIRE_COMFORT_MEDIUM = 1,
  TIRE_COMFORT_SOFT = 2,
  TIRE_SPORT_HARD = 3,
  TIRE_SPORT_MEDIUM = 4,
  TIRE_SPORT_SOFT = 5,
  TIRE_RACING_HARD = 6,
  TIRE_RACING_MEDIUM = 7,
  TIRE_RACING_SOFT = 8
};

// Function declartion
// ----------------------------
float GetAndDisplayTires(bool* tires, Font font);

// Main entry point
// ------------------
int main(void) 
{
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Car Simulation"); 

  if (!IsWindowReady())
  {
    std::cerr << "Window failed to open! Please try again!" << std::endl;
    return EXIT_FAILURE;
  }

  SetTargetFPS(60); 

  Font font = LoadFontEx("fonts/opensans.ttf", 100, 0, 0);

  bool tires[9] = { false };
  tires[0] = true;
  bool showTires = false;
  int level = 1;

  int basePlayerRadius = 270;
  int playerRadius = basePlayerRadius;

  int baseEnemyRadius = 180;
  int enemyRadius = baseEnemyRadius;

  int value = 80;

  Texture2D playerTexture = LoadTexture("images/top-down-gtr.png"); 
  float playerWidth = playerTexture.width;
  float playerHeight = playerTexture.height;
  float playerX = (WINDOW_WIDTH>>1);
  float playerY = (WINDOW_HEIGHT>>1)-playerRadius;
  float playerAngle = 0.0f;
  float playerFriction = 0.8f;
  float playerVelocity = 0.0f;  
  float playerMass = 1800;
  float playerNormalForce = playerMass*10.0f;
  float playerFrictionForce = playerNormalForce * playerFriction;
  float playerCentForce = (playerMass*(playerVelocity*playerVelocity))/playerRadius;
  bool playerAccelating = false;
  bool playerDecelating = false;
  bool playerWin = false;
  bool playerTrackLimits = false;
  int playerTip = -1;
  std::string playerTips[10] = {
    "Try changing the speed to get around the Mustang",
    "Try changing your tire for a higher coefficient of friction, allowing you to go faster",
    "Ensure your centripetal force does not exceed your friction force too much",
    "Friction Force is Normal Force * Coefficient of Friction",
    "Centripetal Force is (mass * radius^2)/radius"
  };

  Texture2D enemyTexture = LoadTexture("images/top-down-mustang.png");
  float enemyWidth = enemyTexture.width;
  float enemyHeight = enemyTexture.height;
  float enemyX = (WINDOW_WIDTH>>1);
  float enemyY = (WINDOW_HEIGHT>>1)-enemyRadius;
  float enemyAngle = 0.0f;
  float enemyFriction = 0.8f;
  float enemyVelocity = 0.0f;
  float enemyMass = 1900;
  float enemyMaxVelocity = 2222.0f;
  bool enemyWin = false;

  int inputMaxVelocity = 150;
  bool editMode = false;
  bool run = false;
  bool drawWin = false;
  bool enemyWinDraw = false;
  std::string displayMode = "RUN";

  // Main loop
  // ----------------------------
  while (!WindowShouldClose()) 
  {
    BeginDrawing(); 
    ClearBackground(LIGHTGRAY); 

    if (level > 3) 
    {
      enemyWin = false;
      playerWin = false;
      playerFriction = 0.8f;
      for (size_t i = 0; i < 9; i++)
        tires[i] = false;
      tires[0] = true;

      const char* victorMessage = "Congratulations! You passed all 3 levels!";
      DrawTextEx(font, victorMessage, { (WINDOW_WIDTH>>1)-(MeasureTextEx(font, victorMessage, 20, 2).x/2), (WINDOW_HEIGHT>>1)-10 }, 20, 2, WHITE);

      if (GuiButton({ (WINDOW_WIDTH>>1)-100, WINDOW_HEIGHT/4.0f, 200, 20 }, "Play Again"))
        level = 1;
      EndDrawing();
      continue;
    }

    switch (level)
    {
      case 1:
        enemyMaxVelocity = (80/3.6f)*100;
        inputMaxVelocity = 150;
        break;
      case 2:
        enemyMaxVelocity = (120/3.6f)*100;
        inputMaxVelocity = 220;
        break;
      case 3:
        enemyMaxVelocity = (200/3.6f)*100;
        inputMaxVelocity = 300;
        break;
    }

    // Draw track
    DrawCircle(WINDOW_WIDTH>>1, WINDOW_HEIGHT>>1, RADIUS<<1, GRAY);
    DrawCircle(WINDOW_WIDTH>>1, WINDOW_HEIGHT>>1, RADIUS+10, BROWN);
    DrawCircle(WINDOW_WIDTH>>1, WINDOW_HEIGHT>>1, RADIUS, GREEN);
    for (int i = 0; i < 360; i+=30)
      DrawRing({ WINDOW_WIDTH>>1, WINDOW_HEIGHT>>1 }, (RADIUS<<1)-(RADIUS>>1), (RADIUS<<1)-(RADIUS>>1)+10, i, i+15, 10, YELLOW);
    for (int i = 0; i < 7; i++)
      DrawRectangleRec({ (WINDOW_WIDTH>>1)-25, (i*20.0f)+5, 50, 10 }, WHITE);

    std::string frictionDisplay = "Friction: -0." + std::to_string((int)(((level-1)*0.1f)*10));
    DrawTextEx(font, frictionDisplay.c_str(), { (WINDOW_WIDTH>>1)-(MeasureTextEx(font, frictionDisplay.c_str(), 20, 2).x/2), (WINDOW_HEIGHT>>1)-12 }, 20, 2, WHITE);

    std::string enemyMaxVelocityDisplay = "Enemy Max Speed: " + std::to_string((int)(enemyMaxVelocity*3.6f)/100) + " km/h";
    DrawTextEx(font, enemyMaxVelocityDisplay.c_str(), { (WINDOW_WIDTH>>1)-(MeasureTextEx(font, enemyMaxVelocityDisplay.c_str(), 20, 2).x/2), (WINDOW_HEIGHT>>1)+12 }, 20, 2, WHITE);

    std::string playerMaxVelocityDisplay = "Your Max Speed: " + std::to_string((int)inputMaxVelocity) + " km/h";
    DrawTextEx(font, playerMaxVelocityDisplay.c_str(), { (WINDOW_WIDTH>>1)-(MeasureTextEx(font, enemyMaxVelocityDisplay.c_str(), 20, 2).x/2), (WINDOW_HEIGHT>>1)+30 }, 20, 2, WHITE);

    if (GuiButton({ WINDOW_WIDTH-55, 50, 50, 20 }, displayMode.c_str()))
    {
      run = !run;
      enemyWinDraw = false;
      drawWin = false;
      if (displayMode == "RUN")
        displayMode = "STOP";
      else
        displayMode = "RUN";
    }

    if (!run)
    {
      playerAngle = 0;
      playerVelocity = 0;
      playerRadius = basePlayerRadius;
      enemyAngle = 0;
      enemyVelocity = 0;
      displayMode = "RUN";

      DrawTextureEx(playerTexture, { (WINDOW_WIDTH>>1)-(playerTexture.width*0.25f)/2, (WINDOW_HEIGHT>>1)-(float)playerRadius-(playerTexture.height*0.25f)/2 }, 0, 0.25f, WHITE);
      DrawTextureEx(enemyTexture, { (WINDOW_WIDTH>>1)-(enemyTexture.width*0.25f)/2, (WINDOW_HEIGHT>>1)-(float)enemyRadius-(enemyTexture.height*0.25f)/2 }, 0, 0.25f, WHITE);
      if (GuiValueBox((Rectangle){ WINDOW_WIDTH-155, 5, 150, 20 }, "Speed in km/h: ", &value, 80, inputMaxVelocity, editMode))
        editMode = !editMode;

      if (GuiButton({ 5, 5, 50, 25 }, "Tires"))
        showTires = !showTires;

      if (showTires)
        playerFriction = GetAndDisplayTires(tires, font) - (level-1)*0.1f;
      playerFrictionForce = playerNormalForce*playerFriction;

      std::string levelDisplay = "Level: " + std::to_string(level) + "/3";
      DrawTextEx(font, levelDisplay.c_str(), { WINDOW_WIDTH-5-MeasureTextEx(font, levelDisplay.c_str(), 20, 2).x, 25 }, 20, 2, BLACK);

      if (playerWin)
        drawWin = true;

      if (enemyWin)
      {
        playerTip++;
        enemyWinDraw = true;
      }

      if (drawWin)
      {
        const char* completionMessage = "Good job you completed the level!";
        DrawTextEx(font, completionMessage, { (WINDOW_WIDTH>>1)-(MeasureTextEx(font, completionMessage, 30, 2).x/2), 20 }, 30, 2, WHITE);
      }

      if (enemyWinDraw)
      {
        const char* lostMessage = "Oh no! You lost to a Mustang! Better find a way to beat it!";
        const char* trackLimitsMessage = "You exceeded track limits!";
        DrawTextEx(font, lostMessage, { (WINDOW_WIDTH>>1)-(MeasureTextEx(font, lostMessage, 30, 2).x/2), 30 }, 30, 2, RED);
        if (playerTrackLimits)
          DrawTextEx(font, trackLimitsMessage, { (WINDOW_WIDTH>>1)-(MeasureTextEx(font, trackLimitsMessage, 30, 2).x/2), 50 }, 30, 2, RED);
        DrawTextEx(font, playerTips[playerTip].c_str(), { 0, WINDOW_HEIGHT-20 }, 20, 2, RED);
      }

      playerWin = false;
      enemyWin = false;

      EndDrawing();
      continue;
    }

    float deltaTime = GetFrameTime();

    playerAngle += playerVelocity/playerRadius * deltaTime;
    if (playerAngle > 360) 
      playerAngle = 0;

    enemyAngle += enemyVelocity/enemyRadius * deltaTime;
    if (enemyAngle > 360)
      enemyAngle = 0;

    float playerRad = playerAngle*DEG2RAD;
    playerX = (WINDOW_WIDTH>>1)+sin(playerRad)*playerRadius;  
    playerY = (WINDOW_HEIGHT>>1)-cos(playerRad)*playerRadius;

    float enemyRad = enemyAngle*DEG2RAD;
    enemyX = (WINDOW_WIDTH>>1)+sin(enemyRad)*enemyRadius;
    enemyY = (WINDOW_HEIGHT>>1)-cos(enemyRad)*enemyRadius;

    // Draw player
    Vector2 playerOrigin = { playerWidth*0.25f/2, playerHeight*0.25f/2 };
    Rectangle playerSrc = { 0, 0, playerWidth, playerHeight };
    Rectangle playerDest = { playerX, playerY, playerWidth*0.25f, playerHeight*0.25f };
    DrawTexturePro(playerTexture, playerSrc, playerDest, playerOrigin, playerAngle, WHITE);

    // Draw enemy
    Vector2 enemyOrigin = { enemyWidth*0.25f/2, enemyHeight*0.25f/2 };
    Rectangle enemySrc = { 0, 0, enemyWidth, enemyHeight };
    Rectangle enemyDest = { enemyX, enemyY, enemyWidth*0.25f, enemyHeight*0.25f };
    DrawTexturePro(enemyTexture, enemySrc, enemyDest, enemyOrigin, enemyAngle, WHITE);
    DrawLine(WINDOW_WIDTH>>1, WINDOW_HEIGHT>>1, enemyX, enemyY, WHITE);
    DrawLine(WINDOW_WIDTH>>1, WINDOW_HEIGHT>>1, playerX, playerY, WHITE);
    std::string enemyDisplayRadius = "r = " + std::to_string(enemyRadius) + 'm';
    DrawTextEx(font, enemyDisplayRadius.c_str(), { (WINDOW_WIDTH>>1)+sin(enemyRad)*(enemyRadius/2), (WINDOW_HEIGHT>>1)-cos(enemyRad)*(enemyRadius/2) }, 20, 2, WHITE);
    std::string playerDisplayRadius = "r = " + std::to_string(playerRadius) + 'm';
    DrawTextEx(font, playerDisplayRadius.c_str(), { (WINDOW_WIDTH>>1)+sin(playerRad)*(playerRadius/2), (WINDOW_HEIGHT>>1)-cos(playerRad)*(playerRadius/2) }, 20, 2, WHITE);

    // Draw GUI
    std::string enemySpeedText = "Speed in km/h: ";
    enemySpeedText += std::to_string((int)round((enemyVelocity/100)*3.6f));
    DrawTextEx(font, enemySpeedText.c_str(), { enemyX-enemyWidth*0.25f/2, enemyY-enemyHeight*0.25f/2 }, 20, 2, WHITE);
    std::string enemyMassText = "Mass in kg: ";
    enemyMassText += std::to_string((int)enemyMass);
    DrawTextEx(font, enemyMassText.c_str(), { enemyX-enemyWidth*0.25f/2, (enemyY-enemyHeight*0.25f/2)+20 }, 20, 2, WHITE);

    std::string playerSpeedText = "Speed in km/h: ";
    playerSpeedText += std::to_string((int)round((playerVelocity/100)*3.6f));
    DrawTextEx(font, playerSpeedText.c_str(), { playerX-playerWidth*0.25f/2, playerY-playerHeight*0.25f/2 }, 20, 2, WHITE);
    std::string playerMassText = "Mass in kg: ";
    playerMassText += std::to_string((int)playerMass);
    DrawTextEx(font, playerMassText.c_str(), { playerX-playerWidth*0.25f/2, (playerY-playerHeight*0.25f/2)+20 }, 20, 2, WHITE);
 
    playerCentForce = (playerMass*((playerVelocity*3.6f)/100)*((playerVelocity*3.6f)/100))/playerRadius;
    if (playerCentForce > playerFrictionForce) 
    {
      playerRadius = basePlayerRadius + (playerCentForce/playerFriction)/7500;
    }

    if (!editMode)
    {
      if ((value/3.6f)*100 > playerVelocity)
      {
        playerAccelating = true;
        playerDecelating = false;
        playerVelocity += 5.0f;
      }
      else if ((value/3.6f)*100 < playerVelocity)
      {
        playerDecelating = true;
        playerAccelating = false;
        playerVelocity -= 7.5f;
      }

      if (playerAccelating && playerVelocity > (value/3.6f)*100)
      {
        playerAccelating = false;
        playerVelocity = (value/3.6f)*100;
      }
      else if (playerDecelating && playerVelocity < (value/3.6f)*100)
      {
        playerDecelating = false;
        playerVelocity = (value/3.6f)*100;
      }
    }

    if (enemyVelocity < enemyMaxVelocity)
    {
      #if defined(PLATFORM_WEB)
        enemyVelocity += 3.1f;
      #else
        enemyVelocity += 3.1f;
      #endif
    }
    else
      enemyVelocity = enemyMaxVelocity;

    if (playerAngle >= 359.0f)
      playerWin = true;
    if (enemyAngle >= 359.0f)
      enemyWin = true;

    if (!CheckCollisionPointCircle({ playerX, playerY }, { WINDOW_WIDTH>>1, WINDOW_HEIGHT>>1 }, (RADIUS<<1)-8))
    {
      enemyWin = true;
      playerTrackLimits = true;
    }

    if (enemyWin)
      run = false;
    
    if (playerWin)
    {
      level++;
      run = false;
    }

    EndDrawing();
  }

  // Release resources to the OS and shutdown raylib
  // ----------------------------------------------------------
  UnloadTexture(playerTexture);
  UnloadTexture(enemyTexture);
  CloseWindow();

  return EXIT_SUCCESS;
}

float GetAndDisplayTires(bool* tires, Font font)
{
  float friction = 0.8f;

  GuiPanel({ 5 , 30, 310, 400 }, "Tires");

  for (size_t i = 0; i < 9; i++)
  {
    bool prev = tires[i];
    DrawRectangleRec({ 10, 50+(i*40.0f)+10, 280, 30 }, { 25, 70, 150, 255 });
    GuiCheckBox({ 295, 50+(i*40.0f)+20, 10, 10 }, nullptr, &tires[i]);
    if (!prev && tires[i])
    {
      for (size_t j = 0; j < 9; j++)
      {
        if (j != i)
          tires[j] = false;
      }
    }
    if (prev && !tires[i])
      tires[i] = true;
    switch (i)
    {
      case TIRE_COMFORT_HARD:
        DrawTextEx(font, "COMFORT: HARD - u=0.8", { 15, 50+(i*40.0f)+20 }, 20, 2, WHITE);
        if (i == 0 && tires[i])
          friction = 0.8f;
        break;
      case TIRE_COMFORT_MEDIUM:
        DrawTextEx(font, "COMFORT: MEDIUM - u=1", { 15, 50+(i*40.0f)+20 }, 20, 2, WHITE);
        if (i == 1 && tires[i])
          friction = 1.0f;
        break;
      case TIRE_COMFORT_SOFT:
        DrawTextEx(font, "COMFORT: SOFT - u=1.1", { 15, 50+(i*40.0f)+20 }, 20, 2, WHITE);
        if (i == 2 && tires[i])
          friction = 1.1f;
        break;
      case TIRE_SPORT_HARD:
        DrawTextEx(font, "SPORT: HARD - u=1.3", { 15, 50+(i*40.0f)+20 }, 20, 2, WHITE);
        if (i == 3 && tires[i])
          friction = 1.3f;
        break;
      case TIRE_SPORT_MEDIUM:
        DrawTextEx(font, "SPORT: MEDIUM - u=1.5", { 15, 50+(i*40.0f)+20 }, 20, 2, WHITE);
        if (i == 4 && tires[i])
          friction = 1.5f;
        break;
      case TIRE_SPORT_SOFT:
        DrawTextEx(font, "SPORT: SOFT - u=1.6", { 15, 50+(i*40.0f)+20 }, 20, 2, WHITE);
        if (i == 5 && tires[i])
          friction = 1.6f;
        break;
      case TIRE_RACING_HARD:
        DrawTextEx(font, "RACING: HARD - u=1.9", { 15, 50+(i*40.0f)+20 }, 20, 2, WHITE);
        if (i == 6 && tires[i])
          friction = 1.9f;
        break;
      case TIRE_RACING_MEDIUM:
        DrawTextEx(font, "RACING: MEDIUM - u=2.1", { 15, 50+(i*40.0f)+20 }, 20, 2, WHITE);
        if (i == 7 && tires[i])
          friction = 2.1f;
        break;
      case TIRE_RACING_SOFT:
        DrawTextEx(font, "RACING: SOFT - u=2.3", { 15, 50+(i*40.0f)+20 }, 20, 2, WHITE);
        if (i == 8 && tires[i])
          friction = 2.3f;
        break;
    }
  }

  return friction;
}
