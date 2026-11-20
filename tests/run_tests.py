#!/usr/bin/env python3
"""Runs ExtraVersion on every test file and compares with tests/expected.txt.

    python3 -I tests/run_tests.py            Linux build (make host)
    ~/build/amiga-fleet-venv/bin/python tests/run_tests.py --guest
                                             AmigaOS 4 build in the OS4 guest
    ... --update                             write tests/expected.txt from this run

Each case runs `ExtraVersion <args>` in a drawer that holds the binary
(named ExtraVersion) and the tests/ folder, so the commands, their output
and their return codes are the same on both systems. Cases marked
guest-only print AmigaDOS error texts and run only in the guest. --update
keeps the expected entries of the cases this run skips.
"""
import os
import re
import shutil
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TESTS = os.path.join(ROOT, "tests")
EXPECTED = os.path.join(TESTS, "expected.txt")
HOST_BIN = os.path.join(ROOT, "build", "host", "ExtraVersion")
OS4_BIN = os.path.join(ROOT, "build", "os4", "ExtraVersion")
TARGET = "os4-amigaone"
G = "RAM:EVTest"                  # guest work drawer
LHA = os.path.expanduser(os.environ.get("LHA", "~/linux_archivers/lha/lha"))

MODES = ["", "FULL", "VSTRING", "VERIFY", "VER VERIFY"]


def natural(name):
    return [int(t) if t.isdigit() else t for t in re.split(r"(\d+)", name)]


def cases():
    """(args, guest_only) in a fixed order."""
    out = []
    for sub in ("bad", "good"):
        for f in sorted(os.listdir(os.path.join(TESTS, sub)), key=natural):
            if f.startswith("REMARK"):
                continue
            for m in MODES:
                out.append((f"tests/{sub}/{f} {m}".strip(), False))
    out += [
        ("", False),                         # its own extra version
        ("FULL", False),
        ("VER", False),
        ("VER VSTRING", False),
        ("ExtraVersion", False),             # reads its own binary
        ("ExtraVersion VER FULL", False),
        ("tests/good/testfile.good.1.1 FULL VSTRING", False),
        ("tests/good/testfile.good.1.1 a", False),   # too many arguments
        ("nosuchfile", True),
        ("tests", True),                     # a drawer
    ]
    return out


def parse_expected(text):
    """{args: (rc, output)} from the '> ExtraVersion args  [rc=N]' blocks."""
    res = {}
    for block in re.split(r"^> ", text, flags=re.M)[1:]:
        head, _, body = block.partition("\n")
        m = re.match(r"ExtraVersion ?(.*?)  \[rc=(-?\d+)\]$", head)
        res[m.group(1)] = (int(m.group(2)), body.rstrip("\n"))
    return res


def fmt(args, rc, out):
    return f"> ExtraVersion {args}".rstrip() + f"  [rc={rc}]\n" + out.rstrip("\n") + "\n"


def run_host(todo):
    work = os.path.join(ROOT, "build", "hosttest")
    shutil.rmtree(work, ignore_errors=True)
    os.makedirs(work)
    shutil.copy(HOST_BIN, os.path.join(work, "ExtraVersion"))
    os.symlink(TESTS, os.path.join(work, "tests"))
    res = {}
    for args in todo:
        p = subprocess.run(["./ExtraVersion"] + args.split(), cwd=work, capture_output=True)
        res[args] = (p.returncode, (p.stdout + p.stderr).decode("latin-1").rstrip("\n"))
    return res


async def run_guest_async(todo):
    import asyncio
    from amiga_fleet_mcp.config import load_config
    from amiga_fleet_mcp.fleet import Fleet
    from amiga_fleet_mcp.tools import fs as fs_tool

    fleet = Fleet(load_config())
    m = fleet.mcpd(TARGET)

    async def cmd(c):
        r = await m.request("exec.cmd", {"command": c, "cwd": G, "timeout_ms": 30000}, timeout_s=60)
        return r.get("exit_code", -1), r.get("output", "")

    # binary + test files in one archive, unpacked by LhA in the guest
    stage = os.path.join(ROOT, "build", "guesttest")
    shutil.rmtree(stage, ignore_errors=True)
    os.makedirs(os.path.join(stage, "EVTest"))
    shutil.copy(OS4_BIN, os.path.join(stage, "EVTest", "ExtraVersion"))
    shutil.copytree(TESTS, os.path.join(stage, "EVTest", "tests"),
                    ignore=shutil.ignore_patterns("REMARK*", "*.py", "*.txt", "__pycache__"))
    subprocess.run([LHA, "aq2o5", "../evtest.lha", "EVTest"], cwd=stage, check=True)
    await m.request("exec.cmd", {"command": f"Delete {G} ALL QUIET FORCE", "timeout_ms": 30000}, timeout_s=60)
    await fs_tool.fs_upload(fleet, TARGET, local_path=os.path.join(ROOT, "build", "evtest.lha"),
                            remote_path="RAM:evtest.lha", verify=True)
    r = await m.request("exec.cmd", {"command": "LhA -q x evtest.lha", "cwd": "RAM:", "timeout_ms": 30000},
                        timeout_s=60)
    if r.get("exit_code") != 0:
        sys.exit("LhA failed in the guest: " + r.get("output", ""))
    res = {}
    for args in todo:
        rc, out = await cmd(f"ExtraVersion {args} <NIL:".replace("  ", " "))
        res[args] = (rc, out.replace("\r", "").rstrip("\n"))
    # MCPd keeps the last jobs' current drawer (G) locked for a moment
    for _ in range(20):
        await m.request("exec.cmd", {"command": f"Delete {G} RAM:evtest.lha ALL QUIET FORCE", "cwd": "RAM:",
                                     "timeout_ms": 30000}, timeout_s=60)
        r = await m.request("exec.cmd", {"command": f"List {G}", "cwd": "RAM:", "timeout_ms": 30000},
                            timeout_s=60)
        if r.get("exit_code") != 0:
            break
        await asyncio.sleep(0.5)
    else:
        print(f"note: {G} is still in the guest (in use)")
    return res


def main():
    guest = "--guest" in sys.argv
    update = "--update" in sys.argv
    all_cases = cases()
    todo = [a for a, gonly in all_cases if guest or not gonly]
    if guest:
        import asyncio
        res = asyncio.run(run_guest_async(todo))
    else:
        if not os.path.exists(HOST_BIN):
            sys.exit("no build/host/ExtraVersion: run make host")
        res = run_host(todo)

    expected = parse_expected(open(EXPECTED, encoding="latin-1").read()) if os.path.exists(EXPECTED) else {}
    if update:
        expected.update(res)
        with open(EXPECTED, "w", encoding="latin-1") as f:
            for args, _ in all_cases:
                if args in expected:
                    f.write(fmt(args, *expected[args]))
        print(f"wrote {EXPECTED} ({len(res)} cases from this run)")
        return

    failed = 0
    for args in todo:
        if args not in expected:
            print(f"NEW  ExtraVersion {args}: not in expected.txt")
            failed += 1
        elif res[args] != expected[args]:
            failed += 1
            print(f"FAIL ExtraVersion {args}")
            print("  expected:", expected[args])
            print("  got:     ", res[args])
    where = "guest" if guest else "host"
    print(f"{where}: {len(todo)} cases, {failed} failed")
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main()
