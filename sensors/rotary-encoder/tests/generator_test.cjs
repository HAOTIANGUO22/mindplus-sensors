const fs = require('fs');
const vm = require('vm');
const assert = require('assert');
const ts = require(process.argv[2]);
process.chdir(require('path').resolve(__dirname, '..'));
const src = fs.readFileSync('arduinoC/main.ts', 'utf8');
const result = ts.transpileModule(src, {reportDiagnostics:true, compilerOptions:{target:ts.ScriptTarget.ES5}});
assert.equal((result.diagnostics || []).filter(d => d.category===ts.DiagnosticCategory.Error).length,0);
const includes = new Map(), objects = new Map(), code = [];
const context = { Generator: {
  ORDER_UNARY_POSTFIX:1,
  addInclude:(id,v)=>includes.set(id,v),
  addObject:(id,t,v)=>objects.set(id,`${t} ${v}`),
  addCode:v=>code.push(Array.isArray(v)?v[0]:v)
}};
vm.createContext(context); vm.runInContext(result.outputText,context);
for(const [menuFile,expected] of [['uno',[2,3,4]],['mpython',[33,32,26]]]) {
  includes.clear(); objects.clear(); code.length=0;
  const menus=JSON.parse(fs.readFileSync(`arduinoC/_menus/${menuFile}.json`));
  const params={};
  for(const [field,menu] of [['A','ENCPIN'],['B','ENCPIN'],['SW','BTNPIN']]) {
    const value=menus[menu][`default_begin_${field}`];
    assert(menus[menu].menu.some(x=>x[1]===value)); params[field]={code:value};
  }
  context.rotaryencoder.begin(params,{});
  assert.equal(code[0],`mpRotaryEncoder.begin(${expected.join(', ')});`);
  for(const name of ['ready','count','delta','direction','update','pressed','clicked']) context.rotaryencoder[name]({},{});
  context.rotaryencoder.setCount({VALUE:{code:'-12'}},{});
  context.rotaryencoder.reverse({REVERSE:{code:'1'}},{});
  context.rotaryencoder.transitions({STEPS:{code:'2'}},{});
  assert.equal(includes.size,1); assert.equal(objects.size,1);
  const calls=code.map(x=>x.endsWith(';')?x:`(void)${x};`).join('\n');
  const dir=`.verification/generated-${menuFile}`; fs.mkdirSync(dir,{recursive:true});
  fs.writeFileSync(`${dir}/generated-${menuFile}.ino`,`${[...includes.values()].join('\n')}\n${[...objects.values()].join('\n')}\nvoid setup(){\n${calls}\n}\nvoid loop(){mpRotaryEncoder.update();}\n`);
}
console.log('PASS: TypeScript transpilation, Uno/mPython defaults, all 11 block generators, include/object deduplication');
