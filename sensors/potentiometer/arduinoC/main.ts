//% color="#B47723" iconWidth=32 iconHeight=32
namespace potentiometer {
    function ensurePot() {
        Generator.addInclude("mpPotentiometer", "#include <MindPlusPotentiometer.h>");
        Generator.addObject("mpPotentiometer", "MindPlusPotentiometer", "mpPotentiometer;");
    }

    //% block="初始化电位器 模拟信号 S 接 [PIN]" blockType="command"
    //% PIN.shadow="dropdown" PIN.options="POTPIN" PIN.defl="POTPIN.A0"
    export function begin(parameter: any, block: any) {
        ensurePot();
        Generator.addCode(`mpPotentiometer.begin(${parameter.PIN.code});`);
    }

    //% block="电位器初始化成功？" blockType="boolean"
    export function ready(parameter: any, block: any) {
        ensurePot();
        Generator.addCode(["mpPotentiometer.ready()", Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="电位器原始读数" blockType="reporter"
    export function raw(parameter: any, block: any) {
        ensurePot();
        Generator.addCode(["mpPotentiometer.raw()", Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="电位器百分比（0～100）" blockType="reporter"
    export function percent(parameter: any, block: any) {
        ensurePot();
        Generator.addCode(["mpPotentiometer.percent()", Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="电位器映射到 [LOW] 至 [HIGH]" blockType="reporter"
    //% LOW.shadow="number" LOW.defl=0
    //% HIGH.shadow="number" HIGH.defl=255
    export function scaled(parameter: any, block: any) {
        ensurePot();
        Generator.addCode([`mpPotentiometer.scaled(${parameter.LOW.code}, ${parameter.HIGH.code})`, Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="电位器原始量程上限" blockType="reporter"
    export function maximum(parameter: any, block: any) {
        ensurePot();
        Generator.addCode(["mpPotentiometer.maximum()", Generator.ORDER_UNARY_POSTFIX]);
    }

    //% block="电位器平均采样次数 [COUNT]" blockType="command"
    //% COUNT.shadow="dropdown" COUNT.options="POTSAMPLES" COUNT.defl="POTSAMPLES.8"
    export function samples(parameter: any, block: any) {
        ensurePot();
        Generator.addCode(`mpPotentiometer.setSamples(${parameter.COUNT.code});`);
    }

    //% block="电位器数值方向 [REVERSE]" blockType="command"
    //% REVERSE.shadow="dropdown" REVERSE.options="POTREVERSE" REVERSE.defl="POTREVERSE.0"
    export function reverse(parameter: any, block: any) {
        ensurePot();
        Generator.addCode(`mpPotentiometer.setReverse(${parameter.REVERSE.code});`);
    }

    //% block="电位器校准 原始最小值 [LOW] 原始最大值 [HIGH]" blockType="command"
    //% LOW.shadow="number" LOW.defl=100
    //% HIGH.shadow="number" HIGH.defl=900
    export function calibrate(parameter: any, block: any) {
        ensurePot();
        Generator.addCode(`mpPotentiometer.calibrate(${parameter.LOW.code}, ${parameter.HIGH.code});`);
    }

    //% block="电位器恢复默认量程" blockType="command"
    export function resetCalibration(parameter: any, block: any) {
        ensurePot();
        Generator.addCode("mpPotentiometer.resetCalibration();");
    }
}
