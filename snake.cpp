#include <ncurses.h>
#include <locale.h>

int main(){

  setlocale(LC_ALL, "");

  WINDOW* win_map;

  initscr();
  resize_term(50,50); // 바탕이 되는 윈도우의 크기를 50*50으로 설정
  start_color();
  init_pair(1,COLOR_BLUE,COLOR_BLUE); // 벽애 쓰일 색 지정
  init_pair(2,COLOR_WHITE,COLOR_WHITE); // 벽 모서리에 쓰일 색 지정
  init_pair(3,COLOR_YELLOW,COLOR_YELLOW); // snake 머리에 쓰일 색 지정
  init_pair(4,COLOR_MAGENTA,COLOR_MAGENTA); // snake 몸통에 쓰일 색 지정
  border('*','*','*','*','*','*','*','*');
  mvprintw(14,20,"Snake Game"); // 맵 위에 표시할 글

  win_map = newwin(21,42,15,4); // map을 나타낼 윈도우
  wbkgd(win_map, COLOR_PAIR(2)); // 맵의 바탕을 하얀색으로 설정

  wattron(win_map, COLOR_PAIR(1));
  wborder(win_map,'#','#','#','#','#','#','#','#'); // 맵의 테두리 설정
  wattroff(win_map,COLOR_PAIR(1));

  wattron(win_map, COLOR_PAIR(3));
  mvwprintw(win_map,10,10,"."); // snake의 머리
  wattroff(win_map,COLOR_PAIR(3));

  wattron(win_map, COLOR_PAIR(4));
  mvwprintw(win_map,10,11,".."); // snake의 몸통
  wattroff(win_map,COLOR_PAIR(4));

  refresh();
  wrefresh(win_map);
  getch();
  endwin();

  return 0;
}
