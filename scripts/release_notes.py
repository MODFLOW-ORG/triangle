"""
Check the release note items and write the release notes.

Items are TOML files in changes/, one for each change a user sees (see
changes/README.md). At a release they become an entry in CHANGELOG.md, which
the release workflow uses as the release description.
"""

import argparse
import datetime
import sys
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
ITEMS = ROOT / "changes"
CHANGELOG = ROOT / "CHANGELOG.md"

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
            data = tomllib.loads(path.read_text())
        except tomllib.TOMLDecodeError as error:
            errors.append(f"{path.name}: not valid TOML: {error}")
            continue
        if set(data) != {"section", "description"}:
            errors.append(f"{path.name}: needs exactly section and description")
            continue
        if data["section"] not in SECTIONS:
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
    for line in CHANGELOG.read_text().splitlines():
        if line.startswith("## "):
            if inside:
                break
            inside = line.startswith(heading)
            continue
        if inside:
            body.append(line)
    if not inside and not body:
        sys.exit(f"CHANGELOG.md has no entry for {version}")
    return "\n".join(body).strip() + "\n"


def release(version, date):
    """Add the items to CHANGELOG.md as the entry for a version and remove them."""
    items = load_items()
    if not items:
        sys.exit("there are no release note items")
    text = CHANGELOG.read_text()
    if f"\n## {version} " in text:
        sys.exit(f"CHANGELOG.md already has an entry for {version}")
    # the new entry goes before the first existing one
    first = text.find("\n## ")
    head, tail = (text, "") if first < 0 else (text[: first + 1], text[first + 1 :])
    new = f"## {version} ({date})\n\n{render(items)}\n"
    CHANGELOG.write_text(head.rstrip("\n") + "\n\n" + new + tail)
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
