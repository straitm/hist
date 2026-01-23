/* Takes a series of pairs of numbers and produces a scatter plot.
 * The input comes from stdin and is formatted like this:
 *
 * x1 y1
 * x2 y2
 *
 * 2dutf8hist takes two arguments: the horizontal and vertical number
 * of columns to use (for the plot itself, including labels)
 *
 * 2dutf8hist will use only ASCII characters if called as 2dasciihist.
 *
 * By "scatter plot" I mean something between scatter plot and
 * histogram.
 *
 * Compile with:
 *
 * gcc -O2 -Wextra -Wall 2dutf8hist.c -o 2dutf8hist
 *
 * I release this to the public domain or with the most permissive
 * license possible.
 *
 * - Matthew Strait
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <libgen.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>

#define MAXLIST 40

static int ascii = 0;

// For multiple series
static char * markers[MAXLIST] = {
"+", "*", "#", "=", "O",
"%", "&", "~", "X", "\"",
"@", "!", "$", "/", "|",
"B", "D", "E", "H", "I",
"K", "M", "N", "Q", "R",
"S", "U", "V", "W", "Z",
"A", "C", "F", "G", "J",
"L", "P", "T", "Y", "l"
};

static char * asciidots[256] =
{
" ","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","+",
"+","+","+","+","+","+","+","#"
};

static char * dots[256] = {
" ","⠁","⠂","⠃","⠄","⠅","⠆","⠇",
"⡀","⡁","⡂","⡃","⡄","⡅","⡆","⡇",
"⠈","⠉","⠊","⠋","⠌","⠍","⠎","⠏",
"⡈","⡉","⡊","⡋","⡌","⡍","⡎","⡏",
"⠐","⠑","⠒","⠓","⠔","⠕","⠖","⠗",
"⡐","⡑","⡒","⡓","⡔","⡕","⡖","⡗",
"⠘","⠙","⠚","⠛","⠜","⠝","⠞","⠟",
"⡘","⡙","⡚","⡛","⡜","⡝","⡞","⡟",
"⠠","⠡","⠢","⠣","⠤","⠥","⠦","⠧",
"⡠","⡡","⡢","⡣","⡤","⡥","⡦","⡧",
"⠨","⠩","⠪","⠫","⠬","⠭","⠮","⠯",
"⡨","⡩","⡪","⡫","⡬","⡭","⡮","⡯",
"⠰","⠱","⠲","⠳","⠴","⠵","⠶","⠷",
"⡰","⡱","⡲","⡳","⡴","⡵","⡶","⡷",
"⠸","⠹","⠺","⠻","⠼","⠽","⠾","⠿",
"⡸","⡹","⡺","⡻","⡼","⡽","⡾","⡿",
"⢀","⢁","⢂","⢃","⢄","⢅","⢆","⢇",
"⣀","⣁","⣂","⣃","⣄","⣅","⣆","⣇",
"⢈","⢉","⢊","⢋","⢌","⢍","⢎","⢏",
"⣈","⣉","⣊","⣋","⣌","⣍","⣎","⣏",
"⢐","⢑","⢒","⢓","⢔","⢕","⢖","⢗",
"⣐","⣑","⣒","⣓","⣔","⣕","⣖","⣗",
"⢘","⢙","⢚","⢛","⢜","⢝","⢞","⢟",
"⣘","⣙","⣚","⣛","⣜","⣝","⣞","⣟",
"⢠","⢡","⢢","⢣","⢤","⢥","⢦","⢧",
"⣠","⣡","⣢","⣣","⣤","⣥","⣦","⣧",
"⢨","⢩","⢪","⢫","⢬","⢭","⢮","⢯",
"⣨","⣩","⣪","⣫","⣬","⣭","⣮","⣯",
"⢰","⢱","⢲","⢳","⢴","⢵","⢶","⢷",
"⣰","⣱","⣲","⣳","⣴","⣵","⣶","⣷",
"⢸","⢹","⢺","⢻","⢼","⢽","⢾","⢿",
"⣸","⣹","⣺","⣻","⣼","⣽","⣾","⣿"
};

static double ** hist[MAXLIST] = { NULL };

static const int BINSPERROW = 4;
static const int BINSPERCOL = 2;

#define MAXYLABELWIDTH 12

static int userWIDTH  = 80-MAXYLABELWIDTH;
static int WIDTH;
static int HEIGHT = 21;

static int MINYLABELWIDTH = 0;

static int alwayscolor = 0;
static int dosnap = 1;
static int allow_partial_draw = 1;

static void sigusr1(__attribute__((unused)) int sig)
{
  if(allow_partial_draw) allow_partial_draw = 0;
  else allow_partial_draw = 1;
}


/* UTF-8 is up to 4 bytes wide, plus 10 bytes for setting and clearing
 * the color, plus leave plenty of room for the axis labels and whatever
 */
#define linechars (14*WIDTH+1000)

#define BLACK    (alwayscolor||isatty(1)?"\033[30m"  :"")
#define BBLACK   (alwayscolor||isatty(1)?"\033[30;1m":"") /* bold black */
#define RED      (alwayscolor||isatty(1)?"\033[31m"  :"")
#define BRED     (alwayscolor||isatty(1)?"\033[31;1m":"") /* bold red */
#define GREEN    (alwayscolor||isatty(1)?"\033[32m"  :"")
#define BGREEN   (alwayscolor||isatty(1)?"\033[32;1m":"") /* bold green */
#define YELLOW   (alwayscolor||isatty(1)?"\033[33m"  :"")
#define BYELLOW  (alwayscolor||isatty(1)?"\033[33;1m":"") /* bold yellow */
#define BLUE     (alwayscolor||isatty(1)?"\033[34m"  :"")
#define BBLUE    (alwayscolor||isatty(1)?"\033[34;1m":"") /* bold blue */
#define MAGENTA  (alwayscolor||isatty(1)?"\033[35m"  :"")
#define BMAGENTA (alwayscolor||isatty(1)?"\033[35;1m":"") /* bold magenta */
#define CYAN     (alwayscolor||isatty(1)?"\033[36m"  :"")
#define BCYAN    (alwayscolor||isatty(1)?"\033[36;1m":"") /* bold cyan */
#define WHITE    (alwayscolor||isatty(1)?"\033[37m"  :"")
#define BWHITE   (alwayscolor||isatty(1)?"\033[37;1m":"") /* bold white */
#define CLR      (alwayscolor||isatty(1)?"\033[m"    :"") /* clear */

struct clist{
  double x, y;
  struct clist * next;
};

static char tempbuf[64];

static double yvalofrow(const double miny, const double maxy,
                        const int row)
{
  const double proviso = maxy - (row+0.5)/HEIGHT*(maxy - miny);
  // Avoid printing silly tiny numbers when really it just means zero.
  // Try to be intelligent enough with the height so that we get
  // the right answer even for really tall plots.
  if(fabs(proviso) < (maxy - miny)*1e-5/HEIGHT) return 0;
  return proviso;
}

static int findylabelwidth(const double miny, const double maxy)
{
  const int yxwidth = sprintf(tempbuf, "%g", maxy);
  const int ynwidth = sprintf(tempbuf, "%g", miny);
  int row;
  int max = yxwidth > ynwidth? yxwidth : ynwidth;
  for(row = 2; row < HEIGHT-1; row++){
    const int width = sprintf(tempbuf, "%g", yvalofrow(miny,maxy,row));
    if(width > max) max = width;
  }
  if(max >= MINYLABELWIDTH) return max;
  return MINYLABELWIDTH;
}


static double roundto(const double in, const double to)
{
  return in < 0? -roundto(-in, to): (double)((int)(in/to + 0.5))*to;
}

static int alignto(const double interval, const double tolerance,
                   double * top, double * bot)
{
  const double botin = *bot, topin = *top;
  const double rawlineheight = (*top - *bot)/HEIGHT;
  if(interval > rawlineheight &&
     interval < rawlineheight + interval*tolerance){
    const double center = roundto((*top + *bot)/2, interval);
    const double tenttop = center + interval*HEIGHT/2.0;
    const double tentbot = center - interval*HEIGHT/2.0;
    if(tenttop < *top || tentbot > *bot) goto fail;

    *bot = tentbot;
    *top = tentbot + HEIGHT*interval;

    // If all values are positive, put the empty space at the bottom. If
    // all negative, put it at the top. Otherwise center the points.
    if(botin > 0 && topin > 0){
      while(*top > topin && *bot - interval > 0){
        *bot -= interval;
        *top -= interval;
      }
      *bot += interval;
      *top += interval;
    }
    else if(botin < 0 && topin < 0){
      while(*bot < botin && *top + interval < 0){
        *bot += interval;
        *top += interval;
      }
      *bot -= interval;
      *top -= interval;
    }

    // If this happens, there is a bug above.
    if(*top < topin) goto fail;
    if(*bot > botin) goto fail;

    return 1;
  }
  fail:
  *top=topin; *bot=botin; return 0;
}

static void nicifytopbot(double * top, double * bot)
{
  // Try to snap to nice intervals
  int e;
  for(e = -37; e <= 37; e++){
    if(alignto(0.10  *pow(10, e), 0.2, top, bot)) return;
    if(alignto(0.125 *pow(10, e), 0.1, top, bot)) return;
    if(alignto(0.15  *pow(10, e), 0.1, top, bot)) return;
    if(alignto(0.20  *pow(10, e), 0.1, top, bot)) return;
    if(alignto(0.25  *pow(10, e), 0.1, top, bot)) return;
    if(alignto((1/3.)*pow(10, e), 0.1, top, bot)) return;
    if(alignto(0.50  *pow(10, e), 0.1, top, bot)) return;
  }

  int i;
  // try really hard to snap to less nice intervals
  for(e = -37; e <= 37; e++)
    for(i = 3; i <= 9; i++)
      if(alignto(i*0.1*pow(10, e), 0.5, top, bot)) return;
}

/* Sets first four arguments to the mins and maxes in x and y found in
 * list, except that if any of the first four arguments are not DBL_MAX (mins)
 * or -DBL_MAX (maxes), then they are left alone.
 */
static int ranges(double * minx, double * maxx,
                  double * miny, double * maxy,
                  struct clist ** list)
{
  const double orig_minx = *minx;
  const double orig_maxx = *maxx;
  const double orig_miny = *miny;
  const double orig_maxy = *maxy;

  // If the user gave us values, then those that we got are not set to
  // MAX. Going to find what values we would use, and then put any that
  // the user has specified back to the user values afterwards.
  const int reset_minx = *minx !=  DBL_MAX;
  const int reset_maxx = *maxx != -DBL_MAX;
  const int reset_miny = *miny !=  DBL_MAX;
  const int reset_maxy = *maxy != -DBL_MAX;

  int i;
  for(i = 0; i < MAXLIST; i++){
    const struct clist * L = list[i];
    if(L == NULL || L->next == NULL) continue;
    do{
      if(L->x < *minx) *minx = L->x;
      if(L->y < *miny) *miny = L->y;
      if(L->x > *maxx) *maxx = L->x;
      if(L->y > *maxy) *maxy = L->y;
    }while((L = L->next)->next != NULL);
  }

  // If all values are the same, move the min and max apart
  if(*minx == *maxx) (*maxx)++, (*maxx)*=2;
  if(*miny == *maxy) (*maxy)++, (*maxy)*=2;

  // We only print periodic axis labels for y, so only nicify it
  if(dosnap) nicifytopbot(maxy, miny);

  // Put back whichever values the user specified
  if(reset_minx) *minx = orig_minx;
  if(reset_maxx) *maxx = orig_maxx;
  if(reset_miny) *miny = orig_miny;
  if(reset_maxy) *maxy = orig_maxy;

  WIDTH = userWIDTH + MAXYLABELWIDTH-findylabelwidth(*miny, *maxy);

  /* If WIDTH < 1 can't actually print anything.  Consider this
   a non-error, though. */
  return WIDTH >= 1;
}

static void clearhist()
{
  int l;
  for(l = 0; l < MAXLIST; l++){
    double ** H = hist[l];
    if(H == NULL) continue;

    int binsx = WIDTH*BINSPERCOL;
    int i;
    for(i = 0; i < binsx; i++) free(H[i]);
    free(H);

    H = NULL;
  }
}

/* hist[list][row][column] */
static void setuphist()
{
  int binsx = WIDTH*BINSPERCOL;
  int binsy = HEIGHT*BINSPERROW;
  int l;
  for(l = 0; l < MAXLIST; l++){
    hist[l] = malloc(sizeof(double*)*(binsx+10));
    double ** H = hist[l];
    int i;
    for(i = 0; i < binsx; i++){
       H[i] = malloc(sizeof(double)*(binsy+10));
       memset(H[i], 0, sizeof(double)*(binsy+10));
    }
  }
}

static void mkhist(double minx, double maxx, double miny,
                   double maxy, struct clist ** list)
{
  int binsx = WIDTH*BINSPERCOL;
  int binsy = HEIGHT*BINSPERROW;

  int i;

  for(i = 0; i < MAXLIST; i++){
    const struct clist * L = list[i];
    if(L == NULL || L->next == NULL) continue;
    do{
      /* binsx(y)-1 so that values equal to the maximum go in the top
         bin and not out of range.  Important when plotting integer
         values. For floating point, mostly silly. */
      int binx =         (L->x - minx)/(maxx - minx) * (binsx-1);
      int biny = binsy - (L->y - miny)/(maxy - miny) * (binsy-1)-1;
      if(binx >= 0 && biny >= 0 && binx < binsx && biny < binsy)
        hist[i][binx][biny]++;
    }while((L = L->next)->next != NULL);
  }
}

static char * reallydraw(char * printbuf, double *** hist,
                         const int ylabelwidth, const double miny,
                         const double maxy, const int colorbynumber)
{
  int row, rowbin, col, colbin;
  for(row = 0; row < HEIGHT; row++){
    int i;
    if(row != 0){
      if(row == 1 || row == HEIGHT-1){
        for(i = 0; i < ylabelwidth; i++)
          printbuf += sprintf(printbuf, " ");
        printbuf += sprintf(printbuf, ascii?"|":"│");
      }
      else {
        const double yval = yvalofrow(miny, maxy, row);
        const int width = sprintf(tempbuf, "%g", yval);
        for(i = 0; i < ylabelwidth-width; i++)
          printbuf += sprintf(printbuf, " ");
        printbuf += sprintf(printbuf, "%g", yval);
        printbuf += sprintf(printbuf, ascii?"|":"┤");
      }
    }
    else{
      const int yxwidth = sprintf(tempbuf, "%g", maxy);
      for(i = 0; i < ylabelwidth-yxwidth; i++)
        printbuf += sprintf(printbuf, " ");
      printbuf += sprintf(printbuf, ascii?"%g/":"%g╱", maxy);
    }

    for(col = 0; col < WIDTH; col++){
      unsigned int printcode = 0;
      unsigned int whichlist = 0;
      int sum = 0;
      int colorok = 1;
      for(rowbin = 0; rowbin < BINSPERROW; rowbin++){
        for(colbin = 0; colbin < BINSPERCOL; colbin++){
          int binx = col*BINSPERCOL + colbin;
          int biny = row*BINSPERROW + rowbin;
          unsigned int histi;
          for(histi = 0; histi < MAXLIST; histi++){
            if(hist[histi][binx][biny] > 0){

              // New series: just overwrite old within the character
              if(histi > whichlist){
                printcode = 0;
                whichlist = histi;
              }
              printcode |= 1 << (rowbin + BINSPERROW*colbin);
            }
            sum += hist[histi][binx][biny];
          }

          // all bins in the character must have at least one entry for
          // us to color it.  Only check the first histogram, because
          // if there is more than one, this isn't what color is used for.
          if(hist[0][binx][biny] == 0) colorok = 0;
        }
      }

      // Well, here we are.  Do we need braille indicating one histogram,
      // ascii indicating one histogram, or ascii indicating several?
      // Are we using color?

      if(colorbynumber && colorok){
        if     (sum > 10000)printbuf += sprintf(printbuf, MAGENTA);
        else if(sum > 3010) printbuf += sprintf(printbuf, RED);
        else if(sum > 1000) printbuf += sprintf(printbuf, YELLOW);
        else if(sum > 301)  printbuf += sprintf(printbuf, GREEN);
        else if(sum > 100)  printbuf += sprintf(printbuf, CYAN);
        else if(sum > 30)   printbuf += sprintf(printbuf, WHITE);
        else colorok = 0;
      }
      else if(!colorbynumber /* i.e. color by list */){
        if     (whichlist%6 == 0) printbuf += sprintf(printbuf, WHITE);
        else if(whichlist%6 == 1) printbuf += sprintf(printbuf, CYAN);
        else if(whichlist%6 == 2) printbuf += sprintf(printbuf, GREEN);
        else if(whichlist%6 == 3) printbuf += sprintf(printbuf, YELLOW);
        else if(whichlist%6 == 4) printbuf += sprintf(printbuf, RED);
        else if(whichlist%6 == 5) printbuf += sprintf(printbuf, MAGENTA);
      }

      // If in ASCII mode without color, can't use different symbols both to
      // indicate bin contents in the primary series and to indicate multiple
      // series, so force the primary series to always be the same symbol.
      if(!colorbynumber && !isatty(1) && !alwayscolor && ascii)
        printbuf += sprintf(printbuf, "%s",
                            printcode?markers[whichlist]:" ");

      // Otherwise, if there are multiple series, series beyond the first get
      // distinct colored ascii symbols and the first series gets white
      // braille.  If there is one series, it is colored by amount in each
      // character.
      else
        printbuf += sprintf(printbuf, "%s",
                            whichlist?markers[whichlist]:
                            (ascii?asciidots:dots)[printcode]);

      if(!colorbynumber || (colorok && sum > 10)) printbuf += sprintf(printbuf, CLR);
    }
    printbuf += sprintf(printbuf, "\n");
  }

  return printbuf;
}

static void draw(struct clist ** list, double minx, double maxx,
                 double miny, double maxy)
{
  clearhist();

  if(!ranges(&minx, &maxx, &miny, &maxy, list)){
    puts("No room to draw");
    return;
  }

  setuphist();

  mkhist(minx, maxx, miny, maxy, list);

  char * printbuf = malloc(linechars*(HEIGHT+2));
  memset(printbuf, 'K', linechars*(HEIGHT+2));

  const int ylabelwidth = findylabelwidth(miny, maxy);

  int i;
  int colorbynumber = 1;
  for(i = 1; i < MAXLIST; i++)
    if(!(list[i] != NULL && list[i]->next != NULL))
      colorbynumber = 0;

  char * bufptr = reallydraw(printbuf, hist, ylabelwidth, miny, maxy,
                             colorbynumber);

  const int ynwidth = sprintf(tempbuf, "%g", miny);
  for(i = 0; i < ylabelwidth-ynwidth; i++) bufptr+=sprintf(bufptr, " ");
  bufptr += sprintf(bufptr, ascii?"%g+":"%g┼", miny);
  for(i = 0; i < WIDTH-1; i++) bufptr += sprintf(bufptr, ascii?"-":"─");
  bufptr += sprintf(bufptr, ascii?"/\n":"╱\n");

  memset(bufptr, ' ', linechars);

  char * imin = bufptr+ylabelwidth;
  const int minkill = ylabelwidth+sprintf(imin, "%g", minx);
  bufptr[minkill] = ' '; // overwrite the NULL with a space

  const int maxlen = sprintf(tempbuf, "%g", maxx);
  int nmax = ylabelwidth+WIDTH-maxlen+1;
  char * imax = bufptr+nmax;
  sprintf(imax, "%g\n", maxx);

  // If we can fit a label in the middle, put one there
  const double midx = (minx+maxx)/2;
  const int len = sprintf(tempbuf, "%g", midx);
  int nmid = ylabelwidth+(WIDTH-len)/2+1;
  char * imid = bufptr+nmid;
  if(nmid - minkill > 0 && nmid+len < nmax){
    const int kill = ylabelwidth+(WIDTH-len)/2+1+sprintf(imid, "%g", midx);
    bufptr[kill] = ' ';
  }

  printf("%s", printbuf);
  free(printbuf);
}

void printhelp()
{
  printf(
  "This is 2dutf8hist, a 2D histogramming/scatterplot program for\n"
  "VT100 compatible terminals and their emulators.\n"
  "\n"
  "With no arguments, 2dutf8hist reads lines with pairs of numbers from \n"
  "stdin and prints a scatterplot to stdout.  As character cells of this\n"
  "scatterplot become completely filled with dots, they are converted\n"
  "into histogram bins in groups of 8 (2 wide, 4 high) and color\n"
  "indicates the number.  The colors are:\n"
  "\n"
  "  Default: <=    30\n"
  "  White:   <=   100\n"
  "  Cyan:    <=   301\n"
  "  Green:   <=  1000\n"
  "  Yellow:  <=  3010\n"
  "  Red:     <= 10000\n"
  "  Magenta: >  10000\n"
  "\n"
  "A 2x4 cell can have an arbitrarily large count in it and remain\n"
  "uncolored if not all 8 subcells are filled.\n"
  "\n"
  "If the output of 2dutf8hist is not a terminal, color is disabled.\n"
  "You can override this by giving \"forcecolor\" as an argument.\n"
  "Probably 2dutf8hist should sense whether its tty is color-capable\n"
  "and disable color if not, but it currently doesn't do this.\n"
  "\n"
  "If called as 2dasciihist intead of 2dutf8hist, it restricts itself\n"
  "to the ASCII character set, giving better compatibility with older\n"
  "hardware and software, but limiting the resolution to 1 dot per\n"
  "character cell rather than 8.  Probably 2dutf8hist should sense\n"
  "whether its tty is UTF-8 capable and switch to this mode if not,\n"
  "but it currently doesn't do this.\n"
  "\n"
  "If you give an integer as the first argument, it sets the width of\n"
  "the plot, *including* the axis and labels (which is sometimes what\n"
  "you want and sometimes annoying --- perhaps it should also be able\n"
  "to set the width of the plot-proper).\n"
  "\n"
  "If you give an integer as the second argument, it sets the height.\n"
  "\n"
  "If you give integers as the third, fourth, fifth, and/or sixth\n"
  "arguments, these set, respectively, the minimum value of the x-axis,\n"
  "the maximum x, the minimum y and the maximum y.  This possibly puts\n"
  "some of the data outside the visible area.\n"
  "\n"
  "The preceding six numerical arguments must appear exactly in the\n"
  "specified positions.  The other arguments can appear anywhere,\n"
  "including in those positions.  Any of the six numerical arguments\n"
  "can be omitted by putting another flag in its place.  If you don't\n"
  "have anything else to put there, put \"noop\" which does nothing.\n"
  "\n"
  "You may give as an argument the letter 'y' followed by a number,\n"
  "for instance \"y7\". This sets the minimum width of the y axis\n"
  "label.  It will be space padded on the left to meet this minimum.\n"
  "\n"
  "Normally 2dutf8hist tries to pick a y-axis range such that there\n"
  "are sensibly round intervals between rows and the labels look nice.\n"
  "This usually has the side effect of wasting some space.  To disable\n"
  "this snapping, give \"nosnap\" as an argument.\n"
  "\n"
  "By default, if 2dutf8hist reads data for more than 3 seconds\n"
  "it starts outputting plots with the data it has so far rather than\n"
  "waiting until the input has finished.  To disable this behavior,\n"
  "perhaps because you are running it non-interactively, give\n"
  "\"nopartialdraw\" as an argument.  A SIGUSR1 toggles 2dutf8hist\n"
  "between allowing partial drawing and not.\n"
  "\n"
  "And here's a particularly overreaching feature. If one of the input\n"
  "lines is \"newseries\", 2dutf8hist interprets the following lines\n"
  "as being from a different data set and plots them with a different\n"
  "colors and symbols.  Series beyond the first are plotted with large\n"
  "symbols and cover up previous series.\n"
  );
}

int argisnum(char * arg)
{
  return (arg[0] >= '0' && arg[0] <= '9') || arg[0] == '-';
}

int main(int argc, char ** argv)
{
  if(!strcmp(basename(argv[0]), "2dasciihist")) ascii = 1;
  if(argc > 1 && (
     !strcmp(argv[1], "-v") ||
     !strcmp(argv[1], "-V") ||
     !strcmp(argv[1], "--version") ||
     !strcmp(argv[1], "-h") ||
     !strcmp(argv[1], "--help") ||
     !strcmp(argv[1], "help") ||
     !strcmp(argv[1], "-?"))){
    printhelp();
    return 0;
  }

  int skiparg[8] = { 0 };
  if(argc > 1 && argisnum(argv[1])){
    userWIDTH  = atoi(argv[1])-MAXYLABELWIDTH-1;
    skiparg[1] = 1;
  }
  if(argc > 2 && argisnum(argv[2])){
    HEIGHT = atoi(argv[2])-3;
    skiparg[2] = 1;
  }

  double minx = DBL_MAX;
  double maxx = -DBL_MAX;
  double miny = DBL_MAX;
  double maxy = -DBL_MAX;
  {
    int errno = 0;
    double * extremes[4] = { &minx, &maxx, &miny, &maxy };
    char * endptr;
    int i;
    /* Look at third through sixth arguments */
    for(i = 0; i < 4; i++){
      int arg = i+3;
      if(argc <= arg) continue;
      double val = strtod(argv[arg], &endptr);
      if(errno == 0 && endptr != argv[arg]){
        *extremes[i] = val;
        skiparg[arg] = 1;
      }
    }
  }

  {
    int a = 1;
    for(; a < argc; a++){
      if(skiparg[a]) continue;
      if(!strcmp(argv[a], "noop")) /* do nothing */;
      else if(!strcmp(argv[a], "forcecolor")) alwayscolor = 1;
      else if(!strcmp(argv[a], "nosnap")) dosnap = 0;
      else if(!strcmp(argv[a], "nopartialdraw")) allow_partial_draw = 0;
      else if(argv[a][0] == 'y') MINYLABELWIDTH = atoi(argv[a] + 1);
      else{
        fprintf(stderr, "Unrecognized argument %s\n", argv[a]);
        return 1;
      }
    }
  }

  signal(SIGUSR1, sigusr1);

  /* Let's consider this a valid request, I suppose, and return success */
  if(HEIGHT < 1) return 0;

  double xin, yin;
  struct clist * list[MAXLIST] = { NULL };

  int listnow = 0;
  list[listnow] = malloc(sizeof(struct clist));
  list[listnow]->next = NULL;
  struct clist * listp = list[listnow];

  unsigned int linen = 0;
  int lasttime = time(NULL);

  char * line;
  size_t linelength = 0;
  while(getline(&line, &linelength, stdin) != -1){
    linen++;
    if(!strcmp(line, "newseries\n")){
      listnow++;
      if(listnow >= MAXLIST){
        fprintf(stderr, "Too many series, max %d, stopped at line %u\n",
                MAXLIST, linen);
        break;
      }
      list[listnow] = malloc(sizeof(struct clist));
      list[listnow]->next = NULL;
      listp = list[listnow];

      continue;
    }

    if(2 != sscanf(line, "%lf %lf", &xin, &yin)){
      fprintf(stderr, "Line %u is bad:\n%s\nStopping here\n",
              linen, line);
      break;
    }

    if(isinf(xin) || isinf(yin)) continue;
    if(isnan(xin) || isnan(yin)) continue;
    listp->x = xin;
    listp->y = yin;
    listp->next = malloc(sizeof(struct clist));
    listp = listp->next;
    listp->next = NULL;

    if(allow_partial_draw){
      const int timenow = time(NULL);
      if(timenow > lasttime+3){
        puts("Partial draw due to long running");
        draw(list, minx, maxx, miny, maxy);
        fflush(stdout);
        lasttime = timenow;
      }
    }
  }

  listp->next = NULL;

  int anything = 0;
  int i;
  for(i = 0; i < MAXLIST; i++)
    if(list[i] != NULL && list[i]->next != NULL)
      anything = 1;

  if(!anything){
    puts("Nothing to draw");
    return 0;
  }

  draw(list, minx, maxx, miny, maxy);

  return 0;
}
