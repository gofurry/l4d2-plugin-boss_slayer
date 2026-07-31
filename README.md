# Boss Slayer Roguelite

[![License: MIT](https://img.shields.io/github/license/gofurry/l4d2-plugin-boss_slayer)](LICENSE)
![SourcePawn](https://img.shields.io/badge/SourcePawn-SourceMod-orange)

一个面向《求生之路 2》合作模式的 SourceMod 肉鸽成长插件。玩家通过击杀或参与击杀 Boss、击杀特殊感染者与普通感染者，以及推进章节获得局内升级选择；构筑按照 SteamID 保存，可跨章节和个人死亡延续，团灭后回滚到当前章节开始时的状态。

> 项目目前处于 v0.6.1 开发阶段。奖励、章节快照、三选一与重掷菜单、十四项能力效果和满级兑换已经实现，仍需要多人长期测试和平衡调整。

## 当前功能

| 行为 | 奖励 |
|---|---:|
| 最后击杀 Tank/Witch | 2 次升级选择 |
| 存活并参与击杀 Tank/Witch | 1 次升级选择 |
| 累计击杀 30 只普通特殊感染者 | 1 次升级选择 |
| 累计击杀 500 只普通感染者 | 1 次升级选择 |
| 进入第一章之后的新章节 | 1 次升级选择 |

- 每次奖励随机显示最多三个未满级技能。
- 关闭菜单不会失去未使用的奖励，输入 `!perk` 可以重新打开。
- 能力选择和满级兑换菜单使用数字键 `9` 稍后选择并关闭。
- 所有技能满级后，多余奖励可以兑换 20+20 生命、满备弹、随机医疗物资或随机投掷物。
- 构筑以 SteamID 为键保存在服务器内存中，章节切换和短线重连不会串号。
- 玩家个人死亡不会丢失构筑；团灭或关卡失败时回滚到进入本章时的状态。
- 完成战役时清除全员构筑。
- 不使用数据库，插件重载或服务器关闭后数据自然消失。

## 环境要求

- Left 4 Dead 2
- Metamod:Source
- SourceMod 1.12 或更新的兼容版本
- Windows PowerShell（仅开发脚本需要）

当前版本只使用 SourceMod 标准 API，不需要 Left 4 DHooks 等第三方扩展。

## 能力池

| 能力 | 最高等级 | 每级效果 | 满级效果 |
|---|---:|---:|---:|
| 火力强化 | 5 | 直接伤害 +10% | +50% |
| 坚韧 | 4 | 战斗、火焰、酸液和友伤 -6% | -24% |
| 猎杀疗愈 | 3 | 击杀 Boss 恢复 10 真实＋10 临时生命 | 30＋30 |
| 近战狂热 | 4 | 近战攻击速度 +8% | +32% |
| 治疗强化 | 4 | 医疗包、止痛药和肾上腺素治疗量 +6% | +24% |
| 医疗补给 | 1 | 连续 180 秒没有任何医疗物资时随机获得一个 | 固定效果 |
| 投掷物补给 | 1 | 连续 180 秒没有投掷物时随机获得一个 | 固定效果 |
| 命运重掷 | 1 | 每次奖励可以免费重抽一次选项 | 固定效果 |
| 战地救援 | 4 | 救起倒地和挂边队友速度 +8% | +32% |
| 弹药回收 | 3 | 击杀普通特感恢复主武器最大备弹 2% | 6%/次 |
| 急救反馈 | 3 | 给队友使用医疗包后自身获得 5 临时生命 | 15 临时生命 |
| 精准猎杀 | 3 | 使用狙击枪击杀普通特感时向弹匣返还 1 发 | 3 发/次 |
| 补给强化 | 1 | 每章首次使用补给品叠加五项属性各 2% | 五项各 +10% |
| 求生律动 | 2 | 有效生命低于 40 时每 30 秒获得 10 临时生命 | 20/次 |

近战狂热现在每级提高 8%，满级为 32%。火力强化不会提高友军伤害。坚韧不减免坠落、溺水、地图处决和挂边掉落等非战斗伤害。生命恢复受正常最大有效生命限制。

补给强化只在每个章节第一次成功使用医疗包、止痛药、肾上腺素或投掷物时增加一层，最多五层。每层同时提供 2% 直接伤害、减伤、救援速度、治疗量和近战攻速。

## 安装

自行编译后，将：

```text
boss_slayer.smx
```

复制到服务器：

```text
left4dead2/addons/sourcemod/plugins/boss_slayer.smx
```

服务器已经运行时执行：

```text
sm plugins reload boss_slayer
sm plugins info boss_slayer
```

## 玩家命令

| 命令 | 说明 |
|---|---|
| `!bsr` | 查看击杀、待选奖励和技能等级状态 |
| `!perk` | 打开一次尚未使用的升级选择 |
| `!perks` | 查看当前构筑 |

开发调试命令：

| 命令 | 说明 |
|---|---|
| `!bsr_testreward` | 增加一次测试奖励 |
| `!bsr_testsikills <数量>` | 模拟特殊感染者击杀 |
| `!bsr_testcommonkills <数量>` | 模拟普通感染者击杀 |
| `!bsr_testperk <0-13> <等级>` | 设置指定能力等级 |
| `!bsr_testsupplystacks <0-5>` | 设置补给强化层数 |
| `!bsr_testmaxperks` | 点满全部能力并增加一次兑换奖励 |
| `!bsr_resetme` | 清除自己的构筑 |
| `sm_bsr_resetall` | ROOT 管理员清除全员构筑 |

调试命令会在正式发布前移除或限制权限。

## 从源码构建

克隆仓库：

```powershell
git clone https://github.com/gofurry/l4d2-plugin-boss_slayer.git
cd l4d2-plugin-boss_slayer
```

复制本机配置模板：

```powershell
Copy-Item .\scripts\config.example.ps1 .\scripts\config.local.ps1
```

编辑 `config.local.ps1`，将 `$GameRoot` 指向本机的 `left4dead2` 目录，然后执行：

```powershell
.\scripts\build.ps1
```

产物位于：

```text
dist/boss_slayer.smx
```

编译并复制到本机游戏目录：

```powershell
.\scripts\deploy.ps1
```

`config.local.ps1`、编译产物和本机 VS Code 路径配置均已被 Git 忽略。

## VS Code

推荐安装 [SourcePawn Studio](https://marketplace.visualstudio.com/items?itemName=Sarrus.sourcepawn-vscode)。

1. 复制 `.vscode/settings.example.json` 为 `.vscode/settings.json`。
2. 填写本机 `spcomp.exe`、SourceMod include 和项目 include 路径。
3. 用 VS Code 打开整个仓库目录。
4. 执行 `SM: Doctor` 检查环境。

快捷任务：

- `Ctrl+Shift+B`：只编译。
- `Tasks: Run Task` → `Boss Slayer: Build and Deploy`：编译并部署。

## 源码结构

```text
src/boss_slayer.sp                 插件入口
include/boss_slayer/
├─ definitions.inc                版本、常量、技能枚举
├─ state.inc                      全局运行状态
├─ storage.inc                    SteamID 与玩家数据
├─ chapter_snapshot.inc           章节初始状态与失败回滚
├─ perks.inc                      技能目录与随机池
├─ boss_tracking.inc              Tank/Witch 参与判定
├─ effects.inc                    伤害、治疗和近战攻速
├─ supplies.inc                   周期补给与备弹恢复
├─ rescue.inc                     救援速度状态
├─ exchange_menu.inc              全技能满级后的兑换菜单
├─ perk_menu.inc                  三选一菜单
├─ rewards.inc                    Boss、特感、小怪、章节奖励
├─ events.inc                     游戏事件
├─ commands.inc                   玩家与调试命令
└─ lifecycle.inc                  地图和客户端生命周期
```

所有模块最终编译成一个 `boss_slayer.smx`，不需要管理多个插件的加载顺序。

## 开发与测试

修改 SourcePawn 后必须运行：

```powershell
.\scripts\build.ps1
```

游戏内检查项目见 [v0.6.1 回归测试](docs/test-checklist.md)。Codex 项目约束见 [AGENTS.md](AGENTS.md)。

## 路线图

- v0.7：Bot 接管、闲置恢复和多人长期测试。
- v0.8：ConVar 配置与平衡调整。
- v0.9：调试命令权限收紧与发布准备。

## 许可证

本项目采用 [MIT License](LICENSE)。
