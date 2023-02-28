# regex recognition
c++实现的正则表达式识别器。包含正则转换NFA和NFA转换DFA过程的展示。
需要编译环境：c++ 11
打开LA2.exe可直接使用本程序。

## 注意事项

+ 时间原因，正则表达式不支持转义符号，也不支持 `.` 来表示全体字符。因此该程序不支持对`| ( ) +`等字符的识别。
+ 使用时若需表示空串，使用`替代，因此本程序不支持对该字符的识别


## 程序运行示例

+ 正规式r：(a|b)*bb((>|<)(=|$\epsilon$)|(!|=)=)1

+ 运行编写的程序，输入正规式，生成NFA（初态为第一行）

  ![image-20220408075822086](https://file.peteralbus.com/assets/blog/imgs/blogimg/image-20220408075822086.png)

+ 程序将生成的NFA转换为DFA

  ![image-20220408075900260](https://file.peteralbus.com/assets/blog/imgs/blogimg/image-20220408075900260.png)

+ 对DFA进行化简，并输出终态集合和支持输入的字符：

  ![image-20220408080001436](https://file.peteralbus.com/assets/blog/imgs/blogimg/image-20220408080001436.png)

+ 读取txt文件内的内容作为输入，进行识别测试

  ![image-20220408080106552](https://file.peteralbus.com/assets/blog/imgs/blogimg/image-20220408080106552.png)

+ 识别结果符合预期，自动机生成成功。

  ![image-20220408080147909](https://file.peteralbus.com/assets/blog/imgs/blogimg/image-20220408080147909.png)