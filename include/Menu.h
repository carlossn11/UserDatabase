#ifndef MENU_H_
#define MENU_H_
#include <ncurses.h>
#undef timeout

#include <array>
#include <cstdint>
#include <string>

#include "../include/Highlights.h"

enum Menus { Main = 0, Login };

class Menu {
 public:
  // TODO: Implementing the rule of 5
  Menu();
  ~Menu();
  // Refer to documentation in Menu.cpp
  void clear();
  void CheckWindowSize();
  void PrintTitle(const std::string &Title) const;
  char input() const;

  // This is the bread and butter of printing the menu to the window
  // a template it used to pass an array of any size (could have used vector didnt think
  // the extra overhead was worth it)
  // using window attribute on and off to highlight the currently selected menu option
  template <typename T, std::size_t SIZE>
  void print(const T &highlighted, const std::array<std::string, SIZE> &options) const {
    // Placement of text with respect of the y axis
    int8_t yPlacement = options.size() / 2;
    // loop starting at index 1 to avoid collisions with title
    for (int i = 1; i < options.size(); ++i) {
      // this is tenary since I might use a map later but checks if current index should
      // be the one highlighted
      if (highlighted == T(i)) wattron(win_, A_STANDOUT);
      mvwprintw(
          win_, (yMax_ / 4) - yPlacement, (xMax_ / 4) - (options[i].size() / 2),
          options[i].c_str());
      if (highlighted == T(i)) wattroff(win_, A_STANDOUT);
      --yPlacement;
    }
  }

 private:
  uint8_t width_;
  uint8_t height_;
  uint8_t xMax_;
  uint8_t yMax_;
  WINDOW* win_;
};

namespace {
template <typename T, std::size_t SIZE>
T MenuHandler(std::array<std::string, SIZE> MenuOptions, T option) {
  Menu menu;
  const int8_t ENTER_KEY = 10, UP_ARROW = 'A', DOWN_ARROW = 'B';
  char ch;
  // ^ Removes access to win_ which is what I want
  while ((ch = menu.input())) {
    menu.clear();
    menu.CheckWindowSize();
    menu.PrintTitle(MenuOptions[0]);
    switch (ch) {
      case UP_ARROW: {
        DecreaseHighlighted<T>(option);
        break;
      }
      case DOWN_ARROW: {
        IncreaseHighlighted<T>(option, MenuOptions.size() - 1);
        break;
      }
      case ENTER_KEY: {
        return option;
        break;
      }
      default: {
        break;
      }
    }
    // Calling print and passing size of array for template @param SIZE
    menu.print<T, MenuOptions.size()>(option, MenuOptions);
  }
  return option;
}
}  // namespace

#endif  // MENU_H_