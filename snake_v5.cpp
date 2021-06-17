#include <ncurses.h>
#include <locale.h>
#include <fstream>
#include <vector>
#include <chrono>
#include <time.h>
#include <iostream>
#define MAPWIDTH 30
#define MAPHEIGHT 30
using namespace std;

char mapdata[MAPHEIGHT+1][MAPWIDTH]; // 맵 데이터를 저장할 배열
WINDOW* win_map; // snake가 움직일 윈도우
int gcnt = 0, pcnt = 0;
int gateout = -1;

int x1 = 1,x2 = 1,y1 = 0,y2 = 0;

void changeData(int y, int x, char newData){
  mapdata[y][x] = newData;
}

void setGrowth(){ // growth item을 맵에 배치시키는 함수
  while(true){ // growth item
    int x = rand() % MAPWIDTH;
    int y = rand() % MAPHEIGHT;
    if(mapdata[y][x] == '0'){
      changeData(y,x,'5');
      gcnt = 0;
      break;
    }
  }
}

void setPoison(){ // poison item을 맵에 배치시키는 함수
  while(true){ // poison item
    int x = rand() % MAPWIDTH;
    int y = rand() % MAPHEIGHT;
    if(mapdata[y][x] == '0'){
      changeData(y,x,'6');
      pcnt = 0;
      break;
    }
  }
}

void delGrowth(){
  for(int i = 0; i < MAPHEIGHT; i++){
    for(int j = 0; j < MAPWIDTH; j++){
      if(mapdata[i][j] == '5'){
        mapdata[i][j] = '0';
        return;
      }
    }
  }
}

void delPoison(){
  for(int i = 0; i < MAPHEIGHT; i++){
    for(int j = 0; j < MAPWIDTH; j++){
      if(mapdata[i][j] == '6'){
        mapdata[i][j] = '0';
        return;
      }
    }
  }
}

void setGate(){ // gate를 맵에 배치시키는 함수
  changeData(y1,x1,'1');
  changeData(y2,x2,'1');

  while(true){ // gate 1
    x1 = rand() % MAPWIDTH;
    y1 = rand() % MAPHEIGHT;
    if(mapdata[y1][x1] == '1'){
      changeData(y1,x1,'7');
      break;
    }
  }
  while(true){ // gate 2
    x2 = rand() % MAPWIDTH;
    y2 = rand() % MAPHEIGHT;
    if(mapdata[y2][x2] == '1' && x2 != x1 && y2 != y1){
      changeData(y2,x2,'7');
      break;
    }
  }
}

class Position{
public:
  int x;
  int y;
  Position(int y, int x){
    this->y = y;
    this->x = x;
  }
};

class Snake{
public:
  vector<Position> snakeinfo; // snake의 머리와 몸통의 좌표가 들어있는 벡터
  char direction;
  bool isDead;
  bool isGrow;
  int score;

  void snakeStart(){
    for(int i = 0; i < 3; i++){
      snakeinfo.push_back(Position(15,14+i));
    }
  }

  Snake(){;
    direction = 'l';
    isDead = false;
    isGrow = false;
    snakeStart();
    score = 100;
  }

  void setDir(char pressedKey){
    switch (pressedKey) {
      case 'a': // 왼쪽 방향을 누르면
        if(direction == 'r') isDead = true;
        else{
          direction = 'l';
        }
        break;
      case 'd': // 오른쪽 방향을 누르면
        if(direction == 'l') isDead = true;
        else{
          direction = 'r';
        }
        break;
      case 'w':
        if(direction == 'd') isDead = true;
        else{
          direction = 'u';
        }
        break;
      case 's':
        if(direction == 'u') isDead = true;
        else{
          direction = 'd';
        }
        break;
    }
  }

  void update(){
    if(snakeinfo.size() < 3){
      isDead = true;
    }

    if(isDead == false){
      if(direction == 'l'){
        snakeinfo.insert(snakeinfo.begin(), Position(snakeinfo[0].y, snakeinfo[0].x - 1));
      }
      else if(direction == 'r'){
        snakeinfo.insert(snakeinfo.begin(), Position(snakeinfo[0].y, snakeinfo[0].x + 1));
      }
      else if(direction == 'u'){
        snakeinfo.insert(snakeinfo.begin(), Position(snakeinfo[0].y - 1, snakeinfo[0].x));
      }
      else if(direction == 'd'){
        snakeinfo.insert(snakeinfo.begin(), Position(snakeinfo[0].y + 1, snakeinfo[0].x));
      }
    }
    if(isGrow == false){
      changeData(snakeinfo[snakeinfo.size() - 1].y, snakeinfo[snakeinfo.size() - 1].x, '0');
      snakeinfo.pop_back();
    }
    else{
      isGrow = false;
    }
  }

  bool isCollide(){
    Position head = snakeinfo[0];
    if(mapdata[head.y][head.x] == '1'){ // 벽에 부딪힌 경우
      isDead = true;
    }
    if(mapdata[head.y][head.x] == '4'){ // 자신의 몸에 부딪힌 경우
      isDead = true;
    }
    else if(mapdata[head.y][head.x] == '5'){ // growth item을 먹은 경우
      isGrow = true;
      setGrowth();
      score += 100;
    }
    else if(mapdata[head.y][head.x] == '6'){ // poison item을 먹은 경우
      changeData(snakeinfo[snakeinfo.size() - 1].y, snakeinfo[snakeinfo.size() - 1].x, '0');
      snakeinfo.pop_back();
      if(snakeinfo.size() < 3){
        isDead = true;
      }
      setPoison();
      score -= 100;
    }
    else if(mapdata[head.y][head.x] == '7'){ // gate에 들어가는 경우
      for(int i = 0; i < MAPHEIGHT; i++){
        for(int j = 0; j < MAPWIDTH; j++){
          if(mapdata[i][j] == '7' && (i != head.y && j != head.x)){
            snakeinfo[0].y = i;
            snakeinfo[0].x = j;
            if(i == 0){
              direction = 'd';
            }
            else if(i == MAPHEIGHT - 1){
              direction = 'u';
            }
            else if(j == 0){
              direction = 'r';
            }
            else if(j == MAPWIDTH - 1){
              direction = 'l';
            }
          }
        }
      }
      gateout = 0;
      score += 50;
    }
  }

  void apply(){
    changeData(snakeinfo[0].y,snakeinfo[0].x,'3'); // snake 머리 mapdata에 저장
    for(int i = 1; i < snakeinfo.size(); i++){
      changeData(snakeinfo[i].y,snakeinfo[i].x,'4');// snake 몸통 mapdata에 저장
    }
  }
};

void mapRender(Snake* player){
  player->update();
  player->isCollide();
  player->apply();
  curs_set(0); // 커서 안보이게 설정
  noecho(); // 입력값 안보이게 설정
  for(int i = 0; i < MAPHEIGHT; i++){ // 맵 데이터 윈도우에 적용
    for(int j = 0; j < MAPWIDTH; j++){
      switch (mapdata[i][j]) {
        case '0': // snake가 지나다닐수 있는 공간
          wattron(win_map, COLOR_PAIR(2));
          mvwprintw(win_map,i,j,".");
          wattroff(win_map,COLOR_PAIR(2));
          break;
        case '1': // snake가 부딪히면 죽는 벽
          wattron(win_map, COLOR_PAIR(1));
          mvwprintw(win_map,i,j,".");
          wattroff(win_map,COLOR_PAIR(1));
          break;
        case '2': // gate가 생성될 수 없는 벽
          wattron(win_map, COLOR_PAIR(1));
          mvwprintw(win_map,i,j,"X");
          wattroff(win_map,COLOR_PAIR(1));
          break;
        case '3': // snake의 머리
          wattron(win_map, COLOR_PAIR(3));
          mvwprintw(win_map,i,j,"H");
          wattroff(win_map,COLOR_PAIR(3));
          break;
        case '4': // snake의 몸통
          wattron(win_map, COLOR_PAIR(4));
          mvwprintw(win_map,i,j,"B");
          wattroff(win_map,COLOR_PAIR(4));
          break;
        case '5': // growth item
          wattron(win_map, COLOR_PAIR(5));
          mvwprintw(win_map,i,j,"G");
          wattroff(win_map,COLOR_PAIR(5));
          break;
        case '6': // poison item
          wattron(win_map, COLOR_PAIR(6));
          mvwprintw(win_map,i,j,"P");
          wattroff(win_map,COLOR_PAIR(6));
          break;
        case '7': // gate
          wattron(win_map, COLOR_PAIR(7));
          mvwprintw(win_map,i,j,"-");
          wattroff(win_map,COLOR_PAIR(7));
          break;
      }
    }
  }
  char scoreboard[10];
  sprintf(scoreboard, "%d", player->score);
  mvprintw(4,42,scoreboard);
  mvprintw(4,46,"/1000");

  refresh();
  wrefresh(win_map);
  napms(250);
}

void stage(int level, bool& clear){
  string src = "map" + to_string(level);
  ifstream fin(src); //맵 데이터 받아와서 저장
  int height = 0;

  while(!fin.eof()){
    char line[100];
    fin.getline(line,100);

    for(int i = 0; i < MAPWIDTH; i++){
      mapdata[height][i] = line[i];
    }

    height++;
  }



  for(int i = 0; i < MAPHEIGHT; i++){ // 맵 데이터 윈도우에 적용
    for(int j = 0; j < MAPWIDTH; j++){
      switch (mapdata[i][j]) {
        case '0': // snake가 지나다닐수 있는 공간
          break;
        case '1': // snake가 부딪히면 죽는 벽
          wattron(win_map, COLOR_PAIR(1));
          //wprintw(win_map, "#");
          mvwprintw(win_map,i,j,".");
          wattroff(win_map,COLOR_PAIR(1));
          break;
        case '2': // gate가 생성될 수 없는 벽
          wattron(win_map, COLOR_PAIR(1));
          //wprintw(win_map, "#");
          mvwprintw(win_map,i,j,"X");
          wattroff(win_map,COLOR_PAIR(1));
          break;
      }
    }
  }

  refresh();
  wrefresh(win_map);

  Snake player;
  mvprintw(1,43,"stage ");
  char lv[3];
  sprintf(lv, "%d", level);
  mvprintw(1,49,lv);
  mvprintw(3,44,"SCORE");
  char scoreboard[10];
  sprintf(scoreboard, "%d", player.score);
  mvprintw(4,42,scoreboard);
  mvprintw(4,46,"/1000");

  mvprintw(7,35,"Growth item ");
  attron(COLOR_PAIR(5));
  mvprintw(7,47,"G");
  attroff(COLOR_PAIR(5));
  mvprintw(7,48," 획득: +100");

  mvprintw(8,35,"Poison item ");
  attron(COLOR_PAIR(6));
  mvprintw(8,47,"P");
  attroff(COLOR_PAIR(6));
  mvprintw(8,48," 획득: -100");

  mvprintw(9,35,"Gate");
  attron(COLOR_PAIR(7));
  mvprintw(9,41,"-");
  attroff(COLOR_PAIR(7));
  mvprintw(9,42," 통과: +50");

  mvprintw(15,35, "왼쪽 방향으로 이동: a");
  mvprintw(17,34, "오른쪽 방향으로 이동: d");
  mvprintw(14,35, "위쪽 방향으로 이동: w");
  mvprintw(16,34, "아래쪽 방향으로 이동: s");

  mvprintw(25,33,"snake가 이동하지 않는다면");
  mvprintw(26,35,"한영키를 눌러보세요");

  nodelay(stdscr,TRUE);
  nodelay(win_map,TRUE);

  mapRender(&player);
  setGrowth();
  setPoison();
  setGate();

  while(true){
    if(player.isDead == true){
      return;
    }
    player.setDir(getch());
    gcnt++; pcnt++;
    if(gcnt >= 60){
      delGrowth();
      setGrowth();
    }
    if(pcnt >= 60){
      delPoison();
      setPoison();
    }
    if(gateout != -1){
      gateout++;
      if(gateout > player.snakeinfo.size()){
        setGate();
        gateout = -1;
      }
    }
    mapRender(&player);
    if(player.score >= 1000){
      mvprintw(4,45," ");
      clear = true;
      return;
    }
  }
}

int main(){

  setlocale(LC_ALL, "");
  curs_set(0); // 커서 안보이게 설정
  noecho(); // 입력값 안보이게 설정
  srand(time(NULL));

  initscr();
  resize_term(32,62); // 바탕이 되는 윈도우의 크기를 50*50으로 설정
  start_color();
  init_pair(1,COLOR_BLUE,COLOR_BLUE); // 벽애 쓰일 색 지정
  init_pair(2,COLOR_WHITE,COLOR_WHITE); // 바탕에 쓰일 색 지정
  init_pair(3,COLOR_BLACK,COLOR_BLACK); // snake 머리에 쓰일 색 지정
  init_pair(4,COLOR_MAGENTA,COLOR_MAGENTA); // snake 몸통에 쓰일 색 지정
  init_pair(5,COLOR_GREEN,COLOR_GREEN); // growth item에 쓰일 색 지정
  init_pair(6,COLOR_RED,COLOR_RED); //poison item에 쓰일 색 지정
  init_pair(7,COLOR_YELLOW,COLOR_YELLOW); // gate에 쓰일 색 지정
  init_pair(8,COLOR_BLACK,COLOR_WHITE);
  border('*','*','*','*','*','*','*','*');

  win_map = newwin(MAPHEIGHT,MAPWIDTH,1,1); // map을 나타낼 윈도우
  wbkgd(win_map, COLOR_PAIR(2)); // 맵의 바탕을 하얀색으로 설정

  bool stageClear = false;
  attron(COLOR_PAIR(8));
  mvprintw(15,20,"Press any key to start");
  attroff(COLOR_PAIR(8));
  getch();
  clear();
  stage(1,stageClear);

  if(stageClear){
    stageClear = false;
    attron(COLOR_PAIR(8));
    mvprintw(22,34,"Press any key to continue");
    attroff(COLOR_PAIR(8));
    nodelay(stdscr,FALSE);
    nodelay(win_map,FALSE);
    getch();
    clear();
    stage(2,stageClear);

    if(stageClear){
      stageClear = false;
      attron(COLOR_PAIR(8));
      mvprintw(22,34,"Press any key to continue");
      attroff(COLOR_PAIR(8));
      nodelay(stdscr,FALSE);
      nodelay(win_map,FALSE);
      getch();
      clear();
      stage(3,stageClear);

      if(stageClear){
        stageClear = false;
        attron(COLOR_PAIR(8));
        mvprintw(22,34,"Press any key to continue");
        attroff(COLOR_PAIR(8));
        nodelay(stdscr,FALSE);
        nodelay(win_map,FALSE);
        getch();
        clear();
        stage(4,stageClear);

        if(stageClear){
          nodelay(stdscr,FALSE);
          nodelay(win_map,FALSE);
          attron(COLOR_PAIR(8));
          mvprintw(20,31,"모든 stage를 클리어했습니다!!");
          mvprintw(22,34,"Press any key to exit");
          attroff(COLOR_PAIR(8));
          getch();
          endwin();
          return 0;
        }
      }
    }
  }

  nodelay(stdscr,FALSE);
  nodelay(win_map,FALSE);
  attron(COLOR_PAIR(8));
  mvprintw(20,31,"stage 클리어에 실패했습니다...");
  mvprintw(22,34,"Press any key to exit");
  attroff(COLOR_PAIR(8));
  getch();

  endwin();

  return 0;
}
