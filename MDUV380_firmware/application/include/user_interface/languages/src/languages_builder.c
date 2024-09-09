/* -*- mode: c; c-file-style: "k&r"; compile-command: "gcc -Wall -O2 -I../ -o languages_builder languages_builder.c"; -*- */

/*
 * Copyright (C) 2023-2024 Daniel Caujolle-Bert, F1RMB
 *
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. Use of this source code or binary releases for commercial purposes is strictly forbidden. This includes, without limitation,
 *    incorporation in a commercial product or incorporation into a product or project which allows commercial use.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>

#include "uiLanguage.h"

// These two aren't exported as gla file
#include "english.h"
#include "japanese.h"

#include "french.h"
#include "german.h"
#include "portuguese.h"
#include "catalan.h"
#include "spanish.h"
#include "italian.h"
#include "danish.h"
#include "finnish.h"
#include "polish.h"
#include "turkish.h"
#include "czech.h"
#include "dutch.h"
#include "slovenian.h"
#include "portugues_brazil.h"
#include "swedish.h"
#include "hungarian.h"
#include "croatian.h"
#include "romanian.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_REV   1

#if defined(_WIN32)
#define OPEN_RO_FLAGS O_RDONLY|O_RAW
#define OPEN_RW_FLAGS O_CREAT|O_WRONLY|O_TRUNC|O_RAW
#else
#define OPEN_RO_FLAGS O_RDONLY
#define OPEN_RW_FLAGS O_CREAT|O_WRONLY|O_TRUNC
#endif


static const char short_options[] = "?hCc";
static const struct option long_options[] = {
     { "help"             , no_argument      , 0, 'h' },
     { "check-languages"  , no_argument      , 0, 'C' },
     { "create-languages" , no_argument      , 0, 'c' },
     { 0                  , no_argument      , 0,  0  }
};

static bool languagesInError = false;

const stringsTable_t languages[]=
{
     catalanLanguage,
     danishLanguage,
     frenchLanguage,
     germanLanguage,
     italianLanguage,
     portuguesLanguage,
     spanishLanguage,
     finnishLanguage,
     polishLanguage,
     turkishLanguage,
     czechLanguage,
     dutchLanguage,
     slovenianLanguage,
     portuguesBrazilLanguage,
     swedishLanguage,
     hungarianLanguage,
     croatianLanguage,
     romanianLanguage,
};

const char *languageEnglishNames[] =
{
     "Catalan",
     "Danish",
     "French",
     "German",
     "Italian",
     "Portuguese",
     "Spanish",
     "Finnish",
     "Polish",
     "Turkish",
     "Czech",
     "Dutch",
     "Slovenian",
     "PortuguesBrazil",
     "Swedish",
     "Hungarian",
     "Croatian",
     "Romanian"
};


static bool CreateLanguageFile(const stringsTable_t *language, const char *filename)
{
     int fd = -1;

     fprintf(stdout, " - Creating file %s: ", filename);
     if ((fd = open(filename, OPEN_RW_FLAGS, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IWGRP)) == -1)
     {
	  perror("open");
	  return false;
     }

     if (write(fd, language, sizeof(stringsTable_t)) != sizeof(stringsTable_t))
     {
	  perror("write");
	  return false;
     }

     if ((fd != -1) && (close(fd) == -1))
     {
	  perror("close");
	  return false;
     }

     fprintf(stdout, "Done\n");
     return true;
}

static void checkLanguage(const stringsTable_t *l, const char *name)
{
     size_t len = sizeof(stringsTable_t) - (sizeof(*l->magicNumber));
     char buffer[LANGUAGE_TEXTS_LENGTH + 1];
     char *p = (char *)l->LANGUAGE_NAME;
     bool hasError = false;

     fprintf(stdout, "Checking '%s' language...\n", name);

     for (size_t i = 0; i < (len / LANGUAGE_TEXTS_LENGTH); i++)
     {
	  memset(buffer, 0, sizeof(buffer));
	  memcpy(buffer, p + (i * LANGUAGE_TEXTS_LENGTH), LANGUAGE_TEXTS_LENGTH);

	  if (strlen(buffer) > (LANGUAGE_TEXTS_LENGTH - 1))
	  {
	       //fprintf(stdout, "                                                   1      \\0\n");
	       //fprintf(stdout, "                                          1234567890123456\n");
	       fprintf(stdout, "  > LENGTH ERROR in member #%3" PRIu64 " / %3" PRIu64 ":  ", i + 1, (len / LANGUAGE_TEXTS_LENGTH));
	       fprintf(stdout, " '%-*s' len: >= %" PRIu64 " (max: %u)\n", LANGUAGE_TEXTS_LENGTH, buffer, strlen(buffer), (LANGUAGE_TEXTS_LENGTH - 1));

	       hasError = true;
	  }
     }

     if (hasError)
     {
	  fprintf(stderr, "\a  !!!\n");
	  fprintf(stderr, "  !!! Language '%s' has error(s). Please fix this.\n", name);
	  fprintf(stderr, "  !!!\n");

	  languagesInError = true;
     }
}

static void checkLanguages(void)
{
     // English
     checkLanguage(&englishLanguage, "English");

     // Japanese
     checkLanguage(&japaneseLanguage, "Japanese");

     // Other languages
     for (size_t i = 0; i < (sizeof(languages) / sizeof(stringsTable_t)); i++)
     {
	  checkLanguage(&languages[i], languageEnglishNames[i]);
     }
}

static void CreateAllLanguageFiles(void)
{
     checkLanguages();

     if (languagesInError)
     {
	  fprintf(stdout, "\n\n ************************************************************************\n");
	  fprintf(stdout, " **** Error(s) found in language file{s), won't build the gla files. ****\n");
	  fprintf(stdout, " ************************************************************************\n\n");
	  return;
     }
     
     for (size_t i = 0; i < (sizeof(languages) / sizeof(stringsTable_t)); i++)
     {
	  char filename[1024];

	  snprintf(filename, sizeof(filename), "%s.gla", languageEnglishNames[i]);

	  if (CreateLanguageFile(&languages[i], filename) == false)
	  {
	       abort();
	  }
     }
}

static void displayHelp(void)
{
     fprintf(stdout, "\n");
     fprintf(stdout, "      --create-languages, -c                    : Create language plugin files (.gla).\n");
     fprintf(stdout, "      --check-languages, -C                     : Check languages files (C header files).\n");
     fprintf(stdout, "\n");
     fprintf(stdout, "** Please note: no argument is equal to --create-languages option. **\n");
     fprintf(stdout, "\n");
}

int main(int argc, char **argv)
{
     int  c = '?';
     int  option_index = 0;

     fprintf(stdout, "languages_builder v%u.%u.%u (c) 2023 Daniel Caujolle-Bert, F1RMB.\n", VERSION_MAJOR, VERSION_MINOR, VERSION_REV);

     if (argc > 1)
     {
          opterr = 0;
          while((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != EOF)
          {
               switch (c)
               {
	       case 'c':
		    goto createLanguages;
		    break;

	       case 'C':
		    checkLanguages();
		    break;

	       case 'h':
               default:
                    displayHelp();
                    break;
	       }
	  }
     }
     else
     {
     createLanguages:
	  CreateAllLanguageFiles();
     }


     return 0;
}
