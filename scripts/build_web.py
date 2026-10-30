"""Package the tracked CYD firmware and flasher for GitHub Pages."""
import hashlib
import json
from pathlib import Path
import shutil

ROOT = Path(__file__).resolve().parents[1]
firmware = ROOT / "firmware/CYDHamClock.bin"
image = firmware.read_bytes()
# This project uses no_ota.csv: app at 0x10000, SPIFFS at 0x210000.
if not 0x210000 < len(image) <= 0x400000:
    raise SystemExit("Expected a merged 4 MB ESP32 image including SPIFFS")
if image[0x1000] != 0xE9 or image[0x10000] != 0xE9:
    raise SystemExit("Missing ESP32 bootloader/application headers")
if image[0x8000:0x8002] != b"\xaa\x50":
    raise SystemExit("Missing partition table")

digest = hashlib.sha256(image).hexdigest()
out = ROOT / "build/web"
out.mkdir(parents=True, exist_ok=True)
shutil.copyfile(ROOT / "web/index.html", out / "index.html")
shutil.copyfile(firmware, out / "CYDHamClock.bin")
shutil.copyfile(ROOT / "LICENSE", out / "LICENSE.txt")
manifest = {
    "name": "ESP32 CYD HamClock",
    "version": digest[:12],
    "new_install_prompt_erase": True,
    "builds": [{"chipFamily": "ESP32", "parts": [
        {"path": "CYDHamClock.bin", "offset": 0}
    ]}],
}
(out / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
(out / "SHA256SUMS.txt").write_text(f"{digest}  CYDHamClock.bin\n")
(out / ".nojekyll").touch()
print(f"Packaged {len(image)} bytes; SHA-256: {digest}")
