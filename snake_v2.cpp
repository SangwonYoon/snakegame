#include <ncurses.h>
#include <locale.h>
#include <fstream>
#include <vector>
#include <chrono>
#define MAPWIDTH 60
#define MAPHEIGHT 30
using namespace std;

char mapdata[MAPHEIGHT+1][MAPWIDTH]; // 맵 데이터를 저장할 배열
WINDOW* win_map; // snake가 움직일 윈도우

void changeData(int y, int x, char newData){
  mapdata[y][x] = newData;
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

  void snakeStart(){
    for(int i = 0; i < 3; i++){
      snakeinfo.push_back(Position(15,30+i));
    }
  }

  Snake(){;
    direction = 'l';
    isDead = false;
    isGrow = false;
    snakeStart();
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

  void apply(){
    changeData(snakeinfo[0].y,snakeinfo[0].x,'3'); // snake 머리 mapdata에 저장
    for(int i = 1; i < snakeinfo.size(); i++){
      changeData(snakeinfo[i].y,snakeinfo[i].x,'4');// snake 몸통 mapdata에 저장
    }
  }
};

void mapRender(Snake* player){
  player->update();
  player->apply();
  curs_set(0); // 커서 안보이게 설정
  noecho(); // 입력값 안보이게 설정
  for(int i = 0; i < MAPHEIGHT; i++){ // 맵 데이터 윈도우에 적용
    for(int j = 0; j < MAPWIDTH; j++){
      switch (mapdata[i][j]) {
        case '0': // snake가 지나다닐수 있는 공간
          wattron(win_map, COLOR_PAIR(2));
          //wprintw(win_map, "#");
          mvwprintw(win_map,i,j,".");
          wattroff(win_map,COLOR_PAIR(2));
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
        case '3': // snake의 머리
          wattron(win_map, COLOR_PAIR(3));
          //wprintw(win_map, "#");
          mvwprintw(win_map,i,j,"H");
          wattroff(win_map,COLOR_PAIR(3));
          break;
        case '4': // snake의 몸통
          wattron(win_map, COLOR_PAIR(4));
          //wprintw(win_map, "#");
          mvwprintw(win_map,i,j,"B");
          wattroff(win_map,COLOR_PAIR(4));
          break;
      }
    }
  }
  refresh();
  wrefresh(win_map);
  napms(1000);

}


int main(){

  setlocale(LC_ALL, "");
  curs_set(0); // 커서 안보이게 설정
  noecho(); // 입력값 안보이게 설정

  initscr();
  resize_term(32,62); // 바탕이 되는 윈도우의 크기를 50*50으로 설정
  start_color();
  init_pair(1,COLOR_BLUE,COLOR_BLUE); // 벽애 쓰일 색 지정
  init_pair(2,COLOR_WHITE,COLOR_WHITE); // 바탕에 쓰일 색 지정
  init_pair(3,COLOR_YELLOW,COLOR_YELLOW); // snake 머리에 쓰일 색 지정
  init_pair(4,COLOR_MAGENTA,COLOR_MAGENTA); // snake 몸통에 쓰일 색 지정
  border('*','*','*','*','*','*','*','*');
  //mvprintw(14,20,"Snake Game"); // 맵 위에 표시할 글

  win_map = newwin(MAPHEIGHT,MAPWIDTH,1,1); // map을 나타낼 윈도우
  wbkgd(win_map, COLOR_PAIR(2)); // 맵의 바탕을 하얀색으로 설정

  ifstream fin("map1"); //맵 데이터 받아와서 저장
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
  nodelay(stdscr,TRUE);
  nodelay(win_map,TRUE);

  while(true){
    if(player.isDead == true){
      break;
    }
    player.setDir(getch());
    mapRender(&player);
/*
    for(int i = 0; i < MAPHEIGHT; i++){ // 맵 데이터 윈도우에 적용
      for(int j = 0; j < MAPWIDTH; j++){
        switch (mapdata[i][j]) {
          case '0': // snake가 지나다닐수 있는 공간
            wattron(win_map, COLOR_PAIR(2));
            //wprintw(win_map, "#");
            mvwprintw(win_map,i,j,".");
            wattroff(win_map,COLOR_PAIR(2));
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
          case '3':
            wattron(win_map, COLOR_PAIR(3));
            //wprintw(win_map, "#");
            mvwprintw(win_map,i,j,"H");
            wattroff(win_map,COLOR_PAIR(3));
            break;
          case '4':
            wattron(win_map, COLOR_PAIR(4));
            //wprintw(win_map, "#");
            mvwprintw(win_map,i,j,"B");
            wattroff(win_map,COLOR_PAIR(4));
            break;
        }
      }
    }
    refresh();
    wrefresh(win_map);

    Sleep(1000);
    */

  }



  endwin();

  return 0;
}
