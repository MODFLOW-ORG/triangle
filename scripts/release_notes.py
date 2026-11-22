"""
Check the release note items and write the release notes.

Items are TOML files in changes/, one for each change a user sees (see
changes/README.md). At a release they become an entry in CHANGELOG.md, which
the release workflow uses as the release description.
"""

import argparse
import datetime
import re
import sys
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
ITEMS = ROOT / "changes"
CHANGELOG = ROOT / "CHANGELOG.md"
MESON = ROOT / "meson.build"
PIXI = ROOT / "pixi.toml"

# item sections, in the order they appear in the notes
SECTIONS = {
    "features": "New features",
    "fixes": "Fixes",
    "build": "Build and distribution",
    "documentation": "Documentation",
}


def load_items():
    """Return the items as (path, section, description), or exit on an error."""
    items = []
    errors = []
    for path in sorted(ITEMS.glob("*.toml")):
        try:
            data = tomllib.loads(path.read_text(encoding="utf-8"))
        except tomllib.TOMLDecodeError as error:
            errors.append(f"{path.name}: not valid TOML: {error}")
            continue
        if set(data) != {"section", "description"}:
            errors.append(f"{path.name}: needs exactly section and description")
            continue
        if not isinstance(data["section"], str) or data["section"] not in SECTIONS:
            errors.append(
                f"{path.name}: section must be one of {', '.join(SECTIONS)}"
            )
        description = data["description"]
        if not isinstance(description, str) or not description.strip():
            errors.append(f"{path.name}: description is empty")
            continue
        items.append((path, data["section"], " ".join(description.split())))
    if errors:
        sys.exit("\n".join(errors))
    return items


def render(items):
    """Return the items as Markdown, one subsection for each section."""
    lines = []
    for section, heading in SECTIONS.items():
        descriptions = [d for _, s, d in items if s == section]
        if descriptions:
            lines += [f"### {heading}", ""]
            lines += [f"- {d}" for d in descriptions]
            lines.append("")
    return "\n".join(lines)


def entry(version):
    """Return the CHANGELOG.md entry for a version, without its heading."""
    heading = f"## {version} "
    body = []
    inside = False
    for line in CHANGELOG.read_text(encoding="utf-8").splitlines():
        if line.startswith("## "):
            if inside:
                break
            inside = line.startswith(heading)
            continue
        if inside:
            body.append(line)
    if not inside and not body:
        sys.exit(f"CHANGELOG.md has no entry for {version}")
    if not "\n".join(body).strip():
        sys.exit(f"the CHANGELOG.md entry for {version} is empty")
    if any(ITEMS.glob("*.toml")):
        print(
            "warning: changes/ has items that are not in a release", file=sys.stderr
        )
    return "\n".join(body).strip() + "\n"


def project_version():
    """Return the version in meson.build, or exit if pixi.toml differs."""
    match = re.search(r"\bversion\s*:\s*'([^']+)'", MESON.read_text(encoding="utf-8"))
    if match is None:
        sys.exit("meson.build has no project version")
    pixi = tomllib.loads(PIXI.read_text(encoding="utf-8"))["workspace"]["version"]
    if pixi != match.group(1):
        sys.exit(
            f"pixi.toml version {pixi} differs from {match.group(1)} in meson.build"
        )
    return match.group(1)


def release(version, date):
    """Add the items to CHANGELOG.md as the entry for a version and remove them."""
    if version != project_version():
        sys.exit(
            f"version {version} differs from {project_version()} in meson.build;"
            " set the version first"
        )
    try:
        datetime.date.fromisoformat(date)
    except ValueError:
        sys.exit(f"date {date} is not YYYY-MM-DD")
    items = load_items()
    if not items:
        sys.exit("there are no release note items")
    text = CHANGELOG.read_text(encoding="utf-8")
    if f"\n## {version} " in text:
        sys.exit(f"CHANGELOG.md already has an entry for {version}")
    # the new entry goes before the first existing one
    first = text.find("\n## ")
    head, tail = (text, "") if first < 0 else (text[: first + 1], text[first + 1 :])
    new = f"## {version} ({date})\n\n{render(items)}\n"
    CHANGELOG.write_text(head.rstrip("\n") + "\n\n" + new + tail, encoding="utf-8")
    for path, _, _ in items:
        path.unlink()


def main():
    parser = argparse.ArgumentParser(description=__doc__.strip().splitlines()[0])
    commands = parser.add_subparsers(dest="command", required=True)
    commands.add_parser("check", help="check the items")
    notes = commands.add_parser(
        "notes", help="print the items, or a released version's entry"
    )
    notes.add_argument("--version", help="released version to print")
    add = commands.add_parser("release", help="move the items into CHANGELOG.md")
    add.add_argument("--version", required=True)
    add.add_argument("--date", default=datetime.date.today().isoformat())
    args = parser.parse_args()

    if args.command == "check":
        items = load_items()
        print(f"{len(items)} release note item(s)")
    elif args.command == "notes":
        print(entry(args.version) if args.version else render(load_items()), end="")
    else:
        release(args.version, args.date)


if __name__ == "__main__":
    main()
