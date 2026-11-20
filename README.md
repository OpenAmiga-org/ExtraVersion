# ExtraVersion

ExtraVersion: a `$EXTVER:` convention for storing a program's original
(non-Amiga) version string, and an AmigaOS 4 Shell command that reads it.
By Marko K. Seppänen. ExtraVersion is an
[OpenAmiga.org project](http://openamiga.org/?function=gitproject&name=ExtraVersion).

Ported software often has a version string that doesn't fit the Amiga
version string format. ExtraVersion adds a second string next to the normal
`$VER:` string (never instead of it) for the original version. The command
reads it the way Version reads `$VER:`.

Download: [extraversion.lha on OS4Depot](https://os4depot.net/?function=showfile&file=utility/shell/extraversion.lha)
(the command, its documentation, an Install script and icons).

## Layout

- `src/extraversion.c`: the command. The same file builds for Linux, for
  the tests.
- `docs/`: `ExtraVersion.doc` (the AutoDoc, installed to
  `SYS:Documentation/C`), the old developer notes and to-do list
  (`DevDoc.txt`) and `OS4Depot.txt`.
- `tests/`: `good/` and `bad/` hold 70 files with correct and
  broken extra version strings (scripts, text files, executables);
  `expected.txt` is the output and return code of every test command;
  `run_tests.py` runs them on Linux or in the OS4 guest.
- `package/`: the release readme (`ExtraVersion.readme`), the Install
  script and the icons (`icons/`: the `.info` files and their pictures).
- `tools/`: `make_lha.sh` (release archive), `make_icons.py` and
  `os4/mkicon.c` (icons), `update_readme.py` (the changelog below, from
  the readme), `os4depot_submit.py` (OS4Depot upload).

## Using it

Put the string in the program or script:

    "\0$EXTVER: Program_Name ExtraVersionString (dd.mm.yyyy) Comment\0"

- `Program_Name`: name of your program, one word (use `_` for spaces)
- `ExtraVersionString`: any non-Amiga version string (use `_` for spaces)
- `dd.mm.yyyy`: day, month and four-digit year
- comment: optional, just for readability

Then read it in a Shell:

    EXTRAVERSION [<library|script|file>] [FULL] [VSTRING] [VER=VERSTAG] [VERIFY] [HELP]
    Template: NAME,FULL/S,HELP/S,VSTRING/S,VER=VERSTAG/S,VERIFY/S

- no option: the name and the extra version
- `FULL`: also the date, and the comment on a second line
- `VSTRING`: the whole string, including `$EXTVER:`; an empty line when
  the file has none
- `VER`: the `$VER:` string instead of `$EXTVER:`
- `VERIFY`: the string as found and each of its parts

No file: ExtraVersion prints its own. No string found: "Could not find
extra version information", return code 20. The full documentation is
`docs/ExtraVersion.doc`.

## Building

    make                # build/os4/ExtraVersion, cross-built in the walkero
                        # docker image walkero/amigagccondocker:os4-gcc15-3.6.1
    make test           # Linux build (ASan, UBSan) + tests/run_tests.py
    make test-os4       # the AmigaOS 4 build, same tests in the OS4 guest
    make lha            # dist/ExtraVersion.lha (tools/make_lha.sh)
    tools/os4depot_submit.py   # what would go to OS4Depot; --send sends it
                               # (Replace file form, email from os4depot.conf)

On AmigaOS 4 with the SDK: `make DOCKER= CC=gcc STRIP=strip`.
`make test-os4` needs the amiga-fleet MCP server's Python environment
(`~/build/amiga-fleet-venv`) and MCPd in the guest. `make lha` checks that
the binary's `$VER` matches `APP_VERSION`/`APP_DATE` and that the readme's
History starts with this version, strips the binary, fills in the
version in the doc and readme, packs the archive and updates the
changelog here.

## Design notes

- Search: the file is read in 64 KB blocks into a static buffer (AmigaOS
  command stacks are small; reading byte by byte through DOS was 40 times
  slower) and its bytes are matched against `$EXTVER:` (or `$VER:` with
  `VER`), upper case only, as Version does. Ctrl-C is checked between
  blocks. The tag runs to a NUL, a
  line end (LF or CR) or the end of the file; tags longer than 511
  characters are not version strings.
- Parse: name = first word, version = the rest up to the `(` of the date
  (blanks trimmed), date = up to `)` (at most 32 characters), comment = the
  rest. A tag without name, version or date is skipped and the search goes
  on, so the bare search key that every program like this one holds is
  never taken for a version.
- ExtraVersion's own binary holds no other `$EXTVER:`/`$VER:` string: the
  search keys are built at run time and the help text prints the `$` as an
  argument.
- Return codes follow Version: 0 found, 20 not found or the file can't be
  opened (AmigaDOS error text), 10 for wrong arguments, 5 after Ctrl-C.
  `VSTRING` alone prints an empty line and returns 0 when nothing is found.
- The 1.1 and 1.2 beta source code was never committed to Subversion. 1.3
  was written from the 1.0 beta source, the 1.2.0b readme and its
  behaviour in the guest (the same output on every working test file).

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

## Changelog

<!-- changelog start: made by tools/update_readme.py from package/ExtraVersion.readme -->
- **1.3** (2026-11-20):
  - First release from the OpenAmiga.org GitHub repository. The version string is now a real Amiga version, 1.3 (1.2.0b read as 1.2).
  - `VER=VERSTAG` and `VERIFY` are back: the 1.1 and 1.2 source code was never committed, so they were written again from the 1.2.0b readme and checked against the 1.2.0b command.
  - Programs that contain the bare search string `$EXTVER:`, such as ExtraVersion itself, are read right: a `$EXTVER:` without name, version and date is skipped. 1.2.0b printed ` Extversion` or parts of other strings for them.
  - `FULL` and `VSTRING` work with the file name in any position (`ExtraVersion FULL file` looked for a file called FULL).
  - No extra version string: prints "Could not find extra version information" and returns 20, like Version (1.2.0b printed nothing, or `()` with `FULL`). `VSTRING` still prints an empty line.
  - A missing file or a drawer gives the AmigaDOS error and returns 20.
  - Without a file name it prints its own extra version.
  - The whole version up to the date is kept (`file 001`, was `file`).
  - Ctrl-C stops the search in big files.
  - Install script, icons and new documentation.
- **1.2.0 beta** (2018-01-02):
  - New switch `VERIFY` that displays the parsed-out information.
  - Test code left out of the release executable, for a smaller file.
  - Leading and trailing white space trimmed.
  - 1.1.0b (December 2017): new switch `VER` (`VERSTAG`), finds the `$VER:` string instead. Fixed one trailing space in the returned string when there was no comment. Corrected the name to "ExtraVersion" in various places, thanks TSK.
  - 1.0.1b (December 2017): an `$EXTVER:` string at the very end of a file without a comment is found. Non-standard versions such as `0.3-Alpha_3` are read.
- **1.0 beta** (2016-02-26):
  - Most of the command rewritten from scratch.
  - Test cases and more than 40 test files for good and bad strings.
  - Works on both text files and executables.
  - Compiled with the newly published SDK.
- **0.4 beta** (2014-05-29):
  - Options `HELP`, `FULL` and `VSTRING`.
  - Fixed extra trailing characters read into the string.
  - Fixed strings in scripts and other text files not being found.
  - Many other fixes and improvements.
- **0.3** (2014-05-12): Alpha 3. Updated `ExtraVersion.doc`.
<!-- changelog end -->

## History

The project was created on OpenAmiga.org in 2008. Marko K. Seppänen wrote
ExtraVersion in 2014 (0.4, beta 1) and rewrote most of it for 1.0 beta in
2016, in the OpenAmiga.org Subversion repository; 1.1 and 1.2 beta
followed on OS4Depot in 2017 and 2018. Moved to GitHub in November 2026,
with its full commit history; 1.3 is the first release from there.
