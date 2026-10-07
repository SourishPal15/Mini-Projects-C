#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

void readLine(char *buf, int size)
{
    if (fgets(buf, size, stdin) == NULL)
    {
        printf("\n");
        exit(0);
    }

    if (strchr(buf, '\n') == NULL)
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
        {
        }
    }

    buf[strcspn(buf, "\r\n")] = '\0';
}

int askChoice(int low, int high)
{
    char line[20];

    while (1)
    {
        printf("Enter your choice: ");
        readLine(line, sizeof(line));

        if (strlen(line) == 1 && line[0] >= '0' + low && line[0] <= '0' + high)
        {
            return line[0] - '0';
        }

        printf("Invalid choice. Please try again.\n\n");
    }
}

void showMainMenu(void)
{
    printf("\n");
    printf("================================\n");
    printf("     Scrambled Word Guesser\n");
    printf("================================\n");
    printf("\n");
    printf("1) Instructions\n");
    printf("2) Play now\n");
    printf("3) Exit\n");
    printf("\n");
}

void showInstructions(void)
{
    printf("\n");
    printf("================================\n");
    printf("          Instructions\n");
    printf("================================\n");
    printf("\n");
    printf("a. A random word will be selected from the word list.\n");
    printf("b. The selected word will be scrambled randomly.\n");
    printf("c. You must guess the original word.\n");
    printf("d. Your guess is not case-sensitive.\n");
    printf("e. Spaces are not allowed in your guess.\n");
    printf("f. You must guess the word within the given number of attempts.\n");
    printf("g. The number of attempts depends on the difficulty level.\n");
    printf("\n");
    printf("2) Play now\n");
    printf("3) Exit\n");
    printf("================================\n");
    printf("\n");
}

int chooseDifficulty(void)
{
    int choice;

    printf("\n");
    printf("================================\n");
    printf("      Choose your Difficulty\n");
    printf("================================\n");
    printf("\n");
    printf("1) Easy\n");
    printf("2) Medium\n");
    printf("3) Hard\n");
    printf("4) Extra Hard\n");
    printf("\n");

    choice = askChoice(1, 4);

    if (choice == 1)
    {
        return 15;
    }
    if (choice == 2)
    {
        return 10;
    }
    if (choice == 3)
    {
        return 5;
    }
    return 1;
}

void showReplayMenu(void)
{
    printf("\n");
    printf("================================\n");
    printf("      Would you like to?\n");
    printf("================================\n");
    printf("\n");
    printf("1) Play Again\n");
    printf("2) Change Difficulty\n");
    printf("3) Exit\n");
    printf("\n");
    printf("================================\n");
    printf("\n");
}

void goodbye(void)
{
    printf("\nThank you for playing!\n");
}

void toUpperCase(char *s)
{
    int i;

    for (i = 0; s[i] != '\0'; i++)
    {
        s[i] = toupper((unsigned char)s[i]);
    }
}

int nextWord(FILE *f, char *word)
{
    char line[100];
    int len;
    int i;
    int ok;

    while (fgets(line, sizeof(line), f) != NULL)
    {
        line[strcspn(line, "\r\n")] = '\0';

        len = strlen(line);
        while (len > 0 && (line[len - 1] == ' ' || line[len - 1] == '\t'))
        {
            line[len - 1] = '\0';
            len--;
        }

        ok = (len > 0);
        for (i = 0; line[i] != '\0'; i++)
        {
            if (!isalpha((unsigned char)line[i]))
            {
                ok = 0;
            }
        }

        if (ok)
        {
            strcpy(word, line);
            return 1;
        }
    }

    return 0;
}

int pickWord(char *word)
{
    FILE *f;
    int count = 0;
    int pick;
    int i;

    f = fopen("words.txt", "r");
    if (f == NULL)
    {
        printf("\nError: could not open words.txt.\n");
        return 0;
    }

    while (nextWord(f, word))
    {
        count++;
    }

    if (count == 0)
    {
        fclose(f);
        printf("\nError: words.txt is empty or has no valid words.\n");
        return 0;
    }

    pick = rand() % count;
    rewind(f);

    for (i = 0; i <= pick; i++)
    {
        nextWord(f, word);
    }

    fclose(f);
    return 1;
}

void scramble(char *s)
{
    int i;
    int j;
    char temp;

    for (i = strlen(s) - 1; i > 0; i--)
    {
        j = rand() % (i + 1);
        temp = s[i];
        s[i] = s[j];
        s[j] = temp;
    }
}

void showScrambled(char *s)
{
    int i;

    printf("Scrambled Word: ");
    for (i = 0; s[i] != '\0'; i++)
    {
        printf("%c", s[i]);
        if (s[i + 1] != '\0')
        {
            printf(" ");
        }
    }
    printf("\n\n");
}

void readGuess(char *guess)
{
    int i;
    int hasSpace;
    int onlyLetters;

    while (1)
    {
        printf("Enter your Guess: ");
        readLine(guess, 100);

        if (guess[0] == '\0')
        {
            printf("Your guess cannot be empty. Please try again.\n\n");
            continue;
        }

        hasSpace = 0;
        onlyLetters = 1;
        for (i = 0; guess[i] != '\0'; i++)
        {
            if (guess[i] == ' ')
            {
                hasSpace = 1;
            }
            else if (!isalpha((unsigned char)guess[i]))
            {
                onlyLetters = 0;
            }
        }

        if (hasSpace)
        {
            printf("Spaces are not allowed. Please enter the word without spaces.\n\n");
            continue;
        }

        if (!onlyLetters)
        {
            printf("Please use letters only.\n\n");
            continue;
        }

        toUpperCase(guess);
        return;
    }
}

int playRound(int attempts)
{
    char word[100];
    char scrambled[100];
    char guess[100];

    if (!pickWord(word))
    {
        return 0;
    }

    toUpperCase(word);
    strcpy(scrambled, word);
    scramble(scrambled);

    printf("\nAttempts remaining: %d\n\n", attempts);

    while (attempts > 0)
    {
        showScrambled(scrambled);
        readGuess(guess);

        if (strcmp(guess, word) == 0)
        {
            printf("\n");
            printf("================================\n");
            printf("          Correct!\n");
            printf("================================\n");
            printf("\n");
            printf("The word was: %s\n", word);
            printf("\n");
            printf("You guessed the word correctly!\n");
            return 1;
        }

        attempts--;

        if (attempts > 0)
        {
            printf("\nIncorrect guess.\n\n");
            printf("Attempts remaining: %d\n\n", attempts);
        }
    }

    printf("\n");
    printf("================================\n");
    printf("          Game Over\n");
    printf("================================\n");
    printf("\n");
    printf("The word was: %s\n", word);
    printf("\n");
    printf("Better luck next time!\n");
    return 1;
}

void playGame(void)
{
    int attempts;
    int choice;

    attempts = chooseDifficulty();

    while (1)
    {
        if (!playRound(attempts))
        {
            return;
        }

        showReplayMenu();
        choice = askChoice(1, 3);

        if (choice == 2)
        {
            attempts = chooseDifficulty();
        }
        else if (choice == 3)
        {
            goodbye();
            return;
        }
    }
}

int main(void)
{
    int choice;

    srand((unsigned)time(NULL));

    while (1)
    {
        showMainMenu();
        choice = askChoice(1, 3);

        if (choice == 1)
        {
            showInstructions();
            choice = askChoice(2, 3);
        }

        if (choice == 2)
        {
            playGame();
            return 0;
        }

        if (choice == 3)
        {
            goodbye();
            return 0;
        }
    }
}

/*

MAKE A FILE EXACTLY NAMED 'words.txt' AND PASTE THESE WORDS HERE:
START COPYING FROM THE START OF 'apple' AND END AT 'dream'  
MAKE SURE THERE ARE NO EXTRA SPACES IN BETWEEN 

apple
banana
orange
grape
mango
lemon
peach
cherry
melon
berry
bread
butter
cheese
sugar
honey
water
coffee
milk
juice
pizza
burger
pasta
rice
bread
cake
cookie
candy
chocolate
breakfast
lunch
dinner
school
college
teacher
student
class
book
paper
pencil
eraser
marker
ruler
board
desk
chair
table
lesson
question
answer
exam
friend
family
mother
father
brother
sister
uncle
aunt
cousin
child
person
people
baby
friend
neighbor
house
home
room
kitchen
garden
window
door
floor
wall
roof
chair
bed
light
clock
phone
computer
laptop
screen
keyboard
mouse
camera
radio
music
movie
game
story
picture
photo
video
letter
message
school
college
office
market
shop
store
bank
hospital
station
airport
hotel
library
park
street
road
bridge
river
lake
ocean
beach
mountain
forest
field
village
city
country
world
earth
space
planet
star
moon
sun
cloud
rain
snow
storm
wind
weather
summer
winter
spring
autumn
morning
evening
night
today
tomorrow
yesterday
week
month
year
minute
second
hour
time
day
food
fruit
vegetable
potato
tomato
carrot
onion
garlic
pepper
salt
spoon
fork
plate
glass
bottle
cup
water
coffee
tea
shirt
pants
dress
skirt
shoe
sock
hat
coat
jacket
watch
bag
pocket
button
color
black
white
red
blue
green
yellow
orange
purple
brown
pink
gray
happy
sad
angry
calm
kind
brave
strong
smart
funny
quiet
loud
young
old
big
small
long
short
high
low
fast
slow
hot
cold
easy
hard
clean
dirty
new
good
great
best
better
bad
first
last
early
late
right
wrong
open
closed
full
empty
light
dark
near
far
inside
outside
above
below
before
after
always
never
often
sometimes
quick
simple
special
important
beautiful
happy
careful
ready
busy
free
safe
strong
bright
clear
fresh
sweet
soft
hard
rough
smooth
round
square
circle
line
point
shape
number
letter
word
name
place
thing
idea
problem
reason
answer
example
story
question
game
team
player
ball
sport
football
cricket
tennis
basketball
match
goal
score
win
lose
play
run
walk
jump
swim
dance
sing
read
write
draw
paint
cook
eat
drink
sleep
wake
sit
stand
talk
listen
watch
look
see
hear
think
learn
teach
study
work
help
make
build
create
find
give
take
bring
carry
keep
put
get
use
need
want
like
love
know
remember
forget
understand
start
stop
begin
finish
move
turn
push
pull
open
close
change
choose
check
try
help
call
send
show
tell
ask
say
speak
live
visit
travel
drive
ride
fly
buy
sell
pay
save
spend
cost
money
price
shop
market
job
work
business
company
team
group
leader
member
friend
family
community
country
people
world
animal
dog
cat
horse
cow
goat
sheep
bird
fish
lion
tiger
bear
monkey
rabbit
mouse
snake
frog
turtle
tree
flower
grass
plant
leaf
branch
fruit
seed
river
water
fire
earth
air
stone
sand
cloud
rain
snow
sun
moon
star
sky
mountain
valley
island
ocean
beach
forest
field
road
bridge
car
bus
train
plane
ship
boat
bike
wheel
engine
ticket
map
trip
travel
journey
place
country
city
town
village
street
building
office
room
door
window
key
lock
box
bag
book
page
paper
pen
pencil
computer
phone
screen
keyboard
internet
website
email
password
program
code
file
folder
number
data
system
machine
robot
science
math
history
language
English
question
answer
test
school
teacher
student
class
college
university
library
book
chapter
page
word
sentence
story
poem
song
music
movie
picture
camera
photo
video
sound
voice
color
shape
size
space
place
time
life
world
dream
hope
future
past
present
chance
choice
plan
goal
idea
reason
result
success
failure
power
energy
light
sound
water
fire
metal
wood
glass
paper
plastic
gold
silver
heart
head
face
eye
ear
nose
mouth
hand
finger
arm
leg
foot
hair
body
shirt
shoe
food
water
health
sleep
dream
smile
laugh
cry
friend
family
love
peace
hope
help
care
trust
truth
life
death
birth
world
future
dream

*/