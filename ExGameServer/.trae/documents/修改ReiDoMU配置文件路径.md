# 修改ReiDoMU配置文件路径

## 问题分析
当前ReiDoMU活动的配置文件路径是通过`gPath.GetFullPath("Event\\ReiDoMu.dat")`设置的，这会读取服务器根目录下Event文件夹中的ReiDoMu.dat文件。

## 修改计划
1. **修改ServerInfo.cpp文件**
   - 将第312行的配置文件路径从`"Event\\ReiDoMu.dat"`修改为`"Data\\Event\\REIDOMU.DAT"`，以匹配用户指定的服务器路径格式。

2. **确认配置文件位置**
   - 确保配置文件位于服务器的`\MUSERVER TT\4.Sub-1\Data\Event`目录下，文件名为`REIDOMU.DAT`。
   - 确保MESSAGE.XML文件位于服务器的`\MUSERVER TT\4.Sub-1\Data`目录下。

3. **验证修改**
   - 检查修改后的代码是否正确读取配置文件。

## 修改原因
用户希望将配置文件路径修改为服务器上的实际路径，这样可以正确读取活动配置和公告消息。