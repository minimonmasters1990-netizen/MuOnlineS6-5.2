## 问题分析

通过分析代码，我发现了导致X轴不居中的原因：

### 1. X轴位置计算错误

在 `UIMapName.cpp` 文件中，地图名称图标的X轴位置计算如下：

```cpp
#define	UIMN_IMG_POS_X		((::GetScreenWidth() * gPosWide.x_fScreenRate_x - UIMN_IMG_WIDTH) / 2.0f) + gPosWide.x_GetAddPos + 65
```

* 这里包含了硬编码的 `+ 65` 偏移量，导致图标偏右

* 还添加了 `gPosWide.x_GetAddPos` 额外偏移量

* 这些额外偏移量破坏了原本的居中计算

### 2. 居中公式被修改

* 原本的居中公式是 `(屏幕宽度 - 图标宽度) / 2`

* 但额外添加的偏移量导致最终位置偏离中心

## 修复方案

### 方案：修正X轴位置计算

* 移除硬编码的偏移量 `+ 65`

* 移除 `gPosWide.x_GetAddPos` 额外偏移量（如果不需要）

* 保持Y轴位置不变

## 实施步骤

1. **分析当前代码**：确认 `UIMapName.cpp` 中的X轴位置计算逻辑
2. **修改X轴位置计算**：移除额外偏移量，使用正确的居中公式
3. **保持Y轴不变**：确保Y轴位置计算和渲染不变
4. **测试验证**：验证修复后的地图名称图标是否在X轴方向居中显示

## 预期效果

* 所有地图名称图标都能在X轴方向居中显示

* Y轴位置保持不变

* 不再有额外偏移量导致位置偏移

## 具体修改

需要修改 `UIMapName.cpp` 文件中的X轴位置计算：

```cpp
// 原代码
#define	UIMN_IMG_POS_X		((::GetScreenWidth() * gPosWide.x_fScreenRate_x - UIMN_IMG_WIDTH) / 2.0f) + gPosWide.x_GetAddPos + 65

// 修改后
#define	UIMN_IMG_POS_X		((::GetScreenWidth() * gPosWide.x_fScreenRate_x - UIMN_IMG_WIDTH) / 2.0f)
```

这样修改后，地图名称
