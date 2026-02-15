## 问题分析

MuHelper的自定义拾取功能无法匹配中文名物品的原因在于字符串处理逻辑存在问题。具体来说：

1. **GetItemDisplayName函数**使用`strnlen(pAttr->Name, MAX_ITEM_NAME)`来计算物品名称长度
2. **strnlen函数**是基于ASCII字符串的，对于UTF-8编码的中文名会计算错误的长度
3. **ITEM_ATTRIBUTE.Name**数组存储的是UTF-8编码的中文字符，每个中文字符占用多个字节
4. **字符串匹配失败**由于计算出的名称长度不正确，导致创建的std::string对象内容不完整，从而无法与用户添加的中文名关键词匹配

## 解决方案

修改`GetItemDisplayName`函数，使其能够正确处理UTF-8编码的中文名：

1. **修改ZzzInventory.cpp中的GetItemDisplayName函数**：
   - 移除`strnlen`的使用，直接使用std::string的构造函数处理null结尾的字符串
   - 或者使用更安全的方式创建字符串，确保完整读取UTF-8编码的中文名

2. **具体修改方案**：
   - 将`size_t nNameLen = strnlen(pAttr->Name, MAX_ITEM_NAME); std::string strDisplayName(pAttr->Name, nNameLen);`
   - 修改为`std::string strDisplayName(pAttr->Name);`
   - 这样std::string构造函数会自动处理null结尾的字符串，无论是否包含UTF-8编码的中文字符

## 预期效果

修改后，MuHelper的自定义拾取功能将能够正确：
1. 读取完整的中文名物品名称
2. 与用户添加的中文名关键词进行匹配
3. 拾取符合条件的中文名物品

## 实施步骤

1. 打开`ZzzInventory.cpp`文件
2. 定位到`GetItemDisplayName`函数
3. 修改字符串创建逻辑
4. 保存文件
5. 重新编译项目

这个修复方案简单直接，不会影响其他功能，同时能解决中文名物品无法被自定义关键词匹配的问题。