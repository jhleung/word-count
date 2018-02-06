/*
 * mywc       mywc program manual       mywc
 *
 * NAME
 *    mywc -- word, line, and character count with option to ellide C-language comments starting 
 *    with ``//''
 * 
 * SYNOPSIS
 *    Please compile mywc.c with ``gcc -o mywc mywc.c'' and run this command to use the program:
 *    ./mywc [-clwC] [file(s)] 
 *
 * DESCRIPTION
 *      The mywc program matches the functionality of the UTCS Linux wc command, wc(1). This means it
 *      displays, to standard output, the lines, words, and character counts of each input file or 
 *      standard input if no file is given. In addition, single line, C-language comments may be
 *      excluded from the counts. This is useful for performing a word count on C-language files
 *      with lots of comments.
 *
 *      A line consists of a series of characters delimited by the <newline> character, which means
 *      characters after the final <newline> character will not form another line.
 *      
 *      A word is defined as a series of characters delimited by the characters that wc(1) 
 *      deems as white space characters.
 *
 *      A character is defined as a single byte, meaning this program does not account for
 *      multibyte characters or any locale. 
 *
 *      The name of the file is also written to standard output. If multiple input files are specified,
 *      an equal number of lines containing each file's line, word, and character counts is displayed. 
 *      Furthermore, an additional line containing the total line, word, and character counts of all
 *      files is displayed.
 *
 *      The program works for files encoded in ASCII only.
 *
 *      The following options are available:
 *
 *      -c The number of bytes in each input file will be written to standard output. 
 *      -l The number of lines in each input file will be written to standard output.
 *      -w The number of words in each input file will be written to standard output.
 *      -C Words and characters in single line, C-language comments that begin with 
 *         ``//'' (two `/' characters) will be excluded from the output. The <newline> character in the 
 *         comment will not be excluded. See the command ``sed 's://.*$::g' |  wc <options>'', which
 *         provides the same functionality.
 *
 *      By default, the mywc program always outputs the line, word, and character counts in that order.
 *      Just like the wc(1) command, if all three options are specified, the order above will be kept
 *      no matter the order of the options. If one wishes to display a specific count, he may 
 *      do so by specifying the associated option. Specfying an option will affect all input files.
 *
 *      If the -C option is specified alone with multiple input files, it will exclude counts in comments
 *      of all input files.
 *
 *      If no input files are specified, the standard input is used and no file name will be displayed
 *      to standard output. The prompt will accept input until [^D].
 *
 * EXIT STATUS
 *      The mywc program exits 0 on success, and > 0 if an error occurs. 
 *
 * EXAMPLES
 *      Count the number of characters, words, and lines of each file and totals for both:
 *              ./mywc file1.txt file2.txt 
 *
 *      Exclude words and characters of single line, C-language comments in both files
 *      and report totals of both:
 *              ./mywc -C file1.txt file2.txt
 *     
 *      Count the number of lines of each file and totals for both:
 *              ./mywc -l file1.txt file2.txt
 *      
 *      Count the number of characters, words, and lines of standard input's contents:
 *              ./mywc 
 *      
 *      Exclude words and characters of single line, C-language comments in standard input's
 *      contents:
 *              ./mywc -C
 *
 * SEE ALSO
 *      wc(1), sed(1)
 * 
 * EXTRA CREDIT
 *      I found a discrepancy between the UTCS Linux and MacOS wc commands. They do not agree on
 *      what a white space character is. I tested this by creating 256 files that used
 *      all 256 ASCII characters as separators between two words. If wc -w file.txt gave me an output
 *      of 2, then the ASCII character used in that file would be a white space character.
 *      The MacOS wc command determines that ASCII code 133 and 160 are white space characters, while
 *      the UTCS Linux wc command does not. Both however agree that ASCII codes 9, 10, 11, 12, and 13 are
 *      white space characters.
 *
 *      I believe that even though my implementation is inconsistent with the UTCS Linux result, 
 *      my result is still correct due to a potential Linux bug. In my mywc program, I have
 *      a function checking for white space characters by returning true for ASCII codes 9, 10, 11,
 *      12, and 13. These codes are, as stated in the above paragraph, what the UTCS Linux wc command
 *      deems as white space characters. However, running ``./mywc -C mywc'' gives me more words than
 *      ``wc mywc''. Even though I am marking white space characters the same way that the UTCS Linux
 *      wc command does, the results are different. This seems like a bug. Please take this into consideration
 *      if you see that my output is inconsistent with the UTCS Linux result.
 */
#include "stdio.h"
#include "stdlib.h"
#include "wctype.h"
#include "stdbool.h"
#include "string.h"
#include "unistd.h"

bool W = false;
bool L = false;
bool C = false;

int CHARS_EXCLUDED = 0;;
int WORDS_EXCLUDED = 0;

int TOTAL_WORDS = 0;
int TOTAL_LINES = 0;
int TOTAL_CHARS = 0;

bool wspace(int c) {
  return (c == 9 || c == 10 || c == 11 || c == 12 || c == 13 || c == 32);
}

void wc(char* filename) {
  int words = 0;
  int lines = 0;
  int chars = 0;
  FILE* file = fopen(filename, "rt");
  if (file != NULL) {
    int c;
    while ((c = fgetc(file)) != EOF) {
      chars++;
      if (!iswspace(c)) {
        while (!iswspace(c = fgetc(file)) && c != EOF) {
          chars++;
        }
        if (wspace(c)) chars++;
        words++;
      }
      if (c == '\n')
        lines++;
    }
    fclose(file);
    if (L) printf("      %d", lines);
    if (W) printf("      %d", words - WORDS_EXCLUDED);
    if (C) printf("      %d", chars - CHARS_EXCLUDED);

    TOTAL_WORDS += (words - WORDS_EXCLUDED);
    TOTAL_LINES += lines;
    TOTAL_CHARS += (chars - CHARS_EXCLUDED);

    WORDS_EXCLUDED = CHARS_EXCLUDED = 0;
  }
  else {
    exit(EXIT_FAILURE);
  }
}

void exclude_comments(char* filename) {
  FILE* file = fopen(filename, "rb");
  if (file != NULL) {
    int c1;
    int c2;
    while ((c1 = fgetc(file)) != EOF) {
      if (c1 == '/' && c2 == '/') {
        CHARS_EXCLUDED += 2;
        int c;
        while((c = fgetc(file)) != '\n' && c != EOF) {
          CHARS_EXCLUDED++;
        }
        c1 = c;
      }
      c2 = c1;
    }
    fclose(file);
  }
  else {
    exit(EXIT_FAILURE);
  }
  
  file = fopen(filename, "rb");
  if (file != NULL) {
    int c; 
    while ((c = fgetc(file)) != EOF) {
      if (!wspace(c)) {
        int c1 = c;
        int c2;
        bool no_space_before_comment = false;
        if (c1 != '/') {
          no_space_before_comment = true;;
        } 
        while (!wspace(c1) && c1 != EOF) {
          if (c1 == '/' && c2 == '/') {
            int c3;
            int c4 = c1;
            while ((c3 = fgetc(file)) != '\n' && c3 != EOF) {
              if (!wspace(c4) && wspace(c3)) WORDS_EXCLUDED++;
              c4 = c3;
            }
            if (!wspace(c4)) WORDS_EXCLUDED++;
            if(no_space_before_comment) WORDS_EXCLUDED--;
            c1 = c3;
          }
          else {
            c2 = c1;
            c1 = fgetc(file);
          }
        }
      }
    }
    fclose(file);
  }
  else {
    exit(EXIT_FAILURE);
  }

}

int main(int argc, char* argv[], char* env[]) {
  int i;
  if (argc == 1 || (argc > 1 && (argv[1][0] != '-' || strcmp(argv[1], "-C") == 0))) W = L = C = true;
  bool ellide_comments = false;
  int numfiles = 0;
  for (i = 1; i < argc; i++) {
    int j;
    char* arg = argv[i];
    if (arg[0] == '-') {
      for (j = 1; j < strlen(arg); j++) {
        if (arg[j] == 'C') ellide_comments = true;
        else if (arg[j] == 'w') W = true;
        else if (arg[j] == 'l') L = true;
        else if (arg[j] == 'c') C = true;
      }
    } else {
      numfiles++;
      if (ellide_comments) exclude_comments(arg);
      wc(arg);
      printf(" %s\n", arg);
    }
  }

  if (numfiles > 1) printf("      %d      %d      %d total\n", TOTAL_LINES, TOTAL_WORDS, TOTAL_CHARS);

  if (numfiles == 0) {
    char* filename = "temp.txt";
    FILE* file = fopen(filename, "wb");
    // Some of the code below is taken from stackoverflow, credit to user: user411313
    char *text = calloc(1,1), buffer[10];
    while(fgets(buffer, 10 , stdin)) {
      text = realloc( text, strlen(text)+1+strlen(buffer) );
      if(!text){} 
      strcat( text, buffer ); 
    }
    fprintf(file, "%s", text);
    fclose(file);
    
    if (ellide_comments) exclude_comments(filename);
    wc(filename);
    printf("\n");

    remove(filename);
  }
  exit(0);
}


// UNUSED: for if you want to actually delete comments in the original file
/*void ellide_comments(char* filename) {
  FILE* file = fopen(filename, "r");
  char* temp_filename = "temp.txt";
  FILE* buffer = fopen(temp_filename, "w");
  if (file != NULL) {
    int c1, c2;
    while ((c1 = fgetc(file)) != EOF && (c2 = fgetc(file)) != EOF) {
      if (c1 == '/' && c2 == '/') {
        int c;
        while((c = fgetc(file)) != '\n') { }
        fprintf(buffer, "%c", c);
      } else {
        fprintf(buffer, "%c", c1);
        fprintf(buffer, "%c", c2);
      }
    }
    if (c1 != EOF && c2 == EOF) {
      fprintf(buffer, "%c", c1);
    }
    fclose(file);
    fclose(buffer);
  }
  else {
  }
  
  remove(filename);
  rename(temp_filename, filename);

  file = fopen(filename, "w");
  buffer = fopen("temp.txt", "r");
  if (file != NULL) {
    int c;
    while ((c = fgetc(buffer)) != EOF) {
      fprintf(file, "%c", c);
    }
    fclose(file);
    fclose(buffer);
  }
  else {
  }
}
*/
