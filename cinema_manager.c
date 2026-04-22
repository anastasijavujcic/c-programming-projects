//------------------------------------------------------------------------------
// a1.c
//
// This is a program for cinema in Graz.
// Our software should be able to calculate for chosen combinations and to give us a receipt with or without additional discount.
// Customers should be able to have recommend mode based on their budget.
//
//------------------------------------------------------------------------------
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_OF_MOVIES 6
#define STUDENT_DISCOUNT 0.30
#define MEMBER_DISCONUNT 0.15
#define NUMBER_OF_POPCORN_TYPES 4
#define NUMBER_OF_DRINK_TYPES 5

char student;
char member;

void AreMember(void);

typedef struct
{
  char title[50];
  double price;
} Movie;

typedef struct
{
  char sort[50];
  double price;
} Sweets;

typedef struct
{
  Movie selectedMovie;
  char student;
  char member;
  int corn_choice;
  int drink_choice;
} Order;

//------------------------------------------------------------------------------
///
/// Ask the user if they are a student.
///
/// @input is local assigned variable for taking an input from user.
/// It checks wether the user has entered the right value or not.
/// @returns the value of student.
///
//

void AreStudent(void)
{
  char input[50];

  printf("\nAre you a student? (enter y for \"Yes\", n for \"No\")\n");
  printf(" > ");

  while (1)
  {
    fgets(input, sizeof(input), stdin);

    if (strlen(input) == 2 && (input[0] == 'y' || input[0] == 'n'))
    {
      student = input[0];
      break;
    }
    else
    {
      printf("Invalid input. Please enter either y for \"Yes\" or n for \"No\"\n");
      printf(" > ");
    }
  }

  if (student == 'n')
  {
    AreMember();
  }
  else
  {
    member = 'n';
  }
}

//------------------------------------------------------------------------------
///
/// Ask the user if they are a member.
///
/// @input is local assigned variable for taking an input from user.
/// It checks wether the user has entered the right value or not.
/// @returns the value of member.
///
//
void AreMember(void)
{
  char input[50];

  printf("\nDo you have a membership card? (enter y for \"Yes\", n for \"No\")\n");
  printf(" > ");
  fgets(input, sizeof(input), stdin);

  while (1)
  {

    if (strlen(input) == 2 && (input[0] == 'y' || input[0] == 'n'))
    {
      member = input[0];
      break;
    }
    else
    {
      printf("Invalid input. Please enter either y for \"Yes\" or n for \"No\"\n");
      printf(" > ");
    }
    fgets(input, sizeof(input), stdin);
  }
}

//------------------------------------------------------------------------------
///
///
/// This is recommender mode where user can see all treats and film options within their budget.
/// It calls arrays movies, popcorns, drinks.
///
/// It calls also previous functions @AreStudent and @AreMember to take values.
/// In loops it checks the all combinations within given budget.
///
//

int recommenderMode(Movie movies[], Sweets popcorns[], Sweets drinks[])
{
  int found_combination = 0;
  double price_discount, budget;
  double total_sum;
  printf("\nWelcome to the recommender system!\n\n");
  printf("What's your budget in €?\n");
  printf(" > ");
  scanf("%lf", &budget);

  while ((getchar()) != '\n');
  AreStudent();

  printf("\nRecommended movies within your budget (€%.2f):\n", budget);

  for (int i = 0; i < NUMBER_OF_MOVIES; i++)
  {

    price_discount = movies[i].price;

    if (student == 'y')
    {
      price_discount *= (1 - STUDENT_DISCOUNT);
    }
    else if (member == 'y')
    {
      price_discount *= (1 - MEMBER_DISCONUNT);
    }
    // for popcorns
    for (int j = 0; j < 4; j++)
    {
      // drinks
      for (int k = 0; k < 5; k++)
      {

        total_sum = price_discount + popcorns[j].price + drinks[k].price;
        if (total_sum <= budget)
        {
          found_combination = 1;
          printf("- Movie \"%s\" + %s + %s (€%.8f)\n",
                 movies[i].title, popcorns[j].sort, drinks[k].sort, total_sum);
        }
      }
    }
  }
  if (found_combination == 0)
  {
    printf("Sorry, we have no offers within your budget (€%.2f)!\n", budget);
    return -1;
  }
  return 2;
}

//------------------------------------------------------------------------------
///
/// Function for movie selection.
/// While loop checks is the correct value added and what did user choose.
/// @return chosenMovie
//
int selectMovie(Movie movies[])
{
  int movie_choice;
  char input[50];

  printf("\nWhat do you want?:\n");
  printf("- leave (enter 0)\n");
  printf("- to proceed choosing a movie on your own (enter 1-6)\n");
  printf("- to be recommended some movie-treats-combos according to your budget (enter 7)\n");
  printf(" > ");

  while (1)
  {
    fgets(input, sizeof(input), stdin);

    if (sscanf(input, "%d", &movie_choice) != 1)
    {
      printf("Invalid input. Please enter number in [0, 7]!\n");
      printf(" > ");
    }
    else if (movie_choice == 0)
    {
      printf("Thanks for your visit! Looking forward to seeing you again!\n");
      return 0;
    }
    else if (movie_choice < 1 || movie_choice > 7)
    {
      printf("Invalid input. Please enter number in [0, 7]!\n");
      printf(" > ");
    }
    else if (movie_choice == 7)
    {
      return 7;
    }
    else
    {
      printf("\nChosen Film: %s\n", movies[movie_choice - 1].title);
      return movie_choice;
    }
  }
}

//------------------------------------------------------------------------------
///
/// Function for Popcorn selection.
/// @return popcorn choice.
///
//

int selectPopcorns(Sweets popcorns[])
{
  int corn_choice = -1;
  char want_popcorns;
  char input[50];

  printf("\nWould you like some popcorn? (enter y for \"Yes\", n for \"No\")\n");
  printf(" > ");
  fgets(input, sizeof(input), stdin);

  while (1)
  {
    if (strlen(input) == 2 && (input[0] == 'y' || input[0] == 'n'))
    {
      want_popcorns = input[0];
      break;
    }
    else
    {
      printf("Invalid input. Please enter either y for \"Yes\" or n for \"No\".\n");
      printf(" > ");
    }
    fgets(input, sizeof(input), stdin);
  }

  if (want_popcorns == 'y')
  {
    printf("\nOur popcorn flavors:\n");
    for (int i = 0; i < 4; i++)
    {
      printf("%d. %s - €%.2f\n", i + 1, popcorns[i].sort, popcorns[i].price);
    }

    while (1)
    {
      printf("Select an option (1-4)\n");
      printf(" > ");
      fgets(input, sizeof(input), stdin);

      if (sscanf(input, "%d", &corn_choice) == 1 && corn_choice >= 1 && corn_choice <= 4)
      {
        break;
      }
      else
      {
        printf("Invalid input. Please select a valid option between 1 and 4.\n");
      }
    }
  }
  return corn_choice;
}

//------------------------------------------------------------------------------
///
/// Function for Drink selection.
/// @return drink choice.
///
//
int selectDrinks(Sweets drinks[])
{
  int drink_choice = -1;
  char want_drink;
  char input[50];

  printf("\nWould you like a drink? (enter y for \"Yes\", n for \"No\")\n");
  printf(" > ");
  fgets(input, sizeof(input), stdin);

  while (1)
  {
    if (strlen(input) == 2 && (input[0] == 'y' || input[0] == 'n'))
    {
      want_drink = input[0];
      break;
    }
    else
    {
      printf("Invalid input. Please enter either y for \"Yes\" or n for \"No\".\n");
      printf(" > ");
    }
    fgets(input, sizeof(input), stdin);
  }

  if (want_drink == 'y')
  {
    printf("\nOur available drinks:\n");

    for (int i = 0; i < 5; i++)
    {
      printf("%d. %s - €%.2f\n", i + 1, drinks[i].sort, drinks[i].price);
    }

    while (1)
    {
      printf("Select an option (1-5)\n");
      printf(" > ");
      fgets(input, sizeof(input), stdin);

      if (sscanf(input, "%d", &drink_choice) == 1 && drink_choice >= 1 && drink_choice <= 5)
      {
        break;
      }
      else
      {
        printf("Invalid input. Please select a valid option between 1 and 5.\n");
      }
    }
  }
  return drink_choice;
}

//------------------------------------------------------------------------------
///
/// Function for printing the receipt.
/// It takes all necessary parameters and calculates total price with discounts.
/// Prints out chosen movie, ticket price, possible discounts,
/// popcorn and drink selection.
//
void printReceipt(Order order, Sweets popcorns[], Sweets drinks[])
{
  double total = order.selectedMovie.price;
  double discount = 0.0;

  printf("\n=== Your Receipt ===\n");
  printf("Movie: %s\n", order.selectedMovie.title);
  printf("Ticket price: €%.2f\n", order.selectedMovie.price);

  if (order.student == 'y')
  {
    discount = order.selectedMovie.price * STUDENT_DISCOUNT;
    printf("Applicable discount: €-%.2f\n", discount);
    total -= discount;
  }
  else if (order.member == 'y')
  {
    discount = order.selectedMovie.price * MEMBER_DISCONUNT;
    printf("Applicable discount: €-%.2f\n", discount);
    total -= discount;
  }

  if (order.corn_choice != -1)
  {
    printf("Popcorn (%s): €%.2f\n", popcorns[order.corn_choice - 1].sort,
           popcorns[order.corn_choice - 1].price);
    total += popcorns[order.corn_choice - 1].price;
  }

  if (order.drink_choice != -1)
  {
    printf("Drink (%s): €%.2f\n", drinks[order.drink_choice - 1].sort,
           drinks[order.drink_choice - 1].price);
    total += drinks[order.drink_choice - 1].price;
  }

  printf("Total: €%.8f\n", total);
  printf("\nCongratulations on your purchase! Enjoy the movie!\n");
}

//------------------------------------------------------------------------------
///
/// This is main function.
/// It lists all movies and allows user to choose to Leave, pick a film and access to recommender mode.
/// Checks wether a user is @param student or @param member
/// @param popcorns and @param drinks take the value from user for their wishes
/// At the end program gives us receipt and returns @param total
//
int main(void)
{
  int movie_choice, corn_choice, drink_choice;

  Movie movies[NUMBER_OF_MOVIES] =
      {
          {"Highway to Hell ESP Edition", 92.08},
          {"The Hitchhiker’s Guide to the Galaxy", 77.42},
          {"Flow", 59.74},
          {"The Place", 43.28},
          {"Interstate 60", 28.00},
          {"Truth seeking at TU Graz", 3.46}};

  Sweets popcorns[] =
      {
        {"Spicy Popcorn", 9.99}, 
        {"Cheese Popcorn", 7.80}, 
        {"Caramel Popcorn", 5.50}, 
        {"Salted Popcorn", 3.50}
      };

  Sweets drinks[] =
      {
        {"Orange Juice", 2.95}, 
        {"Ice Tea", 2.60}, 
        {"Cola", 2.10}, 
        {"Sprite", 2.00}, 
        {"Water", 1.10}
      };

  printf("\n=== Welcome to Cinema Booking System ===\n\n");
  printf("Today in our cinema:\n");

  for (int i = 0; i < NUMBER_OF_MOVIES; i++)
  {
    printf("%d. %s - €%.2f\n", i + 1, movies[i].title, movies[i].price);
  }

  movie_choice = selectMovie(movies);
  if (movie_choice == 0)
  {
    return 0;
  }
  if (movie_choice == 7)
  {
    int rec_reasult = recommenderMode(movies, popcorns, drinks);

    if (rec_reasult == 2)
    {
      return 2;
    }
    if (rec_reasult == -1)
    {
      return 255;
    }
  }
  if (movie_choice == -2)
  {
    return 0;
  }

  // calls to ask a user if they are student or not
  AreStudent();

  corn_choice = selectPopcorns(popcorns);
  drink_choice = selectDrinks(drinks);

  Order myOrder;
  myOrder.selectedMovie = movies[movie_choice - 1];
  myOrder.student = student;
  myOrder.member = member;
  myOrder.corn_choice = corn_choice;
  myOrder.drink_choice = drink_choice;

  printReceipt(myOrder, popcorns, drinks);

  return 1;
}
