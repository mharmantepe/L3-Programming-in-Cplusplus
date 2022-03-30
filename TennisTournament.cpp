/*PROJECT VERSION 3 - MELIS HARMANTEPE*/
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <cstring>
using namespace std;

int const MIN_ABILITY = 1, MAX_ABILITY = 3, MIN_SPEED = 1, MAX_SPEED = 3, POINTS_TO_WIN = 4, SET_GAMES = 3;
int const COURT_WIDTH = 7, COURT_LENGTH = 3, DIM_HIT_ARRAY = COURT_WIDTH + 2, DIM_PLAYERS = 10;
bool const DEBUG_MODE = false;

typedef enum
{
   NOBODY,
   PLAYER1,
   PLAYER2
} tPlayer; //0,1,2

typedef enum
{
   NONE,
   FIFTEEN,
   THIRTY,
   FORTY,
   ADVANTAGE
} tPoints;

typedef int tHitCounter[DIM_HIT_ARRAY]; //count the hits for the statistics on each player’s performance

struct tPlayerData{
   string name;
   int ability, speed, games_won, games_lost, position, winHits, outHits;
   tPoints points;
   tHitCounter hitsArray;
};

typedef tPlayerData tPlayersArray[DIM_PLAYERS]; 

struct tPlayerList{
   int counter=0;
   tPlayersArray players;
};

int menu();
bool load(tPlayerList& playerList);
void save(const tPlayerList& playerList);
void display(const tPlayerList& playerList);
string pointsToStr(tPoints points);
void playerData(string& initials, int& ability, int& speed);
int askData(string dataName, int minValue, int maxValue); //Ask the user for each player’s information (name, ability and speed)
int hit(int position, int ability);
int run(int playerRow, int speed, int ballRow);
tPlayer updateScore(tPlayer pointWinner, tPoints& points1, tPoints& points2, int& games1, int& games2);
void printScore(string name1, string name2, tPoints points1, tPoints points2, int games1, int games2, tPlayer service);
void displayCourt(string name1, string name2, int player1Pos, int player2Pos, tPlayer playerBall, int ballPos);
void playMatch(tPlayerData& player1, tPlayerData& player2);
tPlayer playBall(tPlayer hitterPlayer, tPlayerData& hitter, tPlayerData& receiver, int& ballPos);
tPlayer playPoint(tPlayerData& player1, tPlayerData& player2, tPlayer service);
tPlayer playGame(tPlayerData& player1, tPlayerData& player2, tPlayer service);
void displayStats(tPlayerData player);
int findPlayer(const tPlayerList& playerList, string initials);
void newPlayer(tPlayerList& playerList);
int selectPlayer(const tPlayerList& playerList);
void selectPlayers(const tPlayerList& playerList, int& p1Idx, int& p2Idx);
void initMatchData(tPlayerData& player);
void selectFinalists(const tPlayerList& playerList, int& p1Idx, int& p2Idx);


int main()
{
   srand(time(NULL));
   tPlayerList playerList;
   tPlayerData player1, player2;
   int op = -1, p1Idx, p2Idx;

   if (!load(playerList)) {
      cout << "File not found! Aborting..." << endl;
   }
   else {
      while (op != 0) {
         op = menu();
         switch (op) {
         case 1:
            display(playerList);
            break;
         case 2:
            newPlayer(playerList);
            break;
         case 3:
            selectPlayers(playerList, p1Idx, p2Idx);
            player1 = playerList.players[p1Idx];
            player2 = playerList.players[p2Idx];
            initMatchData(player1);
            initMatchData(player2);
            playMatch(player1, player2);   //start match
            break;
         case 4:
            selectFinalists(playerList, p1Idx, p2Idx);
            player1 = playerList.players[p1Idx];
            player2 = playerList.players[p2Idx];
            initMatchData(player1);
            initMatchData(player2);
            playMatch(player1, player2);
            break;
         }
      }
      save(playerList);
      cout << "Thanks for playing!" << endl;
   }

   return 0;
}

int menu() {
   int op = -1;
   const int numOp = 4;

   while ((op < 0) || (op > numOp)) {
      cout << endl;
      cout << "---------------------" << endl;
      cout << "| TENNIS TOURNAMENT |" << endl;
      cout << "---------------------" << endl;
      cout << endl;
      cout << "<1> Player list" << endl;
      cout << "<2> New player" << endl;
      cout << "<3> New match" << endl;
      cout << "<4> Final" << endl;
      cout << "<0> Exit" << endl;
      cout << "Option: ";
      cin >> op;
      cout << endl;
      if ((op < 0) || (op > numOp)) {
         cout << "Invalid option! Try again!" << endl;
      }
   }

   return op;
}

bool load(tPlayerList& playerList) {
	bool ok = false;
   tPlayersArray player;
   tPlayerData player_data;
	ifstream file;
	string str;

	playerList.counter = 0;
	file.open("players.txt");
	if (file.is_open()) {
		file >> str;
		while ((str != "XXX") && (playerList.counter < DIM_PLAYERS)) {
         player_data.name = str;
			file >> player_data.ability;
			file >> player_data.speed;
         file >> player_data.games_won;
         file >> player_data.games_lost;
			playerList.players[playerList.counter] = player_data;
         playerList.counter++;
			file >> str;
		}
      ok = true;
		file.close();
	}

	return ok;
}

void save(const tPlayerList& playerList) {
   ofstream file;

   file.open("players.txt");
   for (int i = 0; i < playerList.counter; i++) {
      file << playerList.players[i].name << " " << playerList.players[i].ability << " " << 
      playerList.players[i].speed << " " << playerList.players[i].games_won << " " << playerList.players[i].games_lost <<endl;
   }
   file << "XXX" << endl;
   file.close();
}

void display(const tPlayerList& playerList){
   cout << setw(5) << right << "Initials" << setw(10) << right << "Ability" << setw(15) << right << "Speed" <<
   setw(20) << right << "Won" << setw(25) << right << "Lost" << endl;

   for (int i = 0; i < playerList.counter; i++) {
      cout << setw(5) << right << playerList.players[i].name  << setw(10) << right << playerList.players[i].ability << 
      setw(15) << right << playerList.players[i].speed << setw(20) << right << playerList.players[i].games_won << 
      setw(25) << right << playerList.players[i].games_lost << endl;
   }
   cout << endl;
}

void newPlayer(tPlayerList& playerList){
   string initials;
   int ability, speed;

   cout << "Player data..." << endl;
   if (playerList.counter < DIM_PLAYERS){
      cout << "- Enter the initials (3 letters): " ;
      cin >> initials;
      
      for (int i = 0; i < playerList.counter; i++) {
         while(findPlayer(playerList, initials) != -1){
            cout << "Error! A player with those initials already exists" << endl;
            cout << "- Enter the initials (3 letters): " ;
            cin >> initials;
         }
      }
      playerData(initials, ability, speed);
      playerList.players[playerList.counter].name= initials;
      playerList.players[playerList.counter].ability = ability;
      playerList.players[playerList.counter].speed = speed;
      playerList.players[playerList.counter].games_won = 0;
      playerList.players[playerList.counter].games_lost = 0;
      playerList.counter++;
      cout << "New player added!" << endl;
   }else{
      cout << "Sorry! No more room in the players list!" << endl;
   }
}

int findPlayer(const tPlayerList& playerList, string initials){
   int idx = 0;
   bool found = false;

   while (!found && (idx < playerList.counter)){
      if(playerList.players[idx].name == initials){
         found = true;
      }else{
			idx++;
		}
   }
   if(!found){
      idx= -1;
   }

   return idx;
}

int selectPlayer(const tPlayerList& playerList){
   string initials;
   int idx;

   display(playerList);
   cout << "Please enter the initials of the player: ";
   cin >> initials;
   cout << initials << endl;
   idx = findPlayer(playerList,initials);

   while(idx == -1){
      cout << "No player with those initials!" << endl;
      cout << "Please enter the initials of the player: ";
      cin >> initials;
      idx = findPlayer(playerList, initials);
   }
   
   return idx;
}

void selectPlayers(const tPlayerList& playerList, int& p1Idx, int& p2Idx){
   cout << "Select the first player..." << endl;
   p1Idx = selectPlayer(playerList);            
   cout << "Select the second player..." << endl;
   p2Idx = selectPlayer(playerList); //Condition here so that the 2nd player is different
   while(p1Idx == p2Idx){
      cout << "Can't choose the same player!" << endl;
      cout << "Please enter the initials of the second player: " << endl;
      p2Idx = selectPlayer(playerList);
   }
   cout << "The match begins..." << endl;
}

int askData(string dataName, int minValue, int maxValue)
{
   int data_value;
   cout << "- " << dataName << " (value between " << minValue << " and " << maxValue << "): ";
   cin >> data_value;
   while (data_value < minValue || data_value > maxValue)
   {
      cout << "  Please enter valid values!!" << endl;
      cout << "- " << dataName << " (value between " << minValue << " and " << maxValue << "): ";
      cin >> data_value;
   }

   return data_value;
}

void playerData(string& initials, int& ability, int& speed)
{
   while (initials.length() > 3 || initials.length() < 3)
   {
      cout << "  There must be exactly 3 initials!" << endl;
      cout << "- Enter the initials (3 letters): ";
      cin >> initials;
   }
   ability = askData("Ability", MIN_ABILITY, MAX_ABILITY);
   speed = askData("Speed", MIN_SPEED, MAX_SPEED);
   cout << endl;
}

tPlayer updateScore(tPlayer pointWinner, tPlayerData& player1, tPlayerData& player2)
{
   tPlayer gameWinner = NOBODY; 

   if (pointWinner == PLAYER1)
   {
      player1.points = tPoints((player1.points) + 1);
      if (player1.points >= tPoints(int(POINTS_TO_WIN)) && (player1.points >= tPoints(int(player2.points) + 2))){
         player1.games_won++;
         player2.games_lost++;  //Should I increment player2's game_lost ??
         gameWinner = PLAYER1;
      }
   }
   else
   {
      player2.points = tPoints((player2.points) + 1);
      if (player2.points >= tPoints(int(POINTS_TO_WIN)) && (player2.points >= tPoints(int(player1.points) + 2))){
         player2.games_won++;
         player1.games_lost++;
         gameWinner = PLAYER2;
      }
   }
   
   if(gameWinner != NOBODY){
      player1.points = tPoints(0);
      player2.points = tPoints(0);

   }

   return gameWinner;
}

void printScore(tPlayerData player1, tPlayerData player2, tPlayer service)
{
   cout << player1.name << " " << player1.games_won << " : " << pointsToStr(player1.points) << " " << (int(service) == 1 ? " *" : "") << endl;
   cout << player2.name << " " << player2.games_won << " : " << pointsToStr(player2.points) << " " << (int(service) == 2 ? " *" : "") << endl
      << endl;
}

void displayCourt(string name1, string name2, int player1Pos, int
player2Pos, tPlayer playerBall, int ballPos)
{
   //setw(player1Pos * 2 - 1) didnt work for the position, so i did with +2
   cout << setw(player1Pos * 2 + 2) << name1 << endl;
   cout << "  ";
   for (int i = 0; i < COURT_WIDTH; i++)
   {
      cout << "- ";
   }
   cout << endl;

   cout << " ";
   for (int i = 0; i < COURT_WIDTH; i++)
      if (ballPos == i && playerBall == PLAYER1)
         cout << "|o";
      else
         cout << "| ";
   cout << "|" << endl;

   for (int j = 1; j < COURT_LENGTH; j++) {
      cout << " ";
      for (int i = 0; i < COURT_WIDTH; i++)
         cout << "| ";
      cout << "|" << endl;
   }

   int n = 1;
   cout << "--";
   for (int i = 0; i < COURT_WIDTH; i++)
   {
      cout << n << "-";
      n++;
   }
   cout << "-" << endl;

   for (int m = 1; m < COURT_LENGTH; m++) {
      cout << " ";
      for (int k = 0; k < COURT_WIDTH; k++)
         cout << "| ";
      cout << "|" << endl;
   }

   cout << " ";
   for (int k = 0; k < COURT_WIDTH; k++)
      if (ballPos == k && playerBall == PLAYER2)
         cout << "|o";
      else
         cout << "| ";
   cout << "|" << endl;

   cout << "  ";
   for (int i = 0; i < COURT_WIDTH; i++)
   {
      cout << "- ";
   }
   cout << endl;
   cout << setw(player2Pos * 2 + 2) << name2 << endl << endl;
}

void displayStats(tPlayerData player){
   cout << "Statistics for " << player.name << endl;
   double total=0, distrib;

   for(int i=0; i< DIM_HIT_ARRAY; i++){
      total= total + player.hitsArray[i];
   }
   cout << "   Total hits: " << total << endl;      
   cout << "   Winning hits: " << player.winHits << endl;
   cout << "   Out: " << player.outHits << endl;
   cout << "   Hit distribution on the court" << endl;
   cout << "      Row";
   for(int i=0; i < COURT_WIDTH; i++){
      cout << "       " << i ;
   }
   cout << endl;
   cout << "          %";
   for(int i=0; i < COURT_WIDTH; i++){
      distrib = player.hitsArray[i] / total * 100; //I assume that the ratios for each row should be the hits 
                                                   //for that row divided by the total hits *100. NOT SURE IF CORRECT !!! ???
      cout << "     " << fixed << setprecision(1) << distrib ;
   }
   cout << endl << endl;
}

void initMatchData(tPlayerData& player){ 
   player.winHits= 0;
   player.outHits = 0;
   player.games_won = 0;
   player.games_lost =0;
   for(int i=0; i< DIM_HIT_ARRAY; i++ ){
      player.hitsArray[i] =0;
   }
}

void playMatch(tPlayerData& player1, tPlayerData& player2){
   int service = tPlayer(rand() % 2 + 1); // 1 or 2
   tPlayer gameWinner;
   tPlayer setWinner = NOBODY;
   cout << endl;

   while (setWinner == NOBODY)
   {
      cout << "Service for " << (service == PLAYER1 ? player1.name : player2.name) << endl << endl;
      printScore(player1, player2, tPlayer(service));
      cout << "Press Enter to continue..." << endl << endl;
      cin.get();
      cout << (service == PLAYER1 ? player1.name : player2.name) << " hits..." << endl << endl;
      service = tPlayer(3 - int(tPlayer(playGame(player1, player2, tPlayer(service))))) ;

      if (player1.games_won >= SET_GAMES && player1.games_won >= player2.games_won + 2) {
         setWinner = PLAYER1;
      }
      if (player2.games_won >= SET_GAMES && player2.games_won >= player1.games_won + 2) {
         setWinner = PLAYER2;
      }
      
   }
   cout << "   " << player1.name << "  "<< player1.games_won << endl;
   cout << "   " << player2.name << "  " << player2.games_won << endl << endl;
   cout << (setWinner == PLAYER1 ? player1.name : player2.name) << " wins the match!" << endl <<endl;
   cout << "..." << endl;
   
}

tPlayer playGame(tPlayerData& player1, tPlayerData& player2, tPlayer service){
   //accumulated statistics are also displayed at the end of the game !!!!! --> call displayStats() !!!!
   tPlayer gameWinner = NOBODY;
   tPlayer pointWinner;

   while (gameWinner == NOBODY){
      pointWinner = playPoint(player1, player2, service);
      gameWinner = updateScore(pointWinner, player1, player2);
      cout << endl;
      cout << "Point for " << (pointWinner == PLAYER1 ? player1.name : player2.name) << "!!!" << endl <<endl;
      printScore(player1, player2, tPlayer(pointWinner));
      cout << "Press Enter to continue..." << endl;
      cin.get();
   }
   cout << (gameWinner == PLAYER1 ? player1.name : player2.name) << " wins the game!" << endl <<endl;
   displayStats(player1);
   displayStats(player2);

   return gameWinner;  
}

 
tPlayer playPoint(tPlayerData& player1, tPlayerData& player2, tPlayer service){ 
   int ball_position;  
   tPlayer pointWinner = NOBODY;

   ball_position = player1.position = player2.position = COURT_WIDTH / 2 + 1; 
   displayCourt(player1.name, player2.name, player1.position, player2.position, tPlayer(service), ball_position);

   while (pointWinner == NOBODY){
      pointWinner = playBall(service,  (service == PLAYER1 ? player1 : player2),  (service == PLAYER1 ? player2 : player1),  ball_position);
      service = tPlayer(3 - int(service));
   }
   cout << "Press Enter to continue..." << endl << endl;
   cin.get();
   cout << "..." << endl << endl;
   
   return pointWinner;
}

tPlayer playBall(tPlayer hitterPlayer, tPlayerData& hitter, tPlayerData& receiver, int& ballPos) {
	tPlayer ballWinner = NOBODY;
   tPlayer receiverPlayer;
		
   if (hitterPlayer == PLAYER1){
		receiverPlayer = PLAYER2;
   }else{
		receiverPlayer = PLAYER1;
   }
		
   //cout << hitter.name << " hits..." << endl;
	ballPos = hit(hitter.position, hitter.ability);
   hitter.hitsArray[ballPos] = hitter.hitsArray[ballPos] +1;

	if (ballPos <= 0 || ballPos >= COURT_WIDTH + 1){ //OUT
      if (DEBUG_MODE){
         cout << " Out!" << endl << endl;
      }
      if(hitterPlayer == PLAYER1){
         hitter.outHits++; //OUTHITS INCREMENT 
      }else{
         receiver.outHits++;
      }
		ballWinner = receiverPlayer; //receiver gets the ball
	}else{
		receiver.position = run(receiver.position, receiver.speed, ballPos);
		if (receiver.position != ballPos) {        
			ballWinner = hitterPlayer; //hitter gets the ball
			hitter.winHits++;
		}
      
      if (hitterPlayer == PLAYER1){
         receiver.position = receiver.position;
      }else{
         hitter.position = receiver.position;
      }
      
	}

	return ballWinner;
}

 
void selectFinalists(const tPlayerList& playerList, int& p1Idx, int& p2Idx){
   int max1 = 0, max2 = 0; 

   for (int i = 0; i < playerList.counter; i++){ //first largest
      if(playerList.players[i].games_won > max1){
         max1= playerList.players[i].games_won;
         p1Idx = i;
      }
   } 
   for(int j = 0; j < playerList.counter; j++){ //second largest
      if(j != p1Idx){   //eliminate the largest one
         if (playerList.players[j].games_won >= max2){ 
            max2= playerList.players[j].games_won;
            p2Idx = j;
         }
      }
         
   }
}

int hit(int position, int ability)
{
   int destination = rand() % COURT_WIDTH + 1;
   int distance = abs(position - destination);
   int ball_pos;
   double success_probability;

   if (DEBUG_MODE)
   {
      cout << " Desired destination: row " << destination << endl;
   }
   if (distance <= ability)
   {
      ball_pos = destination;
      if (DEBUG_MODE)
      {
         cout << " Easy hit!" << endl;
      }
   }
   else
   {
      if (DEBUG_MODE)
      {
         cout << " Complicated hit...";
      }
      success_probability = 100 - ((distance - ability) / ((COURT_WIDTH - 1) - MIN_ABILITY)) * 100;
      int nb = rand() % 100;
      if (DEBUG_MODE)
      {
         cout << " Success probability: " << success_probability << " - Value: " << nb << endl;
      }
      if (nb < success_probability)
      {
         ball_pos = destination;
         if (DEBUG_MODE)
         {
            cout << " Success!" << endl;
         }
      }
      else
      {
         int random_int = rand() % 2;
         if (DEBUG_MODE)
         {
            cout << " Not a precise hit!" << endl;
         }
         if (random_int == 0)
         {
            ball_pos = destination - 1;
            if (DEBUG_MODE)
            {
               cout << " Ball in row " << ball_pos << endl;
            }
         }
         else
         {
            ball_pos = destination + 1;
            if (DEBUG_MODE)
            {
               cout << " Ball in row " << ball_pos << endl;
            }
         }
      }
   }

   return ball_pos;
}

int run(int playerRow, int speed, int ballRow)
{
   int diff = abs(playerRow - ballRow);
   int new_pos;

   if (speed >= diff)
   {
      new_pos = ballRow;
      if (DEBUG_MODE)
      {
         cout << " The receiver got the ball" << endl
            << endl;
      }
   }
   else
   {
      if (DEBUG_MODE)
      {
         cout << " The receiver missed the ball" << endl
            << endl;
      }
      if (ballRow > playerRow)
      {
         new_pos = playerRow + speed; //right
      }
      else if (ballRow < playerRow)
      {
         new_pos = playerRow - speed; //left
      }
   }

   return new_pos;
}

string pointsToStr(tPoints points)
{
   string call;

   if (points == NONE)
      call = "00";
   else if (points == FIFTEEN)
      call = "15";
   else if (points == THIRTY)
      call = "30";
   else if (points == FORTY)
      call = "40";
   else if (points == ADVANTAGE)
      call = "Ad";

   return call;
}
