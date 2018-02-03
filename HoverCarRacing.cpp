/* Adam James Rushton G20700507
Hover Car Games Assignment           */

/*
Extra features
Map System - dots for cars are same colour as their car - crates, flares and obstacles showing on the map,
also camera position on the map
Highscores -Storing players fastest 5 race times
Menu - inc setting keybinds
Crate system
Turret shooting system
Menu sounds and game sounds
Array table for computer car speeds
*/
#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <sstream>      // Writing to game screen
#include <vector>	    // Dynamic Arrays - Better than standard arrays if values in the sequence need changing
#include <time.h>	    // Used for random number generating and timers
#include <fstream>		// Read/write to files
#include <iostream>     // Display text on the console
#include <iomanip>      // Printing highscores neatly
#include <algorithm>    // Sort vectors, highscores in this program
#include <SFML/Audio.hpp>
#include <conio.h>
// Namespaces
using namespace tle; // T-L Engine definitions (I guess)
using namespace std; // Standard definitions

// Structures
struct AddTime
{   // Group names and time
	string name;
	int time;
};

struct AddPosition
{   // Group names and distance
	string name;
	float distance;
	string state;
};

struct ByTime
{   // Sort by time (lowest at the top)
	bool operator()(AddTime const &a, AddTime const &b)
	{
		return a.time < b.time; // Sort smallest to largest (smallest at the top)
	}
};

struct ByPosition
{   // Sort by distance (lowest at the top)
	bool operator()(AddPosition const &a, AddPosition const &b)
	{
		return a.distance < b.distance; // Sort smallest to largest (smallest at the top)
	}
};

const int dimensions = 3;
struct movementVector
{	// Storing object positions
	float moveVector[dimensions];
};
// Researched from from Gamesnorthwest.
// A "buffer" holds sound data, but the creation of a buffer on its own doesn't play a sound. It is the
// equivalent of a mesh
sf::SoundBuffer buffer;
// A "sound" is an actual sound in the world. A sound must be associated with a buffer to indicate
// which sound data to play. Sources are equivalent to models in the TL-Engine
sf::Sound currentSound;

sf::Vector3f soundPos(0.0f, 0.0f, 0.0f);
sf::Vector3f soundVelocity(0.0f, 0.0f, 0.0f);
sf::Vector3f listenerPos(0.0f, 0.0f, 0.0f);
sf::Vector3f listenerForward(0.0f, 0.0f, -1.0f);
sf::Vector3f listenerUp(0.0f, 1.0f, 0.0f);

// Load a sound in 
void LoadSound(string name)
{
	if (!buffer.loadFromFile(name))
	{
		cout << "[LOG] Error loading " << name << endl;
		while (!_kbhit());
		return;
	}
}

const int numOfParticles = 40;           // Number of particles
movementVector particle[numOfParticles]; // New vector structure for particles
										 // Enum setup
enum currentState { MainMenu, Start, Highscores, About, Settings, BindNewKey, Quit }; // Game states										 
enum settingsLeftList { PauseText = 2, QuitText, ResetCameraText, chaseCamText };    // Settings list
enum settingsRightList {
	PrepareRaceText, HoverCarUpText, HoverCarDownText, HoverCarLeftText,
	HoverCarRightText, BoostText
}; // Settings list 
enum menuNames
{
	StartButton, HighscoresButton, SettingsButton, AboutButton, QuitButton, BackButton, FullScreenKey, FPSCounterKey,
	PauseKey, QuitKey, ResetCameraKey, chaseCamKey, prepareRaceKey, hoverCarForwardsKey, hoverCarBackwardsKey, hoverCarLeftKey,
	hoverCarRightKey, DownViewKey, CancelButton, ResetButton, Highscore1, Highscore2, Highscore3, Highscore4, Highscore5
}; // Buttons and Game keys
enum colours { Blue, Brown, DarkBlue, Green, LightPink, Orange, Pink, Red, Yellow };
enum fumes { FBlue, FPurple, FPink, FRed, FYellow, FGreen, FDarkGreen };
enum carState { Hovering, WaitingForGo, Racing, DestroyedCar };
/* Main menu and text settings */
const int optionsAmt = 27;			  // Amount of text to be highlighted.
const int hoverColour = 0xFFFF7400;   // 16-bit Hex Colour - Highlighting options colour
const int defaultColour = 0xFF000000; // 16-bit Hex Colour - Text colour for everything else
const int titleColour = 0xFFDACCFF;	  // 16-bit Hex Colour - Title text colour
const int gameFontSize = 45;		  // Font size for all page titles
const int titleFontSize = 70;		  // Font size for title text
const int aboutFontSize = 25;		  // Font size for text on about screen
const int titleTextX = 512;			  // X Position for page titles for each screen
const int titleTextY = 35;			  // Y Position for page titles for each screen
int widthOfText[optionsAmt];		  // Measure width of all the options
int heightOfText[optionsAmt];		  // Measure height of all the options
int optionMinX[optionsAmt];			  // Lower boundary X (For bounding a box around the text, applies to all options)
int optionMaxX[optionsAmt];			  // Upper boundary X (For bounding a box around the text, applies to all options)
int optionMinY[optionsAmt];			  // Lower boundary Y (For bounding a box around the text, applies to all options)
int optionMaxY[optionsAmt];		  	  // Upper boundary Y (For bounding a box around the text, applies to all options)
int textPosX[optionsAmt];		  	  // Current Text Position on X-Axis for all options
int textPosY[optionsAmt];			  // Current Text Position on Y-Axis for all options
int toggledColour[optionsAmt];		  // Toggled colour, applies to all options
int currentLine = 0;				  // Position on the screen
									  // All text listings for the menu pages
int engineLength;
bool keyInUse = false;			      // Store key in use or not
const int mainMenuOptions = 5;		  // Amount of options on the Main Menu

string keyCodeNames[kMaxKeyCodes] =
{
	"0", "L Button", "R Button", "3", "M Button", "X Button1", "X Button2", "7", "Back", "Tab", "10",
	"11", "Clear", "Return", "14", "15", "Shift", "Control", "Menu", "P", "C-L", "21", "22", "23", "24",
	"25", "26", "Escape", "Conv", "Non-conv", "Accept", "ModeChange", "Space", "Prior", "Next", "End", "Home",
	"Left", "Up", "Right", "Down", "Select", "Print", "Execute", "Snapshot", "Insert", "Delete", "Help",
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9","58", "59", "60", "61", "62", "63", "64", "A", "B",
	"C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W","X", "Y", "Z",
	"LWin", "RWin", "Apps", "94", "Sleep", "NP 0", "NP 1", "NP 2", "NP 3", "NP 4", "NP 5", "NP 6", "NP 7", "NP 8", "NP 9",
	"*", "+","Seperator", "-", ".", "/", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13",
	"F14", "F15", "F16", "F17", "F18", "F19","F20", "F21", "F22", "F23", "F24", "136", "137", "138", "139", "140", "141",
	"142", "143", "N-Lock", "S-Lock", "144", "145", "146", "147", "148", "149", "150","151", "152", "153", "154", "155",
	"156", "157", "LShift", "RShift", "LControl", "RControl", "LMenu", "RMenu", "<-", "->", "168", "169", "170", "Star",
	"172" ,"Mute", "- Vol", "+ Vol", "FastFwd", "177", "178", "Play/Pause", "180", "181", "182", "CalcKey", "184", "185",
	";", "Plus", "Comma", "Minus", "Period", "/", "'", "193", "194", "195", "196", "197", "198", "199", "200", "201", "202",
	"203", "204", "205", "206", "207", "208", "209", "210", "211", "212", "213", "214","215", "216", "217", "218", "[",
	"BackSlash", "]", "#", "223", "224", "225", "226", "227", "228", "229", "230", "231", "232", "233", "234", "235", "236",
	"237", "238", "239", "240", "241", "242", "243", "244", "245", "Attn", "CrSel", "ExSel", "ErEof", "Play", "Zoom", "252",
	"PA1", "Clear", "255"
}; // My dictionary of all game keys
const int maxEntries = 5;         // Number of entries for scores
const int carAmount = 6;		  // Total number of cars
stringstream currentposition;
const int numOfCheckPoints = 4;
const int numOfWalls = 203;
const int numOfTanks = 113;
const int numOfIsles = 156;
const int numOfBombs = 9;
int carState[carAmount];					  // Store car states

											  // Main menu options, left and right settings
string mainMenuList[mainMenuOptions] = { "Start", "Highscores", "Settings", "About", "Quit" };
/* Sprite details */
bool pauseCreated = false;                             // Pause sprite creation state
const float pauseSpriteX = 400.0f;					   // X to create the pause sprite
const float pauseSpriteY = 125.0f;					   // Y to create the pause sprite
const string mainAboutBG = "menu/mainAboutBGFS.jpg";   // Locating the main menu and about screen background
const string highscoresBG = "menu/highscoresBGFS.jpg"; // Locating the highscores background sprite
const string settingsBG = "menu/settingsBGFS.jpg";     // Locating the settings background sprite
const string pauseName = "menu/pause.png";			   // Locating the pause sprite
const string gameBackdrop = "menu/ui_backdrop.jpg";
const string gamePosition = "menu/ui_position.jpg";
ISprite* uiPosition;
ISprite* uiBackdrop;
ISprite* backdrop;									   // Background sprite
ISprite* pause;										   // Pause sprite
													   /* Camera & Game details */
const EKeyCode cameraForwards = Key_Up;
const EKeyCode cameraBackwards = Key_Down;
const EKeyCode cameraLeft = Key_Left;
const EKeyCode cameraRight = Key_Right;
const float cameraForwardsLimit = 620.0f;
const float cameraBackwardsLimit = -140.0f;
const float cameraLeftLimit = 0.0f;
const float cameraRightLimit = 420.0f;
const float cameraSpeed = 20.0f;
bool gamePaused;
bool gameOver;
bool inGame;
bool cameraShake;
bool onlyWhiteSpace;
float frameRate;
const int maxNumOfChars = 10;
stringstream fullScreenStateText;				  // Full screen
stringstream fpsToggleStateText;				  // FPS Toggle
stringstream carColourText;						  // Display car current colour
stringstream carFumeText;						  // Display fume current colour
stringstream timeElapsedText;					  // Time Remaining
stringstream userNameText;						  // Store display username
stringstream currentKeyText;                      // When user presses a key, display it to the screen
stringstream mainMenuText[mainMenuOptions];		  // Options on the main menu
stringstream gameOverTime;						  // Display score when game over
												  /* Button names */
string backName = "Back";     // Back button, used throughout the menus. show either Back or Save(settings changing) 
string cancelName = "Cancel"; // Cancel, used on settings
string resetName = "Reset";   // Reset, used on highscores and settings
							  /* About Us Text Positions */
const int textLine1X = 440; // X position of text on about page
const int textLine1Y = 280; // Y position of text on about page
const int textLine2X = 310; // X position of text on about page
const int textLine2Y = 400; // Y position of text on about page
const int textLine3X = 690; // X position of text on about page
const int textLine3Y = 400; // Y position of text on about page
const int textLine4X = 180; // X position of text on about page
const int textLine4Y = 560; // Y position of text on about page

const float objFloor = 1.0f;
const float kGameSpeed = 20.0f;
int tempTime = 999; // Takes gamescore, updates highscore entry if new fastest time

/* Classes */
class BestTimes
{ // This is a class to store the fastest 5 times as highscores
private:
	string top5scores = "scores.dat"; // Name of the scores file
	ofstream saveHighscores;          // Write to file
	ifstream loadHighscores;	      // Read and load Highscores
	AddTime p[maxEntries];	          // Array of scores to add
	int playerTimes[maxEntries];      // Array of player scores
	string playerName[maxEntries];	  // Array of player names	
	const char spaceFiller = ' ';	  // Space empty gap with space
	const int numOfTimes = 5;		  // Number of highscores saved
	const int posSpacing = 6;		  // Number of spaces after Position
	const int nameSpacing = 41;		  // Number of spaces after Name
	const int lastPlace = 4;		  // Store last place number
	const int invalidNum = 999;		  // Set temp time to this
public:
	stringstream top5PosText[maxEntries];	// Draw position for each person
	stringstream top5NameText[maxEntries];	// Draw names for each person
	stringstream top5TimeText[maxEntries];	// Draw score for each person
	stringstream posNameScoreText;			// Headers for the display table
	string userNameInfo;					// Displays text before the username
	string userName;						// Stores username

	void UpdateDisplayTimes(IFont* gameFont)
	{
		// Add all the highscores
		for (int i = 0; i < maxEntries; i++) p[i] = { playerName[i], playerTimes[i] };
		// Create a vector filled with AddTime Entry objects.
		vector<AddTime> scoreboard;
		//Add our Entries to the vector
		for (int i = 0; i < maxEntries; i++) scoreboard.push_back(p[i]);

		sort(scoreboard.begin(), scoreboard.end(), ByTime()); // Sort them now the last is the smallest
		if (tempTime < scoreboard[lastPlace].time) // If the user scored higher than the lowest score, replace it.
		{
			for (int i = 0; i < maxEntries; i++)
			{
				if (playerTimes[i] == scoreboard[lastPlace].time)
				{   // If any of the scores is the last score, replace it
					playerName[i] = userName;
					playerTimes[i] = tempTime;
				}
			}
			tempTime = invalidNum; // Reset it
		}
		sort(scoreboard.begin(), scoreboard.end(), ByTime()); // Re-sort them
															  // Display column headers
		posNameScoreText << "#" << setw(posSpacing) << setfill(spaceFiller) << "Name" << setw(nameSpacing) << setfill(spaceFiller) << "Time(s)";
		gameFont->Draw(posNameScoreText.str(), 300, 170, defaultColour);
		posNameScoreText.str("");

		for (int i = 0; i < numOfTimes; i++) // Draw positions
		{
			top5PosText[i] << left << i + 1;
			gameFont->Draw(top5PosText[i].str(), 300, 260 + (i * 80), toggledColour[i + 20]);
			top5PosText[i].str("");
		}
		for (int i = 0; i < numOfTimes; i++) // Draw names
		{
			top5NameText[i] << left << scoreboard[i].name;
			gameFont->Draw(top5NameText[i].str(), 350, 260 + (i * 80), toggledColour[i + 20]);
			top5NameText[i].str("");
		}
		for (int i = 0; i < numOfTimes; i++) // Draw scores
		{
			top5TimeText[i] << left << scoreboard[i].time;
			gameFont->Draw(top5TimeText[i].str(), 750, 260 + (i * 80), toggledColour[i + 20]);
			top5TimeText[i].str("");
		}
	}
	// Reset highscores
	void ResetHS()
	{
		// Default names and scores until the user beats them
		for (int i = 0; i < maxEntries; i++)
		{
			playerName[i] = "Ajrushton1";
			playerTimes[i] = 100 + (i * 100);
		}
	}
	// Write message on console
	void HighscoresResetMSG()
	{
		cout << "Highscores reset successfully." << endl;
	}
	// Saving times to file
	void SaveHS()
	{
		saveHighscores.open(top5scores); // Open the file
		if (saveHighscores.fail()) perror("[LOG] scores.dat");// Error handling

		cout << "[LOG] Saving the scores..." << endl;
		for (int i = 0; i < maxEntries; i++)
		{
			saveHighscores << playerName[i] << endl;  // Save each name
			saveHighscores << playerTimes[i] << endl; // Save each score
		}
		saveHighscores.close(); // close file we are done with it
	}
	// Loading times from file, storing them in a vector
	void LoadHS()
	{
		loadHighscores.open(top5scores); // Open the file
		string line; // Temp store each line
		if (loadHighscores.fail()) // Error handling
		{
			perror("[LOG] scores.dat");
			cout << "[LOG] No highscores found. Using defaults." << endl;
			ResetHS();
			SaveHS();
			loadHighscores.open(top5scores);
		}
		vector<string> hsVector; // Dynamic array to store highscores in

		while (getline(loadHighscores, line)) hsVector.push_back(line); // Add each line to the dynamic array							  

		int ln = 0;
		for (int i = 0; i < maxEntries; i++)
		{
			playerName[i] = hsVector[ln];					  // Add the name to the vector
			ln++;											  // Next line
			playerTimes[i] = (int)atof(hsVector[ln].c_str()); // Add the score (whilst converting it)
			ln++;											  // Next line
		}
		loadHighscores.close(); // CLOSE we are done with it
	}
};

BestTimes fastestTimes; // Variable for highscores class 
const int setAmt = 6;   // Number of settings on each side

class ClassSettings
{ // This is a class to store all keys, allowing the user to change them too
private:
	string gameSettings = "settings.dat"; // File name
	ofstream saveGameSettings;			  // Write to file
	ifstream loadData;					  // Read and load settings from file
	const int leftSettingsStart = 6;	  // Starting number for left settings
	const int leftSettingsEnd = 12;	      // End of settings
	const int rightSettingsStart = 12;    // Starting number for right settings
	const int rightSettingsEnd = 18;      // Ending number for right settings
	static const int numOfColours = 9;	  // Total number of car colours
	static const int numOfFumes = 7;	  // Total number of car fume colours
	string currentGameKeysLeft[setAmt];   // Game keys list (left side)
	string currentGameKeysRight[setAmt];  // Game keys list (right side)
	int currentKey;			     		  // Store current key hit
	string keyName = "";				  // Store key name
	EKeyCode tempPauseGameKey;			  // Temp Pause game
	EKeyCode tempQuitKey;				  // Temp Quit program
	EKeyCode tempResetCamera;			  // Temp Reset position of camera 
	EKeyCode tempchaseCamera;			  // Temp Look up camera 
	EKeyCode tempPrepareRace;			  // Temp Restart level
	EKeyCode tempHoverCarForwards;		  // Temp Forwards
	EKeyCode tempHoverCarBackwards;		  // Temp Backwards
	EKeyCode tempHoverCarLeft;			  // Temp Left
	EKeyCode tempHoverCarRight;			  // Temp Right
	EKeyCode tempHoverCarBoost;			  // Temp Boost
	string loadFullScreen;				  // Stores true or false			  
	string toggleFpsCounter;			  // Stores true or false
	string userCarColour;
	string userFumeColour;
	/* Store into string, to be converted into int */
	string currentQuitKey;
	string currentPauseKey;
	string currentResetCamera;
	string currenthoverCarForwards;
	string currenthoverCarBackwards;
	string currenthoverCarLeft;
	string currenthoverCarRight;
	string currentchaseCamera;
	string currenthoverCarBoost;
	string currentprepareRace;
	/* Key numbers */
	int pauseKeyNumber;
	int quitKeyNumber;
	int resetCamKeyNumber;
	int chaseCameraNumber;
	int prepareRaceNumber;
	int hoverCarForwardsNumber;
	int hoverCarBackwardsNumber;
	int hoverCarLeftNumber;
	int hoverCarRightNumber;
	int hoverCarBoostNumber;
public:
	string leftSettingsList[6] = { "Full Screen:", "FPS Toggle:", "Pause:", "Back/Quit:", "Reset Camera:", "Chase cam:" };
	string rightSettingsList[6] = { "Prepare Race:", "Car Fwds: ", "Car Bckwds: ", "Car Steer Left:", "Car Steer Right: ", "Boost:" };
	string carColours[numOfColours] = { "Blue", "Brown", "DarkBlue", "Green", "LightPink", "Orange", "Pink", "Red", "Yellow" };
	string fumeColours[numOfFumes] = { "Blue", "Purple", "Pink", "Red", "Yellow", "Green", "DarkGreen" };
	string carFumeName = "Fume Colour: ";
	string carColourName = "Colour:";
	bool changingKey = false;
	bool fullScreen = false;			    // Toggle Full Screen
	bool fpsCounter = false;			    // Toggle FPS Counter
	string tempCurrentKeyName = "";		    // Current key hit display
	string notInUseKeyName;				    // Store if not in use
	string keyChangeName;				    // Store which key they clicked on
	string carColourFileName;				// Store current player car colour
	string carFumeFileName;					// Store current player fume name
	EKeyCode firstPersonCamera = Key_2;		// None changable key
	EKeyCode buttonClicker = Mouse_LButton; // None changable key
	EKeyCode restartGame = Key_R;			// None changable key
	EKeyCode pauseGameKey;					// Pause game
	EKeyCode quitKey;					    // Quit program
	EKeyCode resetCamera;					// Reset position of camera 
	EKeyCode chaseCamera;					// Look up camera 
	EKeyCode prepareRace;					// Restart level
	EKeyCode hoverCarForwards;				// Forwards
	EKeyCode hoverCarBackwards;				// Backwards
	EKeyCode hoverCarLeft;					// Left
	EKeyCode hoverCarRight;					// Right
	EKeyCode hoverCarBoost;					// Boost
	int carColour = Blue;
	int fumeColour = Blue;
	void Initialise(IFont* gameFont)
	{
		// Settings text positions
		for (int i = leftSettingsStart; i < leftSettingsEnd; i++)
		{
			textPosX[i] = 100;
			textPosY[i] = 260 + currentLine * 75;
			currentLine++;
		}
		currentLine = 0;
		for (int i = rightSettingsStart; i < rightSettingsEnd; i++)
		{
			textPosX[i] = 530;
			textPosY[i] = 260 + currentLine * 75;
			currentLine++;
		}
		textPosX[25] = 900;
		textPosY[25] = 260;
		widthOfText[25] = gameFont->MeasureTextWidth(carColourName);
		heightOfText[25] = gameFont->MeasureTextHeight(carColourName);
		textPosX[26] = 900;
		textPosY[26] = 345;
		widthOfText[26] = gameFont->MeasureTextWidth(carFumeName);
		heightOfText[26] = gameFont->MeasureTextHeight(carFumeName);

		for (int i = leftSettingsStart; i < leftSettingsEnd; i++)
		{
			widthOfText[i] = gameFont->MeasureTextWidth(leftSettingsList[i - 6]);
			heightOfText[i] = gameFont->MeasureTextHeight(leftSettingsList[i - 6]);
		}
		for (int i = rightSettingsStart; i < rightSettingsEnd; i++)
		{
			widthOfText[i] = gameFont->MeasureTextWidth(rightSettingsList[i - 12]);
			heightOfText[i] = gameFont->MeasureTextHeight(rightSettingsList[i - 12]);
		}
	}
	// Draw settings
	void Draw(IFont* gameFont)
	{
		for (int i = leftSettingsStart; i < 12; i++)
		{   // Left hand side
			gameFont->Draw(leftSettingsList[i - 6], textPosX[i], textPosY[i], toggledColour[i]);
			gameFont->Draw(currentGameKeysLeft[i - 6], textPosX[i] + 225, textPosY[i], toggledColour[i]);
		}
		gameFont->Draw("For Full Screen to take effect, you must restart the game.", 100, 215, defaultColour);
		for (int i = rightSettingsStart; i < rightSettingsEnd; i++)
		{   // Right hand side
			gameFont->Draw(rightSettingsList[i - 12], textPosX[i], textPosY[i], toggledColour[i]);
			gameFont->Draw(currentGameKeysRight[i - 12], textPosX[i] + 250, textPosY[i], toggledColour[i]);
		}
		gameFont->Draw(carColourName, textPosX[25], textPosY[25], toggledColour[25]);
		gameFont->Draw(carFumeName, textPosX[26], textPosY[26], toggledColour[26]);
	}
	// Give the user a chance to set a new key
	void SetNewKeys(I3DEngine* myEngine, int gameState)
	{
		if (keyChangeName == leftSettingsList[PauseText])
		{
			currentKey = tempPauseGameKey;						  // Send current key in
			tempPauseGameKey = ReturnNewKey(myEngine, gameState); // Return a new key (temp)
		}
		else if (keyChangeName == leftSettingsList[QuitText])
		{
			currentKey = tempQuitKey;
			tempQuitKey = ReturnNewKey(myEngine, gameState);
		}
		else if (keyChangeName == leftSettingsList[ResetCameraText])
		{
			currentKey = tempResetCamera;
			tempResetCamera = ReturnNewKey(myEngine, gameState);
		}
		else if (keyChangeName == leftSettingsList[chaseCamText])
		{
			currentKey = tempchaseCamera;
			tempchaseCamera = ReturnNewKey(myEngine, gameState);
		}
		else if (keyChangeName == rightSettingsList[PrepareRaceText])
		{
			currentKey = tempPrepareRace;
			tempPrepareRace = ReturnNewKey(myEngine, gameState);
		}
		else if (keyChangeName == rightSettingsList[HoverCarUpText])
		{
			currentKey = tempHoverCarForwards;
			tempHoverCarForwards = ReturnNewKey(myEngine, gameState);
		}
		else if (keyChangeName == rightSettingsList[HoverCarDownText])
		{
			currentKey = tempHoverCarBackwards;
			tempHoverCarBackwards = ReturnNewKey(myEngine, gameState);
		}
		else if (keyChangeName == rightSettingsList[HoverCarLeftText])
		{
			currentKey = tempHoverCarLeft;
			tempHoverCarLeft = ReturnNewKey(myEngine, gameState);
		}
		else if (keyChangeName == rightSettingsList[HoverCarRightText])
		{
			currentKey = tempHoverCarRight;
			tempHoverCarRight = ReturnNewKey(myEngine, gameState);
		}
		else if (keyChangeName == rightSettingsList[BoostText])
		{
			currentKey = tempHoverCarBoost;
			tempHoverCarBoost = ReturnNewKey(myEngine, gameState);
		}
	}
	// Reset message, display on console
	void SettingsResetMSG()
	{
		cout << "Settings reset successfully." << endl;
	}
	// Log file keys, display on console
	void FileLog()
	{
		cout << "[LOG] Full Screen Enabled: " << boolalpha << loadFullScreen << "(Requires restart for changes to take effect)" << endl;
		cout << "[LOG] FPS Display Enabled: " << boolalpha << toggleFpsCounter << endl;
		cout << "[LOG] Pause Key: " << currentGameKeysLeft[PauseText] << endl;
		cout << "[LOG] Quit Key: " << currentGameKeysLeft[QuitText] << endl;
		cout << "[LOG] Reset Camera Key: " << currentGameKeysLeft[ResetCameraText] << endl;
		cout << "[LOG] Chase Cam Key: " << currentGameKeysLeft[chaseCamText] << endl;
		cout << "[LOG] Prepare Race Key: " << currentGameKeysRight[PrepareRaceText] << endl;
		cout << "[LOG] Hover Car Forwards Key: " << currentGameKeysRight[HoverCarUpText] << endl;
		cout << "[LOG] Hover Car Backwards Key: " << currentGameKeysRight[HoverCarDownText] << endl;
		cout << "[LOG] Hover Car Left Key: " << currentGameKeysRight[HoverCarLeftText] << endl;
		cout << "[LOG] Hover Car Right Key: " << currentGameKeysRight[HoverCarRightText] << endl;
		cout << "[LOG] Boost Key: " << currentGameKeysRight[BoostText] << endl;
		cout << "[LOG] Car Colour: " << carColours[carColour] << endl;
		cout << "[LOG] Car Fumes: " << fumeColours[fumeColour] << endl;
	}
	// Load keys into temporary variables
	void LoadTempKeys()
	{	// Store keys in temporary variables
		tempQuitKey = quitKey;
		tempPauseGameKey = pauseGameKey;
		tempResetCamera = resetCamera;
		tempchaseCamera = chaseCamera;
		tempPrepareRace = prepareRace;
		tempHoverCarForwards = hoverCarForwards;
		tempHoverCarBackwards = hoverCarBackwards;
		tempHoverCarLeft = hoverCarLeft;
		tempHoverCarRight = hoverCarRight;
		tempHoverCarBoost = hoverCarBoost;
	}
	// Update keys (When pressing after changing key or if ResetKeys is called
	void UpdateKeys()
	{   // Update controls
		cout << "[LOG] Game keys updated" << endl;
		quitKey = tempQuitKey;
		pauseGameKey = tempPauseGameKey;
		resetCamera = tempResetCamera;
		chaseCamera = tempchaseCamera;
		prepareRace = tempPrepareRace;
		hoverCarForwards = tempHoverCarForwards;
		hoverCarBackwards = tempHoverCarBackwards;
		hoverCarLeft = tempHoverCarLeft;
		hoverCarRight = tempHoverCarRight;
		hoverCarBoost = tempHoverCarBoost;
	}
	// Reset Game Keys
	void ResetKeys()
	{
		fullScreen = false;				   // Full Screen
		fpsCounter = false;				   // FPS Counter
		pauseGameKey = Key_P;			   // Pause game
		quitKey = Key_Escape;			   // Quit program
		resetCamera = Key_R;			   // Reset position of camera 
		chaseCamera = Key_1;			   // Chase camera 
		prepareRace = Key_Space;		   // Prepare race
		hoverCarForwards = Key_W;          // Forwards
		hoverCarBackwards = Key_S;		   // Backwards
		hoverCarLeft = Key_A;			   // Left
		hoverCarRight = Key_D;		       // Right
		hoverCarBoost = Key_Space;	       // Boost
		LoadTempKeys();					   // Set temp keys equal to the default keys
		carColour = Blue;				   // Set player car to default blue
		fumeColour = Blue;				   // Set player car fume colour to default blue
										   // Reset key names on settings screen
		currentGameKeysLeft[PauseText] = "P";            // Default pause key
		currentGameKeysLeft[QuitText] = "Escape";        // Default quit key
		currentGameKeysLeft[ResetCameraText] = "R";      // Default reset camera key
		currentGameKeysLeft[chaseCamText] = "1";         // Default camera view 1 key
		currentGameKeysRight[PrepareRaceText] = "Space"; // Default restart level key
		currentGameKeysRight[HoverCarUpText] = "W";      // Default hoverCar Forwards key
		currentGameKeysRight[HoverCarDownText] = "S";    // Default hoverCar Backwards key
		currentGameKeysRight[HoverCarLeftText] = "A";    // Default hoverCar Left key
		currentGameKeysRight[HoverCarRightText] = "D";   // Default hoverCar Right key
		currentGameKeysRight[BoostText] = "Space";      // Default camera view 2 key
	}
	// Save the keys to the file
	void SaveFile()
	{
		saveGameSettings.open(gameSettings); // Open the file
		if (saveGameSettings.fail()) perror("[LOG]settings.dat");// Error handling

		cout << "[LOG] SAVING ALL KEYS..." << endl;
		saveGameSettings << boolalpha << fullScreen << endl; // Save Full Screen State
		saveGameSettings << boolalpha << fpsCounter << endl; // Save FPS Counter State
		saveGameSettings << pauseGameKey << endl;
		saveGameSettings << quitKey << endl;
		saveGameSettings << resetCamera << endl;
		saveGameSettings << chaseCamera << endl;
		saveGameSettings << prepareRace << endl;
		saveGameSettings << hoverCarForwards << endl;
		saveGameSettings << hoverCarBackwards << endl;
		saveGameSettings << hoverCarLeft << endl;
		saveGameSettings << hoverCarRight << endl;
		saveGameSettings << hoverCarBoost << endl;
		saveGameSettings << carColour << endl;
		saveGameSettings << fumeColour << endl;
		cout << "[LOG] Save Complete" << endl;

		saveGameSettings.close(); // CLOSE we are done with it
	}

	string GetKeyName(int keyNumber) // Send the key number over
	{
		keyName = keyCodeNames[keyNumber]; // Match key name with number
		return keyName;					   // Return the key name
	}
	// Find the key names for the currently using keys
	void FindAllKeyNames()
	{	// Left side menu key names
		currentGameKeysLeft[PauseText] = GetKeyName(pauseGameKey);
		currentGameKeysLeft[QuitText] = GetKeyName(quitKey);
		currentGameKeysLeft[ResetCameraText] = GetKeyName(resetCamera);
		currentGameKeysLeft[chaseCamText] = GetKeyName(chaseCamera);
		// Right side menu key names
		currentGameKeysRight[PrepareRaceText] = GetKeyName(prepareRace);
		currentGameKeysRight[HoverCarUpText] = GetKeyName(hoverCarForwards);
		currentGameKeysRight[HoverCarDownText] = GetKeyName(hoverCarBackwards);
		currentGameKeysRight[HoverCarLeftText] = GetKeyName(hoverCarLeft);
		currentGameKeysRight[HoverCarRightText] = GetKeyName(hoverCarRight);
		currentGameKeysRight[BoostText] = GetKeyName(hoverCarBoost);
	}
	// Return a new key when pressed 
	EKeyCode ReturnNewKey(I3DEngine* myEngine, int gameState)
	{
		for (int i = 2; i < kMaxKeyCodes; i++) // For all the keys
		{
			if (myEngine->KeyHit(EKeyCode(i))) // If they hit a key in the list
			{
				notInUseKeyName = GetKeyName(i); // Display the temp name of key chosen
				if (notInUseKeyName == currentGameKeysLeft[PauseText] ||
					notInUseKeyName == currentGameKeysLeft[QuitText] ||
					notInUseKeyName == currentGameKeysLeft[ResetCameraText] ||
					notInUseKeyName == currentGameKeysLeft[chaseCamText] ||
					notInUseKeyName == currentGameKeysRight[PrepareRaceText] ||
					notInUseKeyName == currentGameKeysRight[HoverCarUpText] ||
					notInUseKeyName == currentGameKeysRight[HoverCarDownText] ||
					notInUseKeyName == currentGameKeysRight[HoverCarLeftText] ||
					notInUseKeyName == currentGameKeysRight[HoverCarRightText] ||
					notInUseKeyName == currentGameKeysRight[BoostText]) keyInUse = true;
				else
				{
					keyInUse = false;
					tempCurrentKeyName = notInUseKeyName; // Set new key as temp
					return EKeyCode(i);                   // Put the key in temp
				}
			}
		}
		return EKeyCode(currentKey); //return current key
	}
	// Get the colour sprites
	void GetCarColourNames()
	{
		if (carColour == Blue)	    carColourFileName = "spengland.jpg";
		if (carColour == Brown)		carColourFileName = "spbrown.jpg";
		if (carColour == DarkBlue)  carColourFileName = "spdarkblue.jpg";
		if (carColour == Green)     carColourFileName = "spgreen.jpg";
		if (carColour == LightPink) carColourFileName = "splightpink.jpg";
		if (carColour == Orange)	carColourFileName = "sporange.jpg";
		if (carColour == Pink)		carColourFileName = "sppink.jpg";
		if (carColour == Red)	    carColourFileName = "spred.jpg";
		if (carColour == Yellow)	carColourFileName = "spyellow.jpg";
	}
	// Get the different fume sprite colours
	void GetFumeColourNames()
	{
		if (fumeColour == FBlue)      carFumeFileName = "006_tlxadd.tga";
		if (fumeColour == FPurple)    carFumeFileName = "007_tlxadd.tga";
		if (fumeColour == FPink)	  carFumeFileName = "008_tlxadd.tga";
		if (fumeColour == FRed)       carFumeFileName = "009_tlxadd.tga";
		if (fumeColour == FYellow)    carFumeFileName = "010_tlxadd.tga";
		if (fumeColour == FGreen)     carFumeFileName = "011_tlxadd.tga";
		if (fumeColour == FDarkGreen) carFumeFileName = "012_tlxadd.tga";
	}
	// Cycle through the car colours
	void CycleColours()
	{
		if (carColour < numOfColours - 1) carColour++; // Next colour
		else carColour = Blue;
		GetCarColourNames(); // Update the car skin file name
	}
	// Cycle through the fume colours
	void CycleFumes()
	{
		if (fumeColour < numOfFumes - 1) fumeColour++; // Next colour
		else fumeColour = FBlue;
		GetFumeColourNames();
	}
	// Reset key changer when new key applied / ignored
	void ResetKeyChanger()
	{	// No longer changing keys
		tempCurrentKeyName = "";
		notInUseKeyName = "";
		changingKey = false;
	}

	void SaveAndPrint()
	{	// Reset and save keys to file
		ResetKeys();
		SaveFile();
		cout << "[LOG] Error loading game file." << endl;
	}

	void LoadSettings()
	{   // Load user settings from a file
		loadData.open(gameSettings); // Open the file
		if (loadData.fail()) // Error handling
		{
			perror("[LOG] settings.dat");
			cout << "[LOG] Using default settings..." << endl;
		}
		// Load user settings from game file
		loadData >> loadFullScreen >> toggleFpsCounter >> currentPauseKey >> currentQuitKey
			>> currentResetCamera >> currentchaseCamera >> currentprepareRace >> currenthoverCarForwards
			>> currenthoverCarBackwards >> currenthoverCarLeft >> currenthoverCarRight >> currenthoverCarBoost >> userCarColour >> userFumeColour;
		(loadFullScreen == "true") ? fullScreen = true : fullScreen = false;
		(toggleFpsCounter == "true") ? fpsCounter = true : fpsCounter = false;

		if (userCarColour == "0")      carColour = Blue;
		else if (userCarColour == "1") carColour = Brown;
		else if (userCarColour == "2") carColour = DarkBlue;
		else if (userCarColour == "3") carColour = Green;
		else if (userCarColour == "4") carColour = LightPink;
		else if (userCarColour == "5") carColour = Orange;
		else if (userCarColour == "6") carColour = Pink;
		else if (userCarColour == "7") carColour = Red;
		else if (userCarColour == "8") carColour = Yellow;
		else carColour = Blue; // Use blue default

		if (userFumeColour == "0")      fumeColour = FBlue;
		else if (userFumeColour == "1") fumeColour = FPurple;
		else if (userFumeColour == "2") fumeColour = FPink;
		else if (userFumeColour == "3") fumeColour = FRed;
		else if (userFumeColour == "4") fumeColour = FYellow;
		else if (userFumeColour == "5") fumeColour = FGreen;
		else if (userFumeColour == "6") fumeColour = FDarkGreen;
		else fumeColour = Blue; // Use blue default

		if (currentPauseKey != "" &&
			currentQuitKey != "" &&
			currentResetCamera != "" &&
			currentchaseCamera != "" &&
			currentprepareRace != "" &&
			currenthoverCarForwards != "" &&
			currenthoverCarBackwards != "" &&
			currenthoverCarLeft != "" &&
			currenthoverCarRight != "" &&
			currenthoverCarBoost != "")
		{   // Convert string to int
			pauseKeyNumber = stoi(currentPauseKey);
			quitKeyNumber = stoi(currentQuitKey);
			resetCamKeyNumber = stoi(currentResetCamera);
			chaseCameraNumber = stoi(currentchaseCamera);
			prepareRaceNumber = stoi(currentprepareRace);
			hoverCarForwardsNumber = stoi(currenthoverCarForwards);
			hoverCarBackwardsNumber = stoi(currenthoverCarBackwards);
			hoverCarLeftNumber = stoi(currenthoverCarLeft);
			hoverCarRightNumber = stoi(currenthoverCarRight);
			hoverCarBoostNumber = stoi(currenthoverCarBoost);
			// If they havent been tampered with
			if (pauseKeyNumber          > 0 && pauseKeyNumber          < kMaxKeyCodes &&
				quitKeyNumber           > 0 && quitKeyNumber           < kMaxKeyCodes &&
				resetCamKeyNumber       > 0 && resetCamKeyNumber       < kMaxKeyCodes &&
				chaseCameraNumber        > 0 && chaseCameraNumber        < kMaxKeyCodes &&
				prepareRaceNumber       > 0 && prepareRaceNumber       < kMaxKeyCodes &&
				hoverCarForwardsNumber  > 0 && hoverCarForwardsNumber  < kMaxKeyCodes &&
				hoverCarBackwardsNumber > 0 && hoverCarBackwardsNumber < kMaxKeyCodes &&
				hoverCarLeftNumber      > 0 && hoverCarLeftNumber      < kMaxKeyCodes &&
				hoverCarRightNumber     > 0 && hoverCarRightNumber     < kMaxKeyCodes &&
				hoverCarBoostNumber     > 0 && hoverCarBoostNumber     < kMaxKeyCodes)
			{   // Assign all key numbers and retrieve their names
				pauseGameKey = EKeyCode(pauseKeyNumber);
				quitKey = EKeyCode(quitKeyNumber);
				resetCamera = EKeyCode(resetCamKeyNumber);
				chaseCamera = EKeyCode(chaseCameraNumber);
				prepareRace = EKeyCode(prepareRaceNumber);
				hoverCarForwards = EKeyCode(hoverCarForwardsNumber);
				hoverCarBackwards = EKeyCode(hoverCarBackwardsNumber);
				hoverCarLeft = EKeyCode(hoverCarLeftNumber);
				hoverCarRight = EKeyCode(hoverCarRightNumber);
				hoverCarBoost = EKeyCode(hoverCarBoostNumber);

				FindAllKeyNames(); // Find and assign all key names to the above keys
				LoadTempKeys(); // Store the newly assigned keys into temporary keys
			}
			else SaveAndPrint(); // If any numbers have been tampered with, they will use default
		}
		else SaveAndPrint(); // If nothing exists use default

		FileLog();		  // Write current key list on the console
		loadData.close(); // CLOSE we are done with it
	}
};
ClassSettings userSettings; // Variable for settings class

// FPS Class
class FPS
{
private:
	stringstream fpsText; // Game FPS
	float frames;		  // Number of frames
	float nextUpdate;	  // Update every second
	float overtime;       // 1 second overtime
	float fps;			  // Store fps
public:
	void Run(IFont* gameFont)
	{
		++frames; // Keep adding frames
		overtime = clock() - nextUpdate;
		if (overtime > 0)
		{
			fps = frames / (1.0f + overtime / (float(CLOCKS_PER_SEC)));
			frames = 0; // Reset frames
			nextUpdate = clock() + 1 * (float(CLOCKS_PER_SEC));
		}
		fpsText << roundf(fps); // Display FPS
		gameFont->Draw(fpsText.str(), 300, engineLength - 75, defaultColour);
		fpsText.str("");
	}
};
// Abstract class - Idea to have derived classes interpreting the functions from this.
class BoundingBox
{
private:
	float minX, maxX, minY, maxY, minZ, maxZ;
public:
	virtual void SetBox(IModel* object, float objectWidth, float objectLength)
	{
		minX = object->GetX() - objectWidth;
		maxX = object->GetX() + objectWidth;
		minZ = object->GetZ() - objectLength;
		maxZ = object->GetZ() + objectLength;
	}
	virtual float GetMinX() { return minX; }
	virtual float GetMaxX() { return maxX; }
	virtual float GetMinZ() { return minZ; }
	virtual float GetMaxZ() { return maxZ; }
	virtual float GetX(IModel* object) { return object->GetX(); }
	virtual float GetY(IModel* object) { return object->GetY(); }
	virtual float GetZ(IModel* object) { return object->GetZ(); }
};

// Waypoint dummy class
class WayPoint : public BoundingBox
{
public:
	IModel* wpModel;
	WayPoint(IMesh* dummyMesh, int dummyNumber, bool lane2) // Constructor
	{
		// Lane 2
		if (lane2)
		{
			if (dummyNumber == 0) wpModel = dummyMesh->CreateModel(5.0f, objFloor, 472.0f);
			if (dummyNumber == 1) wpModel = dummyMesh->CreateModel(111.0f, objFloor, 622.0f);
			if (dummyNumber == 2) wpModel = dummyMesh->CreateModel(280.0f, objFloor, 622.0f);
			if (dummyNumber == 3) wpModel = dummyMesh->CreateModel(396.0f, objFloor, 454.0f);
			if (dummyNumber == 4) wpModel = dummyMesh->CreateModel(396.0f, objFloor, 27.0f);
			if (dummyNumber == 5) wpModel = dummyMesh->CreateModel(296.0f, objFloor, -116.0f);
			if (dummyNumber == 6) wpModel = dummyMesh->CreateModel(104.0f, objFloor, -116.0f);
			if (dummyNumber == 7) wpModel = dummyMesh->CreateModel(7.0f, objFloor, 24.0f);
		}
		else
		{	// Lane 1
			if (dummyNumber == 0) wpModel = dummyMesh->CreateModel(-2.5f, objFloor, 511.0f);
			if (dummyNumber == 1) wpModel = dummyMesh->CreateModel(86.0f, objFloor, 635.0f);
			if (dummyNumber == 2) wpModel = dummyMesh->CreateModel(330.0f, objFloor, 635.0f);
			if (dummyNumber == 3) wpModel = dummyMesh->CreateModel(401.0f, objFloor, 531.0f);
			if (dummyNumber == 4) wpModel = dummyMesh->CreateModel(401.0f, objFloor, -39.0f);
			if (dummyNumber == 5) wpModel = dummyMesh->CreateModel(315.0f, objFloor, -160.0f);
			if (dummyNumber == 6) wpModel = dummyMesh->CreateModel(80.0f, objFloor, -160.0f);
			if (dummyNumber == 7) wpModel = dummyMesh->CreateModel(-2.5f, objFloor, -40.0f);
		}
	}
	~WayPoint() // Destructor
	{

	}
};
const int numOfWayPoints = 8;
WayPoint* lane1WayPoint[numOfWayPoints];
WayPoint* lane2WayPoint[numOfWayPoints];

class Countdown
{
private:
	clock_t ticks;			    // Number of clock ticks
	clock_t timeLeft;	   	    // Time Remaining
	clock_t totalTimeRemaining;	// Total time remaining
	bool isCountingDown;	    // Check to see if timer is counting down
public:
	int startingTime;
	Countdown(int time) // Countdown constructor 
	{
		isCountingDown = false;
		startingTime = time;
		timeLeft = time;
		ticks = -clock();
		totalTimeRemaining;
	}
	void StopCountdown() // Stop the count down
	{
		isCountingDown = false;
	}
	void StartCountdown() // Start the count down
	{
		ticks = -clock() - timeLeft;
		isCountingDown = true;
	}
	void ResetCountdown() // Reset the count down
	{
		timeLeft = startingTime;
	}
	bool CountingDown()
	{
		return isCountingDown;
	}
	int GetTimeRemaining()
	{
		if (isCountingDown) timeLeft = -clock() - ticks;
		if (timeLeft > 0) return int(timeLeft) / CLOCKS_PER_SEC; // Return time remaining
		else return 0; // Time UP	
	}
	int GetMSRemaining()
	{
		if (isCountingDown) timeLeft = -clock() - ticks;
		if (timeLeft > 0) return int(timeLeft); // Return time remaining
		else return 0; // Time UP	
	}
};

class StopWatch
{
private:
	clock_t ticks;			    // Number of clock ticks
	clock_t totalTime;	   	    // Total Time
	bool hasStarted;	        // Check to see if StopWatch is running
	const int startingTime = 0;
public:
	StopWatch() // StopWatch constructor 
	{
		hasStarted = false;
		totalTime = startingTime;
		ticks = clock();
	}
	void Stop() // Stop the StopWatch
	{
		hasStarted = false;
	}
	void Start() // Start the StopWatch
	{
		ticks = totalTime - clock();
		hasStarted = true;
	}
	void Reset() // Reset the StopWatch
	{
		totalTime = startingTime;
	}
	bool ClockStarted()
	{
		return hasStarted;
	}
	int GetTotalTime()
	{
		if (hasStarted) totalTime = clock() + ticks;
		return int(totalTime) / CLOCKS_PER_SEC; // Return total time
	}
};
// Cross class
class Cross
{
private:
	const float crossHeight = 5.0f;
	static const int time = 5000;
	IModel* cross;
	Countdown* displayTimer;
public:
	Cross(I3DEngine* myEngine, IMesh* crossMesh, int crossNumber)
	{
		if (crossNumber == 0)      cross = crossMesh->CreateModel(2.0f, -crossHeight, 60.0f);
		else if (crossNumber == 1) cross = crossMesh->CreateModel(2.0f, -crossHeight, 300.0f);
		else if (crossNumber == 2) cross = crossMesh->CreateModel(400.0f, -crossHeight, 450.0f);
		else if (crossNumber == 3) cross = crossMesh->CreateModel(400.0f, -crossHeight, 40.0f);
		cross->Scale(0.3f);
		displayTimer = new Countdown(time);
	}

	void DisplayCross() { cross->SetY(crossHeight * 2.0f); }  // Display the cross

	void CheckTimeRemaining()
	{
		if (displayTimer->CountingDown() == false && cross->GetY() >= crossHeight) displayTimer->StartCountdown();
		if (displayTimer->GetTimeRemaining() == 0)
		{
			cross->SetY(-crossHeight);
			displayTimer->StopCountdown();
			displayTimer->ResetCountdown();
		}
	}

	float GetY() { return cross->GetY(); }
};
Cross* cross[numOfCheckPoints];

const float createLine1Z = 400.0f;

// Crate System
const float crateFloor = 3.0f;
class CrateFunctionality
{
private:
	const float crateSpeed = 1.0f;
	const float crateRotationSpeed = 20.0f;
	const float crateLength = 2.8f;
	const float maxHeight = 8.0f;
	Countdown* hideTimer = new Countdown(10000);
	float minX, maxX, minZ, maxZ;
	bool hidden = true;
	enum hovering { Up, Down };
	int state = Up;
public:
	virtual float GetMinX() { return minX; }
	virtual float GetMaxX() { return maxX; }
	virtual float GetMinZ() { return minZ; }
	virtual float GetMaxZ() { return maxZ; }

	virtual void Update(IModel* crateType, int type)
	{
		AssignAABB(crateType);
		if (!hidden)
		{
			switch (state)
			{
			case Up:
				if (crateType->GetY() <= maxHeight)
				{
					crateType->MoveY(crateSpeed*frameRate);
					crateType->RotateY(crateRotationSpeed*frameRate);
				}
				else state = Down;
				break;

			case Down:
				if (crateType->GetY() >= crateFloor)
				{
					crateType->MoveY(-crateSpeed*frameRate);
					crateType->RotateY(-crateRotationSpeed*frameRate);
				}
				else state = Up;
				break;
			}
		}
		// Hide and appear every 20 seconds
		if (!hideTimer->CountingDown())
		{
			hideTimer->StartCountdown();                           // Start the countdown
			if (hidden)
			{
				crateType->SetPosition(-10.0f + (type * 10.0f), crateFloor, createLine1Z);  // Display it
				hidden = false;								                     // Not hidden
			}
			else
			{
				crateType->SetPosition(-100.0f, -5.0f, createLine1Z); // Hide it
				hidden = true;								          // Now hidden
			}
		}
		if (hideTimer->GetTimeRemaining() == 0)
		{
			hideTimer->StopCountdown();						// Stop countdown
			hideTimer->ResetCountdown();					// Reset the countdown
		}
	}

	virtual void Hide(IModel* crateType) { crateType->SetPosition(-100.0f, -5.0f, 0.0f); }

	virtual void AssignAABB(IModel* crateType)
	{
		minX = crateType->GetX() - crateLength;
		maxX = crateType->GetX() + crateLength;
		minZ = crateType->GetZ() - crateLength;
		maxZ = crateType->GetZ() + crateLength;
	}

	virtual float GetX(IModel* crateType) { return crateType->GetX(); }
	virtual float GetY(IModel* crateType) { return crateType->GetY(); }
	virtual float GetZ(IModel* crateType) { return crateType->GetZ(); }
};

// Derived classes
IModel* health;
class HealthCrate : public CrateFunctionality
{
public:
	HealthCrate(IMesh* healthMesh, int crateNumber)
	{
		health = healthMesh->CreateModel(-10.0f, crateFloor, createLine1Z + (crateNumber*10.0f));
	}

	void Hide() { CrateFunctionality::Hide(health); }
	~HealthCrate() // Destructor
	{

	}
};

IModel* slow;
class SlowCrate : public CrateFunctionality
{
public:
	SlowCrate(IMesh* slowMesh, int crateNumber)
	{
		slow = slowMesh->CreateModel(0.0f, crateFloor, createLine1Z + (crateNumber*10.0f));
	}

	void Hide() { CrateFunctionality::Hide(slow); }

	~SlowCrate() // Destructor
	{

	}
};

IModel* poison;
class PoisonCrate : public CrateFunctionality
{
public:
	PoisonCrate(IMesh* poisonMesh, int crateNumber)
	{
		poison = poisonMesh->CreateModel(10.0f, crateFloor, createLine1Z + (crateNumber*10.0f));
	}

	void Hide() { CrateFunctionality::Hide(poison); }
	~PoisonCrate() // Destructor
	{

	}
};

// Creation of the derived classes
const int numOfCrates = 1;
HealthCrate* healthCrate[numOfCrates];
SlowCrate* slowCrate[numOfCrates];
PoisonCrate* poisonCrate[numOfCrates];

class Bomb : public BoundingBox
{
private:
	const float bombFloor = 2.0f;
	Countdown* hideTimer = new Countdown(10000);
	float scaleAmt;
	bool hidden;
public:
	IModel* bomb;
	const int side1 = 4;
	const int side2 = 9;
	Bomb(IMesh* bombMesh, int bombNumber)
	{
		// Create
		if (bombNumber < side1)      bomb = bombMesh->CreateModel(0.0f, bombFloor, 100.0f + (100.0f * bombNumber));
		else if (bombNumber < side2) bomb = bombMesh->CreateModel(400.0f, bombFloor, 100.0f * (bombNumber - 4));
		// Assign box
		SetBox();
	}
	void Hide() { bomb->SetPosition(-100.0f, -5.0f, -100.0f); }
	void Update(int bombNumber)
	{
		SetBox();
		// Hide and appear every 20 seconds
		if (!hideTimer->CountingDown())
		{
			hideTimer->StartCountdown(); // Start the countdown
			if (hidden)
			{
				if (bombNumber < side1)      bomb->SetPosition(0.0f, bombFloor, 50.0f + (100.0f * bombNumber));
				else if (bombNumber < side2) bomb->SetPosition(400.0f, bombFloor, 100.0f * (bombNumber - 4));
				hidden = false; // Not hidden
			}
			else
			{
				bomb->SetPosition(-100.0f, -5.0f, -100.0f); // Hide it
				hidden = true;							 // Now hidden
			}
		}
		if (hideTimer->GetTimeRemaining() == 0)
		{
			hideTimer->StopCountdown();						// Stop countdown
			hideTimer->ResetCountdown();					// Reset the countdown
		}
	}
	~Bomb() // Destructor
	{

	}

	void SetBox() { BoundingBox::SetBox(bomb, 3.0f, 3.0f); }

	void BombExplodes()
	{
		bomb->SetPosition(-100.0f, -5.0f, 0.0f);
		SetBox();
	}

	void ResetBomb(int bombNumber)
	{
		if (bombNumber < 4)      bomb->SetPosition(0.0f, bombFloor, 50.0f + (100.0f * bombNumber));
		else if (bombNumber < 9) bomb->SetPosition(400.0f, bombFloor, 100.0f * (bombNumber - 4));
	}
};

Bomb* trackBombs[numOfBombs];
// Car class
const int playerCar = 0;
const float carLength = 6.4619f;
const float carWidth = 2.23089f;
string winnerName;
int winnerTime;
IFont* countdownFont;
Countdown* threeSecCD = new Countdown(4000);
Countdown* boostTimer = new Countdown(3000);
Countdown* overHeatTimer = new Countdown(5000);
movementVector posInList; // Store vector movement
float distanceX, distanceY, distanceZ, currentDist[carAmount];
string state[carAmount];
AddPosition positions[carAmount];
float rotationDegrees;
const int bounceTime = 2000;
const int displayTime = 5000;
const int assignTime = 3000;
const int shakeTime = 2000;
const int healthTime = 3000;
const int slowTime = 5000;
const int poisonTime = 5000;

class HoverCar
{
private:
	StopWatch* carTimers = new StopWatch;
	Countdown* bounceTimer = new Countdown(bounceTime);
	Countdown* displayTimer = new Countdown(displayTime);
	Countdown* assignSpeedTimer = new Countdown(assignTime);
	Countdown* shakeTimer = new Countdown(shakeTime);
	Countdown* healthTimer = new Countdown(healthTime);
	Countdown* slowTimer = new Countdown(slowTime);
	Countdown* poisonTimer = new Countdown(poisonTime);
	string userName;
	bool startedRace = false;
	bool collision = false;
	bool goneThrough = false;
	bool hitABomb = false;
	bool hasHealthCrate = false;
	bool hasSlowCrate = false;
	bool hasPoisonCrate = false;
	const float wayPtBoundary = 2.0f;
	float speedLimit = 0.3f;			   // 0.3 at uni, 0.6 at home
	const float speedIncrementer = 0.001f; // 0.001 at uni, 0.1 at  home
	const float minAngle = 0.0f;
	const float maxAngle = 0.005f;
	const float bankingLimit = 1.0f;
	const float steerAmt = 20.0f;
	const float startX = 5.0f;
	const float carGround = 7.5f;
	const float computerSpeed = 0.1f;
	const float shakeSpeed = 0.3f;
	const float outOfRange = 100.0f;
	static const int numOfSpeeds = 5;
	float speeds[numOfSpeeds] = { 0.1f, 0.15f, 0.2f, 0.25f, 0.3f };
	//float speeds[numOfSpeeds] = { 1.1f, 1.15f, 1.2f, 1.25f, 1.3f };
	enum carNames { Lesley = 1, Nicky, Nick, Dean, Jonathan };
	enum hoverState { HoverUp, HoverDown, None };
	enum checkPointstate { Start, checkPoint1, checkPoint2, checkPoint3, checkPoint4, Finish };
	int gameState = Start; // Start at the first checkpoint
	int hoveringState = HoverUp;
	int hitpoints = 100;
	int lap = 1;
	int ranHealth;
	int ranDamage;
	const float maxRotation = 0.2f;
	float bank, angle;
	float rotationAmt = 0.0f;
	float currentSpeed;
	float minX, maxX, minZ, maxZ;
	float noseAngle = 30.0f;
	float steerAngle = 0.0f;
	float hoverSpeed = 2.0f;
	static const int numOfGameStates = 6;
	string gameStates[numOfGameStates] = { "Start", "Stage 1/4", "Stage 2/4", "Stage 3/4", "Stage 4/4", "Finish" };
	stringstream currentGameState;
	stringstream hitpointsText;
	stringstream speedText;
	stringstream stopWatchText;
	stringstream timeRemaining;
	stringstream lapText;
	stringstream healthGained;
	stringstream slowness;
	stringstream poison;
public:
	IModel* dummy;
	IModel* hoverCar;
	string carColourFileName;
	string carFumeFileName;
	HoverCar(IMesh* hoverCarMesh, IMesh* dummyMesh, ICamera* myCamera, int carNumber)
	{   // 0-1, 2-3, 4-5
		if (carNumber == 0) hoverCar = hoverCarMesh->CreateModel(0.0f, 2.0f, -20.0f);
		if (carNumber < 3 && carNumber > 0)      hoverCar = hoverCarMesh->CreateModel(-2.5f, 2.0f*carNumber, 30.0f - (carNumber*20.0f));
		else if (carNumber > 2 && carNumber < 6) hoverCar = hoverCarMesh->CreateModel(startX, 2.5f*carNumber - 3, 30.0f - (carNumber - 3)*20.0f);
		currentSpeed = 0.0f;

		switch (carNumber)
		{
		case playerCar:
			dummy = dummyMesh->CreateModel(0.0f, carGround, -20.0f);
			hoverCar->AttachToParent(dummy);
			myCamera->AttachToParent(dummy);

			userName = fastestTimes.userName;
			userSettings.GetCarColourNames();
			SetCarColour();
			break;
		case Nick:	   userName = "Nick";     break;
		case Dean:	   userName = "Dean";     break;
		case Lesley:   userName = "Lesley";   break;
		case Nicky:	   userName = "Nicky";    break;
		case Jonathan: userName = "Jonathan"; break;
		}

		if (carNumber == Lesley)   hoverCar->SetSkin("spred.jpg");
		if (carNumber == Nicky)    hoverCar->SetSkin("spyellow.jpg");
		if (carNumber == Nick)     hoverCar->SetSkin("spdarkblue.jpg");
		if (carNumber == Dean)     hoverCar->SetSkin("sppink.jpg");
		if (carNumber == Jonathan) hoverCar->SetSkin("sporange.jpg");
	}

	~HoverCar() // Destructor
	{

	}

	void ShakeCamera(ICamera* myCamera)
	{
		(cameraShake) ? myCamera->MoveY(shakeSpeed) : myCamera->MoveY(-shakeSpeed);
		cameraShake ^= 1;
	}

	void SetCarColour() { hoverCar->SetSkin(userSettings.carColourFileName); }

	void Hover(IFont* gameFont)
	{
		switch (hoveringState)
		{
		case HoverUp:
			if (hoverCar->GetY() < carGround + 2.0f) hoverCar->MoveY(hoverSpeed * frameRate);
			else hoveringState = HoverDown;
			break;
		case HoverDown:
			if (hoverCar->GetY() > carGround) hoverCar->MoveY(-hoverSpeed * frameRate);
			else hoveringState = HoverUp;
			break;
		case None: break;
		}
	}

	void PauseTimers() { carTimers->Stop(); }

	void GenerateNewCarSpeed()
	{
		if (assignSpeedTimer->CountingDown() == false) assignSpeedTimer->StartCountdown();
		if (assignSpeedTimer->GetTimeRemaining() == 0)
		{
			srand(unsigned(time(NULL)));			// Generate a random seed
			int randomSpeed = rand() % numOfSpeeds; // Generate a random number between 0 and number of speeds
			currentSpeed = speeds[randomSpeed];		// Assign the new car speed
			assignSpeedTimer->StopCountdown();
			assignSpeedTimer->ResetCountdown();
		}
	}

	void DisplayTimerStart()
	{
		if (displayTimer->CountingDown() == false) displayTimer->StartCountdown();
	}

	void DisplayStageComplete(IFont* gameFont)
	{
		if (displayTimer->CountingDown() && displayTimer->GetTimeRemaining() > 0)
		{
			stringstream stageComplete;
			stageComplete << "Stage " << GetGameState() << " complete!";
			gameFont->Draw(stageComplete.str(), 400, 200, defaultColour);
			stageComplete.str("");
		}
		else
		{
			displayTimer->StopCountdown();
			displayTimer->ResetCountdown();
		}
	}

	void NextLap() { lap++; }

	void LoadFinishPositions(int &car)
	{
		ResetPositions(car);
		hoverCar->MoveZ(100.0f);
	}

	void Stop(int &car)
	{
		carTimers->Stop();
		currentSpeed = 0.0f;
		LoadFinishPositions(car);
		if (car == playerCar) gameOver = true;
	}

	int GetLap() { return lap; }

	void NextGameState() { gameState++; }

	void SetGameState(int state) { gameState = state; }

	int GetGameState() { return gameState; }

	void NextWayPoint(int &waypoint, int car)
	{
		if (car > 0 && car < 3)
		{
			if (waypoint == 0)	    hoverCar->LookAt(lane1WayPoint[1]->wpModel);
			else if (waypoint == 1) hoverCar->LookAt(lane1WayPoint[2]->wpModel);
			else if (waypoint == 2) hoverCar->LookAt(lane1WayPoint[3]->wpModel);
			else if (waypoint == 3) hoverCar->LookAt(lane1WayPoint[4]->wpModel);
			else if (waypoint == 4) hoverCar->LookAt(lane1WayPoint[5]->wpModel);
			else if (waypoint == 5) hoverCar->LookAt(lane1WayPoint[6]->wpModel);
			else if (waypoint == 6) hoverCar->LookAt(lane1WayPoint[7]->wpModel);
			else if (waypoint == 7) hoverCar->LookAt(lane1WayPoint[0]->wpModel);
		}
		else if (car > 2)
		{
			if (waypoint == 0)	    hoverCar->LookAt(lane2WayPoint[1]->wpModel);
			else if (waypoint == 1) hoverCar->LookAt(lane2WayPoint[2]->wpModel);
			else if (waypoint == 2) hoverCar->LookAt(lane2WayPoint[3]->wpModel);
			else if (waypoint == 3) hoverCar->LookAt(lane2WayPoint[4]->wpModel);
			else if (waypoint == 4) hoverCar->LookAt(lane2WayPoint[5]->wpModel);
			else if (waypoint == 5) hoverCar->LookAt(lane2WayPoint[6]->wpModel);
			else if (waypoint == 6) hoverCar->LookAt(lane2WayPoint[7]->wpModel);
			else if (waypoint == 7) hoverCar->LookAt(lane2WayPoint[0]->wpModel);
		}
	}

	void ResetPositions(int &carNumber)
	{
		if (carNumber == 0)
		{
			dummy->ResetOrientation();
			dummy->SetPosition(0.0f, carGround, 0.0f);
			hoverCar->SetPosition(0.0f, carGround, 0.0f);
		}
		if (carNumber > 0 && carNumber < 3)      hoverCar->SetPosition(-2.5f, 2.0f*carNumber, 30.0f - (carNumber*20.0f));
		else if (carNumber > 2 && carNumber < 6) hoverCar->SetPosition(startX, 2.5f*carNumber - 3, 30.0f - (carNumber - 3)*20.0f);
	}

	void DrawState(IFont* gameFont)
	{
		currentGameState << gameStates[gameState];
		gameFont->Draw(currentGameState.str(), 5, engineLength - 45, defaultColour);
		currentGameState.str("");
	}

	void DisplayCountdown()
	{
		if (threeSecCD->GetTimeRemaining() <= 0) timeRemaining << "GO!";
		else timeRemaining << threeSecCD->GetTimeRemaining();
		countdownFont->Draw(timeRemaining.str(), 600, 200, defaultColour);
		timeRemaining.str("");
	}

	string GetNames() { return userName; }

	void Restart(int &carNumber)
	{
		gameOver = false;
		hitpoints = 100;
		gameState = 0;
		lap = 1;
		startedRace = false;
		hoveringState = HoverUp;
		ResetPositions(carNumber);
		for (int i = 0; i < numOfBombs; i++) trackBombs[i]->ResetBomb(i);
		hoverCar->ResetOrientation();
		currentSpeed = 0.0f;
		carTimers->Stop();
		carTimers->Reset();
		carState[carNumber] = Hovering;
	}

	float GetLocalX() { return hoverCar->GetLocalX(); }
	float GetLocalY() { return hoverCar->GetLocalY(); }
	float GetLocalZ() { return hoverCar->GetLocalZ(); }
	float GetX() { return hoverCar->GetX(); }
	float GetY() { return hoverCar->GetY(); }
	float GetZ() { return hoverCar->GetZ(); }
	float GetMinX() { return minX; }
	float GetMaxX() { return maxX; }
	float GetMinZ() { return minZ; }
	float GetMaxZ() { return maxZ; }

	bool StartedRace() { return startedRace; }
	bool HasCollided() { return collision; }

	void DummyCollisions(float objMinX, float objMaxX, float objMinZ, float objMaxZ)
	{
		collision = (dummy->GetLocalX() < objMaxX && dummy->GetLocalX() > objMinX &&
			dummy->GetLocalZ() < objMaxZ && dummy->GetLocalZ() > objMinZ);

		if (collision) { Bounce(); }
	}

	void DummyBombCollision(float bombMinX, float bombMaxX, float bombMinZ, float bombMaxZ, int flare)
	{
		hitABomb = (dummy->GetLocalX() < bombMaxX && dummy->GetLocalX() > bombMinX &&
			dummy->GetLocalZ() < bombMaxZ && dummy->GetLocalZ() > bombMinZ);

		if (hitABomb)
		{
			if (currentSound.getStatus() == 0)
			{
				LoadSound("sound/Explosion.wav");
				currentSound.play();
			}
			if (!shakeTimer->CountingDown()) shakeTimer->StartCountdown();
			TakeHitpoints(1);
			trackBombs[flare]->BombExplodes();
		}
	}

	void CameraShaking(ICamera* myCamera)
	{
		if (shakeTimer->CountingDown() && shakeTimer->GetTimeRemaining() > 0)
		{
			ShakeCamera(myCamera);
		}
		else if (shakeTimer->GetTimeRemaining() == 0)
		{
			shakeTimer->StopCountdown();
			shakeTimer->ResetCountdown();
		}
	}

	void HealthPackage(float minX, float maxX, float minZ, float maxZ, int crate, IFont* gameFont)
	{
		hasHealthCrate = (dummy->GetLocalX() < maxX && dummy->GetLocalX() > minX &&
			dummy->GetLocalZ() < maxZ && dummy->GetLocalZ() > minZ);

		if (hasHealthCrate)
		{
			srand(unsigned(time(NULL)));		  // Generate a random seed
			ranHealth = rand() % 20 + 1;		  // Generate health gain between 1 and 20!
			hitpoints += ranHealth;				  // Add the new health
			if (hitpoints > 100) hitpoints = 100; // Maximum amount of health
			healthCrate[crate]->Hide();			  // Force hide the crate
			if (!healthTimer->CountingDown()) healthTimer->StartCountdown(); // Start the display timer
		}

		if (healthTimer->CountingDown())
		{	// Display the health gained while counting down
			healthGained << "+" << ranHealth;
			gameFont->Draw(healthGained.str(), 600, engineLength - 45, kGreen);
			healthGained.str("");
			if (healthTimer->GetTimeRemaining() == 0)
			{	// When the time runs out, stop and reset the countdown
				healthTimer->StopCountdown();
				healthTimer->ResetCountdown();
			}
		}
	}

	void SlowPackage(float minX, float maxX, float minZ, float maxZ, int crate, IFont* gameFont)
	{
		hasSlowCrate = (dummy->GetLocalX() < maxX && dummy->GetLocalX() > minX &&
			dummy->GetLocalZ() < maxZ && dummy->GetLocalZ() > minZ);

		if (hasSlowCrate)
		{
			if (!slowTimer->CountingDown()) slowTimer->StartCountdown(); // Start the countdown
			slowCrate[crate]->Hide(); // Force hide the crate
		}
		else if (slowTimer->CountingDown())
		{
			speedLimit = 0.05f;						                  // Change the speed limit
			if (currentSpeed > speedLimit) currentSpeed = speedLimit; // Set the speed to the new speed limit

			slowness << "FAILURE..." << slowTimer->GetTimeRemaining();
			gameFont->Draw(slowness.str(), 700, engineLength - 45, kRed);
			slowness.str("");
			if (slowTimer->GetTimeRemaining() == 0)
			{	// When the time runs out, stop and reset the countdown
				slowTimer->StopCountdown();
				slowTimer->ResetCountdown();
				speedLimit = 0.3f;
			}
		}
	}
	void GenDMG()
	{
		srand(unsigned(time(NULL)));
		ranDamage = rand() % 3 + 1; // Generate random damage between 1 and 3
	}
	void PoisonPackage(float minX, float maxX, float minZ, float maxZ, int crate, IFont* gameFont, ICamera* myCamera)
	{
		hasPoisonCrate = (dummy->GetLocalX() < maxX && dummy->GetLocalX() > minX &&
			dummy->GetLocalZ() < maxZ && dummy->GetLocalZ() > minZ);

		if (hasPoisonCrate)
		{
			if (!poisonTimer->CountingDown()) poisonTimer->StartCountdown(); // Start the timer
			poisonCrate[crate]->Hide(); // Force hide the crate 
		}
		else if (poisonTimer->CountingDown())
		{
			if (poisonTimer->GetMSRemaining() <= 4000)
			{
				poison << "POISON... " << poisonTimer->GetTimeRemaining() << "...";
			}
			if (poisonTimer->GetMSRemaining() < 4000 && poisonTimer->GetMSRemaining() > 3980 ||
				poisonTimer->GetMSRemaining() < 3000 && poisonTimer->GetMSRemaining() > 2980 ||
				poisonTimer->GetMSRemaining() < 2000 && poisonTimer->GetMSRemaining() > 1980 ||
				poisonTimer->GetMSRemaining() < 1000 && poisonTimer->GetMSRemaining() > 980)
			{
				GenDMG();
				hitpoints -= ranDamage;
				ShakeCamera(myCamera);
			}
			gameFont->Draw(poison.str(), 700, engineLength - 45, kRed);
			poison.str("");
			if (poisonTimer->GetTimeRemaining() == 0)
			{
				poisonTimer->StopCountdown();
				poisonTimer->ResetCountdown();
			}
		}
	}
	void ComputerBombCollision(float bombMinX, float bombMaxX, float bombMinZ, float bombMaxZ, int flare)
	{
		hitABomb = (hoverCar->GetLocalX() < bombMaxX && hoverCar->GetLocalX() > bombMinX &&
			hoverCar->GetLocalZ() < bombMaxZ && hoverCar->GetLocalZ() > bombMinZ);

		if (hitABomb)
		{
			TakeHitpoints(1);
			trackBombs[flare]->BombExplodes();
		}
	}

	void TakeHitpoints(int amount) { hitpoints -= amount; }

	void ComputerCollisions(float objMinX, float objMaxX, float objMinZ, float objMaxZ)
	{
		collision = (hoverCar->GetLocalX() < objMaxX && hoverCar->GetLocalX() > objMinX &&
			hoverCar->GetLocalZ() < objMaxZ && hoverCar->GetLocalZ() > objMinZ);

		if (collision) { Bounce(); }
	}

	bool Bouncing() { return bounceTimer->CountingDown(); }
	int GetBounceRemainder() { return bounceTimer->GetTimeRemaining(); }

	void Bounce()
	{
		if (bounceTimer->GetTimeRemaining() == 2)
		{
			if (currentSpeed > 0.0f)      currentSpeed = -currentSpeed / 3.5f;
			else						  currentSpeed -= currentSpeed * 1.5f;
			if (hitpoints > 0) hitpoints--;
		}
		if (bounceTimer->CountingDown() == false) bounceTimer->StartCountdown();
		if (bounceTimer->GetTimeRemaining() == 0)
		{
			bounceTimer->StopCountdown();
			bounceTimer->ResetCountdown();
		}
	}

	int GetTime() { return carTimers->GetTotalTime(); }

	void Update(int carNumber, IFont* gameFont)
	{
		minX = hoverCar->GetX() - carWidth;
		maxX = hoverCar->GetX() + carWidth;
		minZ = hoverCar->GetZ() - carLength;
		maxZ = hoverCar->GetZ() + carLength;
		Hover(gameFont);

		if (carTimers->ClockStarted() == false) carTimers->Start();
		if (carNumber > playerCar)
		{
			hoverCar->MoveLocalZ(currentSpeed);
			for (int waypoint = 0; waypoint < numOfWayPoints; waypoint++)
			{

				if (carNumber < 3 && hoverCar->GetX() >= lane1WayPoint[waypoint]->GetX(lane1WayPoint[waypoint]->wpModel) -
					wayPtBoundary && hoverCar->GetX() <= lane1WayPoint[waypoint]->GetX(lane1WayPoint[waypoint]->wpModel) + wayPtBoundary &&
					hoverCar->GetZ() >= lane1WayPoint[waypoint]->GetZ(lane1WayPoint[waypoint]->wpModel) -
					wayPtBoundary && hoverCar->GetZ() <= lane1WayPoint[waypoint]->GetZ(lane1WayPoint[waypoint]->wpModel) + wayPtBoundary ||

					carNumber > 2 && hoverCar->GetX() >= lane2WayPoint[waypoint]->GetX(lane2WayPoint[waypoint]->wpModel) -
					wayPtBoundary && hoverCar->GetX() <= lane2WayPoint[waypoint]->GetX(lane2WayPoint[waypoint]->wpModel) + wayPtBoundary &&
					hoverCar->GetZ() >= lane2WayPoint[waypoint]->GetZ(lane2WayPoint[waypoint]->wpModel) -
					wayPtBoundary && hoverCar->GetZ() <= lane2WayPoint[waypoint]->GetZ(lane2WayPoint[waypoint]->wpModel) + wayPtBoundary)
				{	// If hover car is at at waypoint
					NextWayPoint(waypoint, carNumber);
				}
			}
			if (collision) Bounce();
			if (currentSpeed < 0.0f) GraduallyDecreaseSpeed();
			if (currentSpeed < computerSpeed) IncreaseCarSpeed();
		}
		else if (!collision)
		{
			dummy->MoveLocalZ(currentSpeed); //MOVE DUMMY!
			hoverCar->SetZ(dummy->GetZ());
			hoverCar->SetX(dummy->GetX());
			angle = currentSpeed / rotationDegrees;
			bank = rotationAmt / rotationDegrees;
			hoverCar->ResetOrientation();
			hoverCar->RotateLocalX(-angle);
			hoverCar->RotateLocalZ(-bank * 2.0f);
			hoverCar->RotateLocalY(bank);
		}
	}

	void SteerLeft(ICamera* myCamera)
	{
		if (rotationAmt >= -maxRotation) rotationAmt -= 0.4f * frameRate;
		dummy->RotateLocalY(-steerAmt * frameRate);
	}

	void SteerRight(ICamera* myCamera)
	{
		if (rotationAmt < maxRotation) rotationAmt += 0.4f * frameRate;
		dummy->RotateLocalY(steerAmt * frameRate);
	}

	void RemoveBanking()
	{
		if (rotationAmt > 0.0f)  rotationAmt -= 0.4f * frameRate;
		if (rotationAmt < 0.01f)  rotationAmt += 0.4f * frameRate;
	}

	void PrepareRace(ICamera* myCamera)
	{
		myCamera->ResetOrientation();
		if (carState[playerCar] == Hovering)
		{
			for (int i = 0; i < carAmount; i++) carState[i] = WaitingForGo;
		}
	}

	void SetComputerSpeeds() { currentSpeed = computerSpeed; }

	float GetSpeed() { return currentSpeed; }
	int GetIntSpeed() { return int(currentSpeed * 300); }

	void Boost()
	{
		if (carState[playerCar] == Racing && currentSpeed > 0.0f) currentSpeed = speedLimit;
	}

	void IncreaseCarSpeed()
	{
		if (bounceTimer->GetTimeRemaining() == 0 || bounceTimer->CountingDown() == false)
		{
			if (!startedRace && carState[playerCar] == Racing) startedRace = true;
			if (currentSpeed < speedLimit && carState[playerCar] == Racing) currentSpeed += speedIncrementer;
		}
	}

	void DecreaseCarSpeed()
	{
		if (bounceTimer->GetTimeRemaining() == 0 || bounceTimer->CountingDown() == false)
		{
			if (currentSpeed > -speedLimit / 3.0f) currentSpeed -= speedIncrementer;
		}
	}

	void GraduallyDecreaseSpeed()
	{
		if (currentSpeed > 0.0f)                          // If cars going forwards
		{
			currentSpeed -= speedIncrementer / 2.0f;      // Decrease speed
			if (currentSpeed < 0.0f) currentSpeed = 0.0f; // Stop the car
		}
		else
		{                                                 // If cars going backwards
			currentSpeed += speedIncrementer;             // Decrease speed
			if (currentSpeed > 0.0f) currentSpeed = 0.0f; // Stop the car
		}
	}

	void DisplayHitpoints(IFont* gameFont)
	{
		hitpointsText << "HP: " << hitpoints;
		gameFont->Draw(hitpointsText.str(), 450, engineLength - 45, defaultColour);
		hitpointsText.str("");
	}

	int GetHP() { return hitpoints; }
	void DyingStage(int car)
	{
		hoveringState = None;
		carState[car] = DestroyedCar;
		carTimers->Stop();
		state[car] = "[D]";
		if (hoverCar->GetY() > -5.0f) hoverCar->MoveY(-currentSpeed);
	}

	void DisplaySpeed(IFont* gameFont) // Writing car speed on the screen
	{
		speedText << "MPH: " << GetIntSpeed();
		gameFont->Draw(speedText.str(), 450, engineLength - 75, defaultColour);
		speedText.str("");
	}

	void DisplayLap(IFont* gameFont)
	{
		lapText << "Lap: " << lap;
		gameFont->Draw(lapText.str(), 5, engineLength - 75, defaultColour);
		lapText.str("");
	}

	void DisplayStopWatch(IFont* gameFont)
	{
		stopWatchText << carTimers->GetTotalTime() << "s";
		gameFont->Draw(stopWatchText.str(), 300, engineLength - 45, defaultColour);
		stopWatchText.str("");
	}
};

HoverCar* hoverCars[carAmount];

const int crateTypes = 5;
class MiniMap
{
private:
	ISprite* map;
	ISprite* cars[carAmount];
	ISprite* crate[crateTypes][numOfCrates];
	ISprite* flares[numOfBombs];
	ISprite* camera;
	// Add crates here in future
	const string mapName = "minimap.png";
	const float display = 1.0f;
	const float x = 0.55f;
	const float z = 0.28f;
	const float xLoc = 1035.0f;
	const float yLoc = 198.0f;
public:
	MiniMap(I3DEngine* myEngine) // Constructor 
	{
		map = myEngine->CreateSprite(mapName, 1030.0f, 0.0f, display);
		cars[0] = myEngine->CreateSprite("green.png", 0.0f, 0.0f, display); // Change this to car colour
		cars[1] = myEngine->CreateSprite("red.png", 0.0f, 0.0f, display);
		cars[2] = myEngine->CreateSprite("yellow.png", 0.0f, 0.0f, display);
		cars[3] = myEngine->CreateSprite("darkblue.png", 0.0f, 0.0f, display);
		cars[4] = myEngine->CreateSprite("pink.png", 0.0f, 0.0f, display);
		cars[5] = myEngine->CreateSprite("orange.png", 0.0f, 0.0f, display);
		camera = myEngine->CreateSprite("camera.png", 0.0f, 0.0f, display);

		for (int flare = 0; flare < numOfBombs; flare++) flares[flare] = myEngine->CreateSprite("flaremm.jpg", 0.0f, 0.0f, display);
		for (int ct = 0; ct < crateTypes; ct++)
			for (int c = 0; c < numOfCrates; c++)
			{
				if (ct == 0) crate[ct][c] = myEngine->CreateSprite("crate1mm.jpg", 0.0f, 0.0f, display);
				if (ct == 1) crate[ct][c] = myEngine->CreateSprite("crate2mm.jpg", 0.0f, 0.0f, display);
				if (ct == 2) crate[ct][c] = myEngine->CreateSprite("crate3mm.jpg", 0.0f, 0.0f, display);
				if (ct == 3) crate[ct][c] = myEngine->CreateSprite("crate4mm.jpg", 0.0f, 0.0f, display);
				if (ct == 4) crate[ct][c] = myEngine->CreateSprite("crate5mm.jpg", 0.0f, 0.0f, display);
			}
	}
	// The bigger the z, the further back it is
	void Show()
	{
		map->SetZ(0.2f);
		camera->SetZ(0.1f);
		for (int car = 0; car < carAmount; car++) cars[car]->SetZ(0.1f);
		for (int flare = 0; flare < numOfBombs; flare++) flares[flare]->SetZ(0.1f);
		for (int ct = 0; ct < crateTypes; ct++)
			for (int c = 0; c < numOfCrates; c++)
				crate[ct][c]->SetZ(0.1f);
	}

	void Hide()
	{
		map->SetZ(display);
		camera->SetZ(display);
		for (int car = 0; car < carAmount; car++)  cars[car]->SetZ(display);
		for (int flare = 0; flare < numOfBombs; flare++) flares[flare]->SetZ(display);
		for (int ct = 0; ct < crateTypes; ct++)
			for (int c = 0; c < numOfCrates; c++)
				crate[ct][c]->SetZ(display);
	}

	void UpdateCarPos(int car)
	{
		if (car == 0) cars[0]->SetPosition((hoverCars[playerCar]->GetX() * x) + xLoc, (-hoverCars[playerCar]->GetZ() * z) + yLoc);
		if (car == 1) cars[1]->SetPosition((hoverCars[1]->GetX() * x) + xLoc, (-hoverCars[1]->GetZ() * z) + yLoc);
		if (car == 2) cars[2]->SetPosition((hoverCars[2]->GetX() * x) + xLoc, (-hoverCars[2]->GetZ() * z) + yLoc);
		if (car == 3) cars[3]->SetPosition((hoverCars[3]->GetX() * x) + xLoc, (-hoverCars[3]->GetZ() * z) + yLoc);
		if (car == 4) cars[4]->SetPosition((hoverCars[4]->GetX() * x) + xLoc, (-hoverCars[4]->GetZ() * z) + yLoc);
		if (car == 5) cars[5]->SetPosition((hoverCars[5]->GetX() * x) + xLoc, (-hoverCars[5]->GetZ() * z) + yLoc);
	}

	void UpdateObjects(ICamera* myCamera)
	{
		camera->SetPosition(myCamera->GetX() * x + xLoc, -myCamera->GetZ() * z + yLoc);

		for (int flare = 0; flare < numOfBombs; flare++)
		{
			if (trackBombs[flare]->GetY(trackBombs[flare]->bomb) > 0.0f)
			{
				flares[flare]->SetPosition((trackBombs[flare]->GetX(trackBombs[flare]->bomb) * x) + xLoc,
					(-trackBombs[flare]->GetZ(trackBombs[flare]->bomb) * z) + yLoc);
				flares[flare]->SetZ(0.1f);
			}
			else flares[flare]->SetZ(display);
		}

		for (int ct = 0; ct < crateTypes; ct++)
			for (int c = 0; c < numOfCrates; c++)
			{
				if (ct == 0 && slowCrate[c]->GetY(slow) > 0.0f)
				{
					crate[ct][c]->SetPosition((slowCrate[c]->GetX(slow) * x) + xLoc, (-slowCrate[c]->GetZ(slow) * z) + yLoc);
					crate[ct][c]->SetZ(0.1f);
				}
				else if (ct == 0) crate[ct][c]->SetZ(display);
				if (ct == 1 && slowCrate[c]->GetY(poison) > 0.0f)
				{
					crate[ct][c]->SetPosition((poisonCrate[c]->GetX(poison) * x) + xLoc, (-poisonCrate[c]->GetZ(poison) * z) + yLoc);
					crate[ct][c]->SetZ(0.1f);
				}
				else if (ct == 1) crate[ct][c]->SetZ(display);
				if (ct == 2 && slowCrate[c]->GetY(health) > 0.0f)
				{
					crate[ct][c]->SetPosition((healthCrate[c]->GetX(health) * x) + xLoc, (-healthCrate[c]->GetZ(health) * z) + yLoc);
					crate[ct][c]->SetZ(0.1f);
				}
				else if (ct == 2) crate[ct][c]->SetZ(display);
			}
	}

	~MiniMap() // Destructor
	{

	}
};
MiniMap* miniMap;

class TurretSystem : BoundingBox
{
private:
	Countdown* hitTimer = new Countdown(3000);
	const float turretSize = 0.1f;
	const float turretHeight = 3.0f;
	const float bulletSize = 0.2f;
	const float range = 30.0f;
	float vectorX, vectorY, vectorZ;
	float distanceX, distanceY, distanceZ, distance;
	float directionX, directionY, directionZ;
	int shoot = -1;
	bool bulletHitCar;
public:
	IModel* turret;
	IModel* bullet;
	TurretSystem(IMesh* turretMesh, IMesh* bulletMesh, int turretNumber)
	{
		switch (turretNumber)
		{
		case 0:
			turret = turretMesh->CreateModel(18.0f, 10.0f, 32.0f);
			bullet = bulletMesh->CreateModel(18.0f, 10.0f, 32.0f);
			break;
		case 1:
			turret = turretMesh->CreateModel(420.0f, 10.0f, 525.0f);
			bullet = bulletMesh->CreateModel(420.0f, 10.0f, 525.0f);
			break;
		case 2:
			turret = turretMesh->CreateModel(378.0f, 10.0f, 464.0f);
			bullet = bulletMesh->CreateModel(378.0f, 10.0f, 464.0f);
			break;
		case 3:
			turret = turretMesh->CreateModel(283.0f, 10.0f, -90.0f);
			bullet = bulletMesh->CreateModel(283.0f, 10.0f, -90.0f);
			break;
		}
		BoundingBox::SetBox(turret, 5.0f, 5.0f);
		turret->Scale(turretSize);
		turret->ScaleY(turretHeight);
		bullet->Scale(bulletSize);
		bullet->SetSkin("fire.jpg");
	}
	float MinX() { return BoundingBox::GetMinX(); }
	float MaxX() { return BoundingBox::GetMaxX(); }
	float MinZ() { return BoundingBox::GetMinZ(); }
	float MaxZ() { return BoundingBox::GetMaxZ(); }
	~TurretSystem() // Destructor
	{

	}
	void ResetBullet()
	{
		bullet->SetPosition(turret->GetX(), turret->GetY(), turret->GetZ());
		shoot = -1;
	}
	void Update()
	{
		vectorX = bullet->GetX() - hoverCars[playerCar]->GetX();
		vectorY = bullet->GetY() - hoverCars[playerCar]->GetY();
		vectorZ = bullet->GetZ() - hoverCars[playerCar]->GetZ();
		// Square them
		distanceX = vectorX * vectorX;
		distanceY = vectorY * vectorY;
		distanceZ = vectorZ * vectorZ;
		// Add them up
		distance = distanceX + distanceY + distanceZ;
		// Sqrt it
		distance = sqrt(distance);
		// Calculate the direction
		directionX = vectorX / distance;
		directionY = vectorY / distance;
		directionZ = vectorZ / distance;

		if (distance < range && shoot == -1) shoot = 250;

		if (shoot > 0)
		{	// Shoot at the hover car
			bullet->MoveX(-directionX * frameRate * kGameSpeed);
			bullet->MoveY(-directionY * frameRate * kGameSpeed);
			bullet->MoveZ(-directionZ * frameRate * kGameSpeed);
			shoot--;
		}

		if (shoot == 0) ResetBullet();

		(distance < 1.0f) ? bulletHitCar = true : bulletHitCar = false;

		if (bulletHitCar && !hitTimer->CountingDown())
		{
			if (currentSound.getStatus() == 0)
			{
				LoadSound("sound/Fireball.wav");
				currentSound.setLoop(false);
				currentSound.play();
			}
			ResetBullet();
			hoverCars[playerCar]->TakeHitpoints(1);
			hitTimer->StartCountdown();
		}
		if (hitTimer->GetTimeRemaining() == 0)
		{
			hitTimer->StopCountdown();
			hitTimer->ResetCountdown();
		}
		turret->LookAt(hoverCars[playerCar]->dummy);
		turret->Scale(turretSize);
		turret->ScaleY(turretHeight);
	}
};
const int numOfTurrets = 4;
TurretSystem* turret[numOfTurrets];

class Particle
{
private:
	IModel* particlePiece;
	const float kPi = 3.142f;
	const float kHalfCircle = 180.0f;
	const float particleScale = 0.1f;
	float velocity, gravity;
	float distance, distanceX, distanceY, distanceZ, directionX, directionY, directionZ;
	float vectorX, vectorY, vectorZ;
public:
	Particle(IMesh* particleMesh, int particleNumber, HoverCar* hoverCars[carAmount], int carNumber)
	{
		if (particleNumber < 20) particlePiece = particleMesh->CreateModel(hoverCars[carNumber]->GetX() - 0.25f, hoverCars[carNumber]->GetY() + 1.0f, hoverCars[carNumber]->GetZ() - 7.0f - (0.1f*particleNumber));
		else if (particleNumber < 40) particlePiece = particleMesh->CreateModel(hoverCars[carNumber]->GetX(), hoverCars[carNumber]->GetY() + 1.0f, hoverCars[carNumber]->GetZ() - 7.0f - (0.1f*particleNumber - 10));
		particlePiece->Scale(particleScale);
	}

	void SetSkin(string colour) { particlePiece->SetSkin(colour); }

	void SetFumeColour() { particlePiece->SetSkin(userSettings.carFumeFileName); }

	void Update(int particleNumber, int carNumber, ICamera* myCamera)
	{
		// Calculate vector between the car and the particle piece
		vectorX = particlePiece->GetX() - hoverCars[carNumber]->GetX();
		vectorY = particlePiece->GetY() - hoverCars[carNumber]->GetY();
		vectorZ = particlePiece->GetZ() - hoverCars[carNumber]->GetZ();
		// Square them
		distanceX = vectorX * vectorX;
		distanceY = vectorY * vectorY;
		distanceZ = vectorZ * vectorZ;
		// Add the distance up
		distance = distanceX + distanceY + distanceZ;
		// Square root it
		distance = sqrt(distance);
		// Calculate the direction
		directionX = vectorX / distance;
		directionY = vectorY / distance;
		directionZ = vectorZ / distance;

		particle->moveVector[0] = directionX;
		particle->moveVector[1] = directionY;
		particle->moveVector[2] = directionZ;

		particlePiece->MoveX(particle->moveVector[0] * frameRate);
		if (hoverCars[carNumber]->GetSpeed() <= 0.0f) particlePiece->MoveY(0.0f);
		else particlePiece->MoveY(particle->moveVector[1] * frameRate);

		particlePiece->MoveZ(particle->moveVector[2] * hoverCars[carNumber]->GetSpeed() * frameRate);

		particlePiece->LookAt(myCamera);
		particlePiece->Scale(particleScale);
		if (distance > 20.0f)
		{
			if (particleNumber < 20) particlePiece->SetPosition(hoverCars[carNumber]->GetX() - 0.25f, hoverCars[carNumber]->GetY() + 1.0f, hoverCars[carNumber]->GetZ());
			else if (particleNumber < 40) particlePiece->SetPosition(hoverCars[carNumber]->GetX(), hoverCars[carNumber]->GetY() + 1.0f, hoverCars[carNumber]->GetZ());
		}
	}
};
// Isle class
class Isle
{
private:
	const float isleWidth = 2.67529f;
	const float doubleWidth = isleWidth * 2.0f;
	const float isleLength = 3.41748f;
	float minX, maxX, minZ, maxZ;
	const int lane1End = 34;
	const int lane2End = 80;
	const int lane3End = 110;
	const int lane4End = 156;
	IModel* isle;
public:
	Isle(IMesh* isleMesh, int isleNumber)
	{
		if (isleNumber == 0) isle = isleMesh->CreateModel(-20.5f, objFloor, 40.0f);
		else if (isleNumber == 1) isle = isleMesh->CreateModel(19.5f, objFloor, 40.0f);
		else if (isleNumber == 2) isle = isleMesh->CreateModel(19.5f, objFloor, 53.0f);
		else if (isleNumber == 3) isle = isleMesh->CreateModel(-20.5f, objFloor, 53.0f);
		else if (isleNumber < lane1End) isle = isleMesh->CreateModel(120.0f + (isleNumber - 4)*doubleWidth, objFloor, 610.0f); // Closest to me
		else if (isleNumber < lane2End) isle = isleMesh->CreateModel(80.0f + (isleNumber - lane1End)*doubleWidth, objFloor, 660.0f); // Furthest wall
		else if (isleNumber < lane3End) isle = isleMesh->CreateModel(275.0f - (isleNumber - lane2End)*doubleWidth, objFloor, -100.0f);
		else if (isleNumber < lane4End) isle = isleMesh->CreateModel(320.0f - (isleNumber - lane3End)*doubleWidth, objFloor, -170.0f);
		minX = isle->GetX() - isleWidth;
		maxX = isle->GetX() + isleWidth;
		minZ = isle->GetZ() - isleLength;
		maxZ = isle->GetZ() + isleLength;
	}
	float GetMinX() { return minX; }
	float GetMaxX() { return maxX; }
	float GetMinZ() { return minZ; }
	float GetMaxZ() { return maxZ; }
};
// Water tank class
class WaterTank
{
private:
	IModel* tank;
	const int lane1End = 14;
	const int lane2End = 28;
	const int lane3End = 42;
	const int lane4End = 56;
	const int lane5End = 70;
	const int lane6End = 84;
	const int lane7End = 98;
	const int lane8End = 112;
	const float tankLength = 4.75567f;
	const float tankDiameter = tankLength * 2.0f;
	const float tankWidth = 5.09605f;
	float minX, maxX, minZ, maxZ;
public:
	WaterTank(IMesh* tankMesh, int tankNumber)
	{
		if (tankNumber < lane1End)      tank = tankMesh->CreateModel(-20.0f + (tankNumber*7.0f), objFloor, 530.0f + (tankNumber*tankDiameter));
		else if (tankNumber < lane2End) tank = tankMesh->CreateModel(20.0f + (tankNumber - lane1End)*7.0f, objFloor, 480.0f + (tankNumber - lane1End)*tankDiameter);
		else if (tankNumber < lane3End) tank = tankMesh->CreateModel(330.0f + (tankNumber - lane2End)*7.0f, objFloor, 660.0f - (tankNumber - lane2End)*tankDiameter);
		else if (tankNumber < lane4End) tank = tankMesh->CreateModel(280.0f + (tankNumber - lane3End)*7.0f, objFloor, 600.0f - (tankNumber - lane3End)*tankDiameter);
		else if (tankNumber < lane5End) tank = tankMesh->CreateModel(375.0f - (tankNumber - lane4End)*7.0f, objFloor, 40.0f - (tankNumber - lane4End)*tankDiameter);
		else if (tankNumber < lane6End) tank = tankMesh->CreateModel(420.0f - (tankNumber - lane5End)*7.0f, objFloor, -40.0f - (tankNumber - lane5End)*tankDiameter);
		else if (tankNumber < lane7End) tank = tankMesh->CreateModel(110.0f - (tankNumber - lane6End)*7.0f, objFloor, -100.0f + (tankNumber - lane6End)*tankDiameter);
		else if (tankNumber < lane8End) tank = tankMesh->CreateModel(70.0f - (tankNumber - lane7End)*7.0f, objFloor, -170.0f + (tankNumber - lane7End)*tankDiameter);
		else if (tankNumber == lane8End)
		{
			tank = tankMesh->CreateModel(-4.0f, -5.0f, 150.0f);
			tank->RotateZ(-20.0f);
		}
		minX = tank->GetX() - tankWidth;
		maxX = tank->GetX() + tankWidth;
		minZ = tank->GetZ() - tankLength;
		maxZ = tank->GetZ() + tankLength;
	}

	float GetMinX() { return minX; }
	float GetMaxX() { return maxX; }
	float GetMinZ() { return minZ; }
	float GetMaxZ() { return maxZ; }
};
// Wall class
class Wall
{
private:
	IModel* wall;
	const int lane1End = 50;
	const int lane2End = 95;
	const int lane3End = 138;
	const int lane4End = 195;
	const int lane5End = 203;
	const float wallLength = 4.83559f;
	const float wallWidth = 0.934082f;
	const float wallDiameter = wallLength * 2.0f;
	float minX, maxX, minZ, maxZ;
public:
	Wall(IMesh* wallMesh, int wallNumber)
	{
		if (wallNumber < lane1End) wall = wallMesh->CreateModel(-20.5f, objFloor, 46.0f + (wallNumber*wallDiameter));
		else if (wallNumber < lane2End) wall = wallMesh->CreateModel(19.5f, objFloor, 46.0f + (wallNumber - lane1End)*wallDiameter);
		else if (wallNumber < lane3End) wall = wallMesh->CreateModel(380.0f, objFloor, 46.0f + (wallNumber - lane2End)*wallDiameter);
		else if (wallNumber < lane4End) wall = wallMesh->CreateModel(420.0f, objFloor, -30.0f + (wallNumber - lane3End)*wallDiameter);
		else if (wallNumber < lane5End) wall = wallMesh->CreateModel(-20.5f, objFloor, -35.0f + (wallNumber - lane4End)*wallDiameter);
		minX = wall->GetX() - wallWidth;
		maxX = wall->GetX() + wallWidth;
		minZ = wall->GetZ() - wallLength;
		maxZ = wall->GetZ() + wallLength;
	}

	float GetMinX() { return minX; }
	float GetMaxX() { return maxX; }
	float GetMinZ() { return minZ; }
	float GetMaxZ() { return maxZ; }
};
// Airplanes class
const int numOfPlanes = 2;
class Airplane
{
private:
	const float changeDir = 90.0f;
	const float planeSpeed = 10.0f;
	IModel* plane;
	enum planeStates { Forwards, Left, Backwards, Right };
	int direction;
public:
	Airplane(IMesh* planeMesh, int planeNumber) // Constructor
	{
		if (planeNumber == 0)
		{
			plane = planeMesh->CreateModel(50.0f, 100.0f, 30.0f);
			direction = Forwards;
		}
		if (planeNumber == 1)
		{
			plane = planeMesh->CreateModel(300.0f, 100.0f, 400.0f);
			plane->RotateY(180.0f);
			direction = Backwards;
		}
	}

	void Update()
	{
		switch (direction)
		{
		case Forwards:
			if (plane->GetZ() < 500.0f) plane->MoveZ(planeSpeed * frameRate);
			else
			{
				plane->RotateY(changeDir);
				direction = Right;
			}
			break;
		case Right:
			if (plane->GetX() < 300.0f) plane->MoveX(planeSpeed * frameRate);
			else
			{
				plane->RotateY(changeDir);
				direction = Backwards;
			}
			break;
		case Backwards:
			if (plane->GetZ() > 30.0f) plane->MoveZ(-planeSpeed * frameRate);
			else
			{
				plane->RotateY(changeDir);
				direction = Left;
			}
			break;
		case Left:
			if (plane->GetX() > 50.0f) plane->MoveX(-planeSpeed * frameRate);
			else
			{
				plane->ResetOrientation();
				direction = Forwards;
			}
			break;
		}
	}

	~Airplane() // Destructor
	{

	}
};
Airplane* plane[numOfPlanes];
// Checkpoint class
class CheckPoint
{
private:
	IModel* checkPoint;
	int lastCP;
	const float cpLength = 9.86159f;
	const float cpWidth = 1.28539f;
	float collideLeftMinX, collideLeftMaxX, collideLeftMinZ, collideLeftMaxZ;
	float collideRightMinX, collideRightMaxX, collideRightMinZ, collideRightMaxZ;
	float goThroughMinX, goThroughMaxX;
	float goThroughMinZ, goThroughMaxZ;
public:
	CheckPoint(IMesh* checkPointMesh, int checkPointNumber)
	{
		if (checkPointNumber == 0)      checkPoint = checkPointMesh->CreateModel(2.0f, objFloor, 60.0f);
		else if (checkPointNumber == 1) checkPoint = checkPointMesh->CreateModel(2.0f, objFloor, 300.0f);
		else if (checkPointNumber == 2) checkPoint = checkPointMesh->CreateModel(400.0f, objFloor, 450.0f);
		else if (checkPointNumber == 3) checkPoint = checkPointMesh->CreateModel(400.0f, objFloor, 40.0f);
		checkPoint->ScaleY(2.0f);
		goThroughMinX = checkPoint->GetX() - cpLength;
		goThroughMaxX = checkPoint->GetX() + cpLength;
		goThroughMinZ = checkPoint->GetZ() - cpWidth;
		goThroughMaxZ = checkPoint->GetZ() + cpWidth;

		collideLeftMinX = checkPoint->GetX() - 12.0f;
		collideLeftMaxX = checkPoint->GetX() - 7.0f;
		collideLeftMinZ = checkPoint->GetZ() - 10.0f;
		collideLeftMaxZ = checkPoint->GetZ() + 5.0f;

		collideRightMinX = checkPoint->GetX() + 7.0f;
		collideRightMaxX = checkPoint->GetX() + 12.0f;
		collideRightMinZ = checkPoint->GetZ() - 10.0f;
		collideRightMaxZ = checkPoint->GetZ() + 5.0f;
	}

	float GetX() { return checkPoint->GetX(); }
	float GetY() { return checkPoint->GetY(); }
	float GetZ() { return checkPoint->GetZ(); }

	void CarThrough(float carX, float carZ, int currentCP, int car, IFont* gameFont)
	{
		if ((carX >= goThroughMinX && carX <= goThroughMaxX && carZ >= goThroughMinZ && carZ <= goThroughMaxZ) &&
			hoverCars[car]->GetGameState() == 4 && currentCP == 0 && hoverCars[car]->GetLap() < 4)
		{
			hoverCars[car]->NextLap();       // Increase lap
			hoverCars[car]->SetGameState(0); // Reset game state
		}
		if (currentCP == 0 && (hoverCars[playerCar]->GetLap() == 4))
		{	// Game over
			tempTime = hoverCars[playerCar]->GetTime(); // Save players time
		}
		else if (carX >= goThroughMinX && carX <= goThroughMaxX && carZ >= goThroughMinZ && carZ <= goThroughMaxZ && hoverCars[car]->GetGameState() == currentCP)
		{
			cross[currentCP]->DisplayCross();
			if (car == playerCar) hoverCars[playerCar]->DisplayTimerStart();
			hoverCars[car]->NextGameState();
		}
	}
	// Getting left post box
	float GetCollideLeftMinX() { return collideLeftMinX; }
	float GetCollideLeftMaxX() { return collideLeftMaxX; }
	float GetCollideLeftMinZ() { return collideLeftMinZ; }
	float GetCollideLeftMaxZ() { return collideLeftMaxZ; }
	// Getting right post box
	float GetCollideRightMinX() { return collideRightMinX; }
	float GetCollideRightMaxX() { return collideRightMaxX; }
	float GetCollideRightMinZ() { return collideRightMinZ; }
	float GetCollideRightMaxZ() { return collideRightMaxZ; }

	float GetGoThroughMinX() { return goThroughMinX; }
	float GetGoThroughMaxX() { return goThroughMaxX; }
	float GetGoThroughMinZ() { return goThroughMinZ; }
	float GetGoThroughMaxZ() { return goThroughMaxZ; }

};
/* Class variables */
FPS displayFPS;				// Variable for fps class
CheckPoint* carCheckPoint[numOfCheckPoints];
Wall* trackWalls[numOfWalls];
WaterTank* trackTanks[numOfTanks];
Isle* trackIsles[numOfIsles];
Particle* PCpiece[numOfParticles];
Particle* C1piece[numOfParticles];
Particle* C2piece[numOfParticles];
Particle* C3piece[numOfParticles];
Particle* C4piece[numOfParticles];
Particle* C5piece[numOfParticles];
/* Function declarations */
void QuitGame(I3DEngine* myEngine);
int GameDetails(I3DEngine* myEngine, ICamera* myCamera, int gameState, IFont* gameFont);
int LoadMainMenu(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* gameFont, IFont* titleFont);
int AboutGame(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* gameFont, IFont* titleFont, IFont* aboutFont);
int GameSettings(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* gameFont, IFont* titleFont);
int EnterNewKey(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* gameFont, IFont* titleFont);
int GameScores(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* gameFont, IFont* titleFont);
int ButtonHandler(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* gameFont);
ISprite* LeaveGame(I3DEngine* myEngine, int gameState);

/* Main setup */
void main()
{
	system("Title Adam Rushton - Hover Car Racing Console"); // Set TL Engine Title

															 // Load each game sound
	LoadSound("Sound/backgroundnoise.wav"); // (delay in sound file but ok)
	currentSound.setBuffer(buffer);
	currentSound.setVolume(100.0f);     // Full volume (Range: 0.0f to 100.0f)
	currentSound.setPitch(1.0f);
	currentSound.setLoop(true);         // Loop through
	currentSound.setPosition(soundPos);

	currentSound.play();

	// Listener
	sf::Listener::setGlobalVolume(100.0f); // 0 to 100
	sf::Listener::setPosition(listenerPos);
	sf::Listener::setDirection(listenerForward);
	sf::Listener::setUpVector(listenerUp);


	userSettings.LoadSettings(); // Attempt to load the settings file
	fastestTimes.LoadHS();       // Load highscores
								 //while (userName.empty() || userName.length() > maxNumOfChars || onlyWhiteSpace)
								 //{
								 //	cout << "Please enter a username (between 1 and 10 characters): ";
								 //	getline(cin, userName);
								 //	(userName.find_first_not_of(' ') == string::npos) ? onlyWhiteSpace = true : onlyWhiteSpace = false;
								 //}
	fastestTimes.userName = "Adam";

	// Create a 3D engine (using TLX engine here)
	I3DEngine* myEngine = New3DEngine(kTLX);
	(userSettings.fullScreen == false) ? myEngine->StartWindowed() : myEngine->StartFullscreen();	// Load in full screen or windowed
	(userSettings.fullScreen == false) ? engineLength = 720 : engineLength = 1024;
	myEngine->SetWindowCaption("Adam Rushton - Hover Car Racing");						// Set console window title
	myEngine->AddMediaFolder("./media");
	myEngine->AddMediaFolder("./media/Assignment 3 Extra Models");
	// Add media folder
	IFont* gameFont = myEngine->LoadFont("Monotype Corsiva", gameFontSize);
	IFont* titleFont = myEngine->LoadFont("Monotype Corsiva", titleFontSize);
	IFont* aboutFont = myEngine->LoadFont("Monotype Corsiva", aboutFontSize);
	backdrop = myEngine->CreateSprite(mainAboutBG); // Load main menu background

													// Ground
	IMesh* groundMesh = myEngine->LoadMesh("ground.x");
	IModel* ground = groundMesh->CreateModel(0.0f, 0.0f, 0.0f);
	// Skybox
	IMesh* skyboxMesh = myEngine->LoadMesh("skybox 07.x");
	IModel* skybox = skyboxMesh->CreateModel(0.0f, -960.0f, 0.0f);
	// Lamp.x could be used as a gun, if i have sound when it says go, the gun could rotate and make a shoot sound
	// Small Garage
	IMesh* garageMesh = myEngine->LoadMesh("tribune2.x");
	IModel* garage = garageMesh->CreateModel(100.0f, objFloor, 40.0f);
	garage->RotateY(90.0f);
	// Grid like floor
	const int numberOfGrids = 10;
	IMesh* gridFloorMesh = myEngine->LoadMesh("walkway.x");
	IModel* gridFloor[numberOfGrids];
	for (int i = 0; i < numberOfGrids; i++)
	{
		gridFloor[i] = gridFloorMesh->CreateModel(0.0f + (i*47.0f), 1.0f, 250.0f);
		gridFloor[i]->ScaleZ(210.0f);
		gridFloor[i]->ScaleX(15.0f);
	}

	// Objects in the middle
	IMesh* planeMesh = myEngine->LoadMesh("Interstellar.x");
	// Planes
	for (int pl = 0; pl < numOfPlanes; pl++)
	{
		plane[pl] = new Airplane(planeMesh, pl);
	}
	// Checkpoints
	IMesh* checkPointMesh = myEngine->LoadMesh("checkpoint.x");
	IMesh* crossMesh = myEngine->LoadMesh("cross.x");
	for (int i = 0; i < numOfCheckPoints; i++)
	{
		carCheckPoint[i] = new CheckPoint(checkPointMesh, i);
		cross[i] = new Cross(myEngine, crossMesh, i);
	}
	// Minimap
	miniMap = new MiniMap(myEngine);

	// Camera
	ICamera* myCamera = myEngine->CreateCamera(kManual, 0.0f, 20.0f, -60.0f); // Create a camera

	// Isles
	IMesh* isleMesh = myEngine->LoadMesh("islestraight.x");
	for (int i = 0; i < numOfIsles; i++)
	{
		trackIsles[i] = new Isle(isleMesh, i);
	}
	// Water tanks
	IMesh* tankMesh = myEngine->LoadMesh("TankSmall1.x");
	for (int i = 0; i < numOfTanks; i++)
	{
		trackTanks[i] = new WaterTank(tankMesh, i);
	}
	// Walls
	IMesh* wallMesh = myEngine->LoadMesh("wall.x");
	for (int i = 0; i < numOfWalls; i++)
	{
		trackWalls[i] = new Wall(wallMesh, i);
	}
	// Bombs
	IMesh* bombMesh = myEngine->LoadMesh("flare.x");
	for (int i = 0; i < numOfBombs; i++)
	{
		trackBombs[i] = new Bomb(bombMesh, i);
	}
	// Turret system
	IMesh* turretMesh = myEngine->LoadMesh("turret.x");
	IMesh* bulletMesh = myEngine->LoadMesh("bullet.x");
	for (int t = 0; t < numOfTurrets; t++)
	{
		turret[t] = new TurretSystem(turretMesh, bulletMesh, t);
	}
	// Crate system
	IMesh* slowMesh = myEngine->LoadMesh("crate_1.x");
	IMesh* poisonMesh = myEngine->LoadMesh("crate_2.x");
	IMesh* spinMesh = myEngine->LoadMesh("crate_3.x");
	IMesh* healthMesh = myEngine->LoadMesh("crate_4.x");
	IMesh* unstoppableMesh = myEngine->LoadMesh("crate_5.x");

	for (int crate = 0; crate < numOfCrates; crate++)
	{
		healthCrate[crate] = new HealthCrate(healthMesh, crate);
		slowCrate[crate] = new SlowCrate(slowMesh, crate);
		poisonCrate[crate] = new PoisonCrate(poisonMesh, crate);
	}
	// Hover car details
	countdownFont = myEngine->LoadFont("Monotype Corsiva", 150);
	IMesh* hoverCarMesh = myEngine->LoadMesh("race2.x");
	IMesh* dummyMesh = myEngine->LoadMesh("dummy.x");
	for (int i = 0; i < carAmount; i++)
	{
		hoverCars[i] = new HoverCar(hoverCarMesh, dummyMesh, myCamera, i);
		if (i > 0) hoverCars[i]->SetComputerSpeeds();
	}
	rotationDegrees = 0.2f / 10.0f;
	// Waypoint details
	for (int i = 0; i < numOfWayPoints; i++)
	{
		lane1WayPoint[i] = new WayPoint(dummyMesh, i, false);
		lane2WayPoint[i] = new WayPoint(dummyMesh, i, true);
	}
	// Fountain
	IMesh* particleMesh = myEngine->LoadMesh("particle.x");
	userSettings.GetFumeColourNames();
	for (int i = 0; i < numOfParticles; i++)
	{
		PCpiece[i] = new Particle(particleMesh, i, hoverCars, 0);
		C1piece[i] = new Particle(particleMesh, i, hoverCars, 1);
		C2piece[i] = new Particle(particleMesh, i, hoverCars, 2);
		C3piece[i] = new Particle(particleMesh, i, hoverCars, 3);
		C4piece[i] = new Particle(particleMesh, i, hoverCars, 4);
		C5piece[i] = new Particle(particleMesh, i, hoverCars, 5);
		PCpiece[i]->SetFumeColour(); // Set player car fume colour
		C1piece[i]->SetSkin("007_tlxadd.tga");
		C2piece[i]->SetSkin("008_tlxadd.tga");
		C3piece[i]->SetSkin("009_tlxadd.tga");
		C4piece[i]->SetSkin("010_tlxadd.tga");
		C5piece[i]->SetSkin("011_tlxadd.tga");
	}

	for (int i = 0; i > carAmount; i++) carState[i] = Hovering;	// Start all cars as hovering

	int gameState = MainMenu; // Game state starts as main menu

							  // Main Menu Buttons X and Y values
	textPosX[StartButton] = 615;
	textPosY[StartButton] = 275;
	textPosX[HighscoresButton] = 370;
	textPosY[HighscoresButton] = 405;
	textPosX[SettingsButton] = 770;
	textPosY[SettingsButton] = 405;
	textPosX[AboutButton] = 300;
	textPosY[AboutButton] = 550;
	textPosX[QuitButton] = 890;
	textPosY[QuitButton] = 545;
	textPosX[BackButton] = 890;
	textPosY[BackButton] = 545;

	textPosX[CancelButton] = 1040; // Cancel button X
	textPosY[CancelButton] = 545;  // Cancel button Y
	widthOfText[CancelButton] = gameFont->MeasureTextWidth(cancelName);   // Measure width of cancel button
	heightOfText[CancelButton] = gameFont->MeasureTextHeight(cancelName); // Measure height of cancel button
	textPosX[ResetButton] = 1040; // Reset button X
	textPosY[ResetButton] = 545;  // Reset button Y
	widthOfText[ResetButton] = gameFont->MeasureTextWidth(resetName);   // Measure width of reset button
	heightOfText[ResetButton] = gameFont->MeasureTextHeight(resetName); // Measure height of reset button

	for (int i = Highscore1; i < Highscore5 + 1; i++) // Measure all the highscores lines
	{
		widthOfText[i] = gameFont->MeasureTextWidth(fastestTimes.posNameScoreText.str());
		heightOfText[i] = gameFont->MeasureTextHeight(fastestTimes.posNameScoreText.str());
	}
	for (int i = 0; i < mainMenuOptions; i++) // Measure all the main menu options 
	{
		widthOfText[i] = gameFont->MeasureTextWidth(mainMenuList[i]);
		heightOfText[i] = gameFont->MeasureTextHeight(mainMenuList[i]);
	}
	widthOfText[BackButton] = gameFont->MeasureTextWidth(backName);   // Back button measuring width
	heightOfText[BackButton] = gameFont->MeasureTextHeight(backName); // Back button measuring height

	userSettings.Initialise(gameFont); // Measure and draw settings

	for (int i = 0; i < optionsAmt; i++) // Bounding box around all of the options
	{
		optionMinX[i] = textPosX[i];
		optionMaxX[i] = textPosX[i] + widthOfText[i];
		optionMinY[i] = textPosY[i];
		optionMaxY[i] = textPosY[i] + heightOfText[i];
	}

	while (myEngine->IsRunning())
	{
		frameRate = myEngine->Timer();
		// Draw the scene
		myEngine->DrawScene();
		/**** Update your scene each frame here ****/
		int mouseX = myEngine->GetMouseX(); // Record mouse X
		int mouseY = myEngine->GetMouseY(); // Record mouse Y

		(inGame) ? fastestTimes.userNameInfo = "Racing as: " : fastestTimes.userNameInfo = "Username: "; // Text before username
		userNameText << fastestTimes.userNameInfo << fastestTimes.userName;
		gameFont->Draw(userNameText.str(), 0, 0, defaultColour); // Draw text before username
		userNameText.str("");

		if (gameState != Start)
		{
			// If mouse is in the box of one  of the menu options, Switch to highlight colour. 
			// Otherwise show default colour
			for (int i = 0; i < optionsAmt; i++) // Hovering for all options
			{
				if (mouseX >= optionMinX[i] && mouseX <= optionMaxX[i] &&
					mouseY >= optionMinY[i] && mouseY <= optionMaxY[i]) toggledColour[i] = hoverColour;
				else													toggledColour[i] = defaultColour;
			}
		}
		switch (gameState)
		{
		case MainMenu: // Load main menu 
			gameState = LoadMainMenu(myEngine, gameState, mouseX, mouseY, gameFont, titleFont);
			break;

		case Start: // Load the game
			GameDetails(myEngine, myCamera, gameState, gameFont);
			break;

		case Highscores: // Load the highscores
			gameState = GameScores(myEngine, gameState, mouseX, mouseY, gameFont, titleFont);
			break;

		case About: // Load the about page
			gameState = AboutGame(myEngine, gameState, mouseX, mouseY, gameFont, titleFont, aboutFont);
			break;

		case Settings: // Load the settings
			gameState = GameSettings(myEngine, gameState, mouseX, mouseY, gameFont, titleFont);
			break;

		case BindNewKey: // Load the key bind modifier page
			gameState = EnterNewKey(myEngine, gameState, mouseX, mouseY, gameFont, titleFont);
			break;

		case Quit: // Quit the game
			QuitGame(myEngine);
			break;
		}
		if (gameState == Start && gameOver)
		{
			gameState = Highscores;					   // Load the highscores
			myEngine->RemoveSprite(uiBackdrop);		   // Game game backdrop
			myEngine->RemoveSprite(uiPosition);
			backdrop = LeaveGame(myEngine, gameState); // Leave game code
			gamePaused = false;						   // No longer a game to be paused
		}
		if (myEngine->KeyHit(userSettings.quitKey))
		{
			if (inGame)
			{
				myEngine->RemoveSprite(uiBackdrop);
				myEngine->RemoveSprite(uiPosition);
				miniMap->Hide();
				LoadSound("sound/backgroundnoise.wav");
				currentSound.setLoop(true);
				currentSound.play();
				if (pauseCreated)
				{
					myEngine->RemoveSprite(pause);
					pauseCreated = false;
				}
				backdrop = LeaveGame(myEngine, gameState); // Leave game code	
				gameState = MainMenu;					   // Load main menu
			}
			else
			{
				fastestTimes.SaveHS();   // Save the highscores
				userSettings.SaveFile(); // Save the settings
				QuitGame(myEngine);	     // Quit the game
			}
		}
		if (gameOver && myEngine->KeyHit(userSettings.restartGame))
		{
			fastestTimes.SaveHS();			  // if in highscores and press back button Save highscores to file	
			inGame = true;					  // Now back ingame
			miniMap->Show();				  // Show the minimap
			gameOver = false;				  // No longer game over
			for (int car = 0; car < carAmount; car++) hoverCars[car]->Restart(car);
			myEngine->RemoveSprite(backdrop); // Remove highscores backdrop
			uiBackdrop = myEngine->CreateSprite(gameBackdrop, 0.0f, engineLength - 75.0f);
			uiPosition = myEngine->CreateSprite(gamePosition, 1013.0f, engineLength - 278.0f);
			gameState = Start;				  // Return to start game state
		}
	}
	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}

// Game text on screen
void GameTextOnScreen(IFont* gameFont)
{	// Displaying all game text on the screen
	for (int car = 0; car < carAmount; car++)
	{
		if (hoverCars[car]->GetHP() <= 0) hoverCars[car]->DyingStage(car);
		else state[car] = "[A]";
	}
	hoverCars[playerCar]->DisplayHitpoints(gameFont);
	hoverCars[playerCar]->DisplaySpeed(gameFont);
	hoverCars[playerCar]->DisplayStopWatch(gameFont);
	hoverCars[playerCar]->DisplayLap(gameFont);
	hoverCars[playerCar]->DrawState(gameFont);

	// Add all the highscores
	for (int i = 0; i < carAmount; i++) positions[i] = { hoverCars[i]->GetNames(), currentDist[i], state[i] };
	// Create a vector filled with AddTime Entry objects.
	vector<AddPosition> positionboard;
	//Add our Entries to the vector
	for (int i = 0; i < carAmount; i++) positionboard.push_back(positions[i]);
	// Sort the order
	sort(positionboard.begin(), positionboard.end(), ByPosition()); // Sort them now the last is the smallest
																	// Display them
	winnerName = positionboard[0].name;
	if (winnerName == hoverCars[playerCar]->GetNames()) winnerTime = hoverCars[playerCar]->GetTime();
	else if (winnerName == hoverCars[1]->GetNames()) winnerTime = hoverCars[1]->GetTime();
	else if (winnerName == hoverCars[2]->GetNames()) winnerTime = hoverCars[2]->GetTime();
	else if (winnerName == hoverCars[3]->GetNames()) winnerTime = hoverCars[3]->GetTime();
	else if (winnerName == hoverCars[4]->GetNames()) winnerTime = hoverCars[4]->GetTime();
	else if (winnerName == hoverCars[5]->GetNames()) winnerTime = hoverCars[5]->GetTime();


	for (int car = 0; car < carAmount; car++) currentposition << car + 1 << ". " << positionboard[car].state << " " << positionboard[car].name << "\n";
	gameFont->Draw(currentposition.str(), 1020, engineLength - 278, defaultColour);
	currentposition.str("");
}
// FPS Counter
void FPSCounter(IFont* gameFont)
{
	if (userSettings.fpsCounter) displayFPS.Run(gameFont); // If enabled calculate fps
}

ISprite* LeaveGame(I3DEngine* myEngine, int gameState)
{	// Leaving the game sprite creation
	(gameState == Highscores) ? backdrop = myEngine->CreateSprite(highscoresBG) : backdrop = myEngine->CreateSprite(mainAboutBG);
	gamePaused = true; // Pause the game
	inGame = false;    // No longer in a game
	return backdrop;
}

int LoadMainMenu(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* gameFont, IFont* titleFont)
{
	titleFont->Draw("Main Menu", titleTextX, titleTextY, titleColour); // Draw main menu text

	for (int i = 0; i < mainMenuOptions; i++) 	// Write out all the main menus
	{
		mainMenuText[i] << mainMenuList[i];
		gameFont->Draw(mainMenuText[i].str(), textPosX[i], textPosY[i], toggledColour[i]);
		mainMenuText[i].str("");
	}
	// Clicking Main Menu Buttons
	gameState = ButtonHandler(myEngine, gameState, mouseX, mouseY, gameFont);
	return gameState;
}

int GameScores(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* gameFont, IFont* titleFont)
{
	titleFont->Draw("Highscores", titleTextX, titleTextY, titleColour);
	if (gameOver && carState[playerCar] != DestroyedCar)
	{
		gameOverTime << "Winner: " << winnerName << "\nTime: " << winnerTime << "\n\nRace Finished\n\nYour time: " << hoverCars[playerCar]->GetTime() << "\n\n\nPress R \nto play again.";
	}
	else if (gameOver && carState[playerCar] == DestroyedCar)
	{
		gameOverTime << "Your vehicle \nwas destroyed!\nPress R \nto play again.";
	}
	gameFont->Draw(gameOverTime.str(), 0, 250, kGreen);
	gameOverTime.str("");
	fastestTimes.UpdateDisplayTimes(gameFont); // Keep display scores

	gameState = ButtonHandler(myEngine, gameState, mouseX, mouseY, gameFont);
	return gameState;
}

int GameSettings(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* gameFont, IFont* titleFont)
{
	titleFont->Draw("Settings", titleTextX, titleTextY, titleColour);
	userSettings.changingKey = false;
	// Drawing the menus
	userSettings.Draw(gameFont);

	fullScreenStateText << boolalpha << userSettings.fullScreen;
	gameFont->Draw(fullScreenStateText.str(), textPosX[6] + 225, textPosY[6], toggledColour[6]);
	fullScreenStateText.str("");

	fpsToggleStateText << boolalpha << userSettings.fpsCounter;
	gameFont->Draw(fpsToggleStateText.str(), textPosX[7] + 225, textPosY[7], toggledColour[7]);
	fpsToggleStateText.str("");

	carColourText << userSettings.carColours[userSettings.carColour];
	gameFont->Draw(carColourText.str(), textPosX[25] + 225, textPosY[25], toggledColour[25]);
	carColourText.str("");

	carFumeText << userSettings.fumeColours[userSettings.fumeColour];
	gameFont->Draw(carFumeText.str(), textPosX[26] + 225, textPosY[26], toggledColour[26]);
	carFumeText.str("");

	gameState = ButtonHandler(myEngine, gameState, mouseX, mouseY, gameFont);
	return gameState;
}

int EnterNewKey(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* gameFont, IFont* titleFont)
{
	titleFont->Draw("Enter a New Key", titleTextX, titleTextY, titleColour);

	if (keyInUse) gameFont->Draw("Key in use", textPosX[7] + 150, textPosY[7] - 100, kRed);
	else if (!keyInUse && userSettings.tempCurrentKeyName != "") gameFont->Draw("Key available", textPosX[7] + 150, textPosY[7] - 100, kGreen);
	// Change key bindings or stay the same
	currentKeyText << "New key: " << userSettings.notInUseKeyName;
	gameFont->Draw(currentKeyText.str(), textPosX[7] + 150, textPosY[7], defaultColour);
	currentKeyText.str("");

	userSettings.SetNewKeys(myEngine, gameState); // Allow user to set new keys
	gameState = ButtonHandler(myEngine, gameState, mouseX, mouseY, gameFont);
	return gameState;
}

int AboutGame(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* gameFont, IFont* titleFont, IFont* aboutFont)
{
	titleFont->Draw("About", titleTextX, titleTextY, titleColour);
	aboutFont->Draw("3D Hover Car Racing - 3rd Games Assignment", textLine1X, textLine1Y, defaultColour);
	aboutFont->Draw("Programmed by \n Adam Rushton", textLine2X, textLine2Y, defaultColour);
	aboutFont->Draw("First Year Software\nEngineering Student UCLan", textLine3X, textLine3Y, defaultColour);
	aboutFont->Draw("Made with TL-Engine using C++", textLine4X, textLine4Y, defaultColour);
	gameState = ButtonHandler(myEngine, gameState, mouseX, mouseY, gameFont);
	return gameState;
}

int ButtonHandler(I3DEngine* myEngine, int gameState, int mouseX, int mouseY, IFont* gameFont)
{
	(gameState == BindNewKey) ? userSettings.changingKey = true : userSettings.changingKey = false;
	(userSettings.changingKey) ? backName = "Save" : backName = "Back";

	// Draw buttons 
	if (gameState == Highscores || gameState == Settings) gameFont->Draw(resetName, textPosX[ResetButton], textPosY[ResetButton], toggledColour[ResetButton]);
	if (gameState != MainMenu) gameFont->Draw(backName, textPosX[BackButton], textPosY[BackButton], toggledColour[BackButton]);
	if (gameState == BindNewKey) gameFont->Draw(cancelName, textPosX[CancelButton], textPosY[CancelButton], toggledColour[CancelButton]);

	if (myEngine->KeyHit(userSettings.buttonClicker)) // When you click the mouse button
	{
		// Main Menu Buttons Mouse click whilst hovering option 1
		if (gameState == MainMenu)
		{
			if (mouseX >= optionMinX[StartButton] && mouseX <= optionMaxX[StartButton] &&
				mouseY >= optionMinY[StartButton] && mouseY <= optionMaxY[StartButton])
			{
				uiBackdrop = myEngine->CreateSprite(gameBackdrop, 0.0f, engineLength - 75.0f);
				uiPosition = myEngine->CreateSprite(gamePosition, 1013.0f, engineLength - 278.0f);
				inGame = true;
				currentSound.stop();
				miniMap->Show();	  // Show the map
				if (!gamePaused && pauseCreated)
				{
					myEngine->RemoveSprite(pause);
					pauseCreated = false;
				}
				else if (gamePaused && !pauseCreated)
				{
					pause = myEngine->CreateSprite(pauseName, pauseSpriteX, pauseSpriteY);
					pauseCreated = true;
				}
				myEngine->RemoveSprite(backdrop); // Remove background if starting game
				return Start;
			}
			// Mouse click whilst hovering option 2
			if (mouseX >= optionMinX[HighscoresButton] && mouseX <= optionMaxX[HighscoresButton] &&
				mouseY >= optionMinY[HighscoresButton] && mouseY <= optionMaxY[HighscoresButton])
			{
				myEngine->RemoveSprite(backdrop);                // Remove main menu background
				backdrop = myEngine->CreateSprite(highscoresBG); // Create highscores background
				fastestTimes.LoadHS();							 // Open highscores file									 
				return Highscores;								 // Go to highscores screen
			}
			// Mouse click whilst hovering option 3
			if (mouseX >= optionMinX[SettingsButton] && mouseX <= optionMaxX[SettingsButton] &&
				mouseY >= optionMinY[SettingsButton] && mouseY <= optionMaxY[SettingsButton])
			{
				myEngine->RemoveSprite(backdrop);              // Remove main menu background
				backdrop = myEngine->CreateSprite(settingsBG); // Create settings background
				return Settings;
			}
			// Mouse click whilst hovering option 4
			if (mouseX >= optionMinX[AboutButton] && mouseX <= optionMaxX[AboutButton] &&
				mouseY >= optionMinY[AboutButton] && mouseY <= optionMaxY[AboutButton])	return About;
			// Mouse click whilst hovering option 5
			if (mouseX >= optionMinX[QuitButton] && mouseX <= optionMaxX[QuitButton] &&
				mouseY >= optionMinY[QuitButton] && mouseY <= optionMaxY[QuitButton]) return Quit;
		}
		if (gameState == Highscores || gameState == Settings) // Check the gamestate
		{
			if (mouseX >= optionMinX[ResetButton] && mouseX <= optionMaxX[ResetButton] &&
				mouseY >= optionMinY[ResetButton] && mouseY <= optionMaxY[ResetButton]) // Click reset button
			{
				(gameState == Highscores) ? fastestTimes.ResetHS() : userSettings.ResetKeys(); // In HS? - ResetHS. Otherwise they are in settings, so reset settings.
				(gameState == Highscores) ? fastestTimes.HighscoresResetMSG() : userSettings.SettingsResetMSG(); // Display reset message on console
			}
		}
		if (gameState == BindNewKey)
		{
			if (mouseX >= optionMinX[CancelButton] && mouseX <= optionMaxX[CancelButton] &&
				mouseY >= optionMinY[CancelButton] && mouseY <= optionMaxY[CancelButton]) // Click cancel button
			{
				userSettings.ResetKeyChanger(); // Reset temp key name, not in use key name and change state of changing key 
				gameState = Settings;
			}
		}
		if (mouseX >= optionMinX[BackButton] && mouseX <= optionMaxX[BackButton] &&
			mouseY >= optionMinY[BackButton] && mouseY <= optionMaxY[BackButton]) // Click back button
		{
			if (gameState == Highscores) fastestTimes.SaveHS();	// if in highscores and press back button Save highscores to file		    
			if (gameOver)
			{
				for (int car = 0; car < carAmount; car++) hoverCars[car]->Restart(car);
				gameOver = false;
			}
			if (gameState == Highscores || gameState == Settings)
			{
				myEngine->RemoveSprite(backdrop); // Remove background
				backdrop = myEngine->CreateSprite(mainAboutBG); // Create main menu background
			}
			(userSettings.changingKey == false) ? gameState = MainMenu : gameState = Settings;
			if (userSettings.changingKey)
			{
				userSettings.UpdateKeys();      // Update key numbers
				userSettings.FindAllKeyNames(); // Update key names
				userSettings.SaveFile();		// Save updated keys
				userSettings.ResetKeyChanger(); // Reset temp key name, not in use key name and change state of changing key  
			}
		}
		if (gameState == Settings) // When in the settings
		{
			if (mouseX >= optionMinX[FullScreenKey] && mouseX <= optionMaxX[FullScreenKey] &&
				mouseY >= optionMinY[FullScreenKey] && mouseY <= optionMaxY[FullScreenKey])
			{										 // Full screen button hit
				userSettings.fullScreen ^= 1;		 // Switch between the two states
				userSettings.SaveFile();			 // Save keys 
			}
			if (mouseX >= optionMinX[FPSCounterKey] && mouseX <= optionMaxX[FPSCounterKey] &&
				mouseY >= optionMinY[FPSCounterKey] && mouseY <= optionMaxY[FPSCounterKey])
			{										 // FPS Counter button hit
				userSettings.fpsCounter ^= 1;		 // Switch between the two states	
				userSettings.SaveFile();		     // Save keys
			}
			if (mouseX >= optionMinX[PauseKey] && mouseX <= optionMaxX[PauseKey] &&
				mouseY >= optionMinY[PauseKey] && mouseY <= optionMaxY[PauseKey])
			{												 // Pause key button hit
				userSettings.keyChangeName = userSettings.leftSettingsList[PauseText]; // Changing the pause game key
				return BindNewKey;
			}
			if (mouseX >= optionMinX[QuitKey] && mouseX <= optionMaxX[QuitKey] &&
				mouseY >= optionMinY[QuitKey] && mouseY <= optionMaxY[QuitKey])
			{												// Back/Quit key
				userSettings.keyChangeName = userSettings.leftSettingsList[QuitText]; // Changing the back/quit game key
				return BindNewKey;
			}
			if (mouseX >= optionMinX[ResetCameraKey] && mouseX <= optionMaxX[ResetCameraKey] &&
				mouseY >= optionMinY[ResetCameraKey] && mouseY <= optionMaxY[ResetCameraKey])
			{													   // Reset Camera Key
				userSettings.keyChangeName = userSettings.leftSettingsList[ResetCameraText]; // Changing the reset camera key
				return BindNewKey;
			}
			if (mouseX >= optionMinX[chaseCamKey] && mouseX <= optionMaxX[chaseCamKey] &&
				mouseY >= optionMinY[chaseCamKey] && mouseY <= optionMaxY[chaseCamKey])
			{												  // Chase cam key
				userSettings.keyChangeName = userSettings.leftSettingsList[chaseCamText]; // Changing the top view key
				return BindNewKey;
			}
			if (mouseX >= optionMinX[prepareRaceKey] && mouseX <= optionMaxX[prepareRaceKey] &&
				mouseY >= optionMinY[prepareRaceKey] && mouseY <= optionMaxY[prepareRaceKey])
			{														 // Restart Level Key
				userSettings.keyChangeName = userSettings.rightSettingsList[PrepareRaceText]; // Changing the restart level key
				return BindNewKey;
			}
			if (mouseX >= optionMinX[hoverCarForwardsKey] && mouseX <= optionMaxX[hoverCarForwardsKey] &&
				mouseY >= optionMinY[hoverCarForwardsKey] && mouseY <= optionMaxY[hoverCarForwardsKey])
			{												   // hoverCar Up Key
				userSettings.keyChangeName = userSettings.rightSettingsList[HoverCarUpText]; // Changing the hoverCar up key
				return BindNewKey;
			}
			if (mouseX >= optionMinX[hoverCarBackwardsKey] && mouseX <= optionMaxX[hoverCarBackwardsKey] &&
				mouseY >= optionMinY[hoverCarBackwardsKey] && mouseY <= optionMaxY[hoverCarBackwardsKey])
			{													 // hoverCar Down Key
				userSettings.keyChangeName = userSettings.rightSettingsList[HoverCarDownText]; // Changing the hoverCar down key
				return BindNewKey;
			}
			if (mouseX >= optionMinX[hoverCarLeftKey] && mouseX <= optionMaxX[hoverCarLeftKey] &&
				mouseY >= optionMinY[hoverCarLeftKey] && mouseY <= optionMaxY[hoverCarLeftKey])
			{													 // hoverCar Left key
				userSettings.keyChangeName = userSettings.rightSettingsList[HoverCarLeftText]; // Changing the hoverCar left key
				return BindNewKey;
			}
			if (mouseX >= optionMinX[hoverCarRightKey] && mouseX <= optionMaxX[hoverCarRightKey] &&
				mouseY >= optionMinY[hoverCarRightKey] && mouseY <= optionMaxY[hoverCarRightKey])
			{													  // hoverCar Right key
				userSettings.keyChangeName = userSettings.rightSettingsList[HoverCarRightText]; // Changing the hoverCar right key
				return BindNewKey;
			}
			if (mouseX >= optionMinX[DownViewKey] && mouseX <= optionMaxX[DownViewKey] &&
				mouseY >= optionMinY[DownViewKey] && mouseY <= optionMaxY[DownViewKey])
			{	// Boost
				userSettings.keyChangeName = userSettings.rightSettingsList[BoostText]; // Changing the top back key
				return BindNewKey;
			}
			if (mouseX >= optionMinX[25] && mouseX <= optionMaxX[25] &&
				mouseY >= optionMinY[25] && mouseY <= optionMaxY[25])
			{
				userSettings.CycleColours();
				hoverCars[playerCar]->SetCarColour();
				userSettings.SaveFile();
			}
			if (mouseX >= optionMinX[26] && mouseX <= optionMaxX[26] &&
				mouseY >= optionMinY[26] && mouseY <= optionMaxY[26])
			{
				userSettings.CycleFumes();
				for (int i = 0; i < numOfParticles; i++) PCpiece[i]->SetFumeColour();
				userSettings.SaveFile();
			}
		}
	}
	return gameState;
}

int GameDetails(I3DEngine* myEngine, ICamera* myCamera, int gameState, IFont* gameFont)
{
	GameTextOnScreen(gameFont); // Load all game text on the screen

	if (!gamePaused)
	{
		FPSCounter(gameFont); // Work out FPS
		int mouseMoveX = myEngine->GetMouseMovementX();
		if (carState[playerCar] == Hovering)
		{
			stringstream startText;
			myCamera->RotateY(mouseMoveX * 0.1f);
			startText << "Press space bar to begin!" << endl;
			gameFont->Draw(startText.str(), 500, 200, defaultColour);
			startText.str("");
		}
		for (int carNum = 0; carNum < carAmount; carNum++)
		{
			switch (carState[carNum])
			{
			case Hovering:
				if (currentSound.getStatus() == 0)
				{
					LoadSound("sound/test.wav");
					currentSound.play();
				}
				hoverCars[carNum]->Hover(gameFont);
				break;

			case WaitingForGo:
				if (currentSound.getLoop()) currentSound.stop();
				if (carNum == playerCar)
				{
					if (currentSound.getStatus() == 0)
					{
						LoadSound("sound/321soundeffect.wav");
						currentSound.setLoop(false);
						currentSound.play();
					}
					if (threeSecCD->CountingDown() == false) threeSecCD->StartCountdown();
					if (threeSecCD->GetTimeRemaining() > 0) hoverCars[playerCar]->DisplayCountdown();
					else
					{
						myCamera->ResetOrientation();
						for (int i = 0; i < carAmount; i++) carState[i] = Racing;
					}
				}
				break;

			case Racing:
				if (carNum == playerCar)
				{
					if (hoverCars[playerCar]->StartedRace() == false) hoverCars[playerCar]->DisplayCountdown();
					hoverCars[playerCar]->CameraShaking(myCamera);
				}
				hoverCars[carNum]->Update(carNum, gameFont);

				if (hoverCars[carNum]->GetGameState() <= 4)
				{
					// Calculate vector between the car and the next checkpoint
					if (hoverCars[carNum]->GetGameState() < 4)
					{
						posInList.moveVector[0] = hoverCars[carNum]->GetX() - carCheckPoint[hoverCars[carNum]->GetGameState()]->GetX();
						posInList.moveVector[1] = hoverCars[carNum]->GetY() - carCheckPoint[hoverCars[carNum]->GetGameState()]->GetY();
						posInList.moveVector[2] = hoverCars[carNum]->GetZ() - carCheckPoint[hoverCars[carNum]->GetGameState()]->GetZ();
					}
					else
					{
						posInList.moveVector[0] = hoverCars[carNum]->GetX() - carCheckPoint[0]->GetX();
						posInList.moveVector[1] = hoverCars[carNum]->GetY() - carCheckPoint[0]->GetY();
						posInList.moveVector[2] = hoverCars[carNum]->GetZ() - carCheckPoint[0]->GetZ();
					}
					// Square them
					distanceX = posInList.moveVector[0] * posInList.moveVector[0];
					distanceY = posInList.moveVector[1] * posInList.moveVector[1];
					distanceZ = posInList.moveVector[2] * posInList.moveVector[2];
					// Add the distance up
					currentDist[carNum] = distanceX + distanceY + distanceZ;
					// Square root it
					currentDist[carNum] = sqrt(currentDist[carNum]);
					currentDist[carNum] += -(hoverCars[carNum]->GetLap() * 10000) + -(hoverCars[carNum]->GetGameState() * 1000);
				}
				break;

			case DestroyedCar:
				// Stop the clock
				// Game needs to know that the cars are destroyed, so it doesn't try wait for them to finish
				break;
			}
		}
		// The game scene loops

		hoverCars[playerCar]->DisplayStageComplete(gameFont);

		// Display current game state for player car
		for (int i = 0; i < numOfCheckPoints; i++)
		{   // Check for collision between car and sides of the check point
			hoverCars[playerCar]->DummyCollisions(carCheckPoint[i]->GetCollideLeftMinX(), carCheckPoint[i]->GetCollideLeftMaxX(),
				carCheckPoint[i]->GetCollideLeftMinZ(), carCheckPoint[i]->GetCollideLeftMaxZ());
			hoverCars[playerCar]->DummyCollisions(carCheckPoint[i]->GetCollideRightMinX(), carCheckPoint[i]->GetCollideRightMaxX(),
				carCheckPoint[i]->GetCollideRightMinZ(), carCheckPoint[i]->GetCollideRightMaxZ());

			cross[i]->CheckTimeRemaining();

			for (int car = 0; car < carAmount; car++)
			{   // Check for the car going through the checkpoint
				carCheckPoint[i]->CarThrough(hoverCars[car]->GetX(), hoverCars[car]->GetZ(), i, car, gameFont);
				if (i == 0 && (hoverCars[car]->GetLap() == 4)) hoverCars[car]->Stop(car);

				if (carState[playerCar] == DestroyedCar)
				{
					gameOver = true;
					miniMap->Hide(); // Hide the minimap
				}
			}
		}

		for (int t = 0; t < numOfTurrets; t++)
		{	// Check collision between car and turret systems
			hoverCars[playerCar]->DummyCollisions(turret[t]->MinX(), turret[t]->MaxX(),
				turret[t]->MinZ(), turret[t]->MaxZ());
		}
		for (int tank = 0; tank < numOfTanks; tank++)
		{   // Check collision between car and water tanks
			hoverCars[playerCar]->DummyCollisions(trackTanks[tank]->GetMinX(), trackTanks[tank]->GetMaxX(),
				trackTanks[tank]->GetMinZ(), trackTanks[tank]->GetMaxZ());
		}

		for (int wall = 0; wall < numOfWalls; wall++)
		{   // Check collision between car and walls
			hoverCars[playerCar]->DummyCollisions(trackWalls[wall]->GetMinX(), trackWalls[wall]->GetMaxX(),
				trackWalls[wall]->GetMinZ(), trackWalls[wall]->GetMaxZ());
		}

		for (int isle = 0; isle < numOfIsles; isle++)
		{	// Check collision between car and isles
			hoverCars[playerCar]->DummyCollisions(trackIsles[isle]->GetMinX(), trackIsles[isle]->GetMaxX(),
				trackIsles[isle]->GetMinZ(), trackIsles[isle]->GetMaxZ());
		}
		// Check for collision between cars and bomb
		for (int flare = 0; flare < numOfBombs; flare++)
		{
			hoverCars[playerCar]->DummyBombCollision(trackBombs[flare]->GetMinX(), trackBombs[flare]->GetMaxX(),
				trackBombs[flare]->GetMinZ(), trackBombs[flare]->GetMaxZ(), flare);
			for (int car = 0; car < carAmount; car++)
			{	// Check computer car collision with bombs
				hoverCars[car]->ComputerBombCollision(trackBombs[flare]->GetMinX(), trackBombs[flare]->GetMaxX(),
					trackBombs[flare]->GetMinZ(), trackBombs[flare]->GetMaxZ(), flare);
			}
			trackBombs[flare]->Update(flare);
		}
		for (int car = 0; car < carAmount; car++)
		{	// Check collision between player car and all cars
			if (car > 0)
			{
				hoverCars[playerCar]->DummyCollisions(hoverCars[car]->GetMinX(), hoverCars[car]->GetMaxX(),
					hoverCars[car]->GetMinZ(), hoverCars[car]->GetMaxZ());

				hoverCars[car]->GenerateNewCarSpeed();
			}
			if (car == playerCar || car == 2) // Car 1 checks playercar and car 2
			{
				hoverCars[1]->ComputerCollisions(hoverCars[car]->GetMinX() - carWidth, hoverCars[car]->GetMaxX() + carWidth,
					hoverCars[car]->GetMinZ() - carLength, hoverCars[car]->GetMaxZ() + carLength);
			}
			if (car == playerCar) // Car 2 checks playercar 
			{
				hoverCars[2]->ComputerCollisions(hoverCars[car]->GetMinX() - carWidth, hoverCars[car]->GetMaxX() + carWidth,
					hoverCars[car]->GetMinZ() - carLength, hoverCars[car]->GetMaxZ() + carLength);
			}
			if (car == playerCar || car == 4) // Car 3 checks playercar and car 4
			{
				hoverCars[3]->ComputerCollisions(hoverCars[car]->GetMinX() - carWidth, hoverCars[car]->GetMaxX() + carWidth,
					hoverCars[car]->GetMinZ() - carLength, hoverCars[car]->GetMaxZ() + carLength);
			}
			if (car == playerCar || car == 5)			// Car 4 checks playercar and car 5
			{
				hoverCars[4]->ComputerCollisions(hoverCars[car]->GetMinX() - carWidth, hoverCars[car]->GetMaxX() + carWidth,
					hoverCars[car]->GetMinZ() - carLength, hoverCars[car]->GetMaxZ() + carLength);
			}
			if (car == playerCar) // Car 5 checks playercar
			{
				hoverCars[5]->ComputerCollisions(hoverCars[car]->GetMinX() - carWidth, hoverCars[car]->GetMaxX() + carWidth,
					hoverCars[car]->GetMinZ() - carLength, hoverCars[car]->GetMaxZ() + carLength);
			}
		}
		// Crate systems
		for (int crate = 0; crate < numOfCrates; crate++)
		{
			// Crate system
			healthCrate[crate]->Update(health, 0);
			slowCrate[crate]->Update(slow, 1);
			poisonCrate[crate]->Update(poison, 2);

			//	 Check for player car going over the crate
			hoverCars[playerCar]->HealthPackage(healthCrate[crate]->GetMinX(), healthCrate[crate]->GetMaxX(),
				healthCrate[crate]->GetMinZ(), healthCrate[crate]->GetMaxZ(), crate, gameFont);

			hoverCars[playerCar]->SlowPackage(slowCrate[crate]->GetMinX(), slowCrate[crate]->GetMaxX(),
				slowCrate[crate]->GetMinZ(), slowCrate[crate]->GetMaxZ(), crate, gameFont);

			hoverCars[playerCar]->PoisonPackage(poisonCrate[crate]->GetMinX(), poisonCrate[crate]->GetMaxX(),
				poisonCrate[crate]->GetMinZ(), poisonCrate[crate]->GetMaxZ(), crate, gameFont, myCamera);
		}
		// Update airplanes
		for (int i = 0; i < numOfPlanes; i++) plane[i]->Update();

		// Updating particles
		for (int i = 0; i < numOfParticles; i++)
		{
			PCpiece[i]->Update(i, 0, myCamera);
			C1piece[i]->Update(i, 1, myCamera);
			C2piece[i]->Update(i, 2, myCamera);
			C3piece[i]->Update(i, 3, myCamera);
			C4piece[i]->Update(i, 4, myCamera);
			C5piece[i]->Update(i, 5, myCamera);
		}

		for (int car = 0; car < carAmount; car++) miniMap->UpdateCarPos(car);
		for (int t = 0; t < numOfTurrets; t++) turret[t]->Update();

		miniMap->UpdateObjects(myCamera);

		if (myEngine->KeyHeld(userSettings.hoverCarForwards))
		{
			hoverCars[playerCar]->IncreaseCarSpeed();
		}
		else if (myEngine->KeyHeld(userSettings.hoverCarBackwards) && carState[playerCar] == Racing)
		{
			hoverCars[playerCar]->DecreaseCarSpeed();
		}
		else hoverCars[playerCar]->GraduallyDecreaseSpeed();

		if (myEngine->KeyHeld(userSettings.hoverCarLeft) && carState[playerCar] == Racing)
		{
			hoverCars[playerCar]->SteerLeft(myCamera);
		}
		else if (myEngine->KeyHeld(userSettings.hoverCarRight) && carState[playerCar] == Racing)
		{
			hoverCars[playerCar]->SteerRight(myCamera);
		}
		else
		{ // Straighten up
			hoverCars[playerCar]->RemoveBanking();
		}

		if (myEngine->KeyHit(userSettings.prepareRace) && carState[playerCar] == Hovering)
		{
			hoverCars[playerCar]->PrepareRace(myCamera);
			myCamera->SetPosition(0.0f, 20.0f, -60.0f);
		}

		if (myEngine->KeyHit(userSettings.resetCamera))
		{
			myCamera->ResetOrientation();
			myCamera->SetPosition(hoverCars[playerCar]->GetX(), 20.0f, hoverCars[playerCar]->GetZ() - 40.0f);
		}

		//if (myEngine->KeyHeld(userSettings.chaseCamera))
		//{
		//	myCamera->ResetOrientation();
		//}

		if (myEngine->KeyHit(userSettings.firstPersonCamera))
		{
			myCamera->ResetOrientation();
			myCamera->SetPosition(hoverCars[playerCar]->GetX(), hoverCars[playerCar]->GetY() + 3.0f, hoverCars[playerCar]->GetZ() + 3.0f);
		}
		if (myEngine->KeyHit(userSettings.chaseCamera))
		{ // 0.0f, 10.0f, -40.0f
			myCamera->SetPosition(hoverCars[playerCar]->GetX(), 20.0f, hoverCars[playerCar]->GetZ() - 40.0f);
		}
		// Whilst boost timer is going down
		if (myEngine->KeyHeld(userSettings.hoverCarBoost) && boostTimer->GetTimeRemaining() > 0 && carState[playerCar] == Racing)
		{
			if (boostTimer->CountingDown() == false) boostTimer->StartCountdown();
			hoverCars[playerCar]->Boost();
			gameFont->Draw("BOOST!", 675, 640, kGreen);
		}
		else if (boostTimer->GetTimeRemaining() > 0 && carState[playerCar] == Racing)
		{
			boostTimer->StopCountdown();
		}
		// Whilst 1 second and less remaining
		if (myEngine->KeyHeld(userSettings.hoverCarBoost) && boostTimer->GetTimeRemaining() <= 1 && boostTimer->GetTimeRemaining() > 0)
		{
			gameFont->Draw("OVERHEATING...", 675, 675, kRed);
			hoverCars[playerCar]->ShakeCamera(myCamera);
		}
		// Whilst car overheated
		else if (myEngine->KeyHeld(userSettings.hoverCarBoost) && boostTimer->GetTimeRemaining() == 0)
		{
			gameFont->Draw("OVERHEATED!", 675, 675, kRed);
			if (overHeatTimer->CountingDown() == false) overHeatTimer->StartCountdown();
			hoverCars[playerCar]->DecreaseCarSpeed();
		}
		// Resetting all timers
		else if (overHeatTimer->CountingDown() && overHeatTimer->GetTimeRemaining() == 0)
		{
			boostTimer->StopCountdown();
			boostTimer->ResetCountdown();
			overHeatTimer->StopCountdown();
			overHeatTimer->ResetCountdown();
		}

		if (myEngine->KeyHit(cameraForwards) && myCamera->GetZ() <= cameraForwardsLimit)   myCamera->MoveZ(cameraSpeed);
		if (myEngine->KeyHit(cameraBackwards) && myCamera->GetZ() >= -cameraBackwardsLimit) myCamera->MoveZ(-cameraSpeed);
		if (myEngine->KeyHit(cameraLeft) && myCamera->GetX() >= cameraLeftLimit)       myCamera->MoveX(-cameraSpeed);
		if (myEngine->KeyHit(cameraRight) && myCamera->GetX() <= cameraRightLimit)      myCamera->MoveX(cameraSpeed);
	}
	else // Pause timers
	{
		for (int car = 0; car < carAmount; car++)
		{
			hoverCars[car]->PauseTimers();
		}
	}
	// Pause key
	if (myEngine->KeyHit(userSettings.pauseGameKey))
	{
		gamePaused ^= 1; // Toggle pause
		if (gamePaused && !pauseCreated)
		{
			pause = myEngine->CreateSprite(pauseName, pauseSpriteX, pauseSpriteY);
			pauseCreated = true;
		}
		else if (!gamePaused && pauseCreated)
		{
			myEngine->RemoveSprite(pause);
			pauseCreated = false;
		} // Create and remove pause sprite when pressing P
	}
	return gameState;
}

void QuitGame(I3DEngine* myEngine)
{
	myEngine->Stop();
}