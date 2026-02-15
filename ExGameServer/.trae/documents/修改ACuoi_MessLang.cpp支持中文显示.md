# 修改ACuoi\_MessLang.cpp支持中文显示

## 当前实现分析

* ACuoi\_MessLang.cpp使用XML文件加载消息

* MESSAGE\_NEW结构体使用char\[256]存储消息

* ServerDisplayer.cpp已经支持UTF-8中文显示

* GetMessage()函数被广泛用于发送通知和消息

## 问题分析

* 当前实现使用strcpy\_s复制XML属性值到char数组

* 理论上char数组可以存储UTF-8编码的中文字符

* 但需要确保XML文件编码和字符串处理正确

## 修改方案

### 1. 确保XML文件编码

* 将消息配置XML文件保存为UTF-8编码

* 在XML声明中添加encoding="UTF-8"

### 2. 修改ACuoi\_MessLang.cpp

* 检查pugi::xml\_attribute::as\_string()是否正确处理UTF-8

* 确保strcpy\_s正确复制UTF-8字符串

* 验证char数组大小是否足够存储中文消息

### 3. 增强字符串处理

* 确保所有使用GetMessage()的地方正确处理UTF-8编码

* 检查是否有其他地方对消息长度有限制

### 4. 测试验证

* 在XML文件中添加中文消息

* 编译并运行服务器

* 验证中文消息是否正确显示

## 具体修改步骤

### 步骤1: 修改ACuoi\_MessLang.cpp

1. 打开ACuoi\_MessLang.cpp文件
2. 确保XML文件读取时正确处理UTF-8编码
3. 验证strcpy\_s是否正确处理UTF-8字符串

### 步骤2: 配置XML文件

1. 找到消息配置XML文件
2. 确保文件保存为UTF-8编码
3. 在XML声明中添加encoding="UTF-8"
4. 添加中文测试消息

### 步骤3: 测试验证

1. 编译服务器
2. 运行服务器
3. 触发包含中文消息的事件
4. 验证中文消息是否正确显示

## 注意事项

* UTF-8编码的中文字符占用3个字节 per字符

* char\[256]数组最多可存储约85个中文字符，基本够用

* 确保所有字符串处理函数都支持UTF-8编码

* 验证客户端是否也支持UTF-8编码的消息显示

