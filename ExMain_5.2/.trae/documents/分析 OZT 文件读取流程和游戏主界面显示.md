# 实现基于现有OZT命名的分帧动态显示功能

## 1. 设计方案

### 1.1 核心思路
- 使用现有的OZT文件命名方式（不修改文件名）
- 创建新的独立函数加载和处理分帧OZT
- 支持4行5列（20帧）的TGA/OZT图片
- 集成到现有的DanhHieu头衔系统

### 1.2 技术要点
- 纹理坐标计算（4行5列网格）
- 动画帧切换逻辑
- 独立的OZT加载函数
- 与现有系统的无缝集成

## 2. 实现步骤

### 2.1 创建新的独立OZT加载函数

在`GlobalBitmap.cpp`中添加新函数：

```cpp
bool LoadOZTWithFrames(const std::string& filename, GLuint textureId, int rows, int cols)
{
    std::string filename_ozt;
    ExchangeExt(filename, "OZT", filename_ozt);

    FILE *fp = fopen(filename_ozt.c_str(), "rb");
    if(fp == NULL)
    {
        // 如果OZT不存在，尝试打开原始TGA
        fp = fopen(filename.c_str(), "rb");
        if(fp == NULL)
            return false;
    }

    fseek(fp, 0, SEEK_END);
    int Size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    auto* PakBuffer = new unsigned char[Size];
    fread(PakBuffer, 1, Size, fp);
    fclose(fp);

    int index = 12;
    index += 4;
    short nx = *((short*)(PakBuffer + index)); index += 2;
    short ny = *((short*)(PakBuffer + index)); index += 2;
    char bit = *((char*)(PakBuffer + index)); index += 1;
    index += 1;

    if(bit != 32 || nx > MAX_WIDTH || ny > MAX_HEIGHT)
    {
        delete[] PakBuffer;
        return false;
    }

    // 加载纹理
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nx, ny, 0, GL_BGRA, GL_UNSIGNED_BYTE, PakBuffer + index);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    delete[] PakBuffer;
    return true;
}
```

### 2.2 创建分帧渲染辅助函数

在`ACuoi_DanhHieu.cpp`中添加辅助函数：

```cpp
void RenderBitmapWithFrame(GLuint textureId, float x, float y, float width, float height, int frameIndex, int rows, int cols, float r, float g, float b)
{
    // 计算帧坐标
    int col = frameIndex % cols;
    int row = frameIndex / cols;
    
    float uStart = (float)col / (float)cols;
    float vStart = (float)row / (float)rows;
    float uEnd = (float)(col + 1) / (float)cols;
    float vEnd = (float)(row + 1) / (float)rows;
    
    // 渲染指定帧
    RenderBitmap(textureId, x, y, width, height, uStart, vStart, uEnd, vEnd, r, g, b);
}
```

### 2.3 扩展cDanhHieu类

在`StructSendGs.h`中扩展`cDanhHieu`类：

```cpp
class cDanhHieu
{
public:
    // 现有成员
    void SendStartLoad();
    void SendPackButton();
    void SendWsClient(BYTE* lpMsg);
    void Render(int a, int b);
    GetDataDanhHieu* IsCurrentLevel;
    GetDataDanhHieu* IsNextLevel;
    BYTE IsData[4];
    std::map<int, GetDataDanhHieu> m_LoadData;
    POINT SetPos;
    
    // 新增成员（动画相关）
    bool m_bUseAnimation;
    int m_nCurrentFrame;
    int m_nTotalFrames;
    int m_nRows;
    int m_nCols;
    DWORD m_dwLastFrameTime;
    int m_nFrameDelay;
    
    // 新增方法
    void InitAnimation(bool useAnimation, int rows, int cols, int totalFrames, int frameDelay);
    void UpdateAnimation();
    void RenderWithAnimation(int a, int b);
};
```

### 2.4 实现动画相关方法

在`ACuoi_DanhHieu.cpp`中添加实现：

```cpp
void cDanhHieu::InitAnimation(bool useAnimation, int rows, int cols, int totalFrames, int frameDelay)
{
    m_bUseAnimation = useAnimation;
    m_nRows = rows;
    m_nCols = cols;
    m_nTotalFrames = totalFrames;
    m_nFrameDelay = frameDelay;
    m_nCurrentFrame = 0;
    m_dwLastFrameTime = GetTickCount();
}

void cDanhHieu::UpdateAnimation()
{
    if (!m_bUseAnimation)
        return;

    DWORD dwCurrentTime = GetTickCount();
    if (dwCurrentTime - m_dwLastFrameTime > m_nFrameDelay)
    {
        m_nCurrentFrame = (m_nCurrentFrame + 1) % m_nTotalFrames;
        m_dwLastFrameTime = dwCurrentTime;
    }
}

void cDanhHieu::RenderWithAnimation(int a, int b)
{
    if (!this->IsCurrentLevel)
        return;

    if (this->IsData[1] < this->IsNextLevel->AllData[10] + 1)
    {
        if (this->IsData[1] != 0)
        {
            // 更新动画
            UpdateAnimation();
            
            // 计算帧坐标
            int frameIndex = m_nCurrentFrame;
            int col = frameIndex % m_nCols;
            int row = frameIndex / m_nCols;
            
            float uStart = (float)col / (float)m_nCols;
            float vStart = (float)row / (float)m_nRows;
            float uEnd = (float)(col + 1) / (float)m_nCols;
            float vEnd = (float)(row + 1) / (float)m_nRows;
            
            // 渲染动画帧
            RenderBitmap(
                (SEASON3B::CNewUIDanhHieu::IMAGE_HD_LOGO_START + this->IsData[1]) - 1,
                a - 5, b + 60, 150, 40,
                uStart, vStart, uEnd, vEnd,
                1, 1, 0
            );

            IsToolKit.ThisFont(a + 13, b + 47, 0xFFEECCE1, 0x00A2E850, 116, 0, RT3_SORT_CENTER, GlobalText[4004], this->IsData[1]);
            
            // 其他渲染代码保持不变...
        }
        else
        {
            // 无头衔时的渲染保持不变
            RenderBitmap((SEASON3B::CNewUIDanhHieu::IMAGE_HD_LOGO_0), a + 13, b + 42, 116, 112, 0.f, 0.f, 1.0, 128.f / 128.f, 1, 1, 0);
        }
        
        // 后续渲染代码保持不变...
    }
}
```

### 2.5 修改LoadImages方法

修改`CNewUIDanhHieu::LoadImages()`方法，使用新的加载函数：

```cpp
void SEASON3B::CNewUIDanhHieu::LoadImages()
{
    LoadBitmap("Custom\ThangCuoi\RankTitle\nodata.jpg", IMAGE_HD_LOGO_0, GL_LINEAR);

    for (int i = 0; i < gDanhHieu.IsData[2]; i++)
    {
        char IsFile[100];
        sprintf(IsFile, "Custom\ThangCuoi\RankTitle\DanhHieu\%d.tga", i + 1);
        
        // 使用新的分帧加载函数
        LoadOZTWithFrames(IsFile, IMAGE_HD_LOGO_START + i, 4, 5);
    }
}
```

### 2.6 修改Render方法

修改`CNewUIDanhHieu::Render()`方法：

```cpp
bool SEASON3B::CNewUIDanhHieu::Render()
{
    EnableAlphaTest();
    glColor4f(1.f, 1.f, 1.f, 1.f);

    gDanhHieu.SendStartLoad();
    
    g_pUIForm->NewRenderForm(m_Pos.x, m_Pos.y, (float)WINDOW_WIDTH + 20, (float)WINDOW_HEIGHT, GlobalText[3785]);

    g_pUIForm->RenderBack(m_Pos.x + 10, m_Pos.y + 40, float(WINDOW_WIDTH) - 145, 120);
    g_pUIForm->RenderBack(m_Pos.x + 135, m_Pos.y + 40, float(WINDOW_WIDTH) - 145, 120);
    g_pUIForm->RenderBack(m_Pos.x + 10, m_Pos.y + 160, float(WINDOW_WIDTH) - 20, 85);

    // 使用动画渲染
    gDanhHieu.RenderWithAnimation(m_Pos.x, m_Pos.y);

    if (gDanhHieu.IsData[1] < gDanhHieu.IsData[2])
    {
        m_Btn[0].Render();
    }

    DisableAlphaBlend();
    return true;
}
```

### 2.7 修改HealthyBar中的渲染

修改`HealthyBar.cpp`中的渲染代码，支持动画：

```cpp
// 在RenderShowLogRank函数中
if (GetRankLevel >= 1 && GetRankLevel <= 50)
{
    // 计算当前帧（基于时间）
    int currentFrame = (GetTickCount() / 100) % 20;
    int col = currentFrame % 5;
    int row = currentFrame / 5;
    
    float uStart = (float)col / 5.0f;
    float vStart = (float)row / 4.0f;
    float uEnd = (float)(col + 1) / 5.0f;
    float vEnd = (float)(row + 1) / 4.0f;
    
    RenderBitmap(
        (SEASON3B::CNewUIDanhHieu::IMAGE_HD_LOGO_START + GetRankLevel) - 1,
        PosX - 67, PosY - PosDanhHieuY() - 6, 135, 30,
        uStart, vStart, uEnd, vEnd,
        1, 1, 0
    );
}
```

## 3. 初始化和配置

### 3.1 初始化动画

在合适的地方初始化动画参数：

```cpp
// 例如在游戏初始化时
gDanhHieu.InitAnimation(true, 4, 5, 20, 100); // 使用动画，4行5列，20帧，100ms延迟
```

### 3.2 OZT文件准备

1. 创建4行5列的TGA图片（共20帧）
2. 确保图片尺寸正确（例如：150*4*40*5 = 600x200像素）
3. 使用现有命名方式：`Custom\ThangCuoi\RankTitle\DanhHieu\%d.tga`
4. 可选：创建对应的OZT文件以提高加载速度

## 4. 兼容性和性能

### 4.1 兼容性
- 保持与现有系统的完全兼容
- 可以通过`m_bUseAnimation`开关关闭动画
- 当OZT文件不存在时，会自动尝试加载TGA文件

### 4.2 性能优化
- 只在需要时更新动画帧
- 使用纹理坐标计算而非多纹理切换
- 保持渲染调用次数不变

## 5. 总结

此方案实现了：
1. 使用现有OZT文件命名方式
2. 新的独立OZT分帧加载函数
3. 4行5列的分帧动画显示
4. 无缝集成到现有DanhHieu系统
5. 良好的兼容性和性能表现

通过这种方式，您可以为DanhHieu头衔添加生动的动态效果，提升游戏视觉体验。