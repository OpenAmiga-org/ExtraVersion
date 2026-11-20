#!/usr/bin/env python3
"""Submit a new ExtraVersion version to OS4Depot, replacing the published
utility/shell/extraversion.lha. Submissions wait in the maintainer's queue
until approved. (From arcedit's tool, public domain.)

The file id changes with every replace, so it is never stored here: the
script opens the file's readme page, follows its "Replace file" link and
sends to that form (its action and its f_replaces value).

  tools/os4depot_submit.py           show what would be sent (nothing is sent)
  tools/os4depot_submit.py --send    send it (multipart POST, like the web form)

The uploader's email comes from OS4DEPOT_EMAIL or email= in
~/openamiga-rules/os4depot.conf and is sent with "Hide from view" ticked.
Run `make lha` first: it writes dist/ExtraVersion.lha and
build/stage/ExtraVersion.readme.
"""
import html
import os
import re
import subprocess
import sys

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
ARCHIVE = os.path.join(ROOT, "dist", "ExtraVersion.lha")
README = os.path.join(ROOT, "build", "stage", "ExtraVersion.readme")
SITE = "https://os4depot.net/"
PUBLISHED = "utility/shell/extraversion.lha"
AGENT = "Mozilla/5.0 (compatible; os4depot-submit)"


def fetch(url):
    return subprocess.run(["curl", "-sS", "--max-time", "60", "-A", AGENT, url],
                          capture_output=True, check=True).stdout.decode("latin-1")


def upload_email():
    """Never in the readme, the archive or git: OS4DEPOT_EMAIL, else email=
    in ~/openamiga-rules/os4depot.conf."""
    if os.environ.get("OS4DEPOT_EMAIL"):
        return os.environ["OS4DEPOT_EMAIL"]
    conf = os.path.expanduser("~/openamiga-rules/os4depot.conf")
    if os.path.exists(conf):
        for line in open(conf):
            if line.startswith("email="):
                return line.split("=", 1)[1].strip()
    return ""


# readme page -> "Replace file" link -> the replace form
page = fetch(SITE + "?function=showfile&file=" + PUBLISHED)
m = re.search(r"<A HREF='([^']*replace=\d+[^']*)'><IMG ALT='Replace file'", page, re.I)
if m is None:
    sys.exit("no 'Replace file' link on the readme page of " + PUBLISHED)
form = fetch(SITE + html.unescape(m.group(1)).lstrip("/"))
action = re.search(r"<FORM[^>]*enctype='multipart/form-data'[^>]*ACTION='([^']*)'", form, re.I)
replaces = re.search(r"NAME='f_replaces' VALUE='([^']*)'", form, re.I)
if action is None or replaces is None or replaces.group(1) != PUBLISHED:
    sys.exit("the replace form is not the one for " + PUBLISHED +
             f" (f_replaces {replaces.group(1) if replaces else None!r})")
URL = SITE + html.unescape(action.group(1)).lstrip("/")

readme = open(README, encoding="latin-1").read()
head, body = readme.split("hend:\n", 1)
body = body.strip() + "\n"
meta = dict(l.split(":", 1) for l in head.splitlines() if ":" in l)
meta = {k.strip(): v.strip() for k, v in meta.items()}
if "@" in "".join(meta.values()):
    sys.exit("the readme has an unfilled placeholder or an address: use build/stage (make lha)")

# no licence is defined (readme rules): the form has no empty choice, and
# "Other" is its default and what the entry said before
license_ = meta["license"] or "Other"

email = upload_email()
fields = [
    ("f_name", meta["name"]),
    ("f_description", meta["description"]),
    ("f_version", meta["version"]),
    ("f_author", meta["author"]),
    ("f_submitter", meta["submitter"]),
    ("f_email", email),
    ("f_hideemail", "on"),
    ("f_url", meta.get("url", "")),
    ("f_donateurl", ""),
    ("f_category", meta["category"]),
    ("f_replaces", replaces.group(1)),
    ("f_requirements", meta.get("requirements", "")),
    ("f_license", license_),
    ("f_distributesubm", "selected"),
    ("f_distribute", "on"),
    ("f_passphrase", ""),
    ("f_minosversion", meta["minosversion"]),
    ("f_texttype", meta["texttype"]),
    ("f_text", body),
    ("f_comments", ""),
    ("f_submit_go", "Submit"),
]

print(f"POST {URL}")
for name, value in fields:
    if name == "f_text":
        shown = f"<readme body, {len(value)} chars>"
    elif name == "f_email":
        shown = "<set, hidden>" if value else "<missing>"
    else:
        shown = value
    print(f"  {name:17} = {shown!r}")
print(f"  {'f_userfile':17} = @{ARCHIVE} ({os.path.getsize(ARCHIVE)} bytes)")
assert len(meta["description"]) <= 50 and len(meta["name"]) <= 20

if "--send" not in sys.argv:
    print("\n(dry run: nothing sent)")
    sys.exit(0)
if not email:
    sys.exit("not sent: no email (OS4DEPOT_EMAIL or ~/openamiga-rules/os4depot.conf)")

cmd = ["curl", "-sS", "-o", "/dev/stdout", "-w", "\nHTTP %{http_code}\n", "--max-time", "600",
       "-A", AGENT]
for name, value in fields:
    # OS4Depot unescapes backslashes in the posted values ("\0" in the
    # readme became a NUL byte on 2026-11-20), so each one is sent doubled
    cmd += ["--form-string", f"{name}={value.replace(chr(92), chr(92) * 2)}"]
cmd += ["-F", f"f_userfile=@{ARCHIVE};type=application/x-lha;filename=ExtraVersion.lha", URL]
# the form is ISO-8859-15: send the values as Latin-1 bytes ("Seppänen"), not UTF-8
cmd = [c.encode("latin-1") for c in cmd]
out = subprocess.run(cmd, capture_output=True, text=True, encoding="latin-1")
open(os.path.join(ROOT, "build", "os4depot_response.html"), "w",
     encoding="latin-1").write(out.stdout)
print(out.stdout[-300:], out.stderr)
