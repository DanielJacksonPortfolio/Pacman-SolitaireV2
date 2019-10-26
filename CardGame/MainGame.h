#pragma once

#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

#define SCREENH 1000 // Pixel height of screen
#define SCREENW 1000 // Pixel width of screen

#include "S2D/S2D.h"

#include <sstream>

using namespace S2D;

class MainGame : public Game
{
private:
	// VARIABLES (PACMAN)

	bool _debug = false;
	bool _debugKeyDown = false;
	int frameTime = 16;	
	int _ghostMoveTimer = 0;
	bool inTunnel = false;
	bool _killKeyDown = false;
	string mapArray[32];
	int mode = 1;
	bool _moveKeyDown = false;
	int nodeCount = 0;
	int _numberOfDots = 0;
	int _numberOfEnergizers = 0;
	int _numberOfTiles = 0;
	bool _pacmanDead = false;
	bool _pauseKeyDown = false;
	bool _paused = false;
	bool quitKeyDown = false;
	bool _restartKeyDown = false;
	int _score = 0;
	int screenW = 448, screenH = 496; //Pacman Game Size
	bool _started = true;

	//ANIMATIONS
	int _coffeeAnimationTimer = 0;
	int _coffeeFrame = 0;
	int _dotAnimationTimer = 0;
	int _dotFrame = 0;
	int _energizerAnimationTimer = 0;
	int _energizerFrame = 0;
	float animationInterpolation = 0.0f;

	//POSITIONS
	Vector2* _coffeePosition;
	Vector2* _debugStringPosition = new Vector2(10.0f, 25.0f);
	Vector2* _dotPositions[242] = {};
	Vector2* _energizerPosition[4] = {};
	Vector2* _menuStringPosition = new Vector2(SCREENW / 2, SCREENH / 2);
	Vector2* _nodePositions[3472] = {};

	typedef pair<int, int> Pair;

	struct sprite // Storage Contianer for all spirte data (pacman and ghost)
	{
		Vector2* vPos;
		Pair gPos;
		int width = 32;
		int deathAnimationTimer = 0;
		int animationTimer = 0;
		int frame = 0;
		int currentDirection = 0;
		float moveSpeed = 8.0f;
		int nextDirection = 0;
		float movePercent = 0;
		Texture2D* texture = new Texture2D();
		Rect* textureRect;
		Pair lastPosition = gPos;

	}_pacman, _ghost;

	sprite& pacman = _pacman;
	sprite& ghost = _ghost;

	//SIZES
	int _coffeeWidth = 32;
	int _dotWidth = 4;
	int _energizerWidth = 16;

	//SOUNDS
	SoundEffect * _deathSound = new SoundEffect();
	SoundEffect * _eatDotSound = new SoundEffect();
	SoundEffect * _eatFruitSound = new SoundEffect();

	//TEXTURES
	Texture2D* _coffeeTexture = new Texture2D();
	Rect* _coffeeTextureRect = new Rect(1009.0f, 30.2f, _coffeeWidth, _coffeeWidth);
	Texture2D* _dotTexture = new Texture2D();
	Rect* _dotTextureRect = new Rect(975.0f, 30.2f, _dotWidth, _dotWidth);
	Texture2D* _energizerTexture = new Texture2D();
	Rect* _energizerTextureRect = new Rect(975.0f, 50.0f, _energizerWidth, _energizerWidth);
	Texture2D* _mapTexture = new Texture2D();
	Rect* _mapTextureRect = new Rect(0.0f, 0.0f, 448, 496);
	Texture2D* _menuTexture = new Texture2D();
	Rect* _menuTextureRect = new Rect(SCREENW/2-screenW/2, SCREENH / 2 - screenH / 2, screenW, screenH);

public:

	struct Card // Stores info on card
	{
		int val;
		bool hidden = true;
		int suit;
		int color;
		Card(int val, int suit);
	};

	struct Deck // Stores info on a deck
	{
		Vector2* position;
		Vector2* direction;
		vector<Card*> cards;
		int spacing;
		string ID;
		bool shuffle;
		Rect hitBoxRect;
		Rect outlineRect;
		Deck(Vector2 positionVal, Vector2 directionVal, vector<array<int, 2>> & cardVals, int spaces, string id, bool shuff);
		void Draw();
		void Shuffle();
		void UpdateRects();
	};

	struct Mode // Stores info on a mode
	{
		vector<Deck> decks;
		vector<Deck> deckList;
		bool undo;
		Mode(vector<Deck> & idecks, bool iundo);
	};

	// VARIABLES (SOLITAIRE)

	int cardsInDeck;
	int deckLength;
	bool deckTurnover;
	int discardLength;
	bool hiddenCard;
	int hintID;
	vector<vector<int>> hintList;
	bool holding = false;
	int holdOffset[2] = { 0,0 };
	bool hPressed;
	bool gameEnd;
	int gameEndTimer;
	int lastDeckIndex;
	bool leftButtonPressed = false;
	int mainBoardID;
	vector<Mode> modes;
	stringstream numCardsText;
	bool output = true;
	bool rightButtonPressed = false;
	bool showHint;
	int solvabilityMode = 2;
	bool spacePressed;
	int stackIndex;
	stringstream text;
	bool three;
	int timeElapsed;

	// ALL FUNCTIONS (CLASS-BASED)

	virtual void AIPlayerV2();
	virtual void AnimateDirection(sprite & sprite, int elapsedTime);
	bool virtual CheckBoxCollision(float objectXValue, float objectYValue, sprite & sprite, int objectWidth, float modifier);
	virtual void CheckMapCollision(sprite & sprite);
	virtual void CheckMoveKey(sprite & sprite, Input::KeyboardState* keyboardState, Input::Keys keyPressed, int direction);
	virtual void CycleHints();
	virtual bool DirectionCheck(sprite & sprite, int elapsedTime, int directionToCheck);
	virtual void Draw(int elapsedTime);
	virtual void DrawAString(const char * text, float posX, float posY, int color);
	virtual void DrawBoard();
	virtual void EatEntity(float * objectXValue, float * objectYValue, sprite & sprite, int objectWidth, int modifier, int * _score, int scoreModifier, SoundEffect * sound);
	virtual void EvaluateGameState();
	virtual vector<vector<int>> FindPossibleMoves(int possMoveMode);
	virtual void GetInput();
	virtual void HandleCollisions();
	virtual void HandleHand();
	virtual void HintCheck();
	virtual void InitializeModes();
	virtual void InitializePacmanGame();
	virtual void InitializeSolitaireGame();
	virtual void KeyToggle(Input::KeyboardState* keyboardState, Input::Keys keynum, bool * keybool, bool * changebool);
	virtual void LoadContent();
	MainGame(int argc, char* argv[]);
	virtual ~MainGame();
	virtual void MoveSprite(sprite & sprite, int elapsedTime);
	virtual void PopCard(int deck1, int deck2, bool addUnder);
	virtual void ProcessAnimation(int elapsedTime);
	virtual void SaveSolvableDeck();
	virtual void SolvableDeck(int gamemode); 
	virtual void Update(int elapsedTime);
	virtual void UpdatePosition(sprite & sprite);
	virtual void UpdateText(string itext);

};