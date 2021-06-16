#include <ncurses.h>
#include <locale.h>
#include <fstream>
#define MAPWIDTH 60
#define MAPHEIGHT 30
using namespace std;


int main(){

  setlocale(LC_ALL, "");

  WINDOW* win_map; // snake가 움직일 윈도우
  char mapdata[MAPHEIGHT+1][MAPWIDTH]; // 맵 데이터를 저장할 배열

  initscr();
  resize_term(32,62); // 바탕이 되는 윈도우의 크기를 50*50으로 설정
  start_color();
  init_pair(1,COLOR_BLUE,COLOR_BLUE); // 벽애 쓰일 색 지정
  init_pair(2,COLOR_WHITE,COLOR_WHITE); //    바탕에 쓰일 색 지정
  init_pair(3,COLOR_YELLOW,COLOR_YELLOW); // snake 머리에 쓰일 색 지정
  init_pair(4,COLOR_MAGENTA,COLOR_MAGENTA); // snake 몸통에 쓰일 색 지정
  border('*','*','*','*','*','*','*','*');
  //mvprintw(14,20,"Snake Game"); // 맵 위에 표시할 글

  win_map = newwin(MAPHEIGHT,MAPWIDTH,1,1); // map을 나타낼 윈도우
  wbkgd(win_map, COLOR_PAIR(2)); // 맵의 바탕을 하얀색으로 설정

  ifstream fin("map1"); //
  int height = 0;

  while(!fin.eof()){
    char line[100];
    fin.getline(line,100);

    for(int i = 0; i < MAPWIDTH; i++){
      mapdata[height][i] = line[i];
    }

    height++;
  }

  for(int i = 0; i < MAPHEIGHT; i++){
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
  getch();
  endwin();

  return 0;
}
