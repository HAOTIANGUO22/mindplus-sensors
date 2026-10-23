//% color="#247D9C" iconWidth=32 iconHeight=32
namespace rotaryencoder {
    function ensureEncoder() {
        Generator.addInclude("mpRotaryEncoder", "#include <MindPlusRotaryEncoder.h>");
        Generator.addObject("mpRotaryEncoder", "MindPlusRotaryEncoder", "mpRotaryEncoder;");
    }

    //% block="初始化编码器 4P口 S1 [A] S2 [B] 3P口 S [SW]" blockType="command"
    //% A.shadow="dropdown" A.options="ENCPIN" A.defl="ENCPIN.2"
    //% B.shadow="dropdown" B.options="ENCPIN" B.defl="ENCPIN.3"
    //% SW.shadow="dropdown" SW.options="BTNPIN" SW.defl="BTNPIN.4"
    export function begin(parameter: any, block: any) {
        ensureEncoder();
        Generator.addCode(`mpRotaryEncoder.begin(${parameter.A.code}, ${parameter.B.code}, ${parameter.SW.code});`);
    }

    //% block="编码器初始化成功？" blockType="boolean"
    export function ready(parameter: any, block: any) {
        ensureEncoder();
        Generator.addCode(["mpRotaryEncoder.ready()", Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="编码器当前计数" blockType="reporter"
    export function count(parameter: any, block: any) {
        ensureEncoder();
        Generator.addCode(["mpRotaryEncoder.read()", Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="编码器计数设为 [VALUE]" blockType="command"
    //% VALUE.shadow="number" VALUE.defl=0
    export function setCount(parameter: any, block: any) {
        ensureEncoder();
        Generator.addCode(`mpRotaryEncoder.write(${parameter.VALUE.code});`);
    }

    //% block="编码器读取并清除变化量" blockType="reporter"
    export function delta(parameter: any, block: any) {
        ensureEncoder();
        Generator.addCode(["mpRotaryEncoder.takeDelta()", Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="编码器最后方向（正转1 反转-1）" blockType="reporter"
    export function direction(parameter: any, block: any) {
        ensureEncoder();
        Generator.addCode(["mpRotaryEncoder.direction()", Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="编码器计数方向 [REVERSE]" blockType="command"
    //% REVERSE.shadow="dropdown" REVERSE.options="ENCREVERSE" REVERSE.defl="ENCREVERSE.0"
    export function reverse(parameter: any, block: any) {
        ensureEncoder();
        Generator.addCode(`mpRotaryEncoder.setReverse(${parameter.REVERSE.code});`);
    }

    //% block="编码器每格信号变化次数 [STEPS]" blockType="command"
    //% STEPS.shadow="dropdown" STEPS.options="ENCSTEPS" STEPS.defl="ENCSTEPS.4"
    export function transitions(parameter: any, block: any) {
        ensureEncoder();
        Generator.addCode(`mpRotaryEncoder.setTransitions(${parameter.STEPS.code});`);
    }

    //% block="更新编码器按键状态" blockType="command"
    export function update(parameter: any, block: any) {
        ensureEncoder();
        Generator.addCode("mpRotaryEncoder.update();");
    }

    //% block="编码器按键按住？" blockType="boolean"
    export function pressed(parameter: any, block: any) {
        ensureEncoder();
        Generator.addCode(["mpRotaryEncoder.pressed()", Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="编码器按键新按下？（读取后清除）" blockType="boolean"
    export function clicked(parameter: any, block: any) {
        ensureEncoder();
        Generator.addCode(["mpRotaryEncoder.clicked()", Generator.ORDER_UNARY_POSTFIX]);
    }
}
