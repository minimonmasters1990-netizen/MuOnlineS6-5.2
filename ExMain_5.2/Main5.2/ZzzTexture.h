#ifndef __ZZZTEXTURE_H__
#define __ZZZTEXTURE_H__

#include "GlobalBitmap.h"

// ZzzTexture.h - 图像/纹理辅助函数声明
// 说明：本头文件仅声明与 JPEG 读取/写入、位图加载与错误处理相关的外部函数。
//       实现位于 ZzzTexture.cpp 中。注释解释了每个函数的用途与参数约定，便于调用与维护。

// 打开自定义后缀的 JPEG 文件并将像素数据以归一化浮点（0.0 - 1.0）形式写入 BufferFloat。
// 参数：
//   filename    - 原始文件名（函数内部可能会移除扩展并添加项目特定后缀，如 "OZJ"）
//   BufferFloat - 目标缓冲区，调用者须保证已分配足够空间（宽 * 高 * 3）用于存放 RGB 三通道 float 数据。
// 返回：成功返回 true，失败返回 false（注意：不会抛出异常，使用 libjpeg 的错误跳转处理）。
extern bool OpenJpegBuffer(char* filename, float* BufferFloat);

// 将内存中的 RGB 字节缓冲写为 JPEG 文件。
// 参数：
//   filename - 输出文件路径（C 字符串）
//   Width    - 图像宽度（像素）
//   Height   - 图像高度（像素）
//   Buffer   - 指向 RGB 字节数据（连续，按行存放，3 字节/像素）
//   quality  - JPEG 质量（0 - 100）
// 返回：成功返回 TRUE（非 C++ bool 布尔语意），失败返回 FALSE。
extern bool WriteJpeg(char* filename, int Width, int Height, unsigned char* Buffer, int quality);

// 将资源数据另存为文件（用于从数据包或内存中提取图像文件并保存到磁盘）。
// 参数：
//   HeaderSize - 要从 PakBuffer 开头复制到目标文件的头部字节数
//   Ext        - 目标文件扩展名（例如 ".jpg"）
//   filename   - 源文件名（带扩展）
//   PakBuffer  - 指向包含源数据的缓冲；如果为 NULL 则函数会尝试从 Data2\filename 中读取文件内容
//   Size       - PakBuffer 的长度（字节）；如果为 0 且 PakBuffer 为 NULL，函数会计算并填充大小
// 说明：函数会将头部与数据一并写入 Data\ 下的新文件名（由原名去掉扩展并追加 Ext 生成）。
extern void SaveImage(int HeaderSize, char* Ext, char* filename, BYTE* PakBuffer, int Size);

#ifdef KJH_ADD_INGAMESHOP_UI_SYSTEM
// LoadBitmap（带完整路径模式）
// 用途：加载位图并上传到指定的 OpenGL 纹理索引。
// 参数：
//   szFileName - 文件名或完整路径（当 bFullPath 为 true 时认为为完整路径）
//   uiTextureIndex - 目标 OpenGL 纹理 ID（或管理器内部索引）
//   uiFilter   - 纹理过滤模式（默认为 GL_NEAREST）
//   uiWrapMode - 纹理环绕模式（默认为 GL_CLAMP_TO_EDGE）
//   bCheck     - 是否在加载失败时进行错误处理/弹窗（默认 true）
//   bFullPath  - 指示 szFileName 是否已包含完整路径（默认 false）
// 返回：加载成功返回 true，否则 false。
bool LoadBitmap(const char* szFileName, GLuint uiTextureIndex, GLuint uiFilter = GL_NEAREST, GLuint uiWrapMode = GL_CLAMP_TO_EDGE, bool bCheck = true, bool bFullPath = false);
#else // KJH_ADD_INGAMESHOP_UI_SYSTEM
// LoadBitmap（标准）
// 用途同上；此版本始终在文件名前附加 "Data\" 前缀。
// 参数含义同上（不含 bFullPath）。
bool LoadBitmap(const char* szFileName, GLuint uiTextureIndex, GLuint uiFilter = GL_NEAREST, GLuint uiWrapMode = GL_CLAMP_TO_EDGE, bool bCheck = true);
#endif // KJH_ADD_INGAMESHOP_UI_SYSTEM

// 删除/卸载指定的纹理 / 位图 资源。
// 参数：
//   uiTextureIndex - 要卸载的纹理索引
//   bForce         - 是否强制卸载（某些情况下需要强制释放底层资源，默认为 false）
void DeleteBitmap(GLuint uiTextureIndex, bool bForce = false);

// 当发生严重错误时，记录并弹出提示，然后进行必要的清理（断开网络、销毁窗口/声音等）并终止进程。
// 参数：
//   szErrorMsg    - 错误信息文本（将被写入 Debug.txt）
//   bForceDestroy - 指示是否强制执行销毁流程（默认 false）。注意：实现中该函数会执行一系列清理并调用 ExitProcess。
void PopUpErrorCheckMsgBox(const char* szErrorMsg, bool bForceDestroy = false);

#endif// __ZZZTEXTURE_H__