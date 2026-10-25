#!/usr/bin/env python3
"""Prepare ESP-IDF build output as flashable ESP32 release assets."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import shlex
from pathlib import Path


PROJECT_NAME = "open_extraction"
REQUIRED_IMAGES = (
    Path("bootloader/bootloader.bin"),
    Path("partition_table/partition-table.bin"),
    Path(f"{PROJECT_NAME}.bin"),
)


def parse_flash_images(flash_args: str) -> list[tuple[str, Path]]:
    """Return flash addresses and binary paths from ESP-IDF arguments."""
    tokens = shlex.split(flash_args, comments=True)
    images: list[tuple[str, Path]] = []
    for index, token in enumerate(tokens[:-1]):
        if re.fullmatch(r"0x[0-9a-fA-F]+", token):
            image = Path(tokens[index + 1])
            if image.is_absolute() or ".." in image.parts:
                raise ValueError(f"Unsafe image path in flash_args: {image}")
            images.append((token, image))
    return images


def rewrite_flash_args(flash_args: str, image_names: dict[Path, str]) -> str:
    """Rewrite image paths so flash_args works beside separately downloaded files."""
    rewritten_lines = []
    rewritten_images = 0
    for line in flash_args.splitlines():
        tokens = shlex.split(line, comments=True)
        if any(re.fullmatch(r"0x[0-9a-fA-F]+", token) for token in tokens):
            if len(tokens) != 2 or not re.fullmatch(r"0x[0-9a-fA-F]+", tokens[0]):
                raise ValueError(f"Unexpected image entry in flash_args: {line}")
            image = Path(tokens[1])
            rewritten_lines.append(f"{tokens[0]} {shlex.quote(image_names[image])}")
            rewritten_images += 1
        else:
            rewritten_lines.append(line)
    if rewritten_images != len(parse_flash_images(flash_args)):
        raise ValueError("Could not rewrite every image path in flash_args")
    return "\n".join(rewritten_lines) + "\n"


def package_firmware(build_dir: Path, output_dir: Path, commit: str) -> None:
    """Validate ESP-IDF output and prepare individual flashable release assets."""
    build_dir = build_dir.resolve()
    flash_args_path = build_dir / "flash_args"
    if not flash_args_path.is_file():
        raise FileNotFoundError(f"ESP-IDF flash arguments not found: {flash_args_path}")

    flash_args = flash_args_path.read_text(encoding="utf-8")
    flash_images = parse_flash_images(flash_args)
    if not flash_images:
        raise ValueError("ESP-IDF flash_args does not reference any binary images")

    for image in REQUIRED_IMAGES:
        if image not in {image_path for _, image_path in flash_images}:
            raise ValueError(f"Required firmware image is missing from flash_args: {image}")

    referenced_images = list(dict.fromkeys(image for _, image in flash_images))
    packaged_images: dict[Path, Path] = {}
    image_names: dict[Path, str] = {}
    for relative_path in referenced_images:
        source = build_dir / relative_path
        if not source.is_file():
            raise FileNotFoundError(f"Image referenced by flash_args not found: {relative_path}")
        if relative_path.name in image_names.values():
            raise ValueError(
                f"Duplicate binary filename cannot be uploaded separately: {relative_path.name}"
            )
        packaged_images[relative_path] = source
        image_names[relative_path] = relative_path.name

    instructions = Path(__file__).resolve().parents[1] / "firmware" / "FLASHING.md"
    output_dir.mkdir(parents=True, exist_ok=True)
    manifest_images = []
    flash_addresses = {image: address for address, image in flash_images}
    for relative_path, source in packaged_images.items():
        destination_name = image_names[relative_path]
        data = source.read_bytes()
        (output_dir / destination_name).write_bytes(data)
        manifest_images.append(
            {
                "path": destination_name,
                "size": len(data),
                "sha256": hashlib.sha256(data).hexdigest(),
                "flash_address": flash_addresses[relative_path],
            }
        )

    (output_dir / "flash_args").write_text(
        rewrite_flash_args(flash_args, image_names), encoding="utf-8"
    )
    (output_dir / "README.md").write_text(
        instructions.read_text(encoding="utf-8"), encoding="utf-8"
    )
    manifest = {
        "project": PROJECT_NAME,
        "target": "esp32",
        "source_commit": commit,
        "images": manifest_images,
    }
    (output_dir / "manifest.json").write_text(
        json.dumps(manifest, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )


def main() -> None:
    """Parse command-line arguments and package firmware build output."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-dir", required=True, type=Path)
    parser.add_argument("--output-dir", required=True, type=Path)
    parser.add_argument("--commit", required=True)
    args = parser.parse_args()
    package_firmware(args.build_dir, args.output_dir, args.commit)


if __name__ == "__main__":
    main()
