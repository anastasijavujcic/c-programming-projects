//------------------------------------------------------------------------------
// a2.c
//
// Program was made to take all passwords from user in order to store it and
// to make sure it is given by all rules.
//
//------------------------------------------------------------------------------
//

#include <stdio.h>

#define BUFFER_SIZE 512
#define MAX_PASSWORDS 5
#define MAX_PASSWORD_LENGTH 15
#define MIN_PASSWORD_LENGTH 8

//------------------------------------------------------------------------------
///
/// Function removes the new line from imput and replaces it with 0
///
//
void removeNewline(char *str)
{
  int i = 0;
  while (str[i] != '\0')
  {
    if (str[i] == '\n')
    {
      str[i] = '\0';
      break;
    }
    i++;
  }
}

//------------------------------------------------------------------------------
///
/// Function clears the input Buffer in order to make sure that user did not
/// entered the \n or End of File
///
//

void clearInputBuffer(void)
{
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

//------------------------------------------------------------------------------
///
/// Function checks the length of user input and returns us the value
///
//
int getLength(const char *str)
{
  int length = 0;
  while (str[length] != '\0')
  {
    length++;
  }
  return length;
}

//------------------------------------------------------------------------------
///
/// Function isLeter proves if given char is within the ASCII table ol letters
///
//
int isLetter(char c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

//------------------------------------------------------------------------------
///
/// Function changes all big leters to small leters by
/// looking into the ASCII table
/// It @returns small leters
//
int toLowerLetter(char c)
{
  if (c >= 'A' && c <= 'Z')
  {
    return c + 32;
  }
  return c;
}

//------------------------------------------------------------------------------
///
/// Function returns whole string in small leters by calling the function
/// @toLowerLetter in order to give us the right value
///
//
void toLowerString(char *str)
{
  for (int i = 0; str[i] != '\0'; i++)
  {
    str[i] = toLowerLetter(str[i]);
  }
}

//------------------------------------------------------------------------------
///
/// Function @stringComparison compares the values from two strings.
/// It is used to compare the entered value with yes and no question from user.
/// It returns the value of the difference between these two strings.
//
int stringComparison(const char *string1, const char *string2)
{
  int i = 0;
  while (string1[i] != '\0' && string2[i] != '\0')
  {
    if (string1[i] != string2[i])
    {
      return string1[i] - string2[i];
    }
    i++;
  }
  return string1[i] - string2[i];
}

//------------------------------------------------------------------------------
///
/// Function checks is the correct length entered from user.
///
//
int isValidLength(const char *password)
{
  int length = getLength(password);
  if (length < MIN_PASSWORD_LENGTH)
  {
    printf("Password must be at least %d characters long.\n", MIN_PASSWORD_LENGTH);
    return 0;
  }
  else if (length > MAX_PASSWORD_LENGTH)
  {
    printf("Password must not be more than %d characters long.\n", MAX_PASSWORD_LENGTH);
    return 0;
  }
  return 1;
}

//------------------------------------------------------------------------------
///
/// Function checks the upper case letters entered from user.
///
//
int checkUppercaseLetter(const char *password)
{
  for (size_t pos = 0; password[pos] != '\0'; pos++)
  {
    if (password[pos] >= 'A' && password[pos] <= 'Z')
    {
      return 1;
    }
  }
  return 0;
}

//------------------------------------------------------------------------------
///
/// Function checks the lower case letters entered from user.
///
//
int checkLowercaseLetter(const char *password)
{
  for (size_t pos = 0; password[pos] != '\0'; pos++)
  {
    if (password[pos] >= 'a' && password[pos] <= 'z')
    {
      return 1;
    }
  }
  return 0;
}

//------------------------------------------------------------------------------
///
/// Function checks if a digit was entered from user.
///
//
int checkNumber(const char *password)
{
  for (size_t pos = 0; password[pos] != '\0'; pos++)
  {
    if (password[pos] >= '0' && password[pos] <= '9')
    {
      return 1;
    }
  }
  return 0;
}

//------------------------------------------------------------------------------
///
/// Function checks special characters entered from user.
///
//
int checkSpecialChar(const char *password)
{
  for (size_t pos = 0; password[pos] != '\0'; pos++)
  {
    if ((password[pos] > 31 && password[pos] < 48) || (password[pos] > 57 && password[pos] < 65))
    {
      return 1;
    }
    else if ((password[pos] > 90 && password[pos] < 97) || (password[pos] > 122 && password[pos] < 127))
    {
      return 1;
    }
  }
  return 0;
}

//------------------------------------------------------------------------------
///
/// Function checks palindrome in entered password from user.
///
//
int checkPalindrome(const char *password)
{
  int first = 0;
  int last = getLength(password) - 1;

  while (first < last)
  {
    if (password[first] != password[last])
    {
      return 0;
    }
    first++;
    last--;
  }
  return 1;
}

//------------------------------------------------------------------------------
///
/// Function checks is there a repeating pattern given from user.
///
//
int checkRepeatPatterns(const char *password)
{
  int length = getLength(password);
  for (int patternLength = 3; patternLength <= length / 2; patternLength++)
  {
    for (int i = 0; i <= length - patternLength * 2; i++)
    {
      int isNumeric = 1;
      for (int k = 0; k < patternLength; k++)
      {
        if (password[i + k] < '0' || password[i + k] > '9')
        {
          isNumeric = 0;
          break;
        }
      }
      if (!isNumeric)
        continue;

      for (int j = i + patternLength; j <= length - patternLength; j++)
      {
        int match = 1;
        for (int k = 0; k < patternLength; k++)
        {
          if (password[i + k] != password[j + k])
          {
            match = 0;
            break;
          }
        }
        if (match)
        {
          printf("Password must not contain repeating numeric patterns.\n");
          return 1;
        }
      }
    }
  }
  return 0;
}

//------------------------------------------------------------------------------
///
/// Function compares existing passwords with new entered ones in case the user
/// gives the same password then it returns the message.
///
//
int checkExistingPassword(const char passwords[MAX_PASSWORDS][BUFFER_SIZE],
                          int pasword_count, const char *new_password)
{
  for (int i = 0; i < pasword_count; i++)
  {
    if (stringComparison(passwords[i], new_password) == 0)
    {
      printf("This password has already been used.\n");
      return 1;
    }
  }
  return 0;
}

//------------------------------------------------------------------------------
///
/// Function @readInput takes the input from user and compares the input
/// with all condtitions given. If every condition is satisfided then it goes
/// out of the loop and stores the new input
///
//
void readInput(char *password, size_t size, char passwords[MAX_PASSWORDS][BUFFER_SIZE], int password_count)
{
  while (1)
  {
    printf("Please enter a password\n > ");
    if (fgets(password, size, stdin) != NULL)
    {
      if (password[getLength(password) - 1] != '\n')
      {
        clearInputBuffer();
      }

      removeNewline(password);
      if (!isValidLength(password))
      {
        continue;
      }
      if (!checkLowercaseLetter(password))
      {
        printf("Password must contain at least one lowercase character.\n");
        continue;
      }
      if (!checkUppercaseLetter(password))
      {
        printf("Password must contain at least one uppercase character.\n");
        continue;
      }

      if (!checkNumber(password))
      {
        printf("Password must contain at least one digit.\n");
        continue;
      }
      if (!checkSpecialChar(password))
      {
        printf("Password must contain at least one special character.\n");
        continue;
      }
      if (checkPalindrome(password))
      {
        printf("Password must not be a palindrome.\n");
        continue;
      }
      if (checkRepeatPatterns(password))
      {
        continue;
      }
      if (checkExistingPassword(passwords, password_count, password))
      {
        continue;
      }
      if (feof(stdin))
      {
        break;
      }
      break;
    }
  }
}

//------------------------------------------------------------------------------
///
/// Function asks the user if they want another password to enter.
//
int anotherPassword(void)
{
  char input[50];

  while (1)
  {
    printf("Do you want to set another password? (yes/no)\n > ");
    if (fgets(input, sizeof(input), stdin) == NULL)
    {
      continue;
    }
    if (input[getLength(input) - 1] != '\n')
    {
      clearInputBuffer();
    }
    removeNewline(input);
    toLowerString(input);
    if (stringComparison(input, "yes") == 0)
    {
      return 1;
    }
    else if (stringComparison(input, "no") == 0)
    {
      return 0;
    }
    else
    {
      printf("Invalid input. Please enter 'yes' or 'no'.\n");
    }
  }
}

//------------------------------------------------------------------------------
///
/// Function main contains all important variables and functions.
/// This is where program begins and ends, depending on the output.
//
int main(void)
{
  printf("Welcome to the ESP password manager!\n\n");

  char passwords[MAX_PASSWORDS][BUFFER_SIZE] = {0};
  int password_count = 0;

  while (1)
  {
    char password[BUFFER_SIZE] = {0};
    readInput(password, BUFFER_SIZE, passwords, password_count);

    if (password_count < MAX_PASSWORDS)
    {
      for (int i = 0; i < BUFFER_SIZE; i++)
      {
        passwords[password_count][i] = password[i];
      }
      password_count++;
      printf("Password set successfully.\n");
      if (password_count == MAX_PASSWORDS)
      {
        printf("Capacity exhausted! Go touch some grass!\n");
        return 5;
      }
    }
    else
    {
      printf("Capacity exhausted! Go touch some grass!\n");
      return 5;
    }
    if (!anotherPassword())
    {
      printf("Exiting password manager. Goodbye!\n");
      break;
    }
  }
  return 0;
}
