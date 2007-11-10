/* This file is part of GEGL editor -- a gtk frontend for GEGL
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Copyright (C) 2003, 2004, 2006 Øyvind Kolås
 */

#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "gegl-options.h"

static GeglOptions *opts_new (void)
{
  GeglOptions *o = g_malloc0 (sizeof (GeglOptions));

#ifdef HAVE_GTK
  o->mode     = GEGL_RUN_MODE_EDITOR;
#else
  o->mode     = GEGL_RUN_MODE_HELP;
#endif
  o->xml      = NULL;
  o->output   = NULL;
  o->files    = NULL;
  o->file     = NULL;
  o->delay    = 0.0;
  o->rest     = NULL;
  return o;
}

static void
usage (char *application_name)
{
    fprintf (stderr,
"usage: %s [options] <file | -- [op [op] ..]>\n"
"\n"
"  Options:\n"
"     --help      this help information\n"
"     -h\n"
"\n"
"     --file      read xml from named file\n"
"     -i\n"
"\n"
"     --xml       use xml provided in next argument\n"
"     -x\n"
"\n"
"     --dot       output a graphviz graph description\n"
""
"     --output    output generated image to named file\n"
"     -o          (file is saved in PNG format)\n"
"\n"
"     -p          (increment frame counters of various elements when\n"
"                  processing is done.)\n"
"\n"
"     -X          output the XML that was read in\n"
"\n"
"     --verbose   print diagnostics while running\n"
"      -v\n"
"\n"
#if 0
"     --delay     wait for specified number of seconds before exit\n"
"                 or number of seconds to wait before iterating the image list\n"
"     -d\n"
"\n"
#endif
"All parameters following -- are considered ops to be chained together\n"
"into a small composition instead of using an xml file, this allows for\n"
"easy testing of filters. Be aware that the default value will be used\n"
"for all properties.\n"
, application_name);
    exit (0);
}

#define match(string) (!strcmp (*curr, (string)))
#define assert_argument() do {\
    if (!curr[1] || curr[1][0]=='-') {\
        fprintf (stderr, "ERROR: '%s' option expected argument\n", *curr);\
        exit(-1);\
    }\
}while(0)

#define get_float(var) do{\
    assert_argument();\
    curr++;\
    (var)=atof(*curr);\
}while(0)

#define get_int(var) do{\
    assert_argument();\
    curr++;\
    (var)=atoi(*curr);\
}while(0)

#define get_string(var) do{\
    assert_argument();\
    curr++;\
    (var)=*curr;\
}while(0)

#define get_string_forced(var) do{\
    curr++;\
    (var)=*curr;\
}while(0)

static GeglOptions *
parse_args (gint    argc,
            gchar **argv);

static void
print_opts (GeglOptions *o)
{
    fprintf (stderr,
"Parsed commandline:\n"
"\tmode:   %i\n"
"\tfile:   %s\n"
"\txml:    %s\n"
"\toutput: %s\n"
"\trest:   %s\n"
"\tdelay:  %f\n"
"\t\n",
    o->mode,
    o->file,
    o->xml,
    o->output,
    o->rest==NULL?"":"yes",
    o->delay
);
    {
      GList *files = o->files;
      while (files)
        {
          fprintf (stderr, "\t%s\n", (gchar*)files->data);
          files = g_list_next (files);
        }
    }
}

GeglOptions *
gegl_options_parse (gint    argc,
                    gchar **argv)
{
    GeglOptions *o;

    o = parse_args (argc, argv);
    if (o->verbose)
        print_opts (o);
    return o;
}

gboolean
gegl_options_next_file (GeglOptions *o)
{
  GList *current = g_list_find (o->files, o->file);
  current = g_list_next (current);
  if (current)
    {
      g_warning ("%s", o->file);
      o->file = current->data;
      g_warning ("%s", o->file);
      return TRUE;
    }
  return FALSE;
}

gboolean
gegl_options_previous_file (GeglOptions *o)
{
  GList *current = g_list_find (o->files, o->file);
  current = g_list_previous (current);
  if (current)
    {
      o->file = current->data;
      return TRUE;
    }
  return FALSE;
}


static GeglOptions *
parse_args (int    argc,
            char **argv)
{
    GeglOptions *o;
    char **curr;

#ifndef HAVE_GTK
    if (argc==1) {
        usage (argv[0]);
    }
#endif

    o = opts_new ();
    curr = argv+1;

    while (*curr && !o->rest) {
        if (match ("-h")    ||
            match ("--help")) {
            o->mode = GEGL_RUN_MODE_HELP;
            usage (argv[0]);
        }

        else if (match ("--delay") ||
                 match ("-d")) {
            get_float (o->delay);
        }

        else if (match ("--verbose") ||
                 match ("-v")) {
            o->verbose=1;
        }

        else if (match ("-p")){
            o->play=TRUE;
        }

        else if (match ("--file") ||
                 match ("-i")) {
            o->files = g_list_append (o->files, g_strdup (*curr));
        }

        else if (match ("--xml") ||
                 match ("-x")) {
            get_string (o->xml);
        }

        else if (match ("--output") ||
                 match ("-o")) {
            get_string_forced (o->output);
            o->mode = GEGL_RUN_MODE_PNG;
        }

        else if (match ("-X")) {
            o->mode = GEGL_RUN_MODE_XML;
        }

        else if (match ("--dot")) {
            o->mode = GEGL_RUN_MODE_DOT;
        }

        else if (match ("--")) {
            o->rest = curr+1;
            break;
        }

        else if (*curr[0]=='-') {
            fprintf (stderr, "\n\nunknown paramter '%s' giving you help instead\n\n\n", *curr);
            usage (argv[0]);
        }

        else
          {
            o->files = g_list_append (o->files, g_strdup (*curr));
          }
        curr++;
    }

    if (o->files)
      o->file = o->files->data;
    return o;
}
#undef match
#undef assert_argument
