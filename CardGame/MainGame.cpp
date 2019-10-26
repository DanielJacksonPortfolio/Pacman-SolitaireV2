// Includes

#include <array>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>

#include "MainGame.h"

// Namespaces

using namespace std;

// Preprocessor Defintions (Constant values)

#define CARDH 96 // Pixel height of a single card
#define CARDW 71 // Pixel width of a single card
#define SCREENH 1000 // Pixel height of screen
#define SCREENW 1000 // Pixel width of screen

// Enums

enum cardvals{ACE = 0,TWO = 1, THREE = 2, FOUR = 3, FIVE = 4, SIX = 5, SEVEN = 6, EIGHT = 7, NINE = 8, TEN = 9, JACK = 10, QUEEN = 11, KING = 12 , FILL = 13, RANDOM = 14, VALCLUBS = 15, VALSPADES = 16, VALHEARTS = 17, VALDIAMONDS = 18}; // Enum values of every playing card as well as other modes such as fill and random
enum colorNums { RED = 0, GREEN = 1, DARKGREEN = 2 , BLACK = 3}; // Colors
enum directions { RIGHT = 0, LEFT = 1, DOWN = 2, UP = 3 }; // Enums for pacman's direction
enum gameModes { PACMAN = 0, SOLITAIRE = 1 }; // The two gamemodes in the system
enum modes {SOLITAIREAI = 0, KLONDIKE = 1, MENU = 2, SPIDER1SUIT = 3, SPIDER2SUIT = 4, SPIDER = 5, PYRAMID = 6}; // The various modes of solitaire
enum solitaireDeckNames {SHAND = 13, SSTACK1 = 12, SSTACK2 = 11, SSTACK3 = 10, SSTACK4 = 9, SDISCARD = 8,  SDECK = 7, SCOLUMN1 = 6, SCOLUMN2 = 5, SCOLUMN3 = 4, SCOLUMN4 = 3, SCOLUMN5 = 2, SCOLUMN6 = 1, SCOLUMN7 = 0}; // The deck names for solitaire
enum suits { SUITCLUBS = 0, SUITSPADES = 1, SUITHEARTS = 2, SUITDIAMONDS = 3 }; // The 4 playing card suits

Texture2D* cardsTexture = new Texture2D(); // The texture of playing cards (tile sheet)
Color * Colors[4] = { new Color(0.58823529411f, 0.0f, 0.0f),new Color(0.0f, 0.58823529411f, 0.0f),new Color(0.0f, 0.50980392156f, 0.0f) , new Color(0.0f, 0.0f, 0.0f) }; // List of Red, Green, Dark Green and black s2d colors
int currentMode = 0; // Current Solitaire Mode

//Prototype Various Non-class-based functions
void ChangeState(Input::Keys keyPressed, bool * keyState, bool * effect, Input::KeyboardState* keyboardState); // Change a boolean variable from true to false or vice versa on given key press
void ChangeTexture(int frame, float * xCoordPtr, float * yCoordPtr, float xCoord1, float xCoord2, float yCoord1, float yCoord2); // Change the texture of a two frame animation to the opposite frame's texture
void DrawColorRect(const Color * color, float rposX, float rposY, int width, int height); // Draws a colored rectangle of given size
void DrawRect(Texture2D * texture, float vposX, float vposY, float rposX, float rposY, int width, int height); // Draws a rectangle based on given parameters including size and texture
void FlipFrame(int * animationTimer, int * Frame, int delay); // Change frame to alternate frame
void ModifyValue(int condition, int conditionValue, float * value, float modifyValue); // Changes a value by a modifier based on a condition, i.e.MoveSpeed
vector<string> SplitString(string stringToSplit, char delimeter); // Splits a string at a given delimiter

void ChangeState(Input::Keys keyPressed, bool * keyState, bool * effect, Input::KeyboardState* keyboardState)
{
	if (keyboardState->IsKeyDown(keyPressed) && !(*keyState)) // If given key is pressed and wasn't already...
	{
		*effect = !(*effect); // Reverse boolean of effect
		*keyState = true; // Make Key bool pressed
	}
	if (keyboardState->IsKeyUp(keyPressed)) // If key is unpressed
		*keyState = false; // Unpress key bool
}

void ChangeTexture(int frame, float * xCoordPtr, float * yCoordPtr, float xCoord1, float xCoord2, float yCoord1, float yCoord2)
{
	if (frame == 0)
	{
		*xCoordPtr = xCoord1;
		*yCoordPtr = yCoord1;
	}
	else
	{
		*xCoordPtr = xCoord2;
		*yCoordPtr = yCoord2;
	}
}

void DrawColorRect(const Color * color, float rposX, float rposY, int width, int height)
{
	Rect * drawRect = new Rect(rposX, rposY, width, height); // Make a new rectangle
	SpriteBatch::DrawRectangle(drawRect, color); // Draw rectangle with given color
	delete drawRect; // delete rectangle
}

void DrawRect(Texture2D * texture, float vposX, float vposY, float rposX, float rposY, int width, int height)
{
	Rect * drawRect = new Rect(rposX, rposY, width, height); // make a new rectangle
	Vector2 * position = new Vector2(vposX, vposY); // make new vector
	SpriteBatch::Draw(cardsTexture, position, drawRect); // draw rectangle with given texture in position
	delete drawRect; // delete instances
	delete position;
}

void FlipFrame(int * animationTimer, int * Frame, int delay)
{
	(*animationTimer)++; // Increment animation timer
	if (*animationTimer == delay) // if delay passed
	{
		if (*Frame == 0) // filp frame from 0 to 1 or vice versa
			*Frame = 1;
		else
			*Frame = 0;
		*animationTimer = 0;
	}
}

void ModifyValue(int condition, int conditionValue, float * value, float modifyValue)
{
	if (condition == conditionValue)
		*value = *value + modifyValue;
}

vector<string> SplitString(string stringToSplit, char delimeter)
{
	stringstream stringStream(stringToSplit);  // Create string stream of inputted string
	string splitString; // Create string container
	vector<string> splitStrings; // vector of strings
	while (getline(stringStream, splitString, delimeter)) // while there are strings to be split
		splitStrings.push_back(splitString); // push split string into vector
	return splitStrings; // return vector
}

void MainGame::AIPlayerV2()
{
	if (output)
	{
		SpriteBatch::BeginDraw();
		DrawBoard();
		SpriteBatch::EndDraw();
	}
	if (!gameEnd)
	{
		if (modes[currentMode].decks[SSTACK1].cards.size() > 0 && modes[currentMode].decks[SSTACK2].cards.size() > 0 && modes[currentMode].decks[SSTACK3].cards.size() > 0 && modes[currentMode].decks[SSTACK4].cards.size() > 0)
			if (modes[currentMode].decks[SSTACK1].cards.back()->val == KING && modes[currentMode].decks[SSTACK2].cards.back()->val == KING && modes[currentMode].decks[SSTACK3].cards.back()->val == KING && modes[currentMode].decks[SSTACK4].cards.back()->val == KING)
			{
				if (solvabilityMode == 0)
					SaveSolvableDeck();
				gameEnd = true;
				return;
			}
		vector<vector<int>> possibleMoves = FindPossibleMoves(0); // Move Type, Deck1, Deck2, Number Of Cards

		for (int i = 0; i < possibleMoves.size(); i++)
		{
			string currentMove = modes[currentMode].decks[possibleMoves[i][1]].ID + " -> " + modes[currentMode].decks[possibleMoves[i][2]].ID;
			bool cardChanged = false;
			if (possibleMoves[i][0] == 1 || possibleMoves[i][0] == 2 || possibleMoves[i][0] == 3 || possibleMoves[i][0] == 4)
			{
				PopCard(possibleMoves[i][1], possibleMoves[i][2], false);
				if (possibleMoves[i][0] == 1 || possibleMoves[i][0] == 2)
				{
					if (modes[currentMode].decks[possibleMoves[i][1]].cards.size() > 0)
					{
						if (modes[currentMode].decks[possibleMoves[i][1]].cards.back()->hidden)
						{
							cardChanged = true;
							modes[currentMode].decks[possibleMoves[i][1]].cards.back()->hidden = false;
						}
					}
				}
				deckTurnover = false;
			}
			else if (possibleMoves[i][0] == 5 || possibleMoves[i][0] == 6)
			{
				for (int j = 0; j < possibleMoves[i][3]; j++)
				{
					PopCard(possibleMoves[i][1], SHAND, false);
				}
				for (int j = 0; j < possibleMoves[i][3]; j++)
				{
					PopCard(SHAND, possibleMoves[i][2], false);
				}
				if (modes[currentMode].decks[possibleMoves[i][1]].cards.size() > 0)
				{
					if (modes[currentMode].decks[possibleMoves[i][1]].cards.back()->hidden)
					{
						cardChanged = true;
						modes[currentMode].decks[possibleMoves[i][1]].cards.back()->hidden = false;
					}
				}
				deckTurnover = false;
			}
			else if (possibleMoves[i][0] == 7 || possibleMoves[i][0] == 8)
			{
				PopCard(possibleMoves[i][1], possibleMoves[i][2], false);
				deckTurnover = false;
			}
			else if (possibleMoves[i][0] == 9 || possibleMoves[i][0] == 10 || possibleMoves[i][0] == 11)
			{
				for (int j = 0; j < possibleMoves[i][3]; j++)
				{
					PopCard(possibleMoves[i][1], possibleMoves[i][2], false);

					if (modes[currentMode].decks[possibleMoves[i][2]].cards.size() > 0)
					{
						if (possibleMoves[i][0] == 11)
							modes[currentMode].decks[possibleMoves[i][2]].cards.back()->hidden = true;
						else
							modes[currentMode].decks[possibleMoves[i][2]].cards.back()->hidden = false;

						cardChanged = true;
					}
				}
				if (possibleMoves[i][0] == 11)
				{
					deckTurnover = true;
				}
			}

			if (output)
				cout << currentMove << endl;

			AIPlayerV2();
			if (gameEnd)
			{
				if (output)
					cout << "Return" << endl;
				return;
			}

			int tempDeck = possibleMoves[i][2];
			possibleMoves[i][2] = possibleMoves[i][1];
			possibleMoves[i][1] = tempDeck;
			if (possibleMoves[i][0] == 1 || possibleMoves[i][0] == 2 || possibleMoves[i][0] == 3 || possibleMoves[i][0] == 4)
			{
				if (possibleMoves[i][0] == 1 || possibleMoves[i][0] == 2)
					if (modes[currentMode].decks[possibleMoves[i][2]].cards.size() > 0)
					{
						if (cardChanged)
						{
							modes[currentMode].decks[possibleMoves[i][2]].cards.back()->hidden = true;
						}
					}
				PopCard(possibleMoves[i][1], possibleMoves[i][2], false);
			}
			else if (possibleMoves[i][0] == 5 || possibleMoves[i][0] == 6)
			{
				if (modes[currentMode].decks[possibleMoves[i][2]].cards.size() > 0)
				{
					if (cardChanged)
					{
						modes[currentMode].decks[possibleMoves[i][2]].cards.back()->hidden = true;
					}
				}
				for (int j = 0; j < possibleMoves[i][3]; j++)
				{
					PopCard(possibleMoves[i][1], SHAND, false);
				}
				for (int j = 0; j < possibleMoves[i][3]; j++)
				{
					PopCard(SHAND, possibleMoves[i][2], false);
				}
			}
			else if (possibleMoves[i][0] == 7 || possibleMoves[i][0] == 8)
			{
				PopCard(possibleMoves[i][1], possibleMoves[i][2], false);
			}
			else if (possibleMoves[i][0] == 9 || possibleMoves[i][0] == 10 || possibleMoves[i][0] == 11)
			{
				for (int j = 0; j < possibleMoves[i][3]; j++)
				{
					if (cardChanged)
					{
						if (modes[currentMode].decks[possibleMoves[i][1]].cards.size() > 0)
						{

							if (possibleMoves[i][0] == 11)
								modes[currentMode].decks[possibleMoves[i][1]].cards.back()->hidden = false;
							else
								modes[currentMode].decks[possibleMoves[i][1]].cards.back()->hidden = true;
						}
					}
					PopCard(possibleMoves[i][1], possibleMoves[i][2], false);
				}
			}
		}
		if (output)
			cout << "Return" << endl;
		return;
	}
}

void MainGame::AnimateDirection(sprite & sprite, int elapsedTime)
{
	if (DirectionCheck(sprite, elapsedTime, sprite.currentDirection) || inTunnel)
	{
		if (sprite.currentDirection == directions::RIGHT)
		{
			sprite.vPos->X = SCREENW / 2 - screenW / 2 + sprite.gPos.second * 16.0f - 8.0f + (16.0f * sprite.movePercent);
			sprite.vPos->Y = SCREENH / 2 - screenH / 2 + sprite.gPos.first * 16.0f - 8.0f;
		}
		else if (sprite.currentDirection == directions::LEFT)
		{
			sprite.vPos->X = SCREENW / 2 - screenW / 2 + sprite.gPos.second * 16.0f - 8.0f - (16.0f * sprite.movePercent);
			sprite.vPos->Y = SCREENH / 2 - screenH / 2 + sprite.gPos.first * 16.0f - 8.0f;
		}
		else if (sprite.currentDirection == directions::DOWN)
		{
			sprite.vPos->X = SCREENW / 2 - screenW / 2 + sprite.gPos.second * 16.0f - 8.0f;
			sprite.vPos->Y = SCREENH / 2 - screenH / 2 + sprite.gPos.first * 16.0f - 8.0f + (16.0f * sprite.movePercent);
		}
		else if (sprite.currentDirection == directions::UP)
		{
			sprite.vPos->X = SCREENW / 2 - screenW / 2 + sprite.gPos.second * 16.0f - 8.0f;
			sprite.vPos->Y = SCREENH / 2 - screenH / 2 + sprite.gPos.first * 16.0f - 8.0f - (16.0f * sprite.movePercent);
		}
	}
	else
	{
		sprite.vPos->X = SCREENW / 2 - screenW / 2 + sprite.gPos.second * 16.0f - 8.0f;
		sprite.vPos->Y = SCREENH / 2 - screenH / 2 + sprite.gPos.first * 16.0f - 8.0f;
	}

}

MainGame::Card::Card(int ival, int isuit)
{
	val = ival;
	suit = isuit;
	color = floor(suit / 2);
}

bool MainGame::CheckBoxCollision(float objectXValue, float objectYValue, sprite & sprite, int objectWidth, float modifier)
{
	if (!(objectXValue >= sprite.vPos->X + sprite.width - modifier || objectXValue + objectWidth <= sprite.vPos->X + modifier || objectYValue >= sprite.vPos->Y + sprite.width - modifier || objectYValue + objectWidth <= sprite.vPos->Y + modifier))
		return true;
	else
		return false;
}

void MainGame::CheckMapCollision(sprite & sprite)
{
	if (sprite.gPos.second >= 0 && sprite.gPos.second < 29) // Prevents the parser going off grid (for wrapping)
		if (mapArray[sprite.gPos.first][sprite.gPos.second] == '1' || mapArray[sprite.gPos.first][sprite.gPos.second + 1] == '1' || mapArray[sprite.gPos.first + 1][sprite.gPos.second + 1] == '1' || mapArray[sprite.gPos.first + 1][sprite.gPos.second] == '1')
			sprite.gPos = sprite.lastPosition;
}

void MainGame::CheckMoveKey(sprite & sprite, Input::KeyboardState* keyboardState, Input::Keys keyPressed, int direction)
{
	if (keyboardState->IsKeyDown(keyPressed))
	{
		if (!_moveKeyDown)
			sprite.nextDirection = direction;
		_moveKeyDown = true;
	}
}

void MainGame::CycleHints()
{
	if (hintList.size() > 0)
	{
		hintID = (hintID + 1) % (hintList.size());
		//cout << "HintID: " << hintID << " " << modes[currentMode].decks[hintList[hintID][1]].ID << " -> " << modes[currentMode].decks[hintList[hintID][2]].ID << endl;
	}
}

MainGame::Deck::Deck(Vector2 iposition, Vector2 idirection, vector<array<int, 2>> & icards, int ispacing, string iID, bool ishuffle)
{
	cards = {};
	if (icards.size() != 0)
	{
		if (icards[0][0] == FILL) // [0][x] -> x = number of packs 
			for (int i = 0; i < icards[0][1]; i++) // Each Deck of 52
				for (int j = 0; j < 4; j++) // Each suit
					for (int k = 0; k < 13; k++) // Each Value in suit
						cards.push_back(new Card(k, j));

		else if (icards[0][0] == RANDOM) // [0][x] -> x = number of packs 
		{
			srand(time(0));
			int randNum = rand() % 3; // Random suit
			for (int i = 0; i < icards[0][1]; i++) // Number of Decks
				for (int j = 0; j < 13; j++) // Each Value in suit
					cards.push_back(new Card(j, randNum));
		}

		else
			for (int i = 0; i < icards.size(); i++)
			{
				if (icards[i][0] == VALCLUBS || icards[i][0] == VALSPADES || icards[i][0] == VALHEARTS || icards[i][0] == VALDIAMONDS)
					for (int j = 0; j < icards[i][1]; j++) // Number of Decks
						for (int k = 0; k < 13; k++) // Value
						{
							if (icards[i][0] == VALCLUBS) cards.push_back(new Card(k, SUITCLUBS));
							else if (icards[i][0] == VALSPADES) cards.push_back(new Card(k, SUITSPADES));
							else if (icards[i][0] == VALHEARTS) cards.push_back(new Card(k, SUITHEARTS));
							else if (icards[i][0] == VALDIAMONDS) cards.push_back(new Card(k, SUITDIAMONDS));
						}
				else
					cards.push_back(new Card(icards[i][0], icards[i][1]));
			}
	}

	position = new Vector2(iposition);
	direction = new Vector2(idirection);
	spacing = ispacing;
	ID = iID;
	shuffle = ishuffle;
	hitBoxRect = Rect(position->X, position->Y, CARDW, CARDH);
	outlineRect = Rect(position->X - 3, position->Y - 3, CARDW + 6, CARDH + 6);
	if (shuffle) Shuffle();
	UpdateRects();
}

bool MainGame::DirectionCheck(sprite & sprite, int elapsedTime, int directionToCheck)//Checks if next 'move' will be cause a collision and returns it as a bool
{
	inTunnel = false;
	Pair tempgPos = sprite.gPos;
	if (directionToCheck == directions::RIGHT)
		tempgPos.second += 1;
	else if (directionToCheck == directions::LEFT)
		tempgPos.second -= 1;
	else if (directionToCheck == directions::DOWN)
		tempgPos.first += 1;
	else if (directionToCheck == directions::UP)
		tempgPos.first -= 1;

	if (tempgPos.second >= 0 && tempgPos.second < 29) // Prevents the parser going off grid (for wrapping)
		if (mapArray[tempgPos.first][tempgPos.second] == '1' || mapArray[tempgPos.first][tempgPos.second + 1] == '1' || mapArray[tempgPos.first + 1][tempgPos.second + 1] == '1' || mapArray[tempgPos.first + 1][tempgPos.second] == '1')
		{
			return false;
		}

	if ((sprite.gPos.second <= 5 || sprite.gPos.second >= 24) && (sprite.gPos.first == 14 || sprite.gPos.first == 15))
	{
		inTunnel = true;
		return false;
	}

	return true;
}

void MainGame::DrawAString(const char * text, float posX, float posY, int color)
{
	Vector2 * position = new Vector2(posX, posY);
	SpriteBatch::DrawString(text, position, Colors[color]);
	delete position;
}

void MainGame::Deck::Draw()
{
	if (cards.size() > 0)
	{
		if (!(direction->X == 0.0f && direction->Y == 0.0f))
			for (int i = 0; i < cards.size(); i++)
			{
				if (cards[i]->hidden == false)
					DrawRect(cardsTexture, position->X + direction->X*spacing*i, position->Y + direction->Y*spacing*i, (cards[i]->val)*CARDW, cards[i]->suit*CARDH, CARDW, CARDH);
				else
					DrawRect(cardsTexture, position->X + direction->X*spacing*i, position->Y + direction->Y*spacing*i, 0, 4 * CARDH, CARDW, CARDH);
			}
		else
		{
			if (cards.back()->hidden == false)
				DrawRect(cardsTexture, position->X, position->Y, (cards.back()->val)*CARDW, cards.back()->suit*CARDH, CARDW, CARDH);
			else
				DrawRect(cardsTexture, position->X, position->Y, 0, 4 * CARDH, CARDW, CARDH);

		}
	}
	else if (currentMode != PYRAMID || ID == "Discard" || ID == "Deck")
		DrawColorRect(Colors[DARKGREEN], float(hitBoxRect.X), float(hitBoxRect.Y), hitBoxRect.Width, hitBoxRect.Height);
}

void MainGame::Draw(int elapsedTime)
{
	if (mode == PACMAN)
	{
		stringstream debugStream, menuStream, startStream;
		debugStream << "Pacman vPos - X: " << pacman.vPos->X << " Y: " << pacman.vPos->Y << "\nPacman gPos - X: " << pacman.gPos.first << " Y : " << pacman.gPos.second << "\nGhost vPos - X: " << ghost.vPos->X << " Y: " << ghost.vPos->Y << "\nGhost gPos - X: " << ghost.gPos.first << " Y : " << ghost.gPos.second << "\nMove Speed: " << pacman.moveSpeed << "\nDirection: " << pacman.currentDirection << "\nScore: " << _score;
		menuStream << "Game Paused";
		startStream << "Press any key to begin";

		if (_started)
		{
			if (!_paused)
				ProcessAnimation(elapsedTime);

			SpriteBatch::BeginDraw();

			SpriteBatch::Draw(_mapTexture, new Vector2(SCREENW / 2 - screenW / 2, SCREENH / 2 - screenH / 2), _mapTextureRect);
			SpriteBatch::Draw(_coffeeTexture, _coffeePosition, _coffeeTextureRect);

			for (int i = 0; i < _numberOfDots; i++)
				SpriteBatch::Draw(_dotTexture, _dotPositions[i], _dotTextureRect);

			for (int i = 0; i < 4; i++)
				SpriteBatch::Draw(_energizerTexture, _energizerPosition[i], _energizerTextureRect);

			SpriteBatch::Draw(ghost.texture, ghost.vPos, ghost.textureRect);

			SpriteBatch::Draw(pacman.texture, pacman.vPos, pacman.textureRect);

			DrawColorRect(Colors[BLACK], SCREENW / 2 - screenW / 2 - 64.0f, SCREENH / 2 - 16.0f, 32, 32);
			DrawColorRect(Colors[BLACK], SCREENW / 2 - screenW / 2 - 64.0f, SCREENH / 2 - 16.0f, 32, 32);

			if (_debug)
			{

				SpriteBatch::DrawString(debugStream.str().c_str(), _debugStringPosition, Color::White);

				for (int i = 0; i < nodeCount; i++)
					SpriteBatch::DrawRectangle(_nodePositions[i]->X - 1, _nodePositions[i]->Y - 1, 2, 2, Color::Blue);
			}

			DrawColorRect(Colors[BLACK], SCREENW / 2 - screenW / 2 - 64.0f, SCREENH / 2 - 32.0f, 64, 32);
			DrawColorRect(Colors[BLACK], SCREENW / 2 + screenW / 2, SCREENH / 2 - 32.0f, 64, 32);

			if (_paused)
			{
				SpriteBatch::Draw(_menuTexture, _menuTextureRect, nullptr);
				SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::White);
			}
		}
		else
		{
			SpriteBatch::Draw(_menuTexture, _menuTextureRect, nullptr);
			SpriteBatch::DrawString(startStream.str().c_str(), _menuStringPosition, Color::White);
		}
		SpriteBatch::EndDraw();
	}
	else
	{
		if (currentMode != SOLITAIREAI)
		{
			EvaluateGameState();
		}
		else
		{
			AIPlayerV2();
			if (solvabilityMode != 0)
				currentMode = MENU;
			InitializeSolitaireGame();
		}
	}
}

void MainGame::DrawBoard()
{
	if (gameEnd == false)
	{
		for (Deck deck : modes[currentMode].decks)
			deck.Draw();

		UpdateText(to_string(modes[currentMode].decks[SDECK].cards.size()));
		DrawAString(numCardsText.str().c_str(), CARDW / 2, CARDH*1.5, RED);
		if (currentMode == KLONDIKE)
			DrawAString("Press H for hints", 20.0f, SCREENH - 60.0f, RED);

		if (modes[currentMode].decks[SDECK].cards.size() == 0 && modes[currentMode].decks[SDISCARD].cards.size() == 0 && !holding)
		{
			hiddenCard = false;
			for (Deck deck : modes[currentMode].decks)
				if (hiddenCard != true)
					if (deck.ID.substr(0, 2) == "MB")
						for (Card * card : deck.cards)
							if (card->hidden)
							{
								hiddenCard = true;
								break;
							}

			if (hiddenCard == false)
			{
				if (modes[currentMode].decks[SSTACK1].cards.size() > 0 && modes[currentMode].decks[SSTACK2].cards.size() > 0 && modes[currentMode].decks[SSTACK3].cards.size() > 0 && modes[currentMode].decks[SSTACK4].cards.size() > 0)
					if (modes[currentMode].decks[SSTACK1].cards.back()->val != KING || modes[currentMode].decks[SSTACK2].cards.back()->val != KING || modes[currentMode].decks[SSTACK3].cards.back()->val != KING || modes[currentMode].decks[SSTACK4].cards.back()->val != KING)
					{
						bool checkBreak = false;
						for (Deck deck : modes[currentMode].decks)
						{
							if (!checkBreak)
								if (deck.ID.substr(0, 2) == "MB")
									for (int i = 1; i < 5; i++)
									{
										for (int j = 0; j < modes[currentMode].decks.size(); j++)
											if (modes[currentMode].decks[j].ID == ("Stack" + to_string(i)))
												stackIndex = j;
										if (deck.cards.size() > 0 && modes[currentMode].decks[stackIndex].cards.size() > 0)
											if (deck.cards.back()->val == modes[currentMode].decks[stackIndex].cards.back()->val + 1 && deck.cards.back()->suit == modes[currentMode].decks[stackIndex].cards.back()->suit)
											{
												for (int j = 0; j < modes[currentMode].decks.size(); j++)
													if (modes[currentMode].decks[j].ID == deck.ID)
														PopCard(j, stackIndex, false);
												checkBreak = true;
												break;
											}
									}
								else
									break;
						}
					}
			}
		}
	}
}

void MainGame::EatEntity(float * objectXValue, float * objectYValue, sprite & sprite, int objectWidth, int modifier, int * _score, int scoreModifier, SoundEffect * sound)
{
	if (CheckBoxCollision(*objectXValue, *objectYValue, sprite, objectWidth, modifier))
	{
		*_score += scoreModifier;
		*objectXValue = -100;
		*objectYValue = -100;
		Audio::Play(sound);
	}
}

void MainGame::EvaluateGameState()
{
	SpriteBatch::BeginDraw();
	if (currentMode == MENU)
	{
		float xPos = 5.0f;
		float yPos = 20.0f;
		DrawAString("Menu:", xPos, yPos, RED);
		DrawAString("ESCAPE = Exit", SCREENW - 130.0f, yPos, RED);
		DrawAString("1 - Solitaire w/ Solvable Deck", xPos, yPos += 50, RED);
		DrawAString("2 - Solitaire w/ Random Deck", xPos, yPos += 50, RED);
		DrawAString("3 - Pacman Clone", xPos, yPos += 50, RED);
		DrawAString("4 - Solvable Solitaire Deck Finder AI w/ Output (ENDLESS)", xPos, yPos += 50, RED);
		DrawAString("5 - Solvable Solitaire Deck Finder AI No Output (ENDLESS)", xPos, yPos += 50, RED);
		DrawAString("6 - Solvable Solitaire Deck Finder AI w/ Perfect Solution (DEMO)", xPos, yPos += 50, RED);
		DrawAString("7 - Solvable Solitaire Deck Finder AI w/ Solvable Deck (DEMO)", xPos, yPos += 50, RED);
		DrawAString("Press Corresponding Key", xPos, yPos += 100, RED);
	}
	if (!gameEnd)
	{
		if (currentMode == KLONDIKE)
		{
			HandleHand();
			GetInput();
			DrawBoard();
			if (modes[currentMode].decks[SSTACK1].cards.size() > 0 && modes[currentMode].decks[SSTACK2].cards.size() > 0 && modes[currentMode].decks[SSTACK3].cards.size() > 0 && modes[currentMode].decks[SSTACK4].cards.size() > 0)
			{
				if (modes[currentMode].decks[SSTACK1].cards.back()->val == KING && modes[currentMode].decks[SSTACK2].cards.back()->val == KING && modes[currentMode].decks[SSTACK3].cards.back()->val == KING && modes[currentMode].decks[SSTACK4].cards.back()->val == KING)
				{
					//cout << "You Win" << endl;
					modes[currentMode].decks = {};
					UpdateText("You Win");
					gameEnd = true;
				}
			}
		}
	}
	else
	{
		gameEndTimer += timeElapsed;
		if (gameEndTimer < 2000)
			DrawAString(text.str().c_str(), SCREENW / 2 - 50.0f, SCREENH / 2 - 50.0f, RED);
		else
		{
			currentMode = MENU;
			gameEnd = false;
		}
	}
	SpriteBatch::EndDraw();
}

vector<vector<int>> MainGame::FindPossibleMoves(int possMoveMode)
{
	vector<vector<int>> possibleMoves = {};
	for (int currentColumn = 6; currentColumn > -1; currentColumn--) // Iterate through the Columns
	{
		if (modes[currentMode].decks[currentColumn].cards.size() > 0) // if there is a card to check
		{
			for (int currentStack = 12; currentStack > 8; currentStack--) // Iterate through the stacks
			{
				if (modes[currentMode].decks[currentColumn].cards.back()->val != ACE) // if that card is not an ace
				{
					if (modes[currentMode].decks[currentStack].cards.size() > 0) // if the stack you are checking has cards in it
					{
						if (modes[currentMode].decks[currentColumn].cards.back()->suit == modes[currentMode].decks[currentStack].cards.back()->suit && modes[currentMode].decks[currentColumn].cards.back()->val == modes[currentMode].decks[currentStack].cards.back()->val + 1) // if the card can go on the stack
						{
							possibleMoves.insert(possibleMoves.begin(), { 1,currentColumn,currentStack, 1 });
						}
					}
				}
				else // if the card is an ace
				{
					if (modes[currentMode].decks[currentStack].cards.size() == 0) // if the stack is empty (thus the card can go there)
					{
						possibleMoves.insert(possibleMoves.begin(), { 2,currentColumn,currentStack, 1 });
						break;
					}
				}
			}
		}
	}

	if (modes[currentMode].decks[SDISCARD].cards.size() > 0) // if there a card on the discard pile
	{
		for (int currentStack = 12; currentStack > 8; currentStack--) // Iterate through the stacks
		{
			if (modes[currentMode].decks[SDISCARD].cards.back()->val != ACE) // if that card is not an Ace
			{
				if (modes[currentMode].decks[currentStack].cards.size() > 0) // if the stack is not empty
				{
					if (modes[currentMode].decks[SDISCARD].cards.back()->suit == modes[currentMode].decks[currentStack].cards.back()->suit && modes[currentMode].decks[SDISCARD].cards.back()->val == modes[currentMode].decks[currentStack].cards.back()->val + 1) // if the card can go on the stack
					{
						possibleMoves.insert(possibleMoves.begin(), { 3,SDISCARD,currentStack, 1 });
					}
				}
			}
			else // if the card is an ace
			{
				if (modes[currentMode].decks[currentStack].cards.size() == 0)
				{
					possibleMoves.insert(possibleMoves.begin(), { 4,SDISCARD,currentStack, 1 });
					break;
				}
			}
		}
	}

	for (int columnA = 6; columnA > -1; columnA--) // Iterate through columns
	{
		if (modes[currentMode].decks[columnA].cards.size() > 0) // if column not empty
		{
			for (int columnB = 6; columnB > -1; columnB--) // Iterate through columns again
			{
				if (columnA != columnB) // if the two columns are not the same
				{
					int indexRevealed = modes[currentMode].decks[columnA].cards.size(); // The index of the first unhidden card in the column
					int columnASize = modes[currentMode].decks[columnA].cards.size(); // The size of the column  
					for (int i = 0; i < columnASize; i++) // Iterate through the length of the column from top down
					{
						if (modes[currentMode].decks[columnA].cards[i]->hidden != true) // if the card is revealed
						{
							indexRevealed = i; // Save the index
							break; // Break from the loop so you only get the first one
						}
					}

					for (int revealedCards = indexRevealed; revealedCards < columnASize; revealedCards++) // Iterate through from the first index revealed to the size of the deck (The revealed cards only)
					{
						if (modes[currentMode].decks[columnB].cards.size() > 0) // if the second column isn't empty
						{
							if (modes[currentMode].decks[columnA].cards[revealedCards]->color != modes[currentMode].decks[columnB].cards.back()->color && modes[currentMode].decks[columnA].cards[revealedCards]->val + 1 == modes[currentMode].decks[columnB].cards.back()->val) // if the topmost card can go on the end of the second column
							{
								if (modes[currentMode].decks[columnA].cards.size() == 1 || modes[currentMode].decks[columnA].cards.size() > 1 && revealedCards != 0 && (modes[currentMode].decks[columnA].cards[revealedCards - 1]->hidden || modes[currentMode].decks[columnA].cards[revealedCards - 1]->color != modes[currentMode].decks[columnB].cards.back()->color || modes[currentMode].decks[columnA].cards[revealedCards - 1]->val != modes[currentMode].decks[columnB].cards.back()->val))
								{
									int numberOfCards = columnASize - revealedCards; // The total number of unhidden cards (that you can move)
									possibleMoves.insert(possibleMoves.begin(), { 5,columnA,columnB, numberOfCards });
								}
							}
						}
						else if (modes[currentMode].decks[columnB].cards.size() == 0) // if the second column is empty
						{
							if (modes[currentMode].decks[columnA].cards[revealedCards]->val == KING && revealedCards != 0) // if the topmost card is a king, therefore the cards can go on a blank
							{
								int numberOfCards = modes[currentMode].decks[columnA].cards.size() - revealedCards; // The total number of unhidden cards (that you can move)
								possibleMoves.insert(possibleMoves.begin(), { 6,columnA,columnB, numberOfCards });
							}
						}
					}
				}
			}
		}
	}

	if (modes[currentMode].decks[SDISCARD].cards.size() > 0) // if discard pile is not empty
	{
		for (int currentColumn = 6; currentColumn > -1; currentColumn--) // Iterate through columns
		{
			if (modes[currentMode].decks[currentColumn].cards.size() > 0) // if column is not empty
			{
				if (modes[currentMode].decks[SDISCARD].cards.back()->val + 1 == modes[currentMode].decks[currentColumn].cards.back()->val &&  modes[currentMode].decks[SDISCARD].cards.back()->color != modes[currentMode].decks[currentColumn].cards.back()->color) // if card can go on column
				{
					possibleMoves.insert(possibleMoves.begin(), { 7,SDISCARD,currentColumn,1 });
				}
			}
			else if (modes[currentMode].decks[currentColumn].cards.size() == 0) // if column is empty
			{
				if (modes[currentMode].decks[SDISCARD].cards.back()->val == KING) // if card is a King (so can go on a blank space)
				{
					possibleMoves.insert(possibleMoves.begin(), { 8,SDISCARD,currentColumn,1 });
					break;
				}
			}
		}
	}

	// Deck Turnover
	if (possibleMoves.size() == 0 || possMoveMode == 1)
	{
		int size = modes[currentMode].decks[SDECK].cards.size(); // Size of the deck
		if (size > 0) // Cards to be 'discarded'
		{
			if (three) // if the mode is to turn over three cards at a time
			{
				if (size > 3) size = 3;
				possibleMoves.insert(possibleMoves.begin(), { 9,SDECK,SDISCARD,size });
			}
			else
			{
				possibleMoves.insert(possibleMoves.begin(), { 10,SDECK,SDISCARD,1 });
			}
		}
		else // if the deck has not been turned over since the previous valid move and by logic there are no cards to discard
		{
			if (deckTurnover == false || cardsInDeck != modes[currentMode].decks[SDISCARD].cards.size())
			{
				cardsInDeck = modes[currentMode].decks[SDISCARD].cards.size();
				int resetSize = modes[currentMode].decks[SDISCARD].cards.size(); // Size of discard pile
				if (resetSize > 0) // if there are cards in the discard pile
				{
					possibleMoves.insert(possibleMoves.begin(), { 11,SDISCARD,SDECK,resetSize });
				}
			}
		}
	}
	return possibleMoves;
}

void MainGame::GetInput()
{
	if (!gameEnd)
	{
		Input::MouseState * mouseState = Input::Mouse::GetState();
		if (currentMode == KLONDIKE)
		{
			for (Deck deck : modes[currentMode].decks)
				if (deck.hitBoxRect.Contains(Vector2(mouseState->X, mouseState->Y)) || deck.hitBoxRect.Intersects(Rect(modes[currentMode].decks[SHAND].position->X, modes[currentMode].decks[SHAND].position->Y, CARDW, CARDH)))
				{
					if (deck.cards.size() > 0 && deck.ID != "Deck" && mouseState->LeftButton == Input::ButtonState::PRESSED && !holding)
					{
						int size = deck.cards.size();
						for (int i = 0; i < size; i++)
						{
							if (i < size - 1)
							{
								if (deck.ID != "Discard")
									if (Rect(deck.position->X + i * deck.spacing*deck.direction->X, deck.position->Y + i * deck.spacing*deck.direction->Y, CARDW, deck.spacing).Contains(Vector2(mouseState->X, mouseState->Y)))
									{
										if (!deck.cards[i]->hidden)
										{

											int size2 = (size - i);
											for (int j = 0; j < size2; j++)
											{
												int deckID;
												for (int k = 0; k < modes[currentMode].decks.size(); k++)
													if (modes[currentMode].decks[k].ID == deck.ID)
														deckID = k;
												PopCard(deckID, SHAND, true);
											}
											holding = true;
											for (int i = 0; i < modes[currentMode].decks.size(); i++)
												if (modes[currentMode].decks[i].ID == deck.ID)
													lastDeckIndex = i;
											holdOffset[0] = (mouseState->X - deck.position->X - deck.spacing*deck.direction->X*(deck.cards.size() - size2) - 2);
											holdOffset[1] = (mouseState->Y - deck.position->Y - deck.spacing*deck.direction->Y*(deck.cards.size() - size2) - 2);
										}
									}
							}
							else
							{
								if (Rect(deck.position->X + (size - 1)*deck.spacing*deck.direction->X, deck.position->Y + (size - 1)*deck.spacing*deck.direction->Y, CARDW, CARDH).Contains(Vector2(mouseState->X, mouseState->Y)))
								{

									int deckID;
									for (int k = 0; k < modes[currentMode].decks.size(); k++)
										if (modes[currentMode].decks[k].ID == deck.ID)
											deckID = k;

									PopCard(deckID, SHAND, true);
									holding = true;
									for (int i = 0; i < modes[currentMode].decks.size(); i++)
										if (modes[currentMode].decks[i].ID == deck.ID)
											lastDeckIndex = i;
									holdOffset[0] = (mouseState->X - deck.position->X - deck.spacing*deck.direction->X*(size - 1) - 2);
									holdOffset[1] = (mouseState->Y - deck.position->Y - deck.spacing*deck.direction->Y*(size - 1) - 2);
								}
							}
						}
					}
					if (!holding || deck.ID.substr(0, 2) == "MB" || deck.ID.substr(0, 5) == "Stack")
						if (deck.cards.size() > 0 || holding)
							DrawColorRect(Color::White, float(deck.outlineRect.X), float(deck.outlineRect.Y), deck.outlineRect.Width, deck.outlineRect.Height);
				}
		}
		bool changed = false;
		if (mouseState->LeftButton == Input::ButtonState::RELEASED && holding)
		{
			holding = false;
			if (currentMode == KLONDIKE)
				for (Deck deck : modes[currentMode].decks)
					if (deck.ID != modes[currentMode].decks[lastDeckIndex].ID)
						if (deck.hitBoxRect.Contains(Vector2(mouseState->X, mouseState->Y)) || deck.hitBoxRect.Intersects(Rect(modes[currentMode].decks[SHAND].position->X, modes[currentMode].decks[SHAND].position->Y, CARDW, CARDH)))
						{
							if (deck.ID != "Deck" && deck.ID != "Discard")
							{
								if (deck.ID.substr(0, 5) == "Stack")
								{
									if (modes[currentMode].decks[SHAND].cards.size() == 1)
									{
										if (deck.cards.size() > 0)
										{
											if (deck.cards.back()->suit == modes[currentMode].decks[SHAND].cards[0]->suit && modes[currentMode].decks[SHAND].cards[0]->val == deck.cards.back()->val + 1)
											{
												if (modes[currentMode].decks[lastDeckIndex].cards.size() > 0)
													modes[currentMode].decks[lastDeckIndex].cards.back()->hidden = false;
												for (int i = 0; i < modes[currentMode].decks.size(); i++)
													if (modes[currentMode].decks[i].ID == deck.ID)
														lastDeckIndex = i;
												changed = true;
											}
										}
										else
										{
											if (modes[currentMode].decks[SHAND].cards[0]->val == ACE)
											{
												if (modes[currentMode].decks[lastDeckIndex].cards.size() > 0)
													modes[currentMode].decks[lastDeckIndex].cards.back()->hidden = false;
												for (int i = 0; i < modes[currentMode].decks.size(); i++)
													if (modes[currentMode].decks[i].ID == deck.ID)
														lastDeckIndex = i;
												changed = true;
											}
										}
									}
								}
								if (deck.ID.substr(0, 2) == "MB")
								{
									if (deck.cards.size() > 0)
									{
										if (deck.cards.back()->color != modes[currentMode].decks[SHAND].cards[0]->color)
										{
											if (deck.cards.back()->val == modes[currentMode].decks[SHAND].cards[0]->val + 1)
											{
												if (modes[currentMode].decks[lastDeckIndex].cards.size() > 0)
													modes[currentMode].decks[lastDeckIndex].cards.back()->hidden = false;
												for (int i = 0; i < modes[currentMode].decks.size(); i++)
													if (modes[currentMode].decks[i].ID == deck.ID)
														lastDeckIndex = i;
												changed = true;
											}
										}
									}
									else if (modes[currentMode].decks[SHAND].cards[0]->val == KING)
									{
										if (modes[currentMode].decks[lastDeckIndex].cards.size() > 0)
											modes[currentMode].decks[lastDeckIndex].cards.back()->hidden = false;
										for (int i = 0; i < modes[currentMode].decks.size(); i++)
											if (modes[currentMode].decks[i].ID == deck.ID)
												lastDeckIndex = i;
										changed = true;
									}
								}
							}
						}
			HintCheck();
			for (int i = 0; i < modes[currentMode].decks[SHAND].cards.size(); i++)
			{
				modes[currentMode].decks[lastDeckIndex].cards.push_back(modes[currentMode].decks[SHAND].cards[i]);
			}
			modes[currentMode].decks[lastDeckIndex].UpdateRects();
			modes[currentMode].decks[SHAND].cards = {};
			delete modes[currentMode].decks[SHAND].position;
			modes[currentMode].decks[SHAND].position = new Vector2(-1000.0f, -1000.0f);

		}
		if (showHint)
		{
			DrawColorRect(Color::Yellow, float(modes[currentMode].decks[hintList[hintID][1]].outlineRect.X), float(modes[currentMode].decks[hintList[hintID][1]].outlineRect.Y), modes[currentMode].decks[hintList[hintID][1]].outlineRect.Width, modes[currentMode].decks[hintList[hintID][1]].outlineRect.Height);
			DrawColorRect(Color::Yellow, float(modes[currentMode].decks[hintList[hintID][2]].outlineRect.X), float(modes[currentMode].decks[hintList[hintID][2]].outlineRect.Y), modes[currentMode].decks[hintList[hintID][2]].outlineRect.Width, modes[currentMode].decks[hintList[hintID][2]].outlineRect.Height);
		}
	}
}

void MainGame::HandleCollisions()
{
	for (int i = 0; i < _numberOfDots; i++)
		EatEntity(&_dotPositions[i]->X, &_dotPositions[i]->Y, pacman, _dotWidth, 4, &_score, 1, _eatDotSound);
	for (int i = 0; i < 4; i++)
		EatEntity(&_energizerPosition[i]->X, &_energizerPosition[i]->Y, pacman, _energizerWidth, 4, &_score, 10, _eatDotSound);
	EatEntity(&_coffeePosition->X, &_coffeePosition->Y, pacman, _coffeeWidth, 4, &_score, 100, _eatFruitSound);

	CheckMapCollision(pacman);

	if (CheckBoxCollision(ghost.vPos->X, ghost.vPos->Y, pacman, ghost.width, 0.0f))
	{
		_pacmanDead = true;
		Audio::Play(_deathSound);
	}
	CheckMapCollision(ghost);
}

void MainGame::HandleHand()
{
	Input::MouseState * mouseState = Input::Mouse::GetState();
	if (holding)
	{
		delete modes[currentMode].decks[SHAND].position;
		modes[currentMode].decks[SHAND].position = new Vector2(mouseState->X - holdOffset[0], mouseState->Y - holdOffset[1]);
	}
}

void MainGame::HintCheck()
{
	hintID = 0;
	showHint = false;
	if (!gameEnd)
	{
		vector<vector<int>> possibleMoves = FindPossibleMoves(1);
		hintList = possibleMoves;
		hintID = 0;
	}
}

void MainGame::InitializeModes()
{
	modes = {};
	modes.push_back(Mode(vector<Deck> {}, true)); // SOLITAIREAI
	modes[SOLITAIREAI].deckList.push_back(Deck(Vector2(917.0f, 128.0f), Vector2(0.0f, 1.0f), vector<array<int, 2>> {}, 25, "MB7", false));
	modes[SOLITAIREAI].deckList.push_back(Deck(Vector2(824.0f, 128.0f), Vector2(0.0f, 1.0f), vector<array<int, 2>> {}, 25, "MB6", false));
	modes[SOLITAIREAI].deckList.push_back(Deck(Vector2(731.0f, 128.0f), Vector2(0.0f, 1.0f), vector<array<int, 2>> {}, 25, "MB5", false));
	modes[SOLITAIREAI].deckList.push_back(Deck(Vector2(638.0f, 128.0f), Vector2(0.0f, 1.0f), vector<array<int, 2>> {}, 25, "MB4", false));
	modes[SOLITAIREAI].deckList.push_back(Deck(Vector2(545.0f, 128.0f), Vector2(0.0f, 1.0f), vector<array<int, 2>> {}, 25, "MB3", false));
	modes[SOLITAIREAI].deckList.push_back(Deck(Vector2(452.0f, 128.0f), Vector2(0.0f, 1.0f), vector<array<int, 2>> {}, 25, "MB2", false));
	modes[SOLITAIREAI].deckList.push_back(Deck(Vector2(359.0f, 128.0f), Vector2(0.0f, 1.0f), vector<array<int, 2>> {}, 25, "MB1", false));
	if (solvabilityMode == 0) //Random Deck
		modes[SOLITAIREAI].deckList.push_back(Deck(Vector2(10.0f, 10.0f), Vector2(0.0f, 0.0f), vector<array<int, 2>>	{ {FILL, 1}}, 0, "Deck", true));
	else if (solvabilityMode == 1) // Perfect 3 & 1 Card Deal
		modes[SOLITAIREAI].deckList.push_back(Deck(Vector2(10.0f, 10.0f), Vector2(0.0f, 0.0f), vector<array<int, 2>> { {KING, SUITSPADES}, { KING,SUITHEARTS }, { KING,SUITDIAMONDS }, { QUEEN,SUITHEARTS }, { QUEEN,SUITDIAMONDS }, { KING,SUITCLUBS }, { JACK,SUITDIAMONDS }, { QUEEN,SUITCLUBS }, { QUEEN,SUITSPADES }, { JACK,SUITCLUBS }, { JACK,SUITSPADES }, { JACK,SUITHEARTS }, { TEN,SUITSPADES }, { TEN,SUITHEARTS }, { TEN,SUITDIAMONDS }, { NINE,SUITHEARTS }, { NINE,SUITDIAMONDS }, { TEN,SUITCLUBS }, { EIGHT,SUITDIAMONDS }, { NINE,SUITCLUBS }, { NINE,SUITSPADES }, { EIGHT,SUITCLUBS }, { EIGHT,SUITSPADES }, { EIGHT,SUITHEARTS }, { SIX,SUITSPADES }, { FOUR,SUITDIAMONDS }, { SIX,SUITHEARTS }, { THREE,SUITHEARTS }, { FIVE,SUITCLUBS }, { SIX,SUITDIAMONDS }, { TWO,SUITHEARTS }, { THREE,SUITDIAMONDS }, { FIVE,SUITSPADES }, { SEVEN,SUITCLUBS }, { ACE,SUITDIAMONDS }, { TWO,SUITDIAMONDS }, { FOUR,SUITCLUBS }, { FIVE,SUITHEARTS }, { SEVEN,SUITSPADES }, { ACE,SUITSPADES }, { TWO,SUITCLUBS }, { THREE,SUITCLUBS }, { FOUR,SUITSPADES }, { FIVE,SUITDIAMONDS }, { SEVEN,SUITHEARTS }, { ACE,SUITCLUBS }, { ACE,SUITHEARTS }, { TWO,SUITSPADES }, { THREE,SUITSPADES }, { FOUR,SUITHEARTS }, { SIX,SUITCLUBS }, { SEVEN,SUITDIAMONDS } }, 0, "Deck", false));
	else if (solvabilityMode == 2) // Solvable Deck
		SolvableDeck(SOLITAIREAI);
	modes[SOLITAIREAI].deckList.push_back(Deck(Vector2(103.0f, 10.0f), Vector2(1.0f, 0.0f), vector<array<int, 2>> {}, 15, "Discard", false));
	modes[SOLITAIREAI].deckList.push_back(Deck(Vector2(917.0f, 10.0f), Vector2(0.0f, 0.0f), vector<array<int, 2>> {}, 0, "Stack4", false));
	modes[SOLITAIREAI].deckList.push_back(Deck(Vector2(824.0f, 10.0f), Vector2(0.0f, 0.0f), vector<array<int, 2>> {}, 0, "Stack3", false));
	modes[SOLITAIREAI].deckList.push_back(Deck(Vector2(731.0f, 10.0f), Vector2(0.0f, 0.0f), vector<array<int, 2>> {}, 0, "Stack2", false));
	modes[SOLITAIREAI].deckList.push_back(Deck(Vector2(638.0f, 10.0f), Vector2(0.0f, 0.0f), vector<array<int, 2>> {}, 0, "Stack1", false));
	modes[SOLITAIREAI].deckList.push_back(Deck(Vector2(-100.0f, -100.0f), Vector2(0.0f, 1.0f), vector<array<int, 2>> {}, 15, "Hand", false));

	modes.push_back(Mode(vector<Deck> {}, false)); // MENU

	modes.push_back(Mode(vector<Deck> {}, true)); // KLONDIKE
	modes[KLONDIKE].deckList.push_back(Deck(Vector2(917.0f, 128.0f), Vector2(0.0f, 1.0f), vector<array<int, 2>> {}, 25, "MB7", false));
	modes[KLONDIKE].deckList.push_back(Deck(Vector2(824.0f, 128.0f), Vector2(0.0f, 1.0f), vector<array<int, 2>> {}, 25, "MB6", false));
	modes[KLONDIKE].deckList.push_back(Deck(Vector2(731.0f, 128.0f), Vector2(0.0f, 1.0f), vector<array<int, 2>> {}, 25, "MB5", false));
	modes[KLONDIKE].deckList.push_back(Deck(Vector2(638.0f, 128.0f), Vector2(0.0f, 1.0f), vector<array<int, 2>> {}, 25, "MB4", false));
	modes[KLONDIKE].deckList.push_back(Deck(Vector2(545.0f, 128.0f), Vector2(0.0f, 1.0f), vector<array<int, 2>> {}, 25, "MB3", false));
	modes[KLONDIKE].deckList.push_back(Deck(Vector2(452.0f, 128.0f), Vector2(0.0f, 1.0f), vector<array<int, 2>> {}, 25, "MB2", false));
	modes[KLONDIKE].deckList.push_back(Deck(Vector2(359.0f, 128.0f), Vector2(0.0f, 1.0f), vector<array<int, 2>> {}, 25, "MB1", false));
	if (solvabilityMode == 0) //Random Deck
		modes[KLONDIKE].deckList.push_back(Deck(Vector2(10.0f, 10.0f), Vector2(0.0f, 0.0f), vector<array<int, 2>>	{ {FILL, 1}}, 0, "Deck", true));
	else if (solvabilityMode == 1) // Perfect 3 & 1 Card Deal
		modes[KLONDIKE].deckList.push_back(Deck(Vector2(10.0f, 10.0f), Vector2(0.0f, 0.0f), vector<array<int, 2>> { {KING, SUITSPADES}, { KING,SUITHEARTS }, { KING,SUITDIAMONDS }, { QUEEN,SUITHEARTS }, { QUEEN,SUITDIAMONDS }, { KING,SUITCLUBS }, { JACK,SUITDIAMONDS }, { QUEEN,SUITCLUBS }, { QUEEN,SUITSPADES }, { JACK,SUITCLUBS }, { JACK,SUITSPADES }, { JACK,SUITHEARTS }, { TEN,SUITSPADES }, { TEN,SUITHEARTS }, { TEN,SUITDIAMONDS }, { NINE,SUITHEARTS }, { NINE,SUITDIAMONDS }, { TEN,SUITCLUBS }, { EIGHT,SUITDIAMONDS }, { NINE,SUITCLUBS }, { NINE,SUITSPADES }, { EIGHT,SUITCLUBS }, { EIGHT,SUITSPADES }, { EIGHT,SUITHEARTS }, { SIX,SUITSPADES }, { FOUR,SUITDIAMONDS }, { SIX,SUITHEARTS }, { THREE,SUITHEARTS }, { FIVE,SUITCLUBS }, { SIX,SUITDIAMONDS }, { TWO,SUITHEARTS }, { THREE,SUITDIAMONDS }, { FIVE,SUITSPADES }, { SEVEN,SUITCLUBS }, { ACE,SUITDIAMONDS }, { TWO,SUITDIAMONDS }, { FOUR,SUITCLUBS }, { FIVE,SUITHEARTS }, { SEVEN,SUITSPADES }, { ACE,SUITSPADES }, { TWO,SUITCLUBS }, { THREE,SUITCLUBS }, { FOUR,SUITSPADES }, { FIVE,SUITDIAMONDS }, { SEVEN,SUITHEARTS }, { ACE,SUITCLUBS }, { ACE,SUITHEARTS }, { TWO,SUITSPADES }, { THREE,SUITSPADES }, { FOUR,SUITHEARTS }, { SIX,SUITCLUBS }, { SEVEN,SUITDIAMONDS } }, 0, "Deck", false));
	else if (solvabilityMode == 2) // Solvable Deck
		SolvableDeck(KLONDIKE);
	modes[KLONDIKE].deckList.push_back(Deck(Vector2(103.0f, 10.0f), Vector2(1.0f, 0.0f), vector<array<int, 2>> {}, 15, "Discard", false));
	modes[KLONDIKE].deckList.push_back(Deck(Vector2(917.0f, 10.0f), Vector2(0.0f, 0.0f), vector<array<int, 2>> {}, 0, "Stack4", false));
	modes[KLONDIKE].deckList.push_back(Deck(Vector2(824.0f, 10.0f), Vector2(0.0f, 0.0f), vector<array<int, 2>> {}, 0, "Stack3", false));
	modes[KLONDIKE].deckList.push_back(Deck(Vector2(731.0f, 10.0f), Vector2(0.0f, 0.0f), vector<array<int, 2>> {}, 0, "Stack2", false));
	modes[KLONDIKE].deckList.push_back(Deck(Vector2(638.0f, 10.0f), Vector2(0.0f, 0.0f), vector<array<int, 2>> {}, 0, "Stack1", false));
	modes[KLONDIKE].deckList.push_back(Deck(Vector2(-100.0f, -100.0f), Vector2(0.0f, 1.0f), vector<array<int, 2>> {}, 15, "Hand", false));
}

void MainGame::InitializePacmanGame()
{
	int tilesX = 29, tilesY = 32;

	_score = 0;
	_coffeePosition = new Vector2(SCREENW / 2 - screenW / 2 + 208.0f, SCREENH / 2 - screenH / 2 + 264.0f);

	ghost.gPos = { 1,1 };
	pacman.gPos = { 11,14 };

	ghost.vPos = new Vector2(SCREENW / 2 - screenW / 2 + 8.0f * ghost.gPos.second, SCREENH / 2 - screenH / 2 + 8.0f * ghost.gPos.first);
	pacman.vPos = new Vector2(SCREENW / 2 - screenW / 2 + 16.0f * pacman.gPos.second, SCREENH / 2 - screenH / 2 + 16.0f * pacman.gPos.first);

	ifstream mapFile("Files/Map3.txt");
	int lineCount = 0;
	for (string line; getline(mapFile, line);)
	{
		mapArray[lineCount] = line;
		lineCount++;
	}
	mapFile.close();
	_numberOfDots = 0;
	_numberOfEnergizers = 0;
	for (int i = 0; i < tilesY; i++)
		for (int j = 0; j < tilesX; j++)
		{
			if (mapArray[i][j] == '0' && mapArray[i + 1][j] == '0' && mapArray[i][j + 1] == '0' && mapArray[i + 1][j + 1] == '0')
				_nodePositions[nodeCount++] = new Vector2(SCREENW / 2 - screenW / 2 + j * 16 + 8.0f, SCREENH / 2 - screenH / 2 + i * 16 + 8.0f);
			if (mapArray[i][j] == '0' && mapArray[i + 1][j] == '0' && mapArray[i][j + 1] == '0' && mapArray[i + 1][j + 1] == '0')
			{
				if (((i > 19 || i < 9) || (j == 6 || j == 21)) && !(i == 3 && j == 1) && !(i == 3 && j == 26) && !(i == 23 && j == 1) && !(i == 23 && j == 26))
					_dotPositions[_numberOfDots++] = new Vector2(SCREENW / 2 - screenW / 2 + j * 16 + 6.0f, SCREENH / 2 - screenH / 2 + i * 16 + 6.0f);
				else if ((i > 19 || i < 9) || (j == 6 || j == 21))
					_energizerPosition[_numberOfEnergizers++] = new Vector2(SCREENW / 2 - screenW / 2 + j * 16 + 0.0f, SCREENH / 2 - screenH / 2 + i * 16 + 0.0f);
			}
		}
}

void MainGame::InitializeSolitaireGame()
{
	Graphics::SetClearColor(Colors[GREEN]);
	srand(time(0));
	InitializeModes();
	if (modes[currentMode].deckList.size() > 0)
		for (int i = 0; i < modes[currentMode].deckList.size(); i++)
			modes[currentMode].decks.push_back(modes[currentMode].deckList[i]);

	if (currentMode == KLONDIKE || currentMode == SOLITAIREAI)
	{
		for (int i = 7; i > 0; i--)
			for (int j = 0; j < i; j++)
			{
				for (int k = 0; k < modes[currentMode].decks.size(); k++)
					if (modes[currentMode].decks[k].ID == ("MB" + to_string(7 - j)))
					{
						mainBoardID = k;
						break;
					}
				PopCard(SDECK, mainBoardID, false);
				SpriteBatch::BeginDraw();
				DrawBoard();
				SpriteBatch::EndDraw();


			}

		for (int h = 0; h < modes[currentMode].decks.size(); h++)
			if (modes[currentMode].decks[h].ID.substr(0, 2) == "MB")
				if (modes[currentMode].decks[h].cards.size() > 0)
				{
					modes[currentMode].decks[h].cards.back()->hidden = false;
					SpriteBatch::BeginDraw();
					DrawBoard();
					SpriteBatch::EndDraw();
				}

		HintCheck();
	}

	gameEnd = false;
	gameEndTimer = 0;
}

void MainGame::KeyToggle(Input::KeyboardState* keyboardState, Input::Keys keynum, bool * keybool, bool * changebool)
{
	if (keyboardState->IsKeyDown(keynum) && !(*keybool))
	{
		(*keybool) = true;
		(*changebool) = !(*changebool);
	}
	else if (keyboardState->IsKeyUp(keynum) && (*keybool))
		(*keybool) = false;
}

void MainGame::LoadContent()
{
	_deathSound->Load("Sounds/wilhelm.wav");
	_eatFruitSound->Load("Sounds/eatfruit.wav");
	_eatDotSound->Load("Sounds/eatdot.wav");

	_coffeeTexture->Load("Textures/generalsprites.png", false);
	_dotTexture->Load("Textures/generalsprites.png", false);
	_energizerTexture->Load("Textures/generalsprites.png", false);
	ghost.texture->Load("Textures/generalsprites.png", false);
	_mapTexture->Load("Textures/generalsprites.png", false);
	_menuTexture->Load("Textures/Transparency.png", false);
	pacman.texture->Load("Textures/generalsprites.png", false);

	cardsTexture->Load("Textures/cardTileset.png", false);

	ghost.textureRect = new Rect(907.0f, 136.0f, ghost.width, ghost.width);
	pacman.textureRect = new Rect(907.0f, 136.0f, pacman.width, pacman.width);

	three = false;
	currentMode = MENU;
	Graphics::SetClearColor(Colors[GREEN]);
	gameEnd = false;
	gameEndTimer = 0;
}

MainGame::MainGame(int argc, char* argv[]) : Game(argc, argv)
{
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, SCREENW, SCREENH, false, 25, 25, "[Insert Game Name Here]", 60);
	Input::Initialise();
	Graphics::StartGameLoop();
}

MainGame::~MainGame()
{
	delete _coffeePosition;
	delete _coffeeTexture;
	delete _coffeeTextureRect;
	delete _deathSound;
	delete _debugStringPosition;
	delete _dotPositions;
	delete _dotTexture;
	delete _dotTextureRect;
	delete _eatDotSound;
	delete _eatFruitSound;
	delete _energizerTexture;
	delete _energizerTextureRect;
	delete _mapTexture;
	delete _mapTextureRect;
	delete _menuTexture;
	delete _menuTextureRect;
	delete _menuStringPosition;
	delete pacman.vPos;
	delete pacman.texture;
	delete pacman.textureRect;

	delete cardsTexture;
	for (int i = 0; i < 3; i++) // Colors
		delete Colors[i];

	for (int i = 0; i < modes.size(); i++)
	{
		for (int j = 0; j < modes[i].deckList.size(); j++)
		{
			for (int k = 0; k < modes[i].deckList[j].cards.size(); i++)
				delete modes[i].deckList[j].cards[k];
			delete modes[i].deckList[j].position;
			delete modes[i].deckList[j].direction;
		}

		for (int j = 0; j < modes[i].decks.size(); j++)
		{
			for (int k = 0; k < modes[i].decks[j].cards.size(); i++)
				delete modes[i].decks[j].cards[k];
			delete modes[i].decks[j].position;
			delete modes[i].decks[j].direction;
		}
	}

	Audio::Destroy();
	Graphics::Destroy();
	Input::Destroy();

}

MainGame::Mode::Mode(vector<Deck> & idecks, bool iundo)
{
	deckList = idecks;
	undo = iundo;
}

void MainGame::MoveSprite(sprite & sprite, int elapsedTime)
{
	if (DirectionCheck(sprite, elapsedTime, sprite.nextDirection))
	{
		if (sprite.nextDirection != sprite.currentDirection)
			sprite.movePercent = 0;
		sprite.currentDirection = sprite.nextDirection;
	}

	sprite.movePercent += sprite.moveSpeed / elapsedTime;
	if (sprite.movePercent >= 1.0f)
	{
		sprite.movePercent -= 1.0f;
		if (sprite.currentDirection == directions::RIGHT)
			sprite.gPos.second += 1;
		else if (sprite.currentDirection == directions::LEFT)
			sprite.gPos.second -= 1;
		else if (sprite.currentDirection == directions::DOWN)
			sprite.gPos.first += 1;
		else if (sprite.currentDirection == directions::UP)
			sprite.gPos.first -= 1;
	}

	UpdatePosition(sprite);
}

void MainGame::PopCard(int deck1, int deck2, bool addUnder)
{
	if (modes[currentMode].decks[deck1].cards.size() > 0)
	{
		Card * card = modes[currentMode].decks[deck1].cards.back();
		(modes[currentMode].decks[deck1].cards).pop_back();

		if (addUnder)
		{
			vector<Card*> cards = { card };
			for (int i = 0; i < modes[currentMode].decks[deck2].cards.size(); i++)
				cards.push_back(modes[currentMode].decks[deck2].cards[i]);
			modes[currentMode].decks[deck2].cards = cards;
		}
		else
			modes[currentMode].decks[deck2].cards.push_back(card);
	}
	modes[currentMode].decks[deck1].UpdateRects();
	modes[currentMode].decks[deck2].UpdateRects();
	HintCheck();
}

void MainGame::ProcessAnimation(int elapsedTime)
{
	AnimateDirection(pacman, elapsedTime);
	AnimateDirection(ghost, elapsedTime);

	if (!_pacmanDead)
	{
		pacman.animationTimer = ++pacman.animationTimer % 12;
		if (pacman.animationTimer < 4 || pacman.animationTimer > 7) // The two values mean that the animation is cyclic and goes backwards rather than resetting
		{
			pacman.textureRect->X = 907.0f + (pacman.animationTimer / 6 * (pacman.width + 2));
			pacman.textureRect->Y = pacman.currentDirection * (pacman.width + 2);
		}
		else
		{
			pacman.textureRect->X = 975.0f;
			pacman.textureRect->Y = 0.0f;
		}
	}
	else
	{
		pacman.deathAnimationTimer = ++pacman.deathAnimationTimer % 48;
		if (pacman.deathAnimationTimer < 36)
		{
			pacman.textureRect->X = 1009.0f + (pacman.deathAnimationTimer / 3 * (pacman.width + 2));
			pacman.textureRect->Y = 0.0f;
		}
		if (pacman.deathAnimationTimer == 47)
		{
			_pacmanDead = false;

			ghost.gPos = { 1,1 };
			pacman.gPos = { 11,14 };

			*ghost.vPos = Vector2(SCREENW / 2 - screenW / 2 + 8.0f * ghost.gPos.second, SCREENH / 2 - screenH / 2 + 8.0f * ghost.gPos.first);
			*pacman.vPos = Vector2(SCREENW / 2 - screenW / 2 + 16.0f * pacman.gPos.second, SCREENH / 2 - screenH / 2 + 16.0f * pacman.gPos.first);

			if (_score == 382)
			{
				mode = SOLITAIRE;
				Graphics::SetClearColor(Colors[GREEN]);
			}
		}
	}

	FlipFrame(&ghost.animationTimer, &ghost.frame, 4);

	ghost.textureRect->X = 941.0f + ghost.currentDirection * 68 - 34 * ghost.frame;
	ghost.textureRect->Y = 136.0f;

	FlipFrame(&_dotAnimationTimer, &_dotFrame, 24);
	FlipFrame(&_coffeeAnimationTimer, &_coffeeFrame, 32);

	ChangeTexture(_dotFrame, &_dotTextureRect->X, &_dotTextureRect->Y, 981.0f, 997.0f, 40.0f, 56.0f);
	ChangeTexture(_dotFrame, &_energizerTextureRect->X, &_energizerTextureRect->Y, 991.0f, 975.0f, 34.0f, 50.0f);
	ChangeTexture(_coffeeFrame, &_coffeeTextureRect->X, &_coffeeTextureRect->Y, 1009.0f, 1043.0f, 34.0f, 34.0f);
}

void MainGame::SaveSolvableDeck()
{
	string saveString;

	for (Card* card : modes[currentMode].deckList[SDECK].cards)
		saveString = saveString + to_string(card->val) + "," + to_string(card->suit) + ".";
	saveString = saveString.substr(0, saveString.size() - 1);

	cout << saveString << endl;

	ofstream solvableFile;
	solvableFile.open("solvableDecks.txt", ios::app);

	if (!solvableFile.is_open())
	{
		cerr << "File could not be opened";
		exit(1);
	}
	solvableFile << saveString << endl;
	solvableFile.close();
}

void MainGame::Deck::Shuffle()
{
	srand(time(0));
	if (cards.size() > 0)
		random_shuffle(cards.begin(), cards.end());
}

void MainGame::SolvableDeck(int gamemode)
{
	ifstream solvableDecks;
	solvableDecks.open("Files/solvableDecks.txt");
	vector<string> possibleDecks = {};
	string line;
	int count = 1;
	while (getline(solvableDecks, line))
	{
		possibleDecks.push_back((to_string(count++) + "#" + line));
	}
	solvableDecks.close();
	srand(time(0));
	random_shuffle(possibleDecks.begin(), possibleDecks.end());
	string deck = possibleDecks.back();
	vector<string> sections = SplitString(deck, '#');
	//cout << "Deck ID: " << sections[0] << endl; // Deck ID
	string cardstring = sections[1];
	vector<string> cards = SplitString(cardstring, '.');
	vector<vector<string>> cardVals = {};
	for (int i = 0; i < cards.size(); i++)
	{
		cardVals.push_back(SplitString(cards[i], ','));
	}
	vector<array<int, 2>> cardVals2 = {};
	for (int i = 0; i < cardVals.size(); i++)
	{
		cardVals2.push_back({ stoi(cardVals[i][0]),stoi(cardVals[i][1]) });
	}

	modes[gamemode].deckList.push_back(Deck(Vector2(10.0f, 10.0f), Vector2(0.0f, 0.0f), cardVals2, 0, "Deck", false));
}

void MainGame::Update(int elapsedTime)
{
	if (mode == PACMAN)
	{
		Input::KeyboardState* keyboardState = Input::Keyboard::GetState();

		if (!_started) //If you remove these squiggly brackets it breaks all input
		{
			if (keyboardState->GetPressedKeys().size() > 0 && !keyboardState->IsKeyDown(Input::Keys::SPACE) && !keyboardState->IsKeyDown(Input::Keys::Q) && !keyboardState->IsKeyDown(Input::Keys::F)) //Any key Pressed but Space
				_started = true;
		}
		else
		{
			ChangeState(Input::Keys::SPACE, &_pauseKeyDown, &_paused, keyboardState);
			ChangeState(Input::Keys::Q, &_debugKeyDown, &_debug, keyboardState);

			if (!_pacmanDead)
			{
				if (keyboardState->IsKeyDown(Input::Keys::F) && !_killKeyDown)
				{
					_pacmanDead = true;
					_killKeyDown = true;
					Audio::Play(_deathSound);
				}
				if (keyboardState->IsKeyUp(Input::Keys::F))
					_killKeyDown = false;

				if (keyboardState->IsKeyDown(Input::Keys::ESCAPE) && !quitKeyDown)
				{
					mode = SOLITAIRE;
					quitKeyDown = true;
					Graphics::SetClearColor(Colors[GREEN]);
				}
				if (keyboardState->IsKeyUp(Input::Keys::ESCAPE))
					quitKeyDown = false;

				if (!_paused)
				{
					if (CheckBoxCollision(ghost.vPos->X, ghost.vPos->Y, pacman, ghost.width, -112.0f))
					{
						if (ghost.vPos->X - pacman.vPos->X > 0)//Pacman Left of Ghost
						{
							if (ghost.vPos->Y - pacman.vPos->Y < 0)//Pacman Below Ghost
							{
								if (abs(ghost.vPos->X - pacman.vPos->X) > abs(ghost.vPos->Y - pacman.vPos->Y))
									ghost.nextDirection = LEFT;
								else
									ghost.nextDirection = DOWN;
							}
							else //Pacman Above Ghost
							{
								if (abs(ghost.vPos->X - pacman.vPos->X) > abs(ghost.vPos->Y - pacman.vPos->Y))
									ghost.nextDirection = LEFT;
								else
									ghost.nextDirection = UP;
							}
						}
						else //Pacman Right of Ghost
						{
							if (ghost.vPos->Y - pacman.vPos->Y < 0)//Pacman Below Ghost
							{
								if (abs(ghost.vPos->X - pacman.vPos->X) > abs(ghost.vPos->Y - pacman.vPos->Y))
									ghost.nextDirection = RIGHT;
								else
									ghost.nextDirection = DOWN;
							}
							else //Pacman Above Ghost
							{
								if (abs(ghost.vPos->X - pacman.vPos->X) > abs(ghost.vPos->Y - pacman.vPos->Y))
									ghost.nextDirection = RIGHT;
								else
									ghost.nextDirection = UP;
							}
						}
					}
					else
					{
						_ghostMoveTimer = ++_ghostMoveTimer % 12;
						if (_ghostMoveTimer == 0)
							ghost.nextDirection = rand() % 4;
					}

					ghost.lastPosition = ghost.gPos;
					MoveSprite(ghost, elapsedTime);

					CheckMoveKey(pacman, keyboardState, Input::Keys::D, 0);
					CheckMoveKey(pacman, keyboardState, Input::Keys::A, 1);
					CheckMoveKey(pacman, keyboardState, Input::Keys::S, 2);
					CheckMoveKey(pacman, keyboardState, Input::Keys::W, 3);

					if (keyboardState->IsKeyUp(Input::Keys::W) || keyboardState->IsKeyUp(Input::Keys::A) || keyboardState->IsKeyUp(Input::Keys::S) || keyboardState->IsKeyUp(Input::Keys::D))
						_moveKeyDown = false;

					pacman.lastPosition = pacman.gPos;
					MoveSprite(pacman, elapsedTime);

					HandleCollisions();

					if (_score == 382)
					{
						_pacmanDead = true;
						Audio::Play(_eatFruitSound);
					}
				}
			}
		}
	}
	else
	{
		timeElapsed = elapsedTime;

		Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
		Input::MouseState * mouseState = Input::Mouse::GetState();

		if (currentMode == KLONDIKE)
		{
			KeyToggle(keyboardState, Input::Keys::SPACE, &spacePressed, &three);

			if (keyboardState->IsKeyDown(Input::Keys::H) && !hPressed)
			{
				hPressed = true;
				CycleHints();
				showHint = true;
			}
			else if (keyboardState->IsKeyUp(Input::Keys::H) && hPressed)
				hPressed = false;
		}

		if (currentMode == MENU)
		{
			if (keyboardState->IsKeyDown(Input::Keys::ESCAPE) && !quitKeyDown)
			{
				exit(0);
			}
			if (keyboardState->IsKeyUp(Input::Keys::ESCAPE))
				quitKeyDown = false;

			if (keyboardState->IsKeyDown(Input::Keys::NUMPAD1))
			{
				currentMode = KLONDIKE;
				solvabilityMode = 2;
			}
			else if (keyboardState->IsKeyDown(Input::Keys::NUMPAD2))
			{
				solvabilityMode = 0;
				currentMode = KLONDIKE;
			}
			else if (keyboardState->IsKeyDown(Input::Keys::NUMPAD3))
			{
				mode = PACMAN;
				InitializePacmanGame();
				Graphics::SetClearColor(Colors[BLACK]);
			}
			else if (keyboardState->IsKeyDown(Input::Keys::NUMPAD4))
			{
				solvabilityMode = 0;
				currentMode = SOLITAIREAI;
				output = true;
			}
			else if (keyboardState->IsKeyDown(Input::Keys::NUMPAD5))
			{
				solvabilityMode = 0;
				currentMode = SOLITAIREAI;
				output = false;
			}
			else if (keyboardState->IsKeyDown(Input::Keys::NUMPAD6))
			{
				solvabilityMode = 1;
				currentMode = SOLITAIREAI;
				output = true;
			}
			else if (keyboardState->IsKeyDown(Input::Keys::NUMPAD7))
			{
				solvabilityMode = 2;
				currentMode = SOLITAIREAI;
				output = true;
			}
			if (keyboardState->IsKeyDown(Input::Keys::NUMPAD1) || keyboardState->IsKeyDown(Input::Keys::NUMPAD2) || keyboardState->IsKeyDown(Input::Keys::NUMPAD4) || keyboardState->IsKeyDown(Input::Keys::NUMPAD5) || keyboardState->IsKeyDown(Input::Keys::NUMPAD6) || keyboardState->IsKeyDown(Input::Keys::NUMPAD7))
				InitializeSolitaireGame();
		}
		else
		{
			if (keyboardState->IsKeyDown(Input::Keys::ESCAPE))
			{
				UpdateText("QUIT");
				gameEnd = true;
			}
		}

		if (gameEndTimer > 3000 || gameEndTimer == 0)
		{
			if (mouseState->LeftButton == Input::ButtonState::PRESSED && leftButtonPressed == false)
			{
				leftButtonPressed = true;
				if (currentMode == KLONDIKE)
					if (modes[currentMode].decks[SDECK].hitBoxRect.Contains(Vector2(mouseState->X, mouseState->Y)))
					{

						if (three)
						{
							if (modes[currentMode].decks[SDECK].cards.size() >= 3)
								for (int i = 0; i < 3; i++)
								{
									PopCard(SDECK, SDISCARD, false);
									modes[currentMode].decks[SDISCARD].cards.back()->hidden = false;
								}
							else if (modes[currentMode].decks[SDECK].cards.size() == 2)
								for (int i = 0; i < 2; i++)
								{
									PopCard(SDECK, SDISCARD, false);
									modes[currentMode].decks[SDISCARD].cards.back()->hidden = false;
								}
							else if (modes[currentMode].decks[SDECK].cards.size() == 1)
							{
								PopCard(SDECK, SDISCARD, false);
								modes[currentMode].decks[SDISCARD].cards.back()->hidden = false;
							}
							else
							{
								int resetSize = modes[currentMode].decks[SDISCARD].cards.size();
								for (int i = 0; i < resetSize; i++)
								{
									PopCard(SDISCARD, SDECK, false);
									modes[currentMode].decks[SDECK].cards.back()->hidden = true;
								}
							}
						}
						else
							if (modes[currentMode].decks[SDECK].cards.size() > 0)
							{
								PopCard(SDECK, SDISCARD, false);
								modes[currentMode].decks[SDISCARD].cards.back()->hidden = false;
							}
							else
							{
								int resetSize = modes[currentMode].decks[SDISCARD].cards.size();
								for (int i = 0; i < resetSize; i++)
								{
									PopCard(SDISCARD, SDECK, false);
									modes[currentMode].decks[SDECK].cards.back()->hidden = true;
								}
							}
						showHint = false;
					}
			}
			if (mouseState->RightButton == Input::ButtonState::PRESSED && rightButtonPressed == false)
			{
				rightButtonPressed = true;
				if (currentMode == KLONDIKE)
					for (int h = 0; h < modes[currentMode].decks.size(); h++)
						if (modes[currentMode].decks[h].hitBoxRect.Contains(Vector2(mouseState->X, mouseState->Y)))
							for (int i = 1; i < 5; i++)
							{
								for (int j = 0; j < modes[currentMode].decks.size(); j++)
									if (modes[currentMode].decks[j].ID == ("Stack" + to_string(i)))
										stackIndex = j;
								if (modes[currentMode].decks[h].cards.size() > 0 && modes[currentMode].decks[stackIndex].cards.size() > 0)
									if (modes[currentMode].decks[h].cards.back()->val == modes[currentMode].decks[stackIndex].cards.back()->val + 1 && modes[currentMode].decks[h].cards.back()->suit == modes[currentMode].decks[stackIndex].cards.back()->suit)
									{

										PopCard(h, stackIndex, false);
										if (modes[currentMode].decks[h].cards.size() > 0)
											modes[currentMode].decks[h].cards.back()->hidden = false;
										break;
									}
								if (modes[currentMode].decks[h].cards.size() > 0 && modes[currentMode].decks[stackIndex].cards.size() == 0)
								{
									if (modes[currentMode].decks[h].cards.back()->val == ACE)
									{

										PopCard(h, stackIndex, false);
										if (modes[currentMode].decks[h].cards.size() > 0)
											modes[currentMode].decks[h].cards.back()->hidden = false;
										break;
									}
								}
							}
			}

			if (mouseState->LeftButton == Input::ButtonState::RELEASED)
				leftButtonPressed = false;
			if (mouseState->RightButton == Input::ButtonState::RELEASED)
				rightButtonPressed = false;
		}
	}
}

void MainGame::UpdatePosition(sprite & sprite)
{
	if (sprite.gPos.second > 29)
		sprite.gPos.second = 0;
	else if (sprite.gPos.second + 1 < 0)
		sprite.gPos.second = 27;
}

void MainGame::Deck::UpdateRects()
{
	if (cards.size() > 0)
		hitBoxRect = Rect(position->X, position->Y, CARDW + (cards.size() - 1)*spacing*direction->X, CARDH + (cards.size() - 1)*spacing*direction->Y);
	else
		hitBoxRect = Rect(position->X, position->Y, CARDW, CARDH);

	if (cards.size() > 0)
		outlineRect = Rect(position->X - 3, position->Y - 3, CARDW + (cards.size() - 1)*spacing*direction->X + 6, CARDH + (cards.size() - 1)*spacing*direction->Y + 6);
	else
		outlineRect = Rect(position->X - 3, position->Y - 3, CARDW + 6, CARDH + 6);
}

void MainGame::UpdateText(string itext)
{
	text.str("");
	text << itext;
}