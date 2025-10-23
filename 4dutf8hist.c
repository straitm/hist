/* Takes a series of quadruplets of numbers and produces a scatter plot.
 * The input comes from stdin and is formatted like this:
 *
 * x1 y1 z1 t1
 * x2 y2 z2 t2
 *
 * 4dutf8hist takes two arguments: the horizontal and vertical number
 * of columns to use (for the plot itself, or including labels?)
 *
 * 4dutf8hist will use only ASCII characters if called as 4dasciihist.
 *
 * Compile with:
 *
 * gcc -O3 -Wextra -Wall 4dutf8hist.c -o 4dutf8hist -lm -lncurses
 *
 * This is *extremely* half-baked and does not rotate the way you'd
 * like, nor does it have good labels.
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
#include <sys/time.h>
#include <term.h>

static int ascii = 0;

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

static double ** hist = NULL;

static const int BINSPERROW = 4;
static const int BINSPERCOL = 2;

#define MAXYLABELWIDTH 12

static int userWIDTH  = 80-MAXYLABELWIDTH;
static int WIDTH;
static int HEIGHT = 21;

static int alwayscolor = 0;

/* UTF-8 is up to 4 bytes wide, plus 10 bytes for setting and clearing
 * the color, plus leave plenty of room for the axis labels and whatever
 */
#define linechars (14*WIDTH+200)

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

struct pair{
  double x, y;
};

struct quad{
  double x, y, z, t;
};

static char tempbuf[64];



static int termrows, termcols;

/* * character output for tputs() only */
static int outputc(int c)
{
  fputc(c, stdout);
  return 1;
}

/* * cursor positioning, * *  home is 1,1 */
static void cursor_move(int row, int col)
{
  /* reject out of hand anything negative */
  if (--row < 0 || --col < 0) return;

  /* fold out of range values back into the screen */
  if (termrows) row = row % termrows;
  if (termcols) col = col % termcols;

  /* move the cursor */
  tputs(tparm(cursor_address, row, col), 1, outputc);
}

/* * home the cursor and clear to end of screen */
static void cls()
{
  tputs(clear_screen, 1, outputc);
}

/* * set up the terminal */
static void init_terminal()
{
  char *term = getenv("TERM");
  int err_ret;

  if ( !term || ! *term) {
    fprintf(stderr,"The TERM environment variable is not set.\n");
    exit(4);
  }

  setupterm(term, 1, &err_ret);

  switch (err_ret) {
  case -1:
    fprintf(stderr,"Can't access terminfo database - ");
    fprintf(stderr,"check TERMINFO environment variable\n");
    exit(3);
  case 0:
    fprintf(stderr, "Can't find entry for terminal %s\n", term);
    exit(2);
  case 1:
    break;
  default:
    fprintf(stderr, "Unknown tgetent return code %d\n", err_ret);
    exit(1);
  }
}

static double yvalofrow(const double miny, const double maxy,
                        const int row)
{
  return maxy - (row+0.5)/HEIGHT*(maxy - miny);
}

static int findylabelwidth(const double miny, const double maxy)
{
  const int yxwidth = sprintf(tempbuf, "%*.*g", 4, 3, maxy);
  const int ynwidth = sprintf(tempbuf, "%*.*g", 4, 3, miny);
  int row;
  int max = yxwidth > ynwidth? yxwidth : ynwidth;
  for(row = 2; row < HEIGHT-1; row++){
    const int width =
      sprintf(tempbuf, "%*.*g", 4, 3, yvalofrow(miny,maxy,row));
    if(width > max) max = width;
  }
  return max;
}

static int ranges(double * minx, double * maxx,
                  double * miny, double * maxy,
                  const struct pair * list, const int size)
{
  const double orig_minx = *minx;
  const double orig_maxx = *maxx;
  const double orig_miny = *miny;
  const double orig_maxy = *maxy;

  const int reset_minx = *minx !=  DBL_MAX;
  const int reset_maxx = *maxx != -DBL_MAX;
  const int reset_miny = *miny !=  DBL_MAX;
  const int reset_maxy = *maxy != -DBL_MAX;

  int i;
  for(i = 0; i < size; i++){
    if(list[i].x < *minx) *minx = list[i].x;
    if(list[i].y < *miny) *miny = list[i].y;
    if(list[i].x > *maxx) *maxx = list[i].x;
    if(list[i].y > *maxy) *maxy = list[i].y;
  }

  if(*minx == *maxx) (*maxx)++, (*maxx)*=2;
  if(*miny == *maxy) (*maxy)++, (*maxy)*=2;

  if(reset_minx) *minx = orig_minx;
  if(reset_maxx) *maxx = orig_maxx;
  if(reset_miny) *miny = orig_miny;
  if(reset_maxy) *maxy = orig_maxy;

  WIDTH = userWIDTH + MAXYLABELWIDTH-findylabelwidth(*miny, *maxy);

  /* If WIDTH < 1 can't actually print anything.  Consider this
   a non-error, though. */
  return WIDTH >= 1;
}

/* hist[row][column] */
static double ** allochist()
{
  int binsx = WIDTH*BINSPERCOL+100;
  int binsy = HEIGHT*BINSPERROW+100;
  double ** hist = malloc(sizeof(double*)*binsx);
  int i;
  for(i = 0; i < binsx; i++){
     hist[i] = malloc(sizeof(double)*binsy);
     memset(hist[i], 0, sizeof(double)*binsy);
  }
  return hist;
}

static void clearhist()
{
  int binsx = WIDTH*BINSPERCOL;
  int binsy = HEIGHT*BINSPERROW;
  int i;
  for(i = 0; i < binsx; i++)
    memset(hist[i], 0, sizeof(double)*binsy);
}

static double ** mkhist(double minx, double maxx, double miny,
                        double maxy, const struct pair * list,
                        const int size)
{
  int binsx = WIDTH*BINSPERCOL;
  int binsy = HEIGHT*BINSPERROW;
  int i;
  for(i = 0; i < size; i++){
    /* binsx(y)-1 so that values equal to the maximum go in the top
       bin and not out of range.  Important when plotting integer
       values. For floating point, mostly silly. */
    int binx =         (list[i].x - minx)/(maxx - minx) * (binsx-1);
    int biny = binsy - (list[i].y - miny)/(maxy - miny) * (binsy-1)-1;
    if(binx >= 0 && biny >= 0 && binx < binsx && biny < binsy)
      hist[binx][biny]++;
  }
  return hist;
}

static int build4dlist(struct pair * ans, struct quad * list,
                        const int size, const double th,
                        const double phi, const double tim)
{
  int i, n = 0;
  for(i = 0; i < size; i++){
    if(list[i].t > tim) continue;
    const double x = list[i].x, y = list[i].y, z = list[i].z;
    ans[n].x =  x*cos(th) + y*sin(th);
    ans[n].y = -z*sin(phi) + (-x*sin(th) + y*cos(th))*cos(phi);
    n++;
  }
  return n;
}

static char * reallydraw(char * printbuf, double ** hist,
                         const int ylabelwidth, const double miny,
                         const double maxy)
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
        const int width = sprintf(tempbuf, "%4.3g", yval);
        for(i = 0; i < ylabelwidth-width; i++)
          printbuf += sprintf(printbuf, " ");
        printbuf += sprintf(printbuf, "%4.3g", yval);
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
      int sum = 0;
      int colorok = 1;
      for(rowbin = 0; rowbin < BINSPERROW; rowbin++){
        for(colbin = 0; colbin < BINSPERCOL; colbin++){
          int binx = col*BINSPERCOL + colbin;
          int biny = row*BINSPERROW + rowbin;
          if(hist[binx][biny] > 0)
            printcode |= 1 << (rowbin + BINSPERROW*colbin);
          sum += hist[binx][biny];
          if(hist[binx][biny] == 0) colorok = 0;
        }
      }

      if(colorok){
        if     (sum > 10000)printbuf += sprintf(printbuf, MAGENTA);
        else if(sum > 3010) printbuf += sprintf(printbuf, RED);
        else if(sum > 1000) printbuf += sprintf(printbuf, YELLOW);
        else if(sum > 301)  printbuf += sprintf(printbuf, GREEN);
        else if(sum > 100)  printbuf += sprintf(printbuf, CYAN);
        else if(sum > 30)   printbuf += sprintf(printbuf, WHITE);
        else colorok = 0;
      }

      printbuf += sprintf(printbuf, "%s",
                        (ascii?asciidots:dots)[printcode]);
      if(colorok && sum > 10) printbuf += sprintf(printbuf, CLR);
    }
    printbuf += sprintf(printbuf, "\n");
  }
  return printbuf;
}

static void draw(struct pair * const list, const int size,
                 double minx, double maxx, double miny, double maxy)
{
  if(!ranges(&minx, &maxx, &miny, &maxy, list, size)) return;

  termcols = tigetnum("cols");
  termrows = tigetnum("lines");

  if(!hist) hist = allochist();
  clearhist();
  double ** hist = mkhist(minx, maxx, miny, maxy, list, size);

  static char * printbuf = NULL;
  if(!printbuf) printbuf = malloc(linechars*(HEIGHT+2));

  const int ylabelwidth = findylabelwidth(miny, maxy);

  char * bufptr = reallydraw(printbuf, hist, ylabelwidth, miny, maxy);
  char * origbufptr = bufptr;

  const int ynwidth = sprintf(tempbuf, "%g", miny);
  int i;
  for(i = 0; i < ylabelwidth-ynwidth; i++)
    bufptr += sprintf(bufptr, " ");
  bufptr += sprintf(bufptr, ascii?"%g+":"%g┼", miny);
  for(i = 0; i < WIDTH-1; i++) bufptr += sprintf(bufptr, ascii?"-":"─");
  bufptr += sprintf(bufptr, ascii?"/\n":"╱\n");

  memset(bufptr, ' ', linechars - (bufptr - origbufptr));

  const int kill = ylabelwidth+sprintf(bufptr+ylabelwidth, "%g", minx);
  bufptr[kill] = ' ';

  const int len = sprintf(tempbuf, "%g", maxx);
  sprintf(bufptr+ylabelwidth+WIDTH-len+1, "%g\n", maxx);

  cursor_move(1,1);
  printf("%s", printbuf);
}

int main(int argc, char ** argv)
{
  if(!strcmp(basename(argv[0]), "4dasciihist")) ascii = 1;
  if(argc > 1) userWIDTH  = atoi(argv[1])-MAXYLABELWIDTH-1;
  if(argc > 2) HEIGHT = atoi(argv[2])-3;

  double minx = argc > 4? strtod(argv[4], NULL):  DBL_MAX;
  double maxx = argc > 5? strtod(argv[5], NULL): -DBL_MAX;
  double miny = argc > 6? strtod(argv[6], NULL):  DBL_MAX;
  double maxy = argc > 7? strtod(argv[7], NULL): -DBL_MAX;

  /* Consider this a valid request, I suppose, and return success */
  if(HEIGHT < 1) return 0;

  double xin, yin, zin, tin;
  unsigned int allocsize = 1024;
  struct quad * list = malloc(sizeof(struct quad)*allocsize);

  init_terminal();
  cls();

  double maxtime = -DBL_MAX;
  double mintime =  DBL_MAX;
  unsigned int linen = 0, pointn = 0;
  while(1){
    linen++;
    int nfields;
    if((nfields=scanf("%lf %lf %lf %lf\n", &xin,&yin,&zin,&tin))==EOF)
      break;
    if(nfields < 4){
      char foo;
      if(nfields > 0 && EOF == scanf("%c", &foo))
        fprintf(stderr, "File ended in the middle of line %u\n", linen);
      else
        fprintf(stderr, "Garbage on line %u, stopping here\n", linen);
      break;
    }

    if(isinf(xin) || isinf(yin) || isinf(zin) || isinf(tin)) continue;
    if(isnan(xin) || isnan(yin) || isnan(zin) || isnan(tin)) continue;

    if(pointn >= allocsize){
      allocsize *= 2;
      list = realloc(list, sizeof(struct quad)*allocsize);
    }

    list[pointn].x = xin;
    list[pointn].y = yin;
    list[pointn].z = zin;
    list[pointn].t = tin;

    if(tin > maxtime) maxtime = tin;
    if(tin < mintime) mintime = tin;

    pointn++;
  }

  double th, tim = mintime;
  struct pair * flatlist = malloc(sizeof(struct pair)*pointn);
  for(th = 0; ; th += 0.01){
    struct timeval oldtime;
    gettimeofday(&oldtime, NULL);
    const double phi = th/10.1 + 1.57;
    const int nplotpoints =
      build4dlist(flatlist, list, pointn, th, phi, tim);
    draw(flatlist, nplotpoints, minx, maxx, miny, maxy);
    tim += (maxtime-mintime)/1000.;
    if(tim > maxtime*1.1) tim = mintime;
    struct timeval newtime;
    gettimeofday(&newtime, NULL);

    const int took = (newtime.tv_sec - oldtime.tv_sec)*1e6 +
                      newtime.tv_usec - oldtime.tv_usec;

    const int want = 1e6/60;
    if(want-took > 0) usleep(want-took);
  }

  return 0;
}
