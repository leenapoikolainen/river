#include <iostream>
#include <fstream>
#include <cstring>
#include <cassert>

using namespace std;

#include "river.h"

int play_game() {
  // Initialise game
  char left[10] = "MMMCCCB";
  char **scene = make_river_scene(left,"");
  cout << "Game is starting: " << endl;
  print_scene(scene);

  char boat[3];
  int result = VALID_NONGOAL_STATE;
  while (result == VALID_NONGOAL_STATE) {
    cout << "Who do you want to load to the boat?: " ;
    cin >> boat; 
    result = perform_crossing(left, boat);
  }

  if (result == VALID_GOAL_STATE)
    return VALID_GOAL_STATE;
  
  else 
    return result;

}

bool boat_on_left(const char *left) {
  while (*left != '\0') {
    if (*left == 'B')
      return true;
    left++;
  }
  return false;
}

int num_of_characters(const char *left, char ch) {
  int count = 0;
  while (*left != '\0') {
    if (*left == ch)
      count++;
    left++;
  }
  return count;
}

void change_boat_side(char *left) {
  int size = strlen(left);
  if (boat_on_left(left)) {
    for (int i = 0; i < size; i++) {
      if (left[i] == 'B') {
        for (int j = i; j < size; j++)
          left[j] = left[j+1];
      }
    }
  }
  else {
    left[size] = 'B';
    left[size+1] = '\0';
  }
}

void unloading_to_left(int m, int c, char *left) {
  strcpy(left,"");
  for (int i = 0; i < m; i++) {
    left[i] = 'M';
  }
  for (int i = m; i < m + c; i++) {
    left[i] = 'C';
  }
  left[m+c] = 'B';
  left[m+c+1] = '\0';
}

int perform_crossing(char *left, const char *boat) {
  // 1. Load the boat
  // Test the boat string
  int on_boat = strlen(boat);
  if (on_boat == 0 || on_boat > 2)
    return ERROR_INVALID_MOVE;

  // Initial setting
  int m_left = num_of_characters(left, 'M');
  int c_left = num_of_characters(left, 'C');
  int m_right = 3 - m_left;
  int c_right = 3 - c_left;
  
  cout << "boat is " << boat << endl;
  // Loading to the boat
  int m_boat = num_of_characters(boat, 'M');
  int c_boat = num_of_characters(boat, 'C');

  // Check the boat position and reduce the number 
  bool boat_left = boat_on_left(left);
  if (boat_left){
    if (m_boat > m_left)
      return ERROR_INVALID_MISSIONARY_COUNT;
    if (c_boat > c_left)
      return ERROR_INVALID_CANNIBAL_COUNT;
   
    m_left = m_left - m_boat;
    c_left = c_left - c_boat;
   
    strcpy(left, "");
    for (int i = 0 ; i < m_left; i++) {
      left[i] = 'M';
    }
    for (int i = m_left; i < m_left + c_left; i++) {
     left[i] = 'C';
    }
    left[m_left + c_left] = 'B';
    left[m_left + c_left + 1] = '\0';
  }
  else {
    if (m_boat > m_right)
      return ERROR_INVALID_MISSIONARY_COUNT;
    if (c_boat > c_right)
      return ERROR_INVALID_CANNIBAL_COUNT;
    m_right = m_right - m_boat;
    c_right = c_right - c_boat;
  }
  // Print scene
  cout << "Loading the boat..." << endl;
  char **scene = make_river_scene(left, boat);
  print_scene(scene);
  cout << endl;

  // 2. Cross the river
  cout << "Crossing the river..." << endl;
  change_boat_side(left);
  scene = make_river_scene(left, boat);
  print_scene(scene);
  cout << endl;

  // 3. Unload the boat
  cout << "Unloading the boat..." << endl;
  if (boat_on_left(left)) {
    m_left += m_boat;
    c_left += c_boat;
    unloading_to_left(m_left, c_left, left);
  }
  else {
    m_right += m_boat;
    c_right += c_boat;
  }
  scene = make_river_scene(left,"");
  print_scene(scene);
 

  if ((c_right > m_right  && m_right !=0)|| (c_left > m_left && m_left != 0)) {
      return ERROR_MISSIONARIES_EATEN;
  }
  if (c_right == 3 && m_right == 3)
    return VALID_GOAL_STATE;
  else
  {
    return VALID_NONGOAL_STATE;
  }
}

char** make_river_scene(const char *left, const char *boat) {
  char **scene = create_scene();
  // Adding river scene basics
  add_to_scene(scene, 0, 0, "bank.txt");
  add_to_scene(scene, 0, 53, "bank.txt");
  add_to_scene(scene, 3, 30, "sun.txt");
  add_to_scene(scene, 19, 19, "river.txt");

  // read through left
  int m_left = 0, c_left = 0;
  bool boat_left = false;
  while (*left != '\0') {
    if (*left == 'M')
      m_left++;
    if (*left == 'C')
      c_left++;
    if (*left == 'B')
      boat_left = true;
    left++;
  }
  int m_boat = 0, c_boat = 0;

    // Draw boat
  int boat_column = 0;
  if (boat_left) {
    add_to_scene(scene, 17, 19, "boat.txt");
    boat_column = 22;
  }
  else {
    add_to_scene(scene, 17, 36, "boat.txt");
    boat_column = 39;
  }

  int count = 0;
  while (*boat != '\0') {
    if (*boat == 'M') {
      add_to_scene(scene, 11, (count * 6 + boat_column), "missionary.txt");
      m_boat++;
      count++;
    }
    if (*boat == 'C') {
      add_to_scene(scene, 11, (count * 6 + boat_column), "cannibal.txt");
      c_boat++;
      count++;
    }
    boat++;
  }

  int m_right = 3 - m_left - m_boat;
  int c_right = 3 - c_left - c_boat;


  
  // Draw left missionaries
  for (int i = 0, column = 1; i < m_left; i++, column = column + 6){
    add_to_scene(scene, 2, column, "missionary.txt");
  }
  // Draw left cannibals
  for (int i = 0, column = 1; i < c_left; i++, column = column + 6){
    add_to_scene(scene, 11, column, "cannibal.txt");
  }
  // Draw right missionaries
  for (int i = 0, column = 54; i < m_right; i++, column = column + 6){
    add_to_scene(scene, 2, column, "missionary.txt");
  }
  // Draw right cannibals
  for (int i = 0, column = 54; i < c_right; i++, column = column + 6){
    add_to_scene(scene, 11, column, "cannibal.txt");
  }
  

  return scene;
}

/* You are pre-supplied with the functions below. Add your own 
   function definitions to the end of this file. */

/* internal helper function which allocates a dynamic 2D array */
char **allocate_2D_array(int rows, int columns) {
  char **m = new (nothrow) char *[rows];
  assert(m);
  for (int r=0; r<rows; r++) {
    m[r] = new (nothrow) char[columns];
    assert(m[r]);
  }
  return m;
}

/* internal helper function which deallocates a dynamic 2D array */
void deallocate_2D_array(char **m, int rows) {
  for (int r=0; r<rows; r++)
    delete [] m[r];
  delete [] m;
}

/* pre-supplied function which creates an empty ASCII-art scene */
char **create_scene() {
  char **scene = allocate_2D_array(SCENE_HEIGHT, SCENE_WIDTH);

  for (int i=0; i<SCENE_HEIGHT; i++) 
    for (int j=0; j<SCENE_WIDTH; j++)
      scene[i][j] = ' ';

  return scene;
}

/* pre-supplied function which frees up memory allocated for an ASCII-art scene */
void destroy_scene(char **scene) {
  deallocate_2D_array(scene, SCENE_HEIGHT);
}

/* pre-supplied function which displays an ASCII-art scene */
void print_scene(char **scene) {
  for (int i=0; i<SCENE_HEIGHT; i++) {
    for (int j=0; j<SCENE_WIDTH; j++)
      cout << scene[i][j];
    cout << endl;
  }
}

/* helper function which removes carriage returns and newlines from strings */
void filter(char *line) {
  while (*line) {
    if (*line >= ' ')
      line++;
    else 
      *line = '\0';
  }
}

/* pre-supplied function which inserts an ASCII-art drawing stored in a file
   into a given ASCII-art scene starting at coordinates (row,col)  */
bool add_to_scene(char **scene, int row, int col, const char *filename) {

  ifstream in(filename);
  if (!in)
    return false;

  int start_col = col;
  char line[512];
  in.getline(line,512);
  filter(line);
  while (!in.fail()) {
    for (int n=0; n<strlen(line); n++) {
      if (row >= SCENE_HEIGHT)
	return false;
      if (col >= SCENE_WIDTH)
	break;
      scene[row][col++] = line[n];
    }
    row++;
    col = start_col;
    in.getline(line,512);
    filter(line);
  }
  return true;
}

/* pre-supplied helper function to report the status codes encountered in Question 3 */
const char *status_description(int code) {
  switch(code) {
  case ERROR_INVALID_CANNIBAL_COUNT:
    return "Invalid cannibal count";
  case ERROR_INVALID_MISSIONARY_COUNT:
    return "Invalid missionary count";
  case ERROR_INVALID_MOVE:
    return "Invalid move";
  case ERROR_MISSIONARIES_EATEN:
    return "The missionaries have been eaten! Oh dear!";
  case ERROR_INPUT_STREAM_ERROR:
    return "Unexpected input stream error";
  case ERROR_BONUS_NO_SOLUTION:
    return "No solution";
  case VALID_GOAL_STATE:
    return "Problem solved!";
  case VALID_NONGOAL_STATE:
    return "Valid state, everything is OK!";
  }
  return "Unknown error";
}


/* insert your functions here */

