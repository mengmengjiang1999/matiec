# IEC 61131-3 演进与实验性 2025 Profile 决策记录

## 决定

MATIEC 将继续向 IEC 61131-3:2025 的现代语言能力演进，但在没有取得完整规范正文的情况下，不宣称完整、认证或严格符合第四版。

项目采用两个清晰边界：

- `legacy`：保存当前以第二版最终草案为基础的 MATIEC 行为；
- `iec61131-3:2025-experimental`：逐项接收有公开证据并经过测试的现代能力。

第四版实验模式不是一次性切换。每项新增能力必须说明证据、兼容行为、实现层次和测试，未经确认的标准内容不得靠猜测加入。

## 为什么采用实验模式

IEC 官方页面确认 IEC 61131-3:2025 是第 4.0 版，发布日期为 2025-05-22，并公开说明新增 UTF-8 字符串及相关函数；相对 2013 版的完整新增、删除和弃用清单位于受版权保护的 Annex B。

- IEC 官方信息：<https://webstore.iec.ch/en/publication/68533>
- 中国国际标准目录：<https://std.samr.gov.cn/gj/search/gjDetailed?id=7FF9EAB193AE800FFAB01EE427EE9229>

中国等同采用 IEC 61131-3:2025 的修订计划 `20262351-T-604` 已于 2026-04-28 下达，目前正在起草。未来公开征求意见材料可以作为新的公开证据，但在正式材料出现前不预设其内容。

- 国家标准修订计划：<https://std.samr.gov.cn/gb/search/gbDetailed?id=511EEC13B7D09DF3E06397BE0A0AA089>

本项目属于实验性开源项目，不购买标准，不使用来源不明的标准副本，也不把厂商方言自动视为 IEC 规范。

## 证据等级

| 等级 | 含义 | 可以得出的结论 |
| --- | --- | --- |
| `implementation-verified` | 已由本仓库源码和测试证明确实实现 | 只能说明 MATIEC 行为，不能说明属于某版标准 |
| `official-public` | IEC、国家标准机构等公开页面明确说明 | 可以描述公开确认的标准事实 |
| `cross-vendor-provisional` | 多个独立厂商或实现表现一致 | 可以作为实验行为，不能称为标准要求 |
| `unverified` | 缺少足够公开依据 | 只能记录问题，不据此冻结语法 |

一个特性可以同时具有实现证据和标准证据。例如“MATIEC 支持 IL”属于 `implementation-verified`；“IL 在第四版 Annex B 中的精确分类”当前仍为 `unverified`。

## 当前实现能力基线

详细写法见[中文语法用户手册](../user-manual.zh-CN.md)。下表只记录能力边界和后续演进状态。
可由测试校验的 ID、实现状态、证据分类、启用方式和测试路径保存在
`compiler/language_feature_catalog.cc`；本表是面向读者的摘要。

| ID | 能力 | 当前状态 | 启用方式 | 实现证据 | 2025 状态 |
| --- | --- | --- | --- | --- | --- |
| `LEX-CASE-ASCII` | ASCII 关键字和标识符不区分大小写 | 已实现 | 默认 | `iec_flex.ll`、identifier 回归 | 未核对 |
| `LEX-COMMENT` | `(* ... *)` 注释 | 已实现 | 默认 | lexer、语法测试 | 未核对 |
| `LEX-NESTED-COMMENT` | 嵌套块注释 | 已实现 | `-n` | lexer、CLI 选项 | 已知为后期能力，精确 2025 规则未核对 |
| `LEX-INCLUDE-PRAGMA` | `{#include "..."}` | 已实现 | 默认 | lexer、库加载 | MATIEC 扩展 |
| `TYPE-ELEMENTARY-V2` | BOOL、整数、位串、浮点、旧字符串、日期时间 | 已实现 | 默认 | grammar、标准库、生成 C 测试 | 精确变更未核对 |
| `TYPE-DERIVED` | 别名、子范围、枚举、数组、结构、定长字符串 | 已实现 | 默认 | grammar、initialization 回归 | 精确变更未核对 |
| `TYPE-SAFE` | PLCopen `SAFE*` 类型 | 已实现 | `-s` | grammar、CLI 选项 | PLCopen 扩展，不归因于 IEC 2025 |
| `TYPE-REFERENCE` | `REF_TO`、`REF`、`DREF`、`^`、`NULL` | 部分实现，含实验性空初始化 | `-r`；声明初始化另需实验 Profile | grammar、类型诊断、生成 C 回归 | 生命周期与 2025 精确规则未核对 |
| `TYPE-REFERENCE-EXT` | `REF_TO ANY`、派生类型内宽松引用 | 已实现 | `-R` | grammar、正反向 CLI 回归 | MATIEC 非标准扩展 |
| `POU-FUNCTION` | FUNCTION | 已实现 | 默认 | grammar、ST/IL 回归 | 精确变更未核对 |
| `POU-FB` | FUNCTION_BLOCK 与实例 | 已实现 | 默认 | grammar、initialization 回归 | 精确变更未核对 |
| `POU-PROGRAM` | PROGRAM | 已实现 | 默认 | grammar、生成 C 回归 | 精确变更未核对 |
| `LANG-ST` | Structured Text | 已实现主要第二版语法 | 默认 | grammar、syntax 和 generated-C 回归 | 官方公开为第四版文本语言；差异未核对 |
| `LANG-IL` | Instruction List | 已实现 | 默认 | grammar、characterization 回归 | 官方摘要未列入第四版语言集合；Annex B 精确分类未核对 |
| `LANG-SFC-TEXT` | 文本 SFC | 已实现 | 默认 | grammar、SFC 回归 | 官方公开保留 SFC 元素；细节未核对 |
| `LANG-LD-GRAPH` | 图形 LD 输入 | 未实现 | — | README、grammar 边界 | 官方公开为第四版图形语言 |
| `LANG-FBD-GRAPH` | 图形 FBD 输入 | 未实现 | — | README、grammar 边界 | 官方公开为第四版图形语言 |
| `CONFIG-RESOURCE-TASK` | CONFIGURATION、RESOURCE、TASK、程序实例 | 已实现 | 默认 | grammar、configuration 回归 | 官方公开保留配置元素；细节未核对 |
| `CONFIG-VAR-ACCESS` | VAR_ACCESS | 未实现 | — | grammar 中规则被注释 | 未核对 |
| `ED3-NAMESPACE` | 命名空间和限定名 | 部分实现 | `iec61131-3:2025-experimental` | namespace normalizer、单元与 CLI/生成 C 回归 | `cross-vendor-provisional`；MATIEC 临时规则，2025 规范细节未核对 |
| `ED3-OO` | CLASS、INTERFACE、METHOD、继承 | 未实现 | — | 无对应 AST/grammar | 第三版公开摘要确认；2025 细节未核对 |
| `ED4-UTF8` | UTF-8 源码与 `STRING` 字面量 | 部分实现 | `iec61131-3:2025-experimental` | 编码验证、lexer、生成 C 与运行回归 | `official-public`；函数签名等细节待核对 |

## Annex B 未决问题

IEC 公开页面说明 Annex B 包含与 2013 版相比的重要技术变更，但没有公开
完整条目。因此下列问题只记录为 `unverified`，不分配编译器行为：

- IL 在 2025 版的精确分类、兼容要求和迁移措施；
- 2013 至 2025 之间除 UTF-8 外的全部新增、删除和弃用项；
- 引用、命名空间、面向对象元素、`VAR_ACCESS` 及标准库的精确差异。

在获得可授权查阅的规范、公开的国家标准草案，或其他足够权威的
公开证据前，实验 Profile 不根据这些问题猜测语法、诊断或删除策略。

## 声明规则

项目文档可以使用这些表述：

> MATIEC implements a legacy IEC 61131-3 language based on the second-edition final draft.

> The experimental profile evolves toward publicly documented IEC 61131-3:2025 capabilities.

> UTF-8 strings are a publicly confirmed IEC 61131-3:2025 addition; MATIEC's experimental profile currently implements validated UTF-8 source and byte-encoded `STRING` literals, while normative function details remain unresolved.

> Namespace parsing in the experimental profile follows a documented provisional MATIEC contract; it is not presented as normative IEC 61131-3:2025 behavior.

项目文档不得在缺少完整证据时使用这些表述：

> Fully IEC 61131-3:2025 compliant.

> Implements all Edition 4 syntax.

> Certified IEC 61131-3 compiler.

## 后续工作顺序

1. 增加 `legacy` 和 `iec61131-3:2025-experimental` Profile 基础设施，初始不改变语言行为；
2. 将能力 ID、状态、证据和测试引用变成可校验的数据；
3. 以 UTF-8 源码和字符串内部模型作为第一个第四版公开确认特性；
4. 分别完成引用、命名空间、面向对象语法、`VAR_ACCESS` 和标准库差距；
5. 使用公开的中国国标征求意见材料校正实验行为；
6. 只有在能够逐条建立可信符合性矩阵后，才评估移除 `experimental` 标签。

每一类语言能力使用独立 OpenSpec 变更，必须同时包含：

- 来源和证据等级；
- 正向及反向语法测试；
- 语义和类型测试；
- C 生成及运行测试；
- legacy Profile 兼容测试；
- 用户手册和本矩阵更新。

## 版权边界

仓库可以保存：

- IEC 和国家标准机构的公开链接；
- 自己撰写的摘要、设计、测试和实现说明；
- 经授权查阅者提供的条款编号和独立实现结论；
- 不复制标准表达的互操作性测试。

仓库不得保存：

- IEC 标准 PDF；
- 大段逐字摘录；
- 复制的规范表格或完整形式语法；
- 试图重建付费标准全文的文档；
- 来源不明的标准副本。

厂商文档和其他编译器只能作为 `cross-vendor-provisional` 证据。它们可以帮助形成实验实现，但不能单独证明 IEC 61131-3:2025 的规范要求。
