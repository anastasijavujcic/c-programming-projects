//------------------------------------------------------------------------------
// a3.c
//
// Program was made to play a casino game called Blackjack or 21.
// There is opportunity for as many players as possible to play as long as there
// is enough cards to deal the round.
//
//------------------------------------------------------------------------------
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define DEFAULT_PLAYERS 1
#define DEFAULT_DECKS 1
#define DEFAULT_SEED 0
#define DEFAULT_WELCOME_FILE "welcome_message.txt"

typedef enum _ErrorMessages_
{
  CORRECT,
  INCORRECT = 1,
  OUT_OF_MEMORY = 1,
  INVALID_PARA_NR = 2,
  INVALID_TYPE = 3,
  FAILED_FILE_OPENING = 3,
  CARDS_MISSING = 4,
  UNKNOWN_COM = 4,
  EMPTY_SHOE = 4
} ErrorMessages;

typedef enum _Suit_
{
  CLUBS,
  DIAMONDS,
  SPADES,
  HEARTS
} Suit;

typedef enum _Value_
{
  TWO = 2,
  THREE,
  FOUR,
  FIVE,
  SIX,
  SEVEN,
  EIGHT,
  NINE,
  TEN,
  JACK = 10,
  QUEEN = 10,
  KING = 10,
  ACE = 11
} Value;

typedef struct _Card_
{
  Suit suit_;
  int value_;
  char symbol_[3];
} Card;

typedef struct _Hand_
{
  Card *cards_;
  size_t size_;
  size_t capacity_;
} Hand;

typedef struct _Game_
{
  Hand *hands_;
  int num_players_;
  Hand dealer_;
} Game;

//------------------------------------------------------------------------------
///
/// Returns a Unicode symbol string for the given suit.
///
/// @param suit the suit enum value
///
/// @return the corresponding Unicode string
//
const char *suitUnicode(Suit suit)
{
  switch (suit)
  {
  case CLUBS:
    return "♣️";
  case DIAMONDS:
    return "♦️";
  case SPADES:
    return "♠️";
  case HEARTS:
    return "♥️";
  default:
    return "?";
  }
}
//------------------------------------------------------------------------------
///
/// Reads a line of input from stdin until newline or EOF.
/// Dynamically allocates memory to store the input string.
///
/// @return pointer to the read string, or NULL on failure or EOF
//
char *readLine(void)
{
  char *buffer = NULL;
  size_t size = 0;
  size_t capacity = 16;
  int ch;

  buffer = malloc(capacity);
  if (!buffer)
  {
    return NULL;
  }
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
/// Checks if a given string contains only numeric digits.
///
/// @param str the input string to check
///
/// @return non-zero if the string is a number, 0 otherwise
//
int isNumber(const char *str)
{
  if (!str || *str == '\0')
    return 0;
  while (*str)
  {
    if (!isdigit(*str))
    {
      return CORRECT;
    }
    str++;
  }
  return INCORRECT;
}

//------------------------------------------------------------------------------
///
/// Parses command-line arguments and sets game parameters accordingly.
/// Uses default values if no arguments are provided.
///
/// @param argc number of arguments
/// @param argv array of argument strings
/// @param players pointer to number of players
/// @param decks pointer to number of decks
/// @param seed pointer to RNG seed value
/// @param welcome_file pointer to welcome file path
///
/// @return appropriate error code from ErrorMessages
//
int takeComandArguments(int argc, char *argv[], unsigned int *players,
                        unsigned int *decks, unsigned int *seed, char **welcome_file)
{
  *players = DEFAULT_PLAYERS;
  *decks = DEFAULT_DECKS;
  *seed = DEFAULT_SEED;
  *welcome_file = DEFAULT_WELCOME_FILE;

  if (argc == 1)
    return CORRECT;

  if (argc > 5)
  {
    printf("Invalid number of parameters given!\n");
    return INVALID_PARA_NR;
  }

  if (argc >= 2 && !isNumber(argv[1]))
  {
    printf("Invalid type for argument!\n");
    return INVALID_TYPE;
  }
  if (argc >= 3 && !isNumber(argv[2]))
  {
    printf("Invalid type for argument!\n");
    return INVALID_TYPE;
  }
  if (argc >= 4 && !isNumber(argv[3]))
  {
    printf("Invalid type for argument!\n");
    return INVALID_TYPE;
  }

  if (argc >= 2)
    *players = atoi(argv[1]);
  if (argc >= 3)
    *decks = atoi(argv[2]);
  if (argc >= 4)
    *seed = atoi(argv[3]);
  if (argc == 5)
    *welcome_file = argv[4];

  return CORRECT;
}

//------------------------------------------------------------------------------
///
/// Function gives inported wecome message.
///
/// @param filename path to welcome message file
///
/// @return CORRECT if it was successful, FAILED_FILE_OPENING if the file could not be opened
//
ErrorMessages printWelcomeMessage(const char *filename)
{
  FILE *fp = fopen(filename, "r");
  if (!fp)
  {
    printf("Error: Failed to open file!\n");
    return FAILED_FILE_OPENING;
  }
  int ch;
  while ((ch = fgetc(fp)) != EOF)
    putchar(ch);
  fclose(fp);
  return CORRECT;
}

//------------------------------------------------------------------------------
/// Creates a deck of cards based on the number of decks specified.
/// Initializes each card with a suit, value, and symbol.
///
/// @param num_decks number of standard 52-card decks to include
/// @param total_cards pointer to store the total number of cards created
///
/// @return pointer to the dynamically allocated deck, or NULL on allocation failure
//
Card *createDeck(unsigned int num_decks, unsigned int *total_cards)
{
  const char *symbols[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
  int values[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11};
  int cards_per_deck = 52;
  *total_cards = num_decks * cards_per_deck;
  Card *deck = malloc(sizeof(Card) * (*total_cards));
  if (!deck)
  {
    return NULL;
  }
  int card_index = 0;
  for (unsigned int deck_num = 0; deck_num < num_decks; ++deck_num)
  {
    for (int suit_index = CLUBS; suit_index <= HEARTS; ++suit_index)
    {
      for (int card_value_index = 0; card_value_index < 13; ++card_value_index)
      {
        deck[card_index].suit_ = (Suit)suit_index;
        deck[card_index].value_ = values[card_value_index];
        strcpy(deck[card_index].symbol_, symbols[card_value_index]);
        card_index++;
      }
    }
  }
  return deck;
}

//------------------------------------------------------------------------------
/// Shuffles the given deck of cards using the Fisher-Yates algorithm.
///
/// @param deck pointer to the array of cards to shuffle
/// @param total_cards number of cards in the deck
/// @param seed seed for random number generator
//
void shuffleDeck(Card *deck, int total_cards, unsigned int seed)
{
  srand(seed);
  for (int curent_index = total_cards - 1; curent_index > 0; --curent_index)
  {
    int new_index = rand() % (curent_index + 1);
    Card tmp = deck[curent_index];
    deck[curent_index] = deck[new_index];
    deck[new_index] = tmp;
  }
}

//------------------------------------------------------------------------------
/// Calculates the total score of a hand.
/// Treats ACE as either 1 or 11 depending on the best fit.
///
/// @param hand pointer to the hand whose score is calculated
///
/// @return the total score of the hand
//
int scoreInHand(Hand *hand)
{
  int score = 0, ace_count = 0;
  for (size_t card_index = 0; card_index < hand->size_; ++card_index)
  {
    score += hand->cards_[card_index].value_;
    if (hand->cards_[card_index].value_ == 11)
      ace_count++;
  }
  while (score > 21 && ace_count--)
    score -= 10;
  return score;
}

//------------------------------------------------------------------------------
/// Checks if a hand is a natural blackjack (exactly two cards summing to 21).
///
/// @param hand pointer to the hand to check
///
/// @return non-zero if the hand is a blackjack, 0 otherwise
//
int isBlackjack(Hand *hand)
{
  return (hand->size_ == 2 && hand->cards_[0].value_ + hand->cards_[1].value_ == 21);
}

//------------------------------------------------------------------------------
/// Prints all cards in a hand in a stylized ASCII format.
/// If requested, hides the second card (used for hiding dealer's hole card).
///
/// @param hand pointer to the hand of cards to print
/// @param hide_second_card non-zero to mask the second card, zero to show all
//
void printCardsInHand(Hand *hand, int hide_second_card)
{
  int hand_size = hand->size_;

  for (int card_index = 0; card_index < hand_size; ++card_index)
  {
    printf(" +-----+");
    if (card_index != hand_size - 1)
    {
      printf(" ");
    }
  }
  printf("\n");

  for (int card_index = 0; card_index < hand_size; ++card_index)
  {
    if (hide_second_card && card_index == 1)
    {
      printf(" |#####|");
    }
    else
    {
      printf(" |%s   %s|", suitUnicode(hand->cards_[card_index].suit_), suitUnicode(hand->cards_[card_index].suit_));
    }
    if (card_index != hand_size - 1)
    {
      printf(" ");
    }
  }
  printf("\n");

  for (int card_index = 0; card_index < hand_size; ++card_index)
  {
    if (hide_second_card && card_index == 1)
    {
      printf(" |#####|");
    }
    else
    {
      printf(" |  %-2s |", hand->cards_[card_index].symbol_);
    }
    if (card_index != hand_size - 1)
    {
      printf(" ");
    }
  }
  printf("\n");

  for (int card_index = 0; card_index < hand_size; ++card_index)
  {
    if (hide_second_card && card_index == 1)
    {
      printf(" |#####|");
    }
    else
    {
      printf(" |%s   %s|", suitUnicode(hand->cards_[card_index].suit_), suitUnicode(hand->cards_[card_index].suit_));
    }
    if (card_index != hand_size - 1)
    {
      printf(" ");
    }
  }
  printf("\n");

  for (int card_index = 0; card_index < hand_size; ++card_index)
  {
    printf(" +-----+");
    if (card_index != hand_size - 1)
    {
      printf(" ");
    }
  }
  printf("\n");
}

//------------------------------------------------------------------------------
/// Prints the current game state for one round: dealer and all players.
/// Optionally hides the dealer's second card.
///
/// @param game pointer to the game state
/// @param round_number the current round number to display
/// @param hide_dealer_card non-zero if the dealer’s second card should be hidden
//
void printPlayingRounds(Game *game, int round_number, int hide_dealer_card)
{
  printf("\n---- ROUND %d ----\n\n", round_number);
  printf("      DEALER\n");
  printCardsInHand(&game->dealer_, hide_dealer_card);
  if (hide_dealer_card)
  {
    printf(" score: ?\n\n");
  }
  else
  {
    printf(" score: %d\n\n", scoreInHand(&game->dealer_));
  }
  for (int player_index = 0; player_index < game->num_players_; ++player_index)
  {
    printf("     PLAYER %d\n", player_index + 1);
    printCardsInHand(&game->hands_[player_index], 0);
    printf(" score: %d\n\n", scoreInHand(&game->hands_[player_index]));
  }
}

//------------------------------------------------------------------------------
/// Prints the results of a completed round for all players and the dealer.
/// Evaluates each hand and determines the winner, draw, or bust.
///
/// @param game pointer to the current game state
/// @param round_number the current round number
/// @param players_blackjack array indicating which players had blackjack
/// @param num_players total number of players in the game
//
void printResultRound(Game *game, int round_number, int *players_blackjack, int num_players)
{
  int dealer_score = scoreInHand(&game->dealer_);
  printf("\n---- ROUND %d ----\n\n", round_number);
  printf("      DEALER\n");
  printCardsInHand(&game->dealer_, 0);
  printf(" score: %d\n\n", dealer_score);
  for (int player_index = 0; player_index < num_players; ++player_index)
  {
    printf("     PLAYER %d\n", player_index + 1);
    printCardsInHand(&game->hands_[player_index], 0);
    int player_score = scoreInHand(&game->hands_[player_index]);
    printf(" score: %d\n", player_score);
    if (players_blackjack[player_index])
    {
      if (isBlackjack(&game->dealer_))
      {
        printf(" Hand was drawn!\n\n");
      }
      else
      {
        printf(" Player has won!\n\n");
      }
    }
    else if (player_score > 21)
    {
      printf(" Dealer has won!\n\n");
    }
    else if (dealer_score > 21)
    {
      printf(" Player has won!\n\n");
    }
    else if (player_score > dealer_score)
    {
      printf(" Player has won!\n\n");
    }
    else if (player_score < dealer_score)
    {
      printf(" Dealer has won!\n\n");
    }
    else
    {
      printf(" Hand was drawn!\n\n");
    }
  }
}

//------------------------------------------------------------------------------
/// Adds a card to the given hand. If needed, dynamically allocates or resizes
/// the array of cards to accommodate the new card.
///
/// @param hand pointer to the hand to which the card will be added
/// @param card the card to add
///
/// @return CORRECT on success, OUT_OF_MEMORY if allocation fails
//
int additionalCardInHand(Hand *hand, Card card)
{
  if (hand->cards_ == NULL)
  {
    hand->capacity_ = 4;
    hand->cards_ = malloc(hand->capacity_ * sizeof(Card));
    hand->size_ = 0;
  }
  else if (hand->size_ >= hand->capacity_)
  {
    hand->capacity_ *= 2;
    Card *temp = realloc(hand->cards_, hand->capacity_ * sizeof(Card));
    if (!temp)
    {
      return OUT_OF_MEMORY;
    }
    hand->cards_ = temp;
  }
  if (!hand->cards_)
  {
    return OUT_OF_MEMORY;
  }
  hand->cards_[hand->size_++] = card;
  return CORRECT;
}

//------------------------------------------------------------------------------
/// Gets input from a player and performs the corresponding game action.
/// Accepts "hit", "stand", or "quit" as input commands.
/// If "hit" is chosen, a card is drawn. Handles bust and empty shoe cases.
///
/// @param player_index index of the player whose turn it is
/// @param game pointer to the current game state
/// @param deck pointer to the current deck of cards
/// @param deck_index pointer to the index of the next card to draw
/// @param total_cards total number of cards in the deck
/// @param round_number the current round number (used for display)
///
/// @return CORRECT if the player action was valid or completed, error code otherwise
//
int takeUserComand(int player_index, Game *game, Card *deck,
                   unsigned int *deck_index, unsigned int total_cards, int round_number)
{
  char *input = NULL;
  Hand *hand = &game->hands_[player_index];

  while (1)
  {
    printf("PLAYER %d: What do you want to do?\n > ", player_index + 1);
    fflush(stdout);

    input = readLine();
    if (!input)
    {
      return INCORRECT;
    }

    if (strcmp(input, "stand") == 0)
    {
      free(input);
      break;
    }
    else if (strcmp(input, "hit") == 0)
    {
      if (*deck_index >= total_cards)
      {
        printf("Error: Shoe is empty!\n");
        free(input);
        return EMPTY_SHOE;
      }
      additionalCardInHand(hand, deck[(*deck_index)++]);
      if (scoreInHand(hand) > 21)
      {
        free(input);
        break;
      }
      printPlayingRounds(game, round_number, 1);
    }
    else if (strcmp(input, "quit") == 0)
    {
      free(input);
      return INCORRECT;
    }
    else
    {
      printf("Error: Unknown command!\n");
    }

    free(input);
  }

  return CORRECT;
}

//------------------------------------------------------------------------------
/// Frees all dynamically allocated memory related to the game state.
///
/// @param game pointer to the game object
/// @param players number of players
/// @param deck pointer to the deck of cards
//
void freeAllocatedSpace(Game *game, int players, Card *deck)
{
  if (game == NULL)
  {
    return;
  }
  for (int index = 0; index < players; ++index)
  {
    if (game->hands_[index].cards_ != NULL)
    {
      free(game->hands_[index].cards_);
    }
  }
  if (game->hands_ != NULL)
  {
    free(game->hands_);
  }
  if (game->dealer_.cards_ != NULL)
  {
    free(game->dealer_.cards_);
  }
  if (deck != NULL)
  {
    free(deck);
  }
}

//------------------------------------------------------------------------------
/// Initializes the game structure, creates and shuffles the deck,
/// and allocates memory for player hands and dealer hand.
///
/// @param game pointer to the game structure to initialize
/// @param deck pointer to store the created deck
/// @param players number of players
/// @param decks number of decks to use
/// @param seed seed value for shuffling
/// @param total_cards pointer to store total number of cards in the deck
///
/// @return CORRECT on success, OUT_OF_MEMORY on allocation failure
//
int formGameBeginning(Game *game, Card **deck, unsigned int players,
                      unsigned int decks, unsigned int seed, unsigned int *total_cards)
{
  *deck = createDeck(decks, total_cards);
  if (!*deck)
    return OUT_OF_MEMORY;

  shuffleDeck(*deck, *total_cards, seed);

  game->num_players_ = players;
  game->hands_ = calloc(players, sizeof(Hand));
  if (!game->hands_)
  {
    free(*deck);
    return OUT_OF_MEMORY;
  }

  game->dealer_.cards_ = NULL;
  game->dealer_.size_ = 0;
  game->dealer_.capacity_ = 0;

  return CORRECT;
}

//------------------------------------------------------------------------------
/// Resets the hands of all players and the dealer by freeing memory
/// and resetting sizes and capacities to 0.
///
/// @param game pointer to the current game state
/// @param players number of players
//
void resetHands(Game *game, unsigned int players)
{
  for (unsigned int player_index = 0; player_index < players; ++player_index)
  {
    free(game->hands_[player_index].cards_);
    game->hands_[player_index].cards_ = NULL;
    game->hands_[player_index].size_ = 0;
    game->hands_[player_index].capacity_ = 0;
  }
  free(game->dealer_.cards_);
  game->dealer_.cards_ = NULL;
  game->dealer_.size_ = 0;
  game->dealer_.capacity_ = 0;
}

//------------------------------------------------------------------------------
/// Deals the initial two cards to each player and the dealer.
/// In total, each participant receives two cards.
///
/// @param game pointer to the current game state
/// @param deck pointer to the deck of cards
/// @param deck_index pointer to the current index in the deck
//
void dealInitialCards(Game *game, Card *deck, unsigned int *deck_index)
{
  for (int player_index = 0; player_index < game->num_players_; ++player_index)
    additionalCardInHand(&game->hands_[player_index], deck[(*deck_index)++]);

  additionalCardInHand(&game->dealer_, deck[(*deck_index)++]);

  for (int player_index = 0; player_index < game->num_players_; ++player_index)
    additionalCardInHand(&game->hands_[player_index], deck[(*deck_index)++]);

  additionalCardInHand(&game->dealer_, deck[(*deck_index)++]);
}

//------------------------------------------------------------------------------
/// Checks if any players or the dealer have a blackjack at the start of a round.
/// If both dealer and player have blackjack, player is not counted as winner.
///
/// @param game pointer to the game state
/// @param players_blackjack array to store which players have blackjack
/// @param dealer_blackjack pointer to store whether the dealer has blackjack
///
/// @return 1 if anyone has blackjack, 0 otherwise
//
int checkBlackjack(Game *game, int *players_blackjack, int *dealer_blackjack)
{
  *dealer_blackjack = isBlackjack(&game->dealer_);
  int any_blackjack = *dealer_blackjack;

  for (int player_index = 0; player_index < game->num_players_; ++player_index)
  {
    players_blackjack[player_index] = isBlackjack(&game->hands_[player_index]);
    if (players_blackjack[player_index])
      any_blackjack = 1;
  }

  if (*dealer_blackjack)
  {
    for (int player_index = 0; player_index < game->num_players_; ++player_index)
      if (players_blackjack[player_index])
        players_blackjack[player_index] = 0;
  }

  return any_blackjack;
}

//------------------------------------------------------------------------------
/// Executes the dealer's turn by drawing cards until reaching a score of at least 17.
/// Skips turn if no players are eligible (not busted or already blackjack).
///
/// @param game pointer to the game state
/// @param deck pointer to the deck of cards
/// @param deck_index pointer to the current index in the deck
/// @param total_cards total number of cards in the deck
/// @param players_blackjack array indicating which players had blackjack
//
void DealersTurnToPlay(Game *game, Card *deck, unsigned int *deck_index,
                       unsigned int total_cards, int *players_blackjack)
{
  int play = 0;
  for (int player_index = 0; player_index < game->num_players_; ++player_index)
  {
    if (!players_blackjack[player_index] && scoreInHand(&game->hands_[player_index]) <= 21)
    {
      play = 1;
    }
  }

  if (!play)
    return;

  while (scoreInHand(&game->dealer_) < 17 && *deck_index < total_cards)
  {
    additionalCardInHand(&game->dealer_, deck[(*deck_index)++]);
  }
}

//------------------------------------------------------------------------------
/// Main game loop that controls the flow of rounds, player turns,
/// dealer behavior, and final result printing. Continues until players quit
/// or the deck runs out of cards.
///
/// @param game pointer to the game state
/// @param deck pointer to the deck of cards
/// @param total_cards total number of cards in the deck
///
/// @return CORRECT on normal completion, CARDS_MISSING if not enough cards
//

int playGameLoop(Game *game, Card *deck, unsigned int total_cards)
{
  unsigned int deck_index = 0;
  int round_number = 1;
  int players = game->num_players_;

  while (1)
  {
    if ((total_cards - deck_index) < ((unsigned int)(players + 1) * 2))
    {
      printf("Not enough cards left to deal another round!\n");
      return CARDS_MISSING;
    }

    resetHands(game, players);
    dealInitialCards(game, deck, &deck_index);

    int dealer_blackjack;
    int players_blackjack[players];
    memset(players_blackjack, 0, sizeof(players_blackjack));
    int any_blackjack = checkBlackjack(game, players_blackjack, &dealer_blackjack);

    if (any_blackjack)
    {
      printResultRound(game, round_number++, players_blackjack, players);
      continue;
    }

    printPlayingRounds(game, round_number, 1);

    int quit = 0;
    for (int player_index = 0; player_index < players; ++player_index)
    {
      if (players_blackjack[player_index])
        continue;

      int result = takeUserComand(player_index, game, deck, &deck_index, total_cards, round_number);
      if (result == INCORRECT)
      {
        quit = 1;
        break;
      }

      if (player_index < players - 1)
      {
        printPlayingRounds(game, round_number, 1);
      }
    }

    if (quit)
      break;

    DealersTurnToPlay(game, deck, &deck_index, total_cards, players_blackjack);
    printResultRound(game, round_number++, players_blackjack, players);
  }

  return CORRECT;
}

//------------------------------------------------------------------------------
/// Entry point of the program. Initializes the game, reads arguments,
/// sets up the game state, and starts the main game loop.
///
/// @param argc number of command-line arguments
/// @param argv array of argument strings
///
/// @return error code or CORRECT on successful completion
//
int main(int argc, char *argv[])
{
  unsigned int players, decks, seed;
  char *welcome_file;

  ErrorMessages result = takeComandArguments(argc, argv, &players, &decks, &seed, &welcome_file);
  if (result != CORRECT)
    return result;

  printWelcomeMessage(welcome_file);

  Card *deck = NULL;
  unsigned int total_cards;
  Game game;

  result = formGameBeginning(&game, &deck, players, decks, seed, &total_cards);
  if (result != CORRECT)
  {
    printf("Out of memory!\n");
    return result;
  }

  result = playGameLoop(&game, deck, total_cards);
  freeAllocatedSpace(&game, players, deck);
  return result;
}
