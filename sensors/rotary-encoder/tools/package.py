"""Package existing project assets without regenerating or overwriting the cover."""
from pathlib import Path
from zipfile import ZipFile, ZIP_DEFLATED
import json
import struct
import argparse

root = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description='Package the rotary encoder source library.')
parser.add_argument('--output-dir', type=Path, default=root/'dist')
args = parser.parse_args()
cover = (root/'arduinoC/_images/featured.png').read_bytes()
assert cover[:8] == b'\x89PNG\r\n\x1a\n'
assert struct.unpack('>II', cover[16:24]) == (600, 372), 'Cover must be 600x372'

for p in [root/'config.json', *(root/'arduinoC/_menus').glob('*.json')]:
    json.loads(p.read_text(encoding='utf-8'))
version = json.loads((root/'config.json').read_text(encoding='utf-8'))['version']
dest = args.output_dir.resolve()/f'MindPlus-RotaryEncoder-{version}.zip'
dest.parent.mkdir(parents=True, exist_ok=True)
files = [root/'config.json',root/'README.md',root/'LICENSE.TXT',root/'VALIDATION.md',root/'分享说明.txt']
for dirname in ('arduinoC','examples/MindPlus','tests','tools','assets'):
    files.extend(p for p in (root/dirname).rglob('*') if p.is_file() and '__pycache__' not in p.parts)
with ZipFile(dest,'w',ZIP_DEFLATED) as z:
    for p in sorted(files): z.write(p,'MindPlus-RotaryEncoder/'+p.relative_to(root).as_posix())
with ZipFile(dest) as z:
    assert z.testzip() is None
    assert not any(name.lower().endswith('.ino') for name in z.namelist()), 'Only Mind+ examples are published'
    assert 'MindPlus-RotaryEncoder/config.json' in z.namelist()
    assert z.read('MindPlus-RotaryEncoder/arduinoC/_images/featured.png') == cover
print(f'Packaged {len(files)} files: {dest}')
