"""Check links to repository files and Markdown headings with cmark-gfm."""

import posixpath
import subprocess
import sys
import unicodedata
import xml.etree.ElementTree as ET
from pathlib import Path, PurePosixPath
from urllib.parse import unquote, urlsplit


XML_NS = "{http://commonmark.org/xml/1.0}"
HEADING_TEXT = {XML_NS + "text", XML_NS + "code"}
HEADING_BREAK = {XML_NS + "softbreak", XML_NS + "linebreak"}


def parse_markdown(path):
    xml = subprocess.check_output(["cmark-gfm", "--extension", "table", "--to", "xml", "--sourcepos", path])
    return ET.fromstring(xml)


def heading_anchors(document):
    anchors = set()
    for heading in document.iter(XML_NS + "heading"):
        title = "".join(
            (node.text or "") if node.tag in HEADING_TEXT else " "
            for node in heading.iter()
            if node.tag in HEADING_TEXT or node.tag in HEADING_BREAK
        )
        base = "".join(
            char for char in title.lower().strip().replace(" ", "-")
            if char in "-_" or unicodedata.category(char)[0] in "LMN"
        )
        anchor = base
        suffix = 1
        while anchor in anchors:
            anchor = f"{base}-{suffix}"
            suffix += 1
        anchors.add(anchor)
    return anchors


def local_target(source, destination):
    url = urlsplit(destination)
    if url.scheme or url.netloc:
        return None

    path = unquote(url.path)
    if not path:
        target = source
    elif path.startswith("/"):
        target = posixpath.normpath(path.lstrip("/"))
    else:
        target = posixpath.normpath(posixpath.join(posixpath.dirname(source), path))
    return target, unquote(url.fragment)


def main():
    entries = subprocess.check_output(["git", "ls-files", "-z", "--cached", "--others", "--exclude-standard"])
    files = {path for path in entries.decode("utf-8").split("\0") if path and Path(path).is_file()}
    directories = {parent.as_posix() for path in files for parent in PurePosixPath(path).parents}
    documents = {path: parse_markdown(path) for path in sorted(files) if path.lower().endswith(".md")}
    anchors = {path: heading_anchors(document) for path, document in documents.items()}

    failed = False
    for source, document in documents.items():
        for node in document.iter():
            if node.tag not in {XML_NS + "link", XML_NS + "image"}:
                continue
            destination = node.attrib["destination"]
            try:
                link = local_target(source, destination)
                if link is None:
                    continue
                target, fragment = link
                if target not in files and target not in directories:
                    raise ValueError("target does not exist in the repository")
                if fragment and target in anchors and fragment not in anchors[target]:
                    raise ValueError("Markdown heading does not exist")
            except ValueError as error:
                location = node.get("sourcepos", "").split("-", 1)[0]
                print(f"{source}:{location}: {destination}: {error}", file=sys.stderr)
                failed = True

    if not failed:
        print(f"Checked local links in {len(documents)} Markdown files.")
    return int(failed)


if __name__ == "__main__":
    sys.exit(main())
