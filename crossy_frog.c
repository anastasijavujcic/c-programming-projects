//------------------------------------------------------------------------------
// a4.c
//
// Program is made to simulate a Frogger-like game where a frog must cross
// road and water in order to reach the top of the map.
//
//
//------------------------------------------------------------------------------
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define ROAD 1
#define WATER 2
#define SAFE 3

#define LEFT 1
#define RIGHT 0
// Structures and enums ---------------------------------------------------------
typedef enum _ErrorMessages_
{
  CORRECT,
  OUT_OF_MEMORY,
  INVALID_NUM_OF_PARAM_GIVEN,
  UNEXPECTED_PARA_GIVEN,
  INVALID_PARA_GIVEN,
  INVALID_ARGUMENT
} ErrorMessages;

typedef struct _Frog_
{
  int row_;
  int column_;
} Frog;

typedef struct _Obstacle_ Obstacle;
typedef struct _Obstacle_
{
  int position_;
  int size_;
  int type_;
  int obstacle_direction_;
  int speed_;
  int valid_;
  Obstacle *next_;
  Obstacle *prev_;
} Obstacle;

typedef struct _LaneConfiguration_ LaneConfiguration;
typedef struct _LaneConfiguration_
{
  int lane_direction_;
  int speed_;
  int type_;
  Obstacle *obstacles_;
  LaneConfiguration *next_;
} LaneConfiguration;

typedef struct
{
  int height_, width_;
  int moves_;
  int street_min_, street_max_;
  int water_min_, water_max_;
  int seed_;
  char **gameField_;
  LaneConfiguration *lanes_;
  Frog frog_;
  int frog_row_;
  int game_running_;
  int initial_moves_;
} GameConfiguration;

// Forward declarations --------------------------------------------------------------------
ErrorMessages CommandLineArguments(int argc, char *argv[], GameConfiguration *configuration);
void printArgumentError(ErrorMessages res);
ErrorMessages parseSize(char *arg1, char *arg2, GameConfiguration *config);
ErrorMessages parseMoves(char *arg, GameConfiguration *config);
ErrorMessages parseStreetDistance(char *arg1, char *arg2, GameConfiguration *config);
ErrorMessages parseWaterDistance(char *arg1, char *arg2, GameConfiguration *config);
ErrorMessages parseSeed(char *arg, GameConfiguration *config);
char *readLine(void);
int getFirstObstaclePosition(LaneConfiguration *lane);
void newCar(LaneConfiguration *lane, int position, int tmpDirection, int tmpSpeed);
void newLog(LaneConfiguration *lane, int position, int length, int tmpDirection, int tmpSpeed);
void randomlySpawnObject(LaneConfiguration *lane, int position, GameConfiguration *configuration);
void intializationOfLanes(GameConfiguration *configuration);
void generateObstacles(GameConfiguration *configuration);
Obstacle *getLastObstacle(Obstacle *obstalces);
void printGameMap(GameConfiguration *configuration);
void frogCrawl(const char *direction, GameConfiguration *configuration);
void frogMove(Frog *frog, const char *direction, GameConfiguration *configuration);
void checkFrogStatus(GameConfiguration *configuration);
void takeCommand(GameConfiguration *configuration);
void updateMap(GameConfiguration *configuration);
void freeObstacles(Obstacle *obs);
void freeLanes(LaneConfiguration *lane);
void freeGame(GameConfiguration *configuration);
int isFrogOnLog(GameConfiguration *configuration, LaneConfiguration *lane);
int save(const char *filename, GameConfiguration *configuration);
int load(const char *filename, GameConfiguration *configuration);

//------------------------------------------------------------------------------
///
/// Returns the smallest integer greater than or equal to the given double.
/// Custom implementation of the ceiling function.
///
/// @param x input value
/// @return smallest integer >= x
//
int my_ceil(double x)
{
  int xi = (int)x;
  if (x == (double)xi)
    return xi;
  return (x > 0) ? xi + 1 : xi;
}

//------------------------------------------------------------------------------
///
/// Returns the largest integer less than or equal to the given double.
/// Custom implementation of the floor function.
///
/// @param x input value
/// @return largest integer <= x
//
int my_floor(double x)
{
  int xi = (int)x;
  if (x == (double)xi)
    return xi;
  return (x < 0) ? xi - 1 : xi;
}
//------------------------------------------------------------------------------
///
/// Parses a string to an integer. If the string is a valid integer,
/// stores the result in the output parameter.
///
/// @param arg string representing the integer
/// @param out pointer to integer where the result will be stored
/// @return 1 if parsing is successful, 0 otherwise
//
int parse_int(const char *arg, int *out)
{
  char *endptr;
  long val = strtol(arg, &endptr, 10);
  if (*endptr != '\0')
    return 0;
  *out = (int)val;
  return 1;
}
//------------------------------------------------------------------------------
///
/// Returns a random integer in the range [min, max].
/// If max < min, returns min.
///
/// @param min minimum value (inclusive)
/// @param max maximum value (inclusive)
/// @return random integer between min and max
//
int randomInt(int min, int max)
{
  if (max < min)
    return min;
  return min + rand() % (max - min + 1);
}
//------------------------------------------------------------------------------
///
/// Parses command-line arguments and sets values in the GameConfiguration struct.
/// Supports size, moves, street and water distances, and seed parameters.
/// Prints an error and returns an appropriate error code if parsing fails.
///
/// @param argc number of arguments
/// @param argv array of argument strings
/// @param configuration pointer to GameConfiguration struct to populate
///
/// @return appropriate ErrorMessages value
//
ErrorMessages CommandLineArguments(int argc, char *argv[], GameConfiguration *configuration)
{
  // default values
  configuration->height_ = 9;
  configuration->width_ = 40;
  configuration->moves_ = 50;
  configuration->street_min_ = 4;
  configuration->street_max_ = 20;
  configuration->water_min_ = 4;
  configuration->water_max_ = 8;
  configuration->seed_ = 0;
  configuration->frog_row_ = 100;

  for (int index = 1; index < argc; index++)
  {
    ErrorMessages res = CORRECT;

    if (strcmp(argv[index], "--size") == 0)
    {
      if (index + 2 >= argc)
        res = INVALID_NUM_OF_PARAM_GIVEN;
      else
        res = parseSize(argv[index + 1], argv[index + 2], configuration);
      if (res != CORRECT)
      {
        printArgumentError(res);
        return res;
      }
      index += 2;
    }
    else if (strcmp(argv[index], "--moves") == 0)
    {
      if (index + 1 >= argc)
        res = INVALID_NUM_OF_PARAM_GIVEN;
      else
        res = parseMoves(argv[index + 1], configuration);
      if (res != CORRECT)
      {
        printArgumentError(res);
        return res;
      }
      index++;
    }
    else if (strcmp(argv[index], "--street-distance") == 0)
    {
      if (index + 2 >= argc)
        res = INVALID_NUM_OF_PARAM_GIVEN;
      else
        res = parseStreetDistance(argv[index + 1], argv[index + 2], configuration);
      if (res != CORRECT)
      {
        printArgumentError(res);
        return res;
      }
      index += 2;
    }
    else if (strcmp(argv[index], "--water-distance") == 0)
    {
      if (index + 2 >= argc)
        res = INVALID_NUM_OF_PARAM_GIVEN;
      else
        res = parseWaterDistance(argv[index + 1], argv[index + 2], configuration);
      if (res != CORRECT)
      {
        printArgumentError(res);
        return res;
      }
      index += 2;
    }
    else if (strcmp(argv[index], "--seed") == 0)
    {
      if (index + 1 >= argc)
        res = INVALID_NUM_OF_PARAM_GIVEN;
      else
        res = parseSeed(argv[index + 1], configuration);
      if (res != CORRECT)
      {
        printArgumentError(res);
        return res;
      }
      index++;
    }
    else
    {
      printArgumentError(UNEXPECTED_PARA_GIVEN);
      return UNEXPECTED_PARA_GIVEN;
    }
  }

  configuration->initial_moves_ = configuration->moves_;
  return CORRECT;
}
//------------------------------------------------------------------------------
///
/// Prints a user interface readable error message based on the provided error code.
///
/// @param res the error code to explain
//
void printArgumentError(ErrorMessages res)
{
  switch (res)
  {
  case INVALID_PARA_GIVEN:
    printf("Invalid argument type!\n");
    break;
  case INVALID_ARGUMENT:
    printf("Invalid argument value!\n");
    break;
  case INVALID_NUM_OF_PARAM_GIVEN:
    printf("Invalid number of arguments given!\n");
    break;
  case UNEXPECTED_PARA_GIVEN:
    printf("Unexpected argument provided!\n");
    break;
  default:
    break;
  }
}
//------------------------------------------------------------------------------
///
/// Parses the height and width values for the map from command-line arguments.
/// Validates that both values are integers and at least 6.
///
/// @param arg1 string representing height
/// @param arg2 string representing width
/// @param config pointer to GameConfiguration to update
///
/// @return appropriate ErrorMessages value
//
ErrorMessages parseSize(char *arg1, char *arg2, GameConfiguration *config)
{
  int height, width;
  if (!parse_int(arg1, &height) || !parse_int(arg2, &width))
    return INVALID_PARA_GIVEN;
  if (height < 6 || width < 6)
    return INVALID_ARGUMENT;

  config->height_ = height;
  config->width_ = width;
  return CORRECT;
}
//------------------------------------------------------------------------------
///
/// Parses the number of moves from a command-line argument.
/// Validates that the value is a positive integer.
///
/// @param arg string representing the number of moves
/// @param config pointer to GameConfiguration to update
///
/// @return appropriate ErrorMessages value
//
ErrorMessages parseMoves(char *arg, GameConfiguration *config)
{
  int moves;
  if (!parse_int(arg, &moves))
    return INVALID_PARA_GIVEN;
  if (moves < 1)
    return INVALID_ARGUMENT;

  config->moves_ = moves;
  return CORRECT;
}
//------------------------------------------------------------------------------
///
/// Parses the minimum and maximum street distance values from command-line arguments.
/// Validates that values are integers, min >= 2 and max >= min.
///
/// @param arg1 string representing minimum distance
/// @param arg2 string representing maximum distance
/// @param config pointer to GameConfiguration to update
///
/// @return appropriate ErrorMessages value
//
ErrorMessages parseStreetDistance(char *arg1, char *arg2, GameConfiguration *config)
{
  int min, max;
  if (!parse_int(arg1, &min) || !parse_int(arg2, &max))
    return INVALID_PARA_GIVEN;
  if (min < 2 || max < min)
    return INVALID_ARGUMENT;

  config->street_min_ = min;
  config->street_max_ = max;
  return CORRECT;
}
//------------------------------------------------------------------------------
///
/// Parses the minimum and maximum water distance values from command-line arguments.
/// Validates that values are integers, min >= 2 and max >= min.
///
/// @param arg1 string representing minimum distance
/// @param arg2 string representing maximum distance
/// @param config pointer to GameConfiguration to update
///
/// @return appropriate ErrorMessages value
//
ErrorMessages parseWaterDistance(char *arg1, char *arg2, GameConfiguration *config)
{
  int min, max;
  if (!parse_int(arg1, &min) || !parse_int(arg2, &max))
    return INVALID_PARA_GIVEN;
  if (min < 2 || max < min)
    return INVALID_ARGUMENT;

  config->water_min_ = min;
  config->water_max_ = max;
  return CORRECT;
}
//------------------------------------------------------------------------------
///
/// Parses the seed value for the random number generator.
/// Validates that the value is a valid integer.
///
/// @param arg string representing the seed
/// @param config pointer to GameConfiguration to update
///
/// @return appropriate ErrorMessages value
//
ErrorMessages parseSeed(char *arg, GameConfiguration *config)
{
  int seed;
  if (!parse_int(arg, &seed))
    return INVALID_PARA_GIVEN;

  config->seed_ = seed;
  return CORRECT;
}
//------------------------------------------------------------------------------
///
/// Reads a full line from standard input dynamically.
/// Allocates memory as needed and returns a null-terminated string.
///
/// @return pointer to the read string (must be freed), or NULL on failure
//
char *readLine(void)
{
  char *buffer = NULL;
  size_t size = 0, capacity = 16;
  int ch;
  buffer = malloc(capacity);
  if (!buffer)
    return NULL;

  while ((ch = getchar()) != '\n' && ch != EOF)
  {
    if (size + 1 >= capacity)
    {
      capacity *= 2;
      char *new_buffer = realloc(buffer, capacity);
      if (!new_buffer)
      {
        free(buffer);
        return NULL;
      }
      buffer = new_buffer;
    }
    buffer[size++] = (char)ch;
  }

  if (size == 0 && ch == EOF)
  {
    free(buffer);
    return NULL;
  }
  buffer[size] = '\0';
  return buffer;
}
//------------------------------------------------------------------------------
///
/// Returns the position of the first obstacle in the lane based on direction.
/// Used to determine if there is enough space to spawn a new obstacle.
///
/// @param lane pointer to a LaneConfiguration
/// @return position of the first obstacle or a large value if none exist
//
int getFirstObstaclePosition(LaneConfiguration *lane)
{
  Obstacle *obstacle = lane->obstacles_;
  if (!obstacle)
    return INT_MAX;

  int min_pos = obstacle->position_ - obstacle->size_ + 1;
  for (; obstacle; obstacle = obstacle->next_)
  {
    int start = (lane->lane_direction_ == RIGHT) ? obstacle->position_ - obstacle->size_ + 1 : obstacle->position_;
    if (start < min_pos)
      min_pos = start;
  }
  return min_pos;
}
//------------------------------------------------------------------------------
///
/// Creates and inserts a new car obstacle at the beginning of the lane's obstacle list.
/// The car has fixed size 5 and is inserted with given direction and speed.
///
/// @param lane pointer to the lane where the car should be added
/// @param position starting position of the car
/// @param tmp_direction direction of the car (LEFT or RIGHT)
/// @param tmp_speed speed of the car
//
void newCar(LaneConfiguration *lane, int position, int tmp_direction, int tmp_speed)
{
  Obstacle *car = malloc(sizeof(Obstacle));
  if (car == NULL)
  {
    printf("Memory not allocated\n");
    return;
  }

  car->position_ = position;
  car->size_ = 5;
  car->type_ = 0;
  car->obstacle_direction_ = tmp_direction;
  car->speed_ = tmp_speed;
  car->valid_ = 1;
  car->next_ = lane->obstacles_;
  car->prev_ = NULL;
  if (lane->obstacles_)
  {
    lane->obstacles_->prev_ = car;
  }
  lane->obstacles_ = car;
}
//------------------------------------------------------------------------------
///
/// Creates and inserts a new log obstacle at the beginning of the lane's obstacle list.
/// The log is inserted with given length, direction, and speed.
///
/// @param lane pointer to the lane where the log should be added
/// @param position starting position of the log
/// @param length length of the log (number of characters it occupies)
/// @param tmp_direction direction of the log (LEFT or RIGHT)
/// @param tmp_speed speed of the log
//
void newLog(LaneConfiguration *lane, int position, int length, int tmp_direction, int tmp_speed)
{
  Obstacle *log = malloc(sizeof(Obstacle));
  if (log == NULL)
  {
    printf("Memory not allocated\n");
    return;
  }
  log->position_ = position;
  log->size_ = length;
  log->type_ = 1;
  log->obstacle_direction_ = tmp_direction;
  log->speed_ = tmp_speed;
  log->valid_ = 1;
  log->next_ = lane->obstacles_;
  log->prev_ = NULL;
  if (lane->obstacles_)
  {
    lane->obstacles_->prev_ = log;
  }
  lane->obstacles_ = log;
}

//------------------------------------------------------------------------------
///
/// Randomly decides whether to spawn a new obstacle (car or log) on the given lane
/// at the specified position, based on configured distances and current obstacles.
/// Spawning is skipped if minimum distance condition is not met.
///
/// @param lane pointer to the lane where the obstacle may be spawned
/// @param position column position to check/spawn at
/// @param configuration pointer to the current game configuration
//
void randomlySpawnObject(LaneConfiguration *lane, int position, GameConfiguration *configuration)
{
  int min_dist, max_dist;
  if (lane->type_ == ROAD)
  {
    min_dist = configuration->street_min_;
    max_dist = configuration->street_max_;
  }
  else if (lane->type_ == WATER)
  {
    min_dist = configuration->water_min_;
    max_dist = configuration->water_max_;
  }
  else
  {
    return;
  }

  int first_obstacle_position = getFirstObstaclePosition(lane);
  if (first_obstacle_position - position <= min_dist)
    return;

  int upper_limit_for_random = 0;
  if (lane->obstacles_ == NULL)
    upper_limit_for_random = position - configuration->width_ + max_dist + 1;
  else
    upper_limit_for_random = position - first_obstacle_position + max_dist + 1;

  int random = randomInt(0, upper_limit_for_random);
  if (random == 0)
  {
    if (lane->type_ == ROAD)
    {
      newCar(lane, position, lane->lane_direction_, lane->speed_);
    }
    else if (lane->type_ == WATER)
    {
      int length = randomInt(3, 10);
      int log_pos;
      if (lane->lane_direction_ == LEFT)
      {
        log_pos = position;
      }
      else
      {
        log_pos = position;
      }
      newLog(lane, log_pos, length, lane->lane_direction_, lane->speed_);
    }
  }
}
//------------------------------------------------------------------------------
///
/// Initializes all lanes of the game map based on configuration height.
/// Randomly assigns type, direction and speed to each lane. SAFE zones are
/// placed at the top, middle, and bottom.
///
/// @param configuration pointer to the game configuration where lanes will be stored
//
void intializationOfLanes(GameConfiguration *configuration)
{
  LaneConfiguration *previous = NULL;
  int total_lanes = configuration->height_;
  int water_lanes = my_ceil((configuration->height_ - 3) / 2.0);

  for (int lane_index = 0; lane_index < total_lanes; lane_index++)
  {
    LaneConfiguration *new_lane = malloc(sizeof(LaneConfiguration));
    if (new_lane == NULL)
    {
      printf("Memory not allocated\n");
      return;
    }
    new_lane->next_ = NULL;
    new_lane->obstacles_ = NULL;
    if (lane_index == 0)
    {
      new_lane->type_ = SAFE;
    }
    else if (lane_index > 0 && lane_index <= water_lanes)
    {
      new_lane->type_ = WATER;
    }
    else if (lane_index == water_lanes + 1)
    {
      new_lane->type_ = SAFE;
    }
    else if (lane_index > water_lanes + 1 && lane_index < total_lanes - 1)
    {
      new_lane->type_ = ROAD;
    }
    else
    {
      new_lane->type_ = SAFE;
    }

    new_lane->lane_direction_ = randomInt(0, 1);
    new_lane->speed_ = randomInt(1, 4);
    if (!previous)
    {
      configuration->lanes_ = new_lane;
    }
    else
    {
      previous->next_ = new_lane;
    }
    previous = new_lane;
  }
}
//------------------------------------------------------------------------------
///
/// Iterates through all lanes and attempts to spawn obstacles at each position
/// from right to left (high to low index), skipping SAFE lanes.
///
/// @param configuration pointer to the current game configuration
//
void generateObstacles(GameConfiguration *configuration)
{
  LaneConfiguration *lane = configuration->lanes_;
  int width = configuration->width_;

  while (lane)
  {
    if (lane->type_ == SAFE)
    {
      lane = lane->next_;
      continue;
    }
    for (int pos = width - 1; pos >= 0; pos--)
    {
      randomlySpawnObject(lane, pos, configuration);
    }
    lane = lane->next_;
  }
}
//------------------------------------------------------------------------------
///
/// Returns the last obstacle in a doubly linked list of obstacles.
/// If the list is empty, returns NULL.
///
/// @param obstacles pointer to the head of the obstacle list
/// @return pointer to the last obstacle in the list, or NULL if list is empty
//
Obstacle *getLastObstacle(Obstacle *obstalces)
{
  Obstacle *obstacle = obstalces;

  if (obstalces == NULL)
    return NULL;

  while (obstacle->next_ != NULL)
  {
    obstacle = obstacle->next_;
  }

  return obstacle;
}
//------------------------------------------------------------------------------
///
/// Renders the full game map to the terminal, including lanes, obstacles,
/// the frog (with body and feet), and background elements.
/// Each lane is printed twice (duplicated rows) to simulate visual height.
///
/// @param configuration pointer to the current game configuration
//
void printGameMap(GameConfiguration *configuration)
{
  LaneConfiguration *lane = configuration->lanes_;
  int lane_num = 0;

  for (lane = configuration->lanes_; lane != NULL; lane = lane->next_, lane_num++)
  {
    for (int lane_duplication_index = 0; lane_duplication_index < 2; lane_duplication_index++)
    {
      if (lane->type_ != SAFE)
      {
        printf("%d%s ", lane->speed_, (lane->lane_direction_ == RIGHT) ? ">" : "<");
      }
      else
      {
        printf("   ");
      }
      printf("#");

      for (int column_index = 0; column_index < configuration->width_; column_index++)
      {
        int printed = 0;
        int current_row = lane_num * 2 + lane_duplication_index;
        // ---------- FROG RENDERING ----------
        if (current_row == configuration->frog_.row_ &&
            (column_index == configuration->frog_.column_ || column_index == configuration->frog_.column_ + 1))
        {
          printf("o");
          printed = 1;
          continue;
        }

        // FROG BOTTOM (/\\)
        if (current_row == configuration->frog_.row_ + 1 &&
            (column_index == configuration->frog_.column_ || column_index == configuration->frog_.column_ + 1))
        {
          if (column_index == configuration->frog_.column_)
            printf("/");
          else
            printf("\\");
          printed = 1;
          continue;
        }
        // ---------- OBSTACLE RENDERING ----------
        Obstacle *obstacle = (lane->lane_direction_ == RIGHT) ? lane->obstacles_ : getLastObstacle(lane->obstacles_);
        while (obstacle)
        {
          int obstacle_start_col, obstacle_end_col;

          if (lane->lane_direction_ == RIGHT)
          {
            obstacle_start_col = obstacle->position_ - obstacle->size_ + 1;
            obstacle_end_col = obstacle->position_;
          }
          else
          {
            obstacle_start_col = configuration->width_ - obstacle->position_ - 1;
            obstacle_end_col = obstacle_start_col + obstacle->size_ - 1;
          }

          if (column_index >= obstacle_start_col && column_index <= obstacle_end_col)
          {
            if (obstacle->type_ == 1) // Log
            {
              printf("B");
              printed = 1;
              break;
            }
            else // Car
            {
              int car_segment_index = (lane->lane_direction_ == RIGHT) ? (obstacle_end_col - column_index) : (column_index - obstacle_start_col);

              if (lane_duplication_index == 0)
              {
                if (lane->lane_direction_ == RIGHT)
                {
                  const char *car_top = "_\\O||";
                  printf("%c", car_top[car_segment_index]);
                }
                else
                {
                  const char *car_top = "_/O||";
                  printf("%c", car_top[car_segment_index]);
                }
              }
              else
              {
                const char *car_bottom = "O---O";
                printf("%c", car_bottom[car_segment_index]);
              }
              printed = 1;
              break;
            }
          }
          obstacle = (lane->lane_direction_ == RIGHT) ? obstacle->next_ : obstacle->prev_;
        }
        // ---------- BACKGROUND ----------
        if (!printed)
        {
          if (lane->type_ == WATER)
            printf("~");
          else if (lane->type_ == SAFE)
            printf("░");
          else
            printf(" ");
        }
      }
      printf("#\n");
    }
  }
  // ---------- MOVES ----------
  printf("\nMoves left: %d\n", configuration->moves_);
}
//------------------------------------------------------------------------------
///
/// Moves the frog by one unit to the left or right (unlike the regular move
/// which moves by 2 units). If the command is valid and the move is within
/// map bounds, the frog's position is updated, move count is decremented,
/// and the map is updated.
///
/// @param direction movement direction: "left" or "right"
/// @param configuration pointer to the current game configuration
//
void frogCrawl(const char *direction, GameConfiguration *configuration)
{
  int new_column = configuration->frog_.column_;
  int moved = 1;

  if (strcmp(direction, "left") == 0 && new_column >= 1)
  {
    new_column -= 1;
    moved = 0;
  }
  else if (strcmp(direction, "right") == 0 && new_column + 1 < configuration->width_)
  {
    new_column += 1;
    moved = 0;
  }
  else
  {
    printf("Error: Unknown command!\n");
  }

  if (moved == 0)
  {
    configuration->frog_.column_ = new_column;
    configuration->moves_--;

    updateMap(configuration);
  }
}
//------------------------------------------------------------------------------
///
/// Moves the frog in the given direction if possible. Handles all movement
/// commands: up, down, left, right, and stay. If the frog is on a log and
/// moves with it, it is handled accordingly.
/// Decreases the number of remaining moves and updates the map.
///
/// @param frog pointer to the frog struct
/// @param direction movement command ("up", "down", "left", "right", "stay")
/// @param configuration pointer to the current game configuration
//
void frogMove(Frog *frog, const char *direction, GameConfiguration *configuration)
{
  int new_row = frog->row_;
  int new_column = frog->column_;
  int frog_move_with_log = 1;

  if (strcmp(direction, "up") == 0 && new_row >= 2)
    new_row -= 2;
  else if (strcmp(direction, "down") == 0 && new_row + 2 < configuration->height_ * 2)
    new_row += 2;
  else if (strcmp(direction, "left") == 0 && new_column >= 1)
  {
    if (frog->column_ >= 2)
    {
      new_column -= 2;
    }
    else
    {
      new_column = 0;
    }
  }

  else if (strcmp(direction, "right") == 0 && new_column + 2 < configuration->width_)
    new_column += 2;
  else if (strcmp(direction, "stay") == 0)
  {
    frog_move_with_log = 0;
  }

  if (frog_move_with_log || strcmp(direction, "stay") != 0)
  {
    frog->row_ = new_row;
    configuration->frog_row_ = new_row;
    frog->column_ = new_column;
  }
  configuration->moves_--;

  updateMap(configuration);
}
//------------------------------------------------------------------------------
///
/// Checks if the frog is currently standing on a log in a given lane.
/// Compares the frog's current column positions with the bounds of any log obstacle.
///
/// @param configuration pointer to the current game configuration
/// @param lane pointer to the lane to check
/// @return 1 if the frog is on a log, 0 otherwise
//
int isFrogOnLog(GameConfiguration *configuration, LaneConfiguration *lane)
{
  Obstacle *obstacle = lane->obstacles_;
  while (obstacle)
  {
    if (obstacle->type_ == 1)
    {
      int start = (lane->lane_direction_ == RIGHT) ? (obstacle->position_ - obstacle->size_ + 1) : obstacle->position_;
      int end = (lane->lane_direction_ == RIGHT) ? obstacle->position_ : (obstacle->position_ + obstacle->size_ - 1);
      if ((configuration->frog_.column_ >= start && configuration->frog_.column_ <= end) ||
          (configuration->frog_.column_ + 1 >= start && configuration->frog_.column_ + 1 <= end))
      {
        return 1;
      }
    }
    obstacle = obstacle->next_;
  }
  return 0;
}
//------------------------------------------------------------------------------
///
/// Checks the frog's current position and determines if the game should end.
/// Ends the game if the frog reaches the top SAFE lane, gets hit by a car on a ROAD,
/// or falls into water without being on a log.
///
/// @param configuration pointer to the current game configuration
//
void checkFrogStatus(GameConfiguration *configuration)
{
  if (!configuration->game_running_)
    return;

  int frog_lane_index = configuration->frog_.row_ / 2;
  LaneConfiguration *lane = configuration->lanes_;

  for (int index = 0; index < frog_lane_index && lane; index++)
  {
    lane = lane->next_;
  }

  if (!lane)
    return;

  if (frog_lane_index == 0 && lane->type_ == SAFE)
  {
    printf("Congratulations! You made it to the top!\n\n");
    configuration->game_running_ = 0;
    return;
  }

  if (lane->type_ == ROAD)
  {
    Obstacle *obs = lane->obstacles_;
    while (obs)
    {
      int start, end;

      if (lane->lane_direction_ == RIGHT)
      {
        start = obs->position_ - obs->size_ + 1;
        end = obs->position_;
      }
      else // LEFT
      {
        start = configuration->width_ - obs->position_ - 1;
        end = start + obs->size_ - 1;
      }
      if ((configuration->frog_.column_ >= start && configuration->frog_.column_ <= end) ||
          (configuration->frog_.column_ + 1 >= start && configuration->frog_.column_ + 1 <= end))
      {
        printf("Game Over! You got run over!\n\n");
        configuration->game_running_ = 0;
        return;
      }
      obs = obs->next_;
    }
  }
  if (lane->type_ == WATER)
  {
    int on_log = 0;
    Obstacle *obs = lane->obstacles_;
    while (obs)
    {
      int start = (lane->lane_direction_ == RIGHT) ? (obs->position_ - obs->size_ + 1) : obs->position_;
      int end = (lane->lane_direction_ == RIGHT) ? obs->position_ : (obs->position_ + obs->size_ - 1);
      if ((configuration->frog_.column_ >= start && configuration->frog_.column_ <= end) ||
          (configuration->frog_.column_ + 1 >= start && configuration->frog_.column_ + 1 <= end))
      {
        on_log = 1;
        break;
      }
      obs = obs->next_;
    }

    if (!on_log)
    {
      printf("Game Over! You got flushed from the map!\n\n");
      configuration->game_running_ = 0;
      return;
    }
  }
}
//------------------------------------------------------------------------------
///
/// Initializes and starts the game. Resets game state, initializes lanes and obstacles,
/// places the frog at the bottom SAFE zone, and prints the initial game map.
///
/// @param configuration pointer to the current game configuration
//
void gameStart(GameConfiguration *configuration)
{
  // Clean up previous game state
  freeLanes(configuration->lanes_);
  configuration->lanes_ = NULL;

  printf("The Game started! Hop fast, dodge smart and reach the top!\n\n");
  configuration->game_running_ = 1;
  configuration->moves_ = configuration->initial_moves_;
  intializationOfLanes(configuration);
  generateObstacles(configuration);

  LaneConfiguration *lane = configuration->lanes_;
  int row_index = 0, bottom_safe_row = 0;
  for (; lane != NULL; lane = lane->next_, row_index++)
  {
    if (lane->type_ == SAFE)
      bottom_safe_row = row_index;
  }
  configuration->frog_.row_ = bottom_safe_row * 2;
  configuration->frog_.column_ = configuration->width_ / 2 - 1;
  printGameMap(configuration);
}
//------------------------------------------------------------------------------
///
/// Handles a standard movement command (up, down, left, right, stay).
/// Moves the frog, checks the game status (win/loss), and prints the updated map.
///
/// @param input movement command string
/// @param configuration pointer to the current game configuration
//
void gameMove(char *input, GameConfiguration *configuration)
{
  if (!configuration->game_running_)
  {
    printf("Error: Game is not running!\n");
    return;
  }
  frogMove(&configuration->frog_, input, configuration);
  checkFrogStatus(configuration);
  if (configuration->moves_ <= 0 && configuration->game_running_)
  {
    printf("Game Over! You ran out of moves!\n\n");
    configuration->game_running_ = 0;
  }
  printGameMap(configuration);
}
//------------------------------------------------------------------------------
///
/// Handles the "crawl" command for one-tile horizontal movement (left or right).
/// Validates input, moves the frog, checks game status, and updates the map.
///
/// @param input full input string starting with "crawl "
/// @param configuration pointer to the current game configuration
//
void gameCrawlMove(char *input, GameConfiguration *configuration)
{
  char *direction = input + 6;
  if (strlen(direction) == 0)
  {
    printf("Error: Command is missing arguments!\n");
  }
  else if (!configuration->game_running_)
  {
    printf("Error: Game is not running!\n");
  }
  else if (strcmp(direction, "left") != 0 && strcmp(direction, "right") != 0)
  {
    printf("Error: Unknown command!\n");
  }
  else
  {
    frogCrawl(direction, configuration);
    checkFrogStatus(configuration);
    if (!configuration->game_running_)
    {
      printGameMap(configuration);
      return;
    }
    if (configuration->moves_ <= 0 && configuration->game_running_)
    {
      printf("Game Over! You ran out of moves!\n\n");
      printGameMap(configuration);
      configuration->game_running_ = 0;
      return;
    }
    printGameMap(configuration);
  }
}
//------------------------------------------------------------------------------
///
/// Processes the "save" command by writing the current game state to a file.
/// Checks that the game is running and that a filename is provided.
///
/// @param input full input string starting with "save "
/// @param configuration pointer to the current game configuration
//
void handleSaveCommand(char *input, GameConfiguration *configuration)
{
  if (!configuration->game_running_)
  {
    printf("Error: Game is not running!\n");
    return;
  }
  char *filename = input + 5;
  if (strlen(filename) == 0)
  {
    printf("Error: Command is missing arguments!\n");
  }
  else
  {
    save(filename, configuration);
  }
}
//------------------------------------------------------------------------------
///
/// Processes the "load" command by loading a saved game state from a file.
/// Checks that the game is running and that a filename is provided, then
/// prints the loaded map.
///
/// @param input full input string starting with "load "
/// @param configuration pointer to the current game configuration
//
void handleLoadCommand(char *input, GameConfiguration *configuration)
{
  if (!configuration->game_running_)
  {
    printf("Error: Game is not running!\n");
    return;
  }
  char *filename = input + 5;
  if (strlen(filename) == 0)
  {
    printf("Error: Command is missing arguments!\n");
  }
  else
  {
    load(filename, configuration);
    printGameMap(configuration);
  }
}
//------------------------------------------------------------------------------
///
/// Checks if the input string is one of the valid movement commands:
/// "up", "down", "left", "right", or "stay".
///
/// @param input command string
/// @return 1 if valid movement command, 0 otherwise
//
int takeMove(const char *input)
{
  return strcmp(input, "up") == 0 || strcmp(input, "down") == 0 ||
         strcmp(input, "left") == 0 || strcmp(input, "right") == 0 ||
         strcmp(input, "stay") == 0;
}
//------------------------------------------------------------------------------
///
/// Main input loop of the game. Waits for user input, interprets commands,
/// and dispatches them to the appropriate game control functions.
///
/// Supported commands:
///   - start
///   - quit
///   - up/down/left/right/stay
///   - crawl left/right
///   - save <filename>
///   - load <filename>
///
/// @param configuration pointer to the current game configuration
//
void takeCommand(GameConfiguration *configuration)
{
  while (1)
  {
    printf(" > ");
    fflush(stdout);

    char *input = readLine();
    if (input == NULL)
      break;

    if (strcmp(input, "start") == 0)
    {
      gameStart(configuration);
    }
    else if (strcmp(input, "quit") == 0)
    {
      printf("Bye!\n");
      free(input);
      break;
    }
    else if (takeMove(input))
    {
      gameMove(input, configuration);
    }
    else if (strncmp(input, "crawl ", 6) == 0)
    {
      gameCrawlMove(input, configuration);
    }
    else if (strncmp(input, "save", 4) == 0)
    {
      handleSaveCommand(input, configuration);
    }
    else if (strncmp(input, "load", 4) == 0)
    {
      handleLoadCommand(input, configuration);
    }
    else
    {
      printf("Unknown command.\n");
    }
    free(input);
  }
}

//------------------------------------------------------------------------------
///
/// Updates the game map by moving obstacles according to their speed and direction.
/// Also checks if the frog is on a log and should be carried with it.
/// Spawns new obstacles if space allows.
///
/// @param configuration pointer to the current game configuration
//
void updateMap(GameConfiguration *configuration)
{
  LaneConfiguration *lane = configuration->lanes_;
  int width = configuration->width_;
  int frogLaneIndex = configuration->frog_.row_ / 2;
  int laneIndex = 0;

  while (lane)
  {
    if (lane->type_ == SAFE)
    {
      lane = lane->next_;
      laneIndex++;
      continue;
    }

    int frogOnThisLane = (laneIndex == frogLaneIndex);
    int wasOnLog = 0;

    if (frogOnThisLane && lane->type_ == WATER)
    {
      wasOnLog = isFrogOnLog(configuration, lane);
    }

    for (int index = 0; index < lane->speed_; index++)
    {
      // Move
      Obstacle *obs = lane->obstacles_;
      while (obs)
      {
        obs->position_ += 1;
        obs = obs->next_;
      }

      Obstacle *curr = lane->obstacles_;
      while (curr)
      {
        Obstacle *next = curr->next_;
        if ((lane->lane_direction_ == RIGHT && curr->position_ - curr->size_ + 1 >= width) ||
            (lane->lane_direction_ == LEFT && curr->position_ < 0))
        {
          if (curr->prev_)
            curr->prev_->next_ = curr->next_;
          else
            lane->obstacles_ = curr->next_;
          if (curr->next_)
            curr->next_->prev_ = curr->prev_;
          free(curr);
        }
        curr = next;
      }
      if (wasOnLog && frogOnThisLane)
      {
        if (lane->lane_direction_ == RIGHT && configuration->frog_.column_ + 2 < width)
          configuration->frog_.column_++;
        else if (lane->lane_direction_ == LEFT && configuration->frog_.column_ > 0)
          configuration->frog_.column_--;
      }
      randomlySpawnObject(lane, 0, configuration);
    }
    lane = lane->next_;
    laneIndex++;
  }
}
//------------------------------------------------------------------------------
///
/// Frees all dynamically allocated memory for a linked list of obstacles.
///
/// @param obs pointer to the first obstacle in the list
//
void freeObstacles(Obstacle *obs)
{
  while (obs)
  {
    Obstacle *next = obs->next_;
    free(obs);
    obs = next;
  }
}
//------------------------------------------------------------------------------
///
/// Frees all lanes and their associated obstacles in the game configuration.
///
/// @param lane pointer to the first lane in the list
//
void freeLanes(LaneConfiguration *lane)
{
  while (lane)
  {
    LaneConfiguration *next = lane->next_;
    freeObstacles(lane->obstacles_);
    free(lane);
    lane = next;
  }
}
//------------------------------------------------------------------------------
///
/// Frees all dynamically allocated memory associated with the game configuration,
/// including lanes, obstacles, and the 2D game field.
///
/// @param configuration pointer to the current game configuration
//
void freeGame(GameConfiguration *configuration)
{
  freeLanes(configuration->lanes_);

  if (configuration->gameField_)
  {
    for (int height_index = 0; height_index < configuration->height_; height_index++)
    {
      free(configuration->gameField_[height_index]);
    }
    free(configuration->gameField_);
  }
}
//------------------------------------------------------------------------------
///
/// Saves the current game state to a binary file, including frog position,
/// game configuration (map size, moves, distances), and all lanes with
/// obstacles. Uses a custom binary format with a magic header.
///
/// @param filename the name of the file to save to
/// @param configuration pointer to the current game configuration
/// @return 1 on success, 0 on failure
//
int save(const char *filename, GameConfiguration *configuration)
{
  if (!configuration->game_running_)
  {
    printf("Error: Game is not running!\n");
    return 0;
  }
  FILE *file = fopen(filename, "wb");
  if (!file)
  {
    printf("Error opening file for saving!\n");
    return 0;
  }
  fwrite("ESP\n", 1, 4, file);
  fwrite(&configuration->height_, 4, 1, file);
  fwrite(&configuration->width_, 4, 1, file);
  fwrite(&configuration->street_min_, 4, 1, file);
  fwrite(&configuration->street_max_, 4, 1, file);
  fwrite(&configuration->water_min_, 4, 1, file);
  fwrite(&configuration->water_max_, 4, 1, file);
  fwrite(&configuration->moves_, 4, 1, file);

  int frog_lane = configuration->frog_.row_ / 2;
  fwrite(&frog_lane, 4, 1, file);
  fwrite(&configuration->frog_.column_, 4, 1, file);

  LaneConfiguration *lane = configuration->lanes_;
  while (lane)
  {
    unsigned char info = 0;
    info |= (lane->type_ << 6);
    info |= (lane->lane_direction_ << 3);
    info |= ((lane->speed_ - 1) & 0x07) << 4;
    info |= (lane->obstacles_ ? 1 : 0);

    fwrite(&info, 1, 1, file);
    Obstacle *obstacle = lane->obstacles_;
    while (obstacle)
    {
      unsigned char obstacle_info = 0;
      obstacle_info |= (obstacle->type_ == 1 ? 1 : 0) << 1;
      obstacle_info |= (obstacle->next_ ? 1 : 0);
      fwrite(&obstacle_info, 1, 1, file);
      fwrite(&obstacle->position_, 4, 1, file);
      fwrite(&obstacle->size_, 4, 1, file);
      obstacle = obstacle->next_;
    }
    lane = lane->next_;
  }
  fclose(file);
  printf("Game saved successfully!\n\n");
  return 1;
}
//------------------------------------------------------------------------------
///
/// Loads a saved game state from a binary file. Frees any existing map data,
/// then reconstructs the game configuration and all lanes/obstacles.
/// Validates file with a magic header ("ESP\\n").
///
/// @param filename the name of the file to load from
/// @param configuration pointer to the game configuration to populate
/// @return 1 on success, 0 on failure
//
int load(const char *filename, GameConfiguration *configuration)
{
  FILE *file = fopen(filename, "rb");
  if (!file)
  {
    printf("Error opening file for loading!\n");
    return 0;
  }
  char magic[4];
  if (fread(magic, 1, 4, file) || strncmp(magic, "ESP\n", 4) != 0)
  {
    printf("Error: Invalid file content!\n");
    fclose(file);
    return 0;
  }

  freeGame(configuration);
  memset(configuration, 0, sizeof(GameConfiguration));

  fread(&configuration->height_, 4, 1, file);
  fread(&configuration->width_, 4, 1, file);
  fread(&configuration->street_min_, 4, 1, file);
  fread(&configuration->street_max_, 4, 1, file);
  fread(&configuration->water_min_, 4, 1, file);
  fread(&configuration->water_max_, 4, 1, file);
  fread(&configuration->moves_, 4, 1, file);

  int frog_lane, frog_column;
  fread(&frog_lane, 4, 1, file);
  fread(&frog_column, 4, 1, file);
  configuration->frog_.row_ = frog_lane * 2;
  configuration->frog_.column_ = frog_column;

  LaneConfiguration *previous = NULL;
  for (int index = 0; index < configuration->height_; index++)
  {
    LaneConfiguration *lane = malloc(sizeof(LaneConfiguration));
    memset(lane, 0, sizeof(LaneConfiguration));

    unsigned char info;
    fread(&info, 1, 1, file);
    lane->type_ = (info >> 6) & 0x03;
    lane->lane_direction_ = (info >> 3) & 0x01;
    lane->speed_ = ((info >> 4) & 0x07) + 1;

    if (info & 0x1)
    {
      Obstacle *last_obstacle = NULL;
      while (1)
      {
        Obstacle *obstacle = malloc(sizeof(Obstacle));
        unsigned char obstacle_info;
        fread(&obstacle_info, 1, 1, file);
        fread(&obstacle->position_, 4, 1, file);
        fread(&obstacle->size_, 4, 1, file);

        obstacle->type_ = (obstacle_info >> 1) & 0x01;
        obstacle->next_ = NULL;
        obstacle->prev_ = last_obstacle;

        if (last_obstacle)
        {
          last_obstacle->next_ = obstacle;
        }
        else
        {
          lane->obstacles_ = obstacle;
        }
        last_obstacle = obstacle;
        if (!(obstacle_info & 0x02))
          break;
      }
    }
    lane->next_ = NULL;
    if (previous)
      previous->next_ = lane;
    else
      configuration->lanes_ = lane;
    previous = lane;
  }
  configuration->game_running_ = 1;
  printf("Game loaded successfully!\n\n");
  fclose(file);
  return 1;
}
//------------------------------------------------------------------------------
///
/// Entry point of the Crossy Frog game. Parses command-line arguments,
/// initializes game configuration and memory, and launches the command loop.
/// Cleans up all memory before exiting.
///
/// @param argc number of command-line arguments
/// @param argv array of command-line argument strings
/// @return error code (0 if successful, non-zero otherwise)
//
int main(int argc, char **argv)
{
  GameConfiguration configuration;
  memset(&configuration, 0, sizeof(GameConfiguration));

  ErrorMessages result = CommandLineArguments(argc, argv, &configuration);
  if (result != CORRECT)
    return result;

  srand(configuration.seed_);

  printf("Welcome to Crossy Frog! Get ready for an adventure!\n\n");

  configuration.gameField_ = malloc(configuration.height_ * sizeof(char *));
  for (int i = 0; i < configuration.height_; i++)
  {
    configuration.gameField_[i] = malloc((configuration.width_ + 1) * sizeof(char));
  }
  configuration.game_running_ = 0;

  takeCommand(&configuration);

  freeGame(&configuration);

  return CORRECT;
}
