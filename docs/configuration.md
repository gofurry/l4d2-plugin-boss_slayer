# 服务器配置说明

插件首次加载后会在服务器生成：

```text
left4dead2/cfg/sourcemod/boss_slayer.cfg
```

下表中的项目都可以由服务器管理员修改。插件会在运行时读取这些 ConVar，通常不需要重新编译；修改配置后建议换图或执行配置文件，确保服务器使用新值。

## 管理、消息与奖励

| ConVar | 默认值 | 允许范围 | 作用 |
|---|---:|---:|---|
| `sm_bsr_chat_level` | `1` | 0–2 | 0 最简、1 正常、2 详细 |
| `sm_bsr_debug_commands` | `0` | 0–1 | 是否允许具有 `n` 权限的管理员使用开发调试命令 |
| `sm_bsr_boss_killer_rewards` | `2` | 0–10 | 存活真人 Boss 击杀者获得的选择次数 |
| `sm_bsr_boss_participant_rewards` | `1` | 0–10 | 其他存活真人参与者获得的选择次数 |
| `sm_bsr_si_kills_per_reward` | `30` | 1–10000 | 获得奖励所需普通特感击杀数 |
| `sm_bsr_si_kill_rewards` | `1` | 0–10 | 每次达到特感门槛获得的选择次数 |
| `sm_bsr_common_kills_per_reward` | `500` | 1–100000 | 获得奖励所需普通感染者击杀数 |
| `sm_bsr_common_kill_rewards` | `1` | 0–10 | 每次达到普通感染者门槛获得的选择次数 |
| `sm_bsr_chapter_rewards` | `1` | 0–10 | 第一章之后每章入口奖励次数 |

`sm_bsr_debug_commands` 默认关闭。它不会赋予任何权限；只有已经拥有 `ADMFLAG_CHEATS`（标志 `n`）或通过 SourceMod override 获得对应命令访问权的管理员，才能在开关设为 `1` 后使用调试命令。

从旧版本升级且配置文件没有该项时，请手动添加：

```cfg
sm_bsr_debug_commands "0"
```

## 能力数值

小数使用比例表示，例如 `0.10` 等于 10%。

| ConVar | 默认值 | 允许范围 | 作用 |
|---|---:|---:|---|
| `sm_bsr_firepower_per_level` | `0.10` | 0–1.00 | 每级火力强化直接伤害 |
| `sm_bsr_toughness_per_level` | `0.06` | 0–0.20 | 每级坚韧战斗减伤 |
| `sm_bsr_boss_heal_real_per_level` | `10` | 0–100 | 每级猎杀疗愈真实生命 |
| `sm_bsr_boss_heal_temp_per_level` | `10` | 0–100 | 每级猎杀疗愈临时生命 |
| `sm_bsr_melee_speed_per_level` | `0.08` | 0–1.00 | 每级近战狂热攻速 |
| `sm_bsr_healing_boost_per_level` | `0.06` | 0–1.00 | 每级治疗强化治疗量 |
| `sm_bsr_field_rescue_per_level` | `0.08` | 0–1.00 | 每级战地救援速度 |
| `sm_bsr_ammo_reclamation_per_level` | `0.02` | 0–1.00 | 每级弹药回收备弹比例 |
| `sm_bsr_first_aid_feedback_per_level` | `5` | 0–100 | 每级急救反馈临时生命 |
| `sm_bsr_precision_hunter_rounds_per_level` | `1` | 0–30 | 每级精准猎杀弹匣返弹数 |
| `sm_bsr_supply_training_bonus_per_stack` | `0.02` | 0–0.20 | 每层补给强化的五项属性加成 |
| `sm_bsr_supply_training_max_stacks` | `5` | 1–20 | 补给强化最大层数 |
| `sm_bsr_low_health_threshold` | `40.0` | 1–100 | 求生律动有效生命阈值 |
| `sm_bsr_low_health_interval` | `30.0` | 1–600 | 求生律动触发间隔（秒） |
| `sm_bsr_low_health_temp_per_level` | `10` | 0–100 | 每级求生律动临时生命 |

## 补给与满级兑换

| ConVar | 默认值 | 允许范围 | 作用 |
|---|---:|---:|---|
| `sm_bsr_supply_interval` | `180.0` | 1–3600 | 空物品栏持续多久后获得补给（秒） |
| `sm_bsr_exchange_real_health` | `20` | 0–100 | 满级生命兑换的真实生命 |
| `sm_bsr_exchange_temp_health` | `20` | 0–100 | 满级生命兑换的临时生命 |

## 当前由代码固定的规则

以下内容不是配置项，修改它们需要改源码并重新编译：

- 十四项能力的种类、编号与最高等级。
- 每次随机展示三个不同且未满级的能力。
- 所有能力满级后改为兑换菜单，以及兑换物品池。
- 精准猎杀支持的主武器类别、榴弹发射器排除规则与各武器弹匣上限。
- 火力强化和坚韧具体识别哪些伤害类型。
- 医疗补给与投掷物补给的随机物品池。
- 构筑仅保存在服务器内存中，不写入数据库或磁盘。
- 只有正常 `map_transition` 延续构筑；直接开图或直接换图视为新战役第一章。
- 个人死亡保留，关卡失败回滚章节快照，战役完成清空。
- 仅真人幸存者获得进度与能力；Bot 不继承、不累计。
- 仅标准合作战役 `coop` 与写实战役 `realism` 启用；其他游戏模式全部禁用。
- 玩家命令、`n` 权限调试命令和 `z` 权限全员重置命令的基础边界；单个命令仍可由 SourceMod override 调整。

`sm_bsr_version` 是只读版本信息，不会写入配置文件。
