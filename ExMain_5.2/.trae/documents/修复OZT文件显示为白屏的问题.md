# 问题分析

通过分析ACuoi_DanhHieu.cpp的代码，我发现了OZT文件显示为白屏的几个可能原因：

1. **文件路径分隔符问题**：
   - ACuoi_DanhHieu.cpp使用`/`作为路径分隔符：`"Custom/ThangCuoi/RankTitle/DanhHieu/%d.tga"`
   - 在Windows系统中，应该使用`\\`作为路径分隔符

2. **OZT文件加载逻辑问题**：
   - LoadOZTWithFrames函数假设OZT文件格式与TGA相同
   - 可能存在文件路径解析问题导致无法找到OZT文件

3. **渲染参数问题**：
   - 渲染时使用的宽度和高度为150x40，可能与实际图片尺寸不匹配

# 修复方案

## 1. 修改文件路径分隔符
将ACuoi_DanhHieu.cpp中的路径分隔符从 `/` 改为 `\\`，以符合Windows系统的要求。

## 2. 调整渲染参数
在RenderWithAnimation和Render函数中，修改渲染参数：
- 将渲染宽度和高度从150x40调整为80x50（与其他头衔一致）
- 调整渲染位置，确保显示效果正确

## 3. 改进LoadOZTWithFrames函数（位于GlobalBitmap.cpp）
修改LoadOZTWithFrames函数，确保正确解析OZT文件，并添加错误处理。

## 4. 添加错误处理
在文件加载失败时添加日志输出，确保即使OZT文件加载失败也能正常显示默认图片。

# 具体修改步骤

## 步骤1：修改ACuoi_DanhHieu.cpp中的LoadImages函数
- 将路径格式从 `"Custom/ThangCuoi/RankTitle/DanhHieu/%d.tga"` 改为 `"Custom\\ThangCuoi\\RankTitle\\DanhHieu\\%d.tga"`

## 步骤2：修改渲染参数
在RenderWithAnimation和Render函数中修改以下代码：
- 第91-96行：将 `RenderBitmap` 调用中的宽度和高度从150x40改为80x50
- 第117行：将 `RenderBitmap` 调用中的宽度和高度从150x40改为80x50
- 第318行：将 `RenderBitmap` 调用中的宽度和高度从150x40改为80x50
- 第337行：将 `RenderBitmap` 调用中的宽度和高度从150x40改为80x50
- 相应调整渲染位置，确保显示效果正确

## 步骤3：改进LoadOZTWithFrames函数（GlobalBitmap.cpp:914-973）
1. **增强文件路径解析能力**：
   - 改进文件路径处理，确保在Windows系统上正确处理路径分隔符
   - 确保OZT文件路径生成正确

2. **添加文件存在性检查**：
   - 在尝试打开文件前，先检查文件是否存在
   - 添加更详细的错误处理

3. **改进OZT文件格式解析**：
   - 确保正确解析OZT文件头部信息
   - 处理不同格式的OZT文件

## 步骤4：添加错误处理
- 在文件加载失败时添加日志输出
- 确保即使OZT文件加载失败也能正常显示默认图片

# 预期结果
修复后，OZT文件应该能够正常显示，不再出现白屏问题，并且显示效果与其他头衔一致。