# MATIEC 用户手册：支持的 IEC 61131-3 语法

本文面向使用 `iec2c` 或 `iec2iec` 编写、检查和编译 PLC 文本程序的用户，说明当前仓库**实际实现**的语法。它不是 IEC 61131-3 标准全文的替代品；当标准、历史文档与实现不一致时，以本仓库的词法器、语法文件和回归测试为准。

MATIEC 的前端主要基于 IEC 61131-3 第 2 版最终草案，并选择性实现了第 3 版引用、嵌套注释等能力，以及少量需显式启用的非标准扩展。

## 语言 Profile

两个命令行工具都接受 `--std=<profile>`：

- `legacy`：默认值，保留现有 MATIEC 语法和输出行为；
- `iec61131-3:2025-experimental`：面向公开证据所确认的 2025 能力演进。目前增加 UTF-8 源码与 `STRING` 字面量、引用声明初始化，以及明确标注为 MATIEC 临时规则的命名空间、FB 方法和配置级 `VAR_ACCESS` 子集；它不是完整或认证符合 IEC 61131-3:2025 的声明。

例如：

```sh
iec2c --std=legacy program.st
iec2iec --std=iec61131-3:2025-experimental program.st
```

Profile 只表示标准版本方向。`-r`、`-R`、`-s`、`-n`、`-a` 等现有开关仍是独立的语言扩展，显式开关优先决定对应扩展是否启用；选择实验 Profile 不会自动打开它们。

## 目录

1. [支持范围概览](#1-支持范围概览)
2. [快速开始](#2-快速开始)
3. [词法规则](#3-词法规则)
4. [常量与字面量](#4-常量与字面量)
5. [数据类型](#5-数据类型)
6. [变量与声明块](#6-变量与声明块)
7. [程序组织单元（POU）](#7-程序组织单元pou)
8. [Structured Text（ST）](#8-structured-textst)
9. [Instruction List（IL）](#9-instruction-listil)
10. [文本 Sequential Function Chart（SFC）](#10-文本-sequential-function-chartsfc)
11. [配置、资源与任务](#11-配置资源与任务)
12. [会改变语法的命令行开关](#12-会改变语法的命令行开关)
13. [常见边界与注意事项](#13-常见边界与注意事项)
14. [排查语法错误](#14-排查语法错误)
15. [实现依据](#15-实现依据)

## 1. 支持范围概览

| 类别 | 状态 | 说明 |
| --- | --- | --- |
| Structured Text（ST） | 支持 | 表达式、赋值、调用、选择和循环语句 |
| Instruction List（IL） | 支持 | 文本指令、跳转、调用、标签和返回 |
| Sequential Function Chart（SFC） | 支持 | 文本形式的步、转换、动作和限定符 |
| Function Block Diagram（FBD） | 不直接支持 | 不解析图形化 FBD 输入 |
| Ladder Diagram（LD） | 不直接支持 | 不解析图形化 LD 输入 |
| 配置模型 | 支持 | `CONFIGURATION`、`RESOURCE`、`TASK`、程序实例 |
| `VAR_ACCESS` | 实验性部分支持 | 配置级简单 `VAR_GLOBAL` 路径；导出 `ACCESS.csv` |
| IEC 61131-3 第 3 版引用 | 可选 | 使用 `-r` 或 `-R` 启用 |
| 命名空间与限定名 | 实验性部分支持 | 仅 `iec61131-3:2025-experimental`；采用 MATIEC 临时规则 |
| 面向对象元素 | 实验性部分支持 | 仅支持 FB 的公开方法和静态派发；不支持类、接口、继承 |
| PLCopen SAFE 类型 | 可选 | 使用 `-s` 启用 |

一个输入文件可以包含多个类型声明、函数、功能块、程序和配置，也可以让使用 ST、IL、文本 SFC 的不同 POU 共存。单个 POU 的正文应采用一种文本表示，不要在同一正文内任意混写 ST 与 IL。

## 2. 快速开始

下面是一个最小 ST 程序：

```iecst
PROGRAM Counter
  VAR
    Value : INT := 0;
  END_VAR

  Value := Value + 1;
END_PROGRAM
```

只做语法分析，不进入后续语义阶段，也不生成 C：

```sh
./iec2c -y -I lib counter.st
```

生成 C 文件：

```sh
mkdir -p build/generated
./iec2c -I lib -T build/generated counter.st
```

规范化并重新输出 IEC 文本：

```sh
./iec2iec -I lib counter.st > normalized.st
```

`iec2c` 和 `iec2iec` 每次只接收一个入口文件。可以使用 include pragma 从入口文件载入其他声明。

## 3. 词法规则

### 3.1 大小写与标识符

关键字和标识符均不区分大小写。因此 `PROGRAM`、`Program` 和 `program` 等价，`MotorSpeed` 与 `motorspeed` 也指向同一个名字。

标识符由 ASCII 字母、数字和单下划线组成，并遵守以下规则：

- 首字符是字母，或者是后接字母/数字的下划线；
- 后续字符可以是字母、数字或单下划线；
- 不能以单独的下划线结尾，也不能包含连续下划线；
- 保留关键字不能当作普通新标识符使用。

示例：

```text
Motor1       有效
_temp2       有效
motor_speed  有效
2motor       无效
motor__speed 无效
motor_       无效
```

### 3.2 空白、换行与分号

空格、制表符和换行通常只用于分隔记号。ST 的每条普通语句以及声明列表中的每一项以分号 `;` 结束：

```iecst
VAR
  Enabled : BOOL := TRUE;
  Count   : DINT := 0;
END_VAR

Count := Count + 1;
```

IL 依赖换行区分指令，不在每条指令后添加分号。

### 3.3 注释

默认支持块注释：

```iecst
(* 这是注释 *)
```

嵌套块注释属于可选能力，使用 `-n` 启用：

```iecst
(* 外层注释
   (* 内层注释 *)
*)
```

当前词法器不把 `//` 定义为通用行注释，建议统一使用 `(* ... *)`。

### 3.4 Pragma 与文件包含

MATIEC 接受花括号 pragma，并专门处理以下形式：

```text
{disable code generation}
{enable code generation}
{#include "types.st" }
```

include 文件按 `-I` 指定的目录查找。一般 pragma 会进入语法树并由相应阶段处理；未知 pragma 不应被假定具有运行时效果。

## 4. 常量与字面量

### 4.1 数值、位串和布尔值

数字之间可以插入单下划线以提高可读性。

| 类别 | 示例 |
| --- | --- |
| 十进制整数 | `0`、`42`、`1_000_000`、`-12` |
| 二进制整数 | `2#1010_0110` |
| 八进制整数 | `8#755` |
| 十六进制整数 | `16#FF_A0` |
| 实数 | `3.14`、`1.0E-3` |
| 类型化整数 | `INT#42`、`DINT#-100` |
| 类型化位串 | `BYTE#16#FF`、`WORD#2#1010` |
| 布尔值 | `TRUE`、`FALSE`、`BOOL#1`、`BOOL#0` |

### 4.2 字符串

单引号字符串表示单字节字符串，双引号字符串表示双字节字符串：

```iecst
'ASCII text'
"Wide text"
```

支持 IEC `$` 转义，包括 `$$`、`$L`/`$N`/`$P`/`$R`/`$T`（也接受小写），以及十六进制字符形式 `$xx` 或 `$xxxx`。例如：

```iecst
'line 1$Nline 2'
'price: $$10'
```

实验 Profile 允许在单引号 `STRING` 中直接写 UTF-8，例如
`'中文🚀'`。源码（包括物理 include 文件）会先做 UTF-8 合法性检查，
非法序列会在词法处理前报出行列位置；UTF-8 BOM 只允许出现在文件开头。
内部沿用既有 `STRING` ABI，`len` 和 126 字节上限按编码后的字节数计算，
标识符仍只允许 ASCII。当前没有加入名称或签名未经公开权威资料确认的
第四版字符串函数，也不宣称该字节计数规则就是 IEC 的索引语义。详细边界见
[UTF-8 证据与行为说明](standards/utf8-source-and-strings.md)。

### 4.3 时间与日期

| 类型 | 可用前缀 | 示例 |
| --- | --- | --- |
| 持续时间 | `T#`、`TIME#` | `T#100ms`、`TIME#1d_2h_3m_4s` |
| 时刻 | `TOD#`、`TIME_OF_DAY#` | `TOD#12:30:00` |
| 日期 | `D#`、`DATE#` | `DATE#2026-09-06` |
| 日期与时刻 | `DT#`、`DATE_AND_TIME#` | `DT#2026-09-06-12:30:00` |

持续时间允许负号，也支持在最低一级使用小数，例如 `T#1.5s`。

### 4.4 枚举值与空引用

枚举值可以直接书写，也可以使用类型限定形式消除歧义：

```iecst
State := Running;
State := MachineState#Running;
```

`NULL` 是引用扩展中的空引用字面量，需要 `-r` 或 `-R`。

## 5. 数据类型

### 5.1 基本类型

| 类型组 | 类型 |
| --- | --- |
| 布尔 | `BOOL` |
| 有符号整数 | `SINT`、`INT`、`DINT`、`LINT` |
| 无符号整数 | `USINT`、`UINT`、`UDINT`、`ULINT` |
| 位串 | `BYTE`、`WORD`、`DWORD`、`LWORD` |
| 浮点 | `REAL`、`LREAL` |
| 字符串 | `STRING`、`WSTRING` |
| 时间 | `TIME` |
| 日期/时刻 | `DATE`、`TIME_OF_DAY`/`TOD`、`DATE_AND_TIME`/`DT` |

`ANY`、`ANY_NUM`、`ANY_INT` 等通用类型名主要用于标准函数的类型描述，不应当作普通变量的具体存储类型。

### 5.2 SAFE 类型

使用 `-s` 后可使用 PLCopen Safety 类型：

```text
SAFEBOOL
SAFESINT SAFEINT SAFEDINT SAFELINT
SAFEUSINT SAFEUINT SAFEUDINT SAFEULINT
SAFEBYTE SAFEWORD SAFEDWORD SAFELWORD
SAFEREAL SAFELREAL
SAFESTRING SAFEWSTRING
SAFETIME SAFEDATE SAFETIME_OF_DAY SAFETOD
SAFEDATE_AND_TIME SAFEDT
```

这些类型的语法可被接受，不代表仅靠编译器即可满足某项功能安全认证。

### 5.3 派生类型

派生类型集中写在 `TYPE ... END_TYPE` 中，每个声明后带分号。

```iecst
TYPE
  CounterValue : DINT := 0;
  Percentage   : INT (0..100) := 0;
  MachineState : (Idle, Starting, Running, Fault) := Idle;
  Samples      : ARRAY [0..9] OF REAL;
  Matrix       : ARRAY [1..3, 1..4] OF INT;
  Label        : STRING[31] := 'unnamed';

  MotorData : STRUCT
    Speed   : REAL := 0.0;
    Enabled : BOOL := FALSE;
    State   : MachineState := Idle;
  END_STRUCT;
END_TYPE
```

支持的派生形式包括：

- 基本类型别名及默认值；
- 整数子范围；
- 枚举及默认值；
- 一维和多维数组；
- 结构体及成员默认值；
- 指定长度的 `STRING[n]` 和 `WSTRING[n]`；
- 启用引用语法后的引用类型。

数组初始化支持普通列表、少于完整长度的列表、嵌套列表和重复项：

```iecst
VAR
  A : ARRAY [1..5] OF INT := [1, 2, 3, 4, 5];
  B : ARRAY [1..6] OF INT := [3(0), 10, 20, 30];
END_VAR
```

结构初始化使用成员名：

```iecst
VAR
  Motor : MotorData := (Speed := 12.5, Enabled := TRUE, State := Running);
END_VAR
```

数组边界默认必须是整数常量。`-a` 允许用符号名表示边界，但这是非标准扩展。

### 5.4 引用类型

使用 `-r` 启用 IEC 61131-3 第 3 版风格的引用：

```iecst
TYPE
  IntRef : REF_TO INT;
END_TYPE

PROGRAM ReferenceDemo
  VAR
    Value : INT := 7;
    Ptr   : IntRef;
  END_VAR

  Ptr := REF(Value);
  Value := DREF(Ptr);
  Value := Ptr^;
END_PROGRAM
```

`REF(x)` 取得引用，`DREF(x)` 和后缀 `^` 解引用。实验 Profile 与
`-r`/`-R` 同时使用时，还允许 `Ptr : REF_TO INT := NULL`。不同目标类型的
引用不能互相赋值。`-R` 包含 `-r` 的能力，并额外允许非标准的
`REF_TO ANY`，以及在数组或结构成员中更宽松地使用 `REF_TO`。

引用在生成的 C 中是原始指针。编译器检查目标类型，但不跟踪所有权、空值
或目标生命周期；只能在目标存储仍有效时使用，且不得解引用 `NULL`。完整边界见
[引用语义清单](standards/reference-semantics.md)。

## 6. 变量与声明块

### 6.1 常用声明块

| 声明块 | 用途 |
| --- | --- |
| `VAR_INPUT ... END_VAR` | POU 输入 |
| `VAR_OUTPUT ... END_VAR` | POU 输出 |
| `VAR_IN_OUT ... END_VAR` | 按引用传递的输入输出参数 |
| `VAR ... END_VAR` | 本地变量或实例 |
| `VAR_TEMP ... END_VAR` | 临时变量 |
| `VAR_EXTERNAL ... END_VAR` | 外部变量 |
| `VAR_GLOBAL ... END_VAR` | 配置或资源全局变量 |
| `VAR_CONFIG ... END_VAR` | 实例专用初始化 |

根据声明位置，支持 `CONSTANT`、`RETAIN` 和 `NON_RETAIN` 限定。例如：

```iecst
VAR CONSTANT
  MaxRetries : UINT := 3;
END_VAR

VAR RETAIN
  TotalCycles : UDINT := 0;
END_VAR
```

并非每个限定符都能用于每一种声明块；编译器会按 POU 和配置上下文检查组合是否合法。

### 6.2 直接表示变量

直接地址形式为 `%` + 区域 + 可选大小 + 数字地址：

```text
%IX0.0   输入位
%QX1.2   输出位
%MW10    内存字
%ID4     输入双字
%QL2     输出长字
```

区域为 `I`、`Q`、`M`，大小为可选的 `X`、`B`、`W`、`D`、`L`。支持多段数字地址。声明示例：

```iecst
VAR
  StartButton AT %IX0.0 : BOOL;
  MotorOn     AT %QX0.0 : BOOL;
  Setpoint    AT %MW10  : INT;
END_VAR
```

配置型未完整地址 `%I*`、`%Q*`、`%M*` 也有对应声明语法，用于稍后绑定具体位置。

### 6.3 变量访问

支持普通变量、数组下标、结构成员、多级组合以及引用解引用：

```iecst
Count
Samples[Index]
Motor.State
Plant.Motors[2].Speed
Ptr^
```

## 7. 程序组织单元（POU）

### 7.1 函数

函数使用 `FUNCTION name : return_type` 声明，返回值通过给函数名赋值产生：

```iecst
FUNCTION Clamp : INT
  VAR_INPUT
    Value : INT;
    Low   : INT;
    High  : INT;
  END_VAR

  IF Value < Low THEN
    Clamp := Low;
  ELSIF Value > High THEN
    Clamp := High;
  ELSE
    Clamp := Value;
  END_IF;
END_FUNCTION
```

非标准 `VOID` 返回类型以及把函数调用写成独立 ST 语句需要 `-b`。

### 7.2 功能块

功能块可保存实例状态：

```iecst
FUNCTION_BLOCK RisingCounter
  VAR_INPUT
    Enable : BOOL;
  END_VAR
  VAR_OUTPUT
    Count : UDINT;
  END_VAR

  IF Enable THEN
    Count := Count + 1;
  END_IF;
END_FUNCTION_BLOCK
```

实例及调用：

```iecst
VAR
  Counter : RisingCounter;
END_VAR

Counter(Enable := StartButton);
CurrentCount := Counter.Count;
```

### 7.3 程序

```iecst
PROGRAM Main
  VAR
    Counter : RisingCounter;
  END_VAR

  Counter(Enable := TRUE);
END_PROGRAM
```

函数和功能块通常隐式获得 `EN`/`ENO` 参数；`-e` 禁用它们的生成。默认要求 POU 至少包含一个输入、输出或输入输出参数的历史限制可用 `-i` 放宽。

### 7.4 实验性命名空间

命名空间只在 `iec61131-3:2025-experimental` 中启用：

```iecst
NAMESPACE Factory.Motion
TYPE Speed : INT; END_TYPE
END_NAMESPACE

USING Factory.Motion;

PROGRAM Main
  VAR Current : Speed; END_VAR
END_PROGRAM
```

支持点分限定名、`USING` 导入和 `INTERNAL` 可见性。当前命名空间优先，多个
导入同时命中会报歧义；`INTERNAL` 仅允许同一命名空间树访问。不支持别名、
重开命名空间、局部名字遮蔽和跨 include 文件拆分命名空间。生成代码会出现
`MATIECNS...` 形式的临时内部名，该拼写不是稳定 ABI。上述规则是项目为继续
实验而定义的行为，并非对 IEC 第四版原文的复述。完整边界见
[实验性命名空间语义](standards/namespace-semantics.md)。

### 7.5 实验性 Function Block 方法

实验 Profile 支持直接属于 `FUNCTION_BLOCK` 的公开方法：

```iecst
FUNCTION_BLOCK Counter
  VAR Count : INT; END_VAR
  Count := Count;

  METHOD PUBLIC Increment : INT
    VAR_INPUT Delta : INT; END_VAR
    Count := Count + Delta;
    Increment := Count;
  END_METHOD
END_FUNCTION_BLOCK

Result := CounterInstance.Increment(2);
```

方法可以访问所属 FB 字段；局部变量按普通函数的调用期生命周期处理。调用采用
静态派发，所属实例以引用方式传入，因此字段修改保留。当前不支持 `CLASS`、
`INTERFACE`、继承、覆盖、属性、动态派发、重载和非公开方法。内部
`MATIECMETHOD...` 名称不是稳定 ABI。详见
[实验性方法语义](standards/object-method-semantics.md)。

### 7.6 实验性标准函数 `ASSERT`

实验 Profile 支持把一个布尔条件写成独立断言语句：

```iecst
ASSERT(Speed <= MaximumSpeed);
```

当前实现会对条件求值，然后继续执行；无论结果真假都不产生外部通知。这是明确
记录的发布态 no-op 策略。调用必须独占一行，不能嵌入表达式，也不能取得返回值。
前端会为使用该调用的编译单元注入实验性 `VOID` 声明；生成的 C 名称不是源码 API
或稳定 ABI。legacy Profile 不做此转换，所以已有项目自定义的表达式函数
`ASSERT` 保持不变。详见
[实验性 ASSERT 语义](standards/experimental-assert-semantics.md)。

## 8. Structured Text（ST）

### 8.1 表达式与运算符

从低到高的主要优先级如下：

| 优先级 | 运算符 |
| --- | --- |
| 低 | `OR` |
| | `XOR` |
| | `AND`、`&` |
| | `=`、`<>` |
| | `<`、`>`、`<=`、`>=` |
| | `+`、`-` |
| | `*`、`/`、`MOD` |
| | `**` |
| 高 | 一元 `-`、`NOT`、括号、变量、常量、函数调用 |

赋值使用 `:=`：

```iecst
Result := (A + B) * 2;
Ready := Enabled AND NOT Faulted;
```

### 8.2 函数与功能块调用

函数支持位置参数和命名参数；功能块调用使用命名输入最清晰：

```iecst
Limited := Clamp(Value, 0, 100);
Limited := Clamp(Value := Value, Low := 0, High := 100);
Counter(Enable := StartButton);
```

形式参数 `:=` 传入值，`=>` 连接输出参数：

```iecst
Block(IN := Source, OUT => Destination);
```

还支持 `NOT output_name => variable` 的取反输出连接形式。

### 8.3 选择语句

```iecst
IF Temperature > 100.0 THEN
  Alarm := TRUE;
ELSIF Temperature < 0.0 THEN
  Heater := TRUE;
ELSE
  Alarm := FALSE;
  Heater := FALSE;
END_IF;

CASE State OF
  Idle:
    MotorOn := FALSE;
  Starting, Running:
    MotorOn := TRUE;
  Fault:
    Alarm := TRUE;
ELSE
  MotorOn := FALSE;
END_CASE;
```

`CASE` 选择项支持单值、逗号列表和整数子范围。

### 8.4 循环和控制

```iecst
FOR Index := 0 TO 9 BY 1 DO
  Sum := Sum + Samples[Index];
END_FOR;

WHILE Count < Limit DO
  Count := Count + 1;
END_WHILE;

REPEAT
  Count := Count - 1;
UNTIL Count = 0
END_REPEAT;
```

支持 `RETURN;`、`EXIT;` 和 `CONTINUE;`。`EXIT` 用于离开循环，`CONTINUE` 跳到下一次迭代。

## 9. Instruction List（IL）

IL POU 以一行一条指令书写：

```iecst
FUNCTION AddOne : INT
  VAR_INPUT
    Value : INT;
  END_VAR

  LD Value
  ADD 1
  ST AddOne
END_FUNCTION
```

当前解析器包含以下主要指令组：

| 指令组 | 指令 |
| --- | --- |
| 装载/存储 | `LD`、`LDN`、`ST`、`STN` |
| 逻辑 | `NOT`、`AND`/`&`、`ANDN`/`&N`、`OR`、`ORN`、`XOR`、`XORN` |
| 置位/复位 | `S`、`R`、`S1`、`R1` |
| 算术 | `ADD`、`SUB`、`MUL`、`DIV`、`MOD` |
| 比较 | `GT`、`GE`、`EQ`、`LT`、`LE`、`NE` |
| 计数/定时输入 | `CLK`、`CU`、`CD`、`PV`、`IN`、`PT` |
| 调用 | `CAL`、`CALC`、`CALCN` |
| 返回 | `RET`、`RETC`、`RETCN` |
| 跳转 | `JMP`、`JMPC`、`JMPCN` |

支持标签、括号化指令列表、函数调用的形式/非形式参数，以及 `=>` 输出连接。由于 IL 在 IEC 61131-3 后续版本中已被弃用，新项目通常更适合优先采用 ST；这里的支持主要用于兼容现有工程。

## 10. 文本 Sequential Function Chart（SFC）

SFC 可以作为 `PROGRAM` 或 `FUNCTION_BLOCK` 的正文。一个最小网络包含初始步、转换、后续步和动作：

```iecst
PROGRAM Sequence
  VAR
    Start : BOOL;
    Motor : BOOL;
  END_VAR

  INITIAL_STEP Idle:
  END_STEP

  TRANSITION FROM Idle TO Running
    := Start;
  END_TRANSITION

  STEP Running:
    Drive(N);
  END_STEP

  ACTION Drive:
    Motor := TRUE;
  END_ACTION
END_PROGRAM
```

转换支持单步或步列表作为源和目标，也支持命名和优先级形式。动作正文可以使用 ST、IL 或嵌套 SFC 的相应文本形式。

动作关联写为 `ActionName(Qualifier[, indicators...]);`。支持的限定符为：

| 类别 | 限定符 |
| --- | --- |
| 非定时 | `N`、`R`、`S`、`P`、`P0`、`P1` |
| 定时 | `L`、`D`、`SD`、`DS`、`SL`，后接时间值 |

定时示例：

```iecst
STEP Delayed:
  OpenValve(D, T#500ms);
END_STEP
```

## 11. 配置、资源与任务

完整的部署层次为 `CONFIGURATION` → `RESOURCE` → `TASK`/程序实例：

```iecst
PROGRAM MainProgram
  VAR
    CycleCount : UDINT;
  END_VAR
  CycleCount := CycleCount + 1;
END_PROGRAM

CONFIGURATION Controller
  VAR_GLOBAL
    EmergencyStop AT %IX0.0 : BOOL;
  END_VAR

  RESOURCE Cpu ON GenericTarget
    TASK FastTask(INTERVAL := T#10ms, PRIORITY := 1);
    PROGRAM MainInstance WITH FastTask : MainProgram;
  END_RESOURCE
END_CONFIGURATION
```

任务初始化支持：

```text
TASK name(PRIORITY := integer);
TASK name(SINGLE := data_source, PRIORITY := integer);
TASK name(INTERVAL := data_source, PRIORITY := integer);
TASK name(SINGLE := data_source, INTERVAL := data_source, PRIORITY := integer);
```

`PRIORITY` 必填，`SINGLE` 和 `INTERVAL` 可选但顺序固定。程序实例可以使用 `RETAIN`/`NON_RETAIN`、`WITH task`，并通过括号配置输入和输出连接。

配置末尾可使用 `VAR_CONFIG` 为具体程序或功能块实例设置初始化值。实验
Profile 还支持一个受限的配置级 `VAR_ACCESS`：

```iecst
VAR_ACCESS
  RemoteSetpoint : Setpoint : INT READ_WRITE;
  MonitorSetpoint : Setpoint : INT;
END_VAR
```

这里的 `Setpoint` 必须是在同一 `CONFIGURATION` 中声明的简单 `VAR_GLOBAL`
名称，类型必须一致；省略方向时默认为 `READ_ONLY`，常量不能声明为
`READ_WRITE`。成功生成后会在 `-T` 目录写入 `ACCESS.csv`。资源、程序、功能块
层级路径、直接地址及结构/数组元素仍未实现。完整边界见
[`access-variable-semantics.md`](standards/access-variable-semantics.md)。

## 12. 会改变语法的命令行开关

| 开关 | 作用 | 性质 |
| --- | --- | --- |
| `-p` | 允许前向引用，启用预解析流程 | 历史扩展 |
| `-l` | 使用宽松的数据类型等价模型 | 非标准扩展 |
| `-s` | 启用 `SAFE*` 类型 | PLCopen Safety |
| `-n` | 允许嵌套注释 | IEC 第 3 版能力 |
| `-r` | 启用 `REF_TO`、`REF`、`DREF`、`^`、`NULL` | IEC 第 3 版能力 |
| `-R` | 包含 `-r`，另启用 `REF_TO ANY` 及派生类型内引用 | 非标准扩展 |
| `-a` | 数组边界允许符号名 | 非标准扩展 |
| `-i` | 允许没有输入/输出/输入输出参数的 POU | 非标准扩展 |
| `-b` | 允许 `VOID` 函数及独立的 ST 函数调用语句 | 非标准扩展 |
| `-e` | 不生成隐式 `EN`/`ENO` 参数 | 生成策略 |
| `-c` | 为枚举类型生成转换函数 | 生成策略 |

其他常用开关：

| 开关 | 作用 |
| --- | --- |
| `-y` | 仅执行语法分析，不生成输出 |
| `-f` | 错误信息显示完整记号位置 |
| `-I dir` | 设置 include 和库搜索目录 |
| `-T dir` | 设置生成文件目录 |
| `-O options` | 传递代码生成阶段选项 |

请用 `./iec2c -h` 查看当前构建的完整选项说明。

## 13. 常见边界与注意事项

- 本项目直接解析的是文本源，不是 FBD、LD 编辑器文件；XML 或图形工程需由上游工具转换。
- 前端以 IEC 61131-3 第 2 版草案为基础，并非第 3 版全部语法的实现。
- `VAR_ACCESS` 仅在实验 Profile 下支持同配置的简单全局变量路径，不支持完整层级路径。
- 命名空间只在实验 Profile 下部分实现，使用 MATIEC 临时语义而非已验证的第四版完整规则。
- 面向对象能力当前只实现公开 FB 方法的静态派发，不应理解为完整类/接口模型。
- `REF_TO` 是选择性扩展；默认命令行下不能直接使用。
- 未知 pragma 虽可能通过解析，但不表示代码生成器会赋予它特定语义。
- 语法通过不等于目标系统行为已经验证。生成的 C 仍需与目标 PLC 运行时、I/O 映射和调度环境集成测试。
- 编译器支持同一进程中的顺序编译，但当前生成式前端仍有全局兼容状态，不支持同一进程内并行调用。

## 14. 排查语法错误

先使用只检查模式和完整位置：

```sh
./iec2c -y -f -I lib source.st
```

排查顺序建议如下：

1. 检查声明和 ST 语句末尾的分号；
2. 检查 `END_IF`、`END_CASE`、`END_VAR`、`END_PROGRAM` 等结束关键字；
3. 确认名字在使用前已经声明，必要时评估 `-p`；
4. 确认 SAFE、引用、嵌套注释等语法已使用对应开关；
5. 确认 `-I lib` 以及自定义 include 路径正确；
6. 语法通过后，再区分类型检查、初始化检查和 C 工具链错误。

## 15. 实现依据

本手册根据以下仓库内容整理：

- 词法规则：[`stage1_2/iec_flex.ll`](../stage1_2/iec_flex.ll)
- 语法规则：[`stage1_2/iec_bison.yy`](../stage1_2/iec_bison.yy)
- 标准函数和功能块库：[`lib`](../lib/)
- 语法回归：[`tests/syntax`](../tests/syntax/)
- 初始化回归：[`tests/initialization`](../tests/initialization/)

若实现发生变化，应同步更新本手册，并为新增或收紧的语法补充回归测试。
