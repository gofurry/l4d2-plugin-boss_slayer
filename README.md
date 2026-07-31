# Boss Slayer Roguelite

[![License: MIT](https://img.shields.io/github/license/gofurry/l4d2-plugin-boss_slayer)](LICENSE)
![SourcePawn](https://img.shields.io/badge/SourcePawn-SourceMod-orange)

一个面向《求生之路 2》合作模式的 SourceMod 肉鸽成长插件。玩家通过击杀或参与击杀 Boss、击杀特殊感染者与普通感染者，以及推进章节获得局内升级选择；构筑按照 SteamID 保存，可跨章节延续，并在死亡或战役结算时重置。

> 项目目前处于 v0.4.2 开发阶段。奖励、存储和三选一菜单已经实现；技能的实际伤害、减伤和治疗效果计划在 v0.5 接入。

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
- 构筑以 SteamID 为键保存在服务器内存中，章节切换和短线重连不会串号。
- 玩家真正死亡时清除个人构筑；团灭或完成战役时清除全员构筑。
- 不使用数据库，插件重载或服务器关闭后数据自然消失。

## 环境要求

- Left 4 Dead 2
- Metamod:Source
- SourceMod 1.12 或更新的兼容版本
- Windows PowerShell（仅开发脚本需要）

当前版本只使用 SourceMod 标准 API，不需要 Left 4 DHooks 等第三方扩展。

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
├─ perks.inc                      技能目录与随机池
├─ boss_tracking.inc              Tank/Witch 参与判定
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

游戏内检查项目见 [v0.4.2 回归测试](docs/test-checklist.md)。Codex 项目约束见 [AGENTS.md](AGENTS.md)。

## 路线图

- v0.5：火力强化和巨兽猎手实际生效。
- v0.6：坚韧和绝境求生。
- v0.7：特感猎手和 Boss 击杀治疗。
- v0.8：Bot 接管与多人长期测试。
- v0.9：ConVar 配置与平衡调整。

## 许可证

本项目采用 [MIT License](LICENSE)。
