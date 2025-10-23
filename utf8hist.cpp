/*
  utf8hist converts a whitespace-delimited text file of format:

  bincenter value error

  to UTF-8 graphics, or ASCII-only graphics is called as asciihist.

  This is very handy for making quick plots when you are already
  working at a terminal.  Typically it is combined with 'bin' like this:

  yourprogram | bin 50 0 100 | utf8hist
  
  In which yourprogram outputs a list of numbers, bin puts them in
  50 bins from 0 to 100, and utf8hist reads the bins and plots them.
  
  I wrote this one afternoon. I'm sure there are better things out
  there.  There are some obvious design problems with my approach.

  Compile with:

  g++ -Wextra -Wall utf8hist.cpp -o utf8hist

  I release this to the public domain or with the most permissive
  license possible.

  - Matthew Strait
*/

#include <vector>
using std::vector;
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// This is the *maximum* number of rows in the plot proper
// Things go better if this is odd.
static unsigned int H = 25;

static bool USE_UTF8 = false;

static double ext(const vector<double> & val,
                  const vector<double> & err, const double mult)
{
  if(val.size() != err.size()) exit(1);
  if(val.size() == 0) return 0;
  double extr = val[0];
  for(unsigned int i = 0; i < val.size(); i++){
    const double test = val[i] + mult*err[i];
    if(mult <= 0 && test < extr) extr = test;
    if(mult >  0 && test > extr) extr = test;
  }
  return extr;
}

static double max(const vector<double> & val,
                  const vector<double> & err)
{
  return ext(val, err, 1.5);
}

static double min(const vector<double> & val,
                  const vector<double> & err)
{
  return ext(val, err, -1.5);
}

static vector<int> getheights(const vector<double> & num,
                              const double top, const double bot,
                              const bool isval)
{
  vector<int> heights;

  const double mult = isval?3:2;
  const double submult = isval?1:0;

  for(unsigned int i = 0; i < num.size(); i++)
    heights.push_back(int(mult*H*(num[i]-submult*bot)/(top-bot) ));

  return heights;
}

static vector<int> getvalheights(const vector<double> & val,
                   const double top, const double bot)
{
  return getheights(val, top, bot, true);
}

static vector<int> geterrheights(const vector<double> & err,
                   const double top, const double bot)
{
  return getheights(err, top, bot, false);
}

static void printrow(int & ylabelwidth, const vector<int> & vals,
                     const vector<int> & errs, const double top,
                     const double bot, const int row)
{
  // Print in style f or e depending on how big the biggest number is.
  // This is similar to using style g, but I want it to use the same
  // style for the whole axis, which using g can't do.
  ylabelwidth = printf(
    fabs(top) < 1e4 && fabs(bot) < 1e4 && fabs(top) > 1?
    "%+8.2f ":"%+8.2e ", bot + (row+0.5)*(top-bot)/H);
  if(USE_UTF8) printf("%c%c%c", 0xe2, 0x94, 0x82);
  else printf("|");

  for(unsigned int col = 0; col < vals.size(); col++){
    const int majorval = vals[col]/3;
    const int minorval = vals[col]%3;
    if(majorval == row){
      if(errs[col] > 0){
        // half a unit of error
        if(errs[col] > 1 || !USE_UTF8){
          if(USE_UTF8) printf("%c%c%c", 0xe2, 0x94, 0xbc);
          else printf("+");
        }
        else{
          if(minorval == 0) printf("%c%c%c", 0xe2, 0x94, 0xbc);
          else if(minorval == 1) printf("+");
          else printf("%c%c%c", 0xe2, 0x94, 0xbc);
        }
      }
      else{
        if(minorval == 0) printf("_");
        else if(minorval == 1) printf("-");
        else{
          if(USE_UTF8) printf("%c%c%c", 0xE2, 0x80, 0xBE);
          else printf("~");
        }
      }
    }
    else if(majorval-((errs[col]-1)/2) <= row &&
            majorval+((errs[col]-1)/2) >= row){
      if(USE_UTF8) printf("%c%c%c", 0xe2, 0x94, 0x82);
      else printf("|");
    }
    else if(majorval-((errs[col])/2) == row ||
            (USE_UTF8 && minorval == 0 && errs[col] == 1 &&
             majorval - 1 == row)){
      if(USE_UTF8) printf("%c%c%c", 0xe2, 0x95, 0xb5); // ╵
      else printf("'");
    }
    else if(majorval+((errs[col])/2) == row ||
            (USE_UTF8 && minorval == 2 && errs[col] == 1 &&
             majorval + 1 == row)){
      if(USE_UTF8) printf("%c%c%c", 0xe2, 0x95, 0xb7); // ╷
      else printf(".");
    }
    else printf(" ");
  }
  printf("\n");
}

static void print(const vector<int> & vals, const vector<int> & errs,
                  const vector<double> & bincs, const double top,
                  const double bot)
{
  int ylabelwidth = 8; // should be overwritten
  for(int row = H-1; row >= 0; row--)
    printrow(ylabelwidth, vals, errs, top, bot, row);

  // print whitespace to left of origin
  for(int i = 0; i < ylabelwidth; i++) printf(" ");

  // print origin
  if(USE_UTF8) printf("%c%c%c", 0xe2, 0x94, 0x94);
  else printf("+");

  // Manually keep these two in sync
  const int xlabelwidth = 9;
  const char * const xformat =
    (fabs(bincs[0]) > 1e4  || fabs(bincs[bincs.size()-1]) > 1e4) ||
    (fabs(bincs[0]) < 1e-4 && fabs(bincs[bincs.size()-1]) < 1e-4)?
    "%+8.2e":"%+8.2f";

  // print the x-axis
  for(unsigned int row=0; row < bincs.size(); row += xlabelwidth+1){
    if(USE_UTF8) printf("%c%c%c", 0xe2, 0x94, 0xac);
    else printf("+");
    for(int i = 0; i < xlabelwidth; i++){
      if(USE_UTF8) printf("%c%c%c", 0xe2, 0x94, 0x80);
      else printf("-");
    }
  }
  printf("\n");

  bool truncatedxlabels = false;
  // print the x-axis labels
  for(int i = 0; i < ylabelwidth-xlabelwidth/2+1; i++) printf(" ");
  for(unsigned int row = 0; row < bincs.size(); row += xlabelwidth+1){
    char label[xlabelwidth+1];
    const int wrote=snprintf(label, xlabelwidth+1, xformat, bincs[row]);
    if(wrote >= xlabelwidth+1){
      truncatedxlabels = true;
      label[xlabelwidth] = 0;
    }

    // Center the label
    int spaces = 0;
    for(int i = 0; i < xlabelwidth; i++) if(label[i] == ' ') spaces++;

    for(int i = 0; i < (1+spaces)/2; i++) printf(" ");
    const int cprinted = (1+spaces)/2 + printf("%s", label+spaces);
    for(int i = 0; i < xlabelwidth+1 - cprinted; i++) printf(" ");
  }
  printf("\n");
  if(truncatedxlabels) puts("Truncated some x labels. Sorry.");
}

static double roundto(const double in, const double to)
{
  if(in < 0) return -roundto(-in, to);
  return double(int(in/to + 0.5))*to;
}

static bool alignto(const double interval, const double tolerance,
                    double & top, double & bot)
{
  const double botin = bot, topin = top;
  const double binspacing = (top - bot)/H;
  if(interval > binspacing &&
     interval < binspacing + interval*tolerance){
    const double center = roundto((top + bot)/2, interval);
    const double tenttop = center + interval*H/2.0;
    const double tentbot = center - interval*H/2.0;
    if(tenttop < top || tentbot > bot) goto fail;

    bot = tentbot;
    top = tentbot + H*interval;

    // If all values are positive, put the empty space at the top.
    // If all negative, put it at the bottom. Otherwise center the
    // points.
    if(botin > 0 && topin > 0){
      while(top > topin && bot - interval > 0){
        bot -= interval;
        top -= interval;
      }
    }
    else if(botin < 0 && topin < 0){
      while(bot < botin && top + interval < 0){
        bot += interval;
        top += interval;
      }
    }

    return true;
  }
  fail:
  top=topin; bot=botin; return false;
}

static void nicifytopbot(double & top, double & bot)
{
  // Try to snap to nice intervals
  for(int e = -37; e <= 37; e++){
    if(alignto(0.10  *pow(10, e), 0.2, top, bot)) return;
    if(alignto(0.125 *pow(10, e), 0.1, top, bot)) return;
    if(alignto(0.15  *pow(10, e), 0.1, top, bot)) return;
    if(alignto(0.20  *pow(10, e), 0.1, top, bot)) return;
    if(alignto(0.25  *pow(10, e), 0.1, top, bot)) return;
    if(alignto((1/3.)*pow(10, e), 0.1, top, bot)) return;
    if(alignto(0.50  *pow(10, e), 0.1, top, bot)) return;
  }

  // try really hard to snap to less nice intervals
  for(int e = -37; e <= 37; e++)
    for(int i = 3; i <= 9; i++)
      if(alignto(i*0.1*pow(10, e), 0.5, top, bot)) return;
}

int main(int argc, char ** argv)
{
  USE_UTF8 = !strcmp(basename(argv[0]), "utf8hist");
  FILE * input = fopen(argc >= 2? argv[1]: "/dev/stdin", "r");

  if(!input){
    input = fopen("/dev/stdin", "r");
    if(!input){ fprintf(stderr, "What?\n"); exit(1); }
    if(argc >= 2) H = atoi(argv[1]);
  }
  if(argc >= 3) H = atoi(argv[2]);

  vector<double> bincs, vals, errs;
  float binc, val, err;
  while(3 == fscanf(input, "%f %f %f", &binc, &val, &err)){
    bincs.push_back(binc);
    vals.push_back(val);
    errs.push_back(err);
  }

  if(vals.size() < 1) exit(1);

  double top = max(vals, errs), bot = min(vals, errs);

  nicifytopbot(top, bot);

  const vector<int> valheights = getvalheights(vals, top, bot);
  const vector<int> errheights = geterrheights(errs, top, bot);

  print(valheights, errheights, bincs, top, bot);
}
