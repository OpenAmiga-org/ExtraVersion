# ExtraVersion

ExtraVersion: a `$EXTVER:` convention for storing a program's original
(non-Amiga) version string, and an AmigaOS 4 command that reads it. By Marko
K. Seppänen.

Ported software often has a version string that doesn't fit the Amiga
version string format. ExtraVersion adds a second string next to the normal
`$VER:` string (never instead of it) for the original version.

Version 1.0 beta (2016-02-26).

## The extra version string

From the readme: put this string of characters in the file:

    "\0$EXTVER: Program_Name ExtraVersionString (dd.mm.yyyy) Some comment.\0"

- `Program_Name`: name of your program
- `ExtraVersionString`: any non-Amiga version string (use `_` for spaces)
- `dd.mm.yyyy`: day, month and four-digit year
- comment: optional, just for readability

## The command

    EXTRAVERSION [<library|script|file>] [FULL|VSTRING|HELP]
    Template: NAME, FULL/S, HELP/S, VSTRING/S

- no option: prints the name and extra version string
- `FULL`: name, extra version, date and comment on one line
- `VSTRING`: the complete extra version string, including `$EXTVER:`

It works on both text files and executables. The full documentation is
`docs/ExtraVersion.doc`.

## Building

`src/Makefile` builds with GCC (newlib) on AmigaOS 4: `make` builds
`ExtraVersion`, `make test` a test build, `make release` and `make lha` the
release in `release/`. `src/tests/good` and `src/tests/bad` hold more than
60 test files with correct and broken extra version strings.

## Contents

- `src/`: `extraversion.c`, the makefile, a built `ExtraVersion` and the test
  files.
- `docs/`: the readme, the AutoDoc-style `ExtraVersion.doc`, the developer
  notes and to-do list (`DevDoc.txt`) and the OS4Depot upload text.
- `release/`: the 1.0 beta release, also as `ExtraVersion.lha`.

## Original project description

The OpenAmiga.org project page described the goal like this:

> Since a lot of software are being ported from other platforms that use
> version strings that are not compatible with Amiga version strings, a need
> to be able to store both an amiga version string and the version string of
> the original software has arisen.
>
> This project is about inventing a method for storing such an extra version
> string inside software binaries, as well as about creating a tool to
> extract the data.
>
> Once completed the solution will be presented to Hyperion as a proposal for
> the official AmigaOS way of storing this type of information.
>
> Note that the new version string is Not a replacement for the Amiga version
> string. It is only a complement and an amiga version string must always be
> included in the software independent of the original software version.

## History

The project was created on OpenAmiga.org in 2008. Marko K. Seppänen wrote
ExtraVersion in 2014 (0.4, beta 1) and rewrote most of it for 1.0 beta in
2016, in the OpenAmiga.org Subversion repository. Moved to GitHub in
November 2026, with its full commit history.
