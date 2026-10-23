"""Usage from any directory: python <workspace>/tools/package.py rotary-encoder."""
import argparse
from pathlib import Path
import shutil
import subprocess
import sys

workspace = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description='Build a sensor source ZIP and collect sharing files.')
parser.add_argument('sensor', help='Folder name under sensors/, e.g. rotary-encoder')
args = parser.parse_args()
sensor = (workspace/'sensors'/args.sensor).resolve()
if sensor.parent != (workspace/'sensors').resolve() or not (sensor/'config.json').is_file():
    parser.error('Specify one existing sensor folder containing config.json.')
packager = sensor/'tools/package.py'
if not packager.is_file():
    parser.error('Sensor must provide tools/package.py with --output-dir support.')
destination = workspace/'releases'/sensor.name
destination.mkdir(parents=True, exist_ok=True)
subprocess.run([sys.executable, str(packager), '--output-dir', str(destination)], cwd=sensor, check=True)
for program in (sensor/'examples/MindPlus').glob('*.mp'):
    shutil.copy2(program, destination/program.name)
for program in (sensor/'examples/MindPlus').glob('*.sb3'):
    shutil.copy2(program, destination/program.name)
sharing = sensor/'分享说明.txt'
if sharing.is_file():
    shutil.copy2(sharing, destination/sharing.name)
print(f'Sharing files: {destination}')
print('Official .mpext exports are retained as-is; export them from Mind+ after library changes.')
