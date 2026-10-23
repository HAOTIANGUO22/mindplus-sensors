"""Build an Uno percentage .mp project using a user-saved Mind+ project as template."""
from pathlib import Path
from zipfile import ZipFile, ZIP_DEFLATED
from io import BytesIO
import argparse
import copy
import json

root = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser()
parser.add_argument('--template', type=Path, default=root/'examples/MindPlus/电位器示例程序.mp')
args = parser.parse_args()
with ZipFile(args.template) as source:
    project = json.loads(source.read('project.json'))
    media = {n: source.read(n) for n in source.namelist() if '/' not in n and n != 'project.json'}

ext = 'haotianguo22-potentiometer-thirdex'
def block(opcode, parent=None, next=None, inputs=None, fields=None):
    return dict(opcode=opcode, next=next, parent=parent, inputs=inputs or {}, fields=fields or {}, shadow=False, topLevel=parent is None)

blocks = {
    'pot-start': block('arduino.arduino_functions', next='pot-baud'),
    'pot-baud': block('arduino.arduino_set_serial', 'pot-start', 'pot-init', fields={'BAUD':['115200',None]}),
    'pot-init': block(ext+'.begin', 'pot-baud', 'pot-loop', fields={'PIN':['A0',None]}),
    'pot-loop': block('control_loop', 'pot-init', inputs={'SUBSTACK':[2,'pot-print']}),
    'pot-print': block('arduino.arduino_serial_send', 'pot-loop', 'pot-wait', inputs={'STRING':[3,'pot-percent',[10,'0']]}, fields={'WAY':['string output',None],'LINE':['Wrap',None]}),
    'pot-percent': block(ext+'.percent', 'pot-print'),
    'pot-wait': block('control_wait', 'pot-print', inputs={'DURATION':[1,[5,'0.1']]})
}
blocks['pot-start'].update(x=120,y=100)
for page in ('page0','page1','page2'):
    for target in project.get(page,{}).get('targets',[]):
        for key in ('blocks','comments','variables','mcvariables','strings','lists','broadcasts'):
            if key in target: target[key] = {}
target = next(t for t in project['page1']['targets'] if not t['isStage'])
target['blocks'] = blocks
page = project['page1']
page['extensions'] = ['arduino']
page['thirdExtensions'] = [ext]
page['monitors'] = []
page['pageInfo'] = {'page':1,'codeLanguage':'cpp','writeCode':''}
project['pageIndex'] = 1

config = copy.deepcopy(json.loads((root/'config.json').read_text(encoding='utf-8')))
embedded = {}
for p in (root/'arduinoC').rglob('*'):
    if p.is_file() and 'libraries' not in p.relative_to(root/'arduinoC').parts:
        embedded[p.relative_to(root).as_posix()] = p.read_bytes()
library_bytes = BytesIO()
with ZipFile(library_bytes,'w',ZIP_DEFLATED) as library_zip:
    for p in (root/'arduinoC/libraries').rglob('*'):
        if p.is_file(): library_zip.writestr(p.relative_to(root/'arduinoC/libraries').as_posix(),p.read_bytes())
embedded['arduinoC/libraries/libraries.zip'] = library_bytes.getvalue()
config['asset']['arduinoC']['files'] = sorted(n[len('arduinoC/'):] for n in embedded if n != 'arduinoC/main.ts')
embedded['config.json'] = json.dumps(config,ensure_ascii=False,indent=2).encode('utf-8')

out = root/'examples/MindPlus/电位器_Uno_A0_百分比示例.mp'
out.parent.mkdir(parents=True,exist_ok=True)
with ZipFile(out,'w',ZIP_DEFLATED) as z:
    z.writestr('project.json',json.dumps(project,ensure_ascii=False,separators=(',',':')).encode('utf-8'))
    for name,data in media.items(): z.writestr(name,data)
    dirs = {ext+'/'}
    for name in embedded:
        parts = name.split('/')[:-1]
        for i in range(1,len(parts)+1): dirs.add(ext+'/'+'/'.join(parts[:i])+'/')
    for name in sorted(dirs): z.writestr(name,b'')
    for name,data in embedded.items(): z.writestr(ext+'/'+name,data)

with ZipFile(out) as z:
    assert z.testzip() is None
    saved = json.loads(z.read('project.json'))
    assert saved['page1']['thirdExtensions'] == [ext]
    assert z.read(ext+'/arduinoC/main.ts') == (root/'arduinoC/main.ts').read_bytes()
    with ZipFile(BytesIO(z.read(ext+'/arduinoC/libraries/libraries.zip'))) as libs:
        assert libs.testzip() is None
        assert libs.read('MindPlusPotentiometer/MindPlusPotentiometer.cpp') == (root/'arduinoC/libraries/MindPlusPotentiometer/MindPlusPotentiometer.cpp').read_bytes()
    for page_name in ('page0','page1','page2'):
        for target in saved.get(page_name,{}).get('targets',[]):
            for asset in target.get('costumes',[]) + target.get('sounds',[]):
                assert asset['md5ext'] in z.namelist()
for name,b in blocks.items():
    if b['next']: assert blocks[b['next']]['parent'] == name
    for inp in b['inputs'].values():
        if isinstance(inp[1],str): assert blocks[inp[1]]['parent'] == name
assert sum(b['topLevel'] for b in blocks.values()) == 1
print('PASS: native .mp archive, embedded current extension/driver, assets, and 7-block graph')
print(out)
