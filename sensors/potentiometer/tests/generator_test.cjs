const fs = require('fs'), vm = require('vm'), assert = require('assert'), path = require('path');
const ts = require(process.argv[2]);
process.chdir(path.resolve(__dirname, '..'));
const source = fs.readFileSync('arduinoC/main.ts','utf8');
const js = ts.transpileModule(source,{reportDiagnostics:true,compilerOptions:{target:ts.ScriptTarget.ES5}});
assert.equal((js.diagnostics||[]).filter(d=>d.category===ts.DiagnosticCategory.Error).length,0);
const include=new Map(), objects=new Map(), output=[];
const ctx={Generator:{ORDER_UNARY_POSTFIX:1,addInclude:(id,x)=>include.set(id,x),addObject:(id,t,x)=>objects.set(id,`${t} ${x}`),addCode:x=>output.push(Array.isArray(x)?`(void)${x[0]};`:x)}};
vm.createContext(ctx); vm.runInContext(js.outputText,ctx);
assert.equal(Object.keys(ctx.potentiometer).length,10);
for(const [board,pin] of [['arduino','A0'],['esp32','33']]) {
  include.clear(); objects.clear(); output.length=0;
  const menu=JSON.parse(fs.readFileSync(`arduinoC/_menus/${board}.json`,'utf8'));
  assert.equal(menu.POTPIN.default_begin_PIN,pin);
  assert.equal(menu.POTSAMPLES.default_samples_COUNT,'8');
  ctx.potentiometer.begin({PIN:{code:pin}},{});
  assert.equal(output[0],`mpPotentiometer.begin(${pin});`);
  for(const fn of ['ready','raw','percent','maximum','resetCalibration']) ctx.potentiometer[fn]({},{});
  ctx.potentiometer.scaled({LOW:{code:'-10.5'},HIGH:{code:'20.5'}},{});
  ctx.potentiometer.calibrate({LOW:{code:'100'},HIGH:{code:'900'}},{});
  for(const [,value] of menu.POTSAMPLES.menu) ctx.potentiometer.samples({COUNT:{code:value}},{});
  for(const [,value] of menu.POTREVERSE.menu) ctx.potentiometer.reverse({REVERSE:{code:value}},{});
  for(const [,value] of menu.POTPIN.menu) ctx.potentiometer.begin({PIN:{code:value}},{});
  assert.equal(include.size,1); assert.equal(objects.size,1);
  const dir=`.verification/generated-${board}`; fs.mkdirSync(dir,{recursive:true});
  fs.writeFileSync(`${dir}/generated-${board}.ino`,`${[...include.values()].join('\n')}\n${[...objects.values()].join('\n')}\nvoid setup(){\n${output.join('\n')}\n}\nvoid loop(){(void)mpPotentiometer.percent();}\n`);
}
console.log('PASS: all 10 block generators, board-specific defaults, every pin/sample/reverse menu option, include/object deduplication');
