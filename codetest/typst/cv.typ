#show heading: set text(font: "Linux Biolinum")

#show link: underline

// Uncomment the following lines to adjust the size of text
// The recommend resume text size is from `10pt` to `12pt`
// #set text(
//   size: 12pt,
// )

// Feel free to change the margin below to best fit your own CV
#set page(
  margin: (x: 0.9cm, y: 1.3cm),
)

// For more customizable options, please refer to official reference: https://typst.app/docs/reference/
  
#set par(justify: true)

#let chiline() = {v(-3pt); line(length: 100%); v(-5pt)}

= 李睿涵

liruihan23\@mails.ucas.ac.cn | 1329068252\@qq.com


== 教育背景
#chiline()

*合肥工业大学* #h(1fr) 2018/09 -- 2022/06 \
软件工程 #h(1fr)  \
主修课程：数据结构，计算机系统基础，操作系统，数字电路


*国科大杭州高等研究院* #h(1fr) 2023/09 -- 至今 \
计算机技术 #h(1fr)  \
主修课程：算法设计与分析，计算机体系结构，集成电路与SOC设计基础

== 项目经历
#chiline()

*基于x86的小型操作系统* #h(1fr) 2022/12 -- 2023/04 \
项目类别:毕业设计项目\
- 实现了一个可以运行在qemu虚拟机上的，基于x86指令集的小型操作系统
- 初步实现了操作系统中的MBR主引导记录，实现了内存管理系统以及中断系统，实现了多线程上下文切换的机制，可以使用文本模式显示一定内容，编写了键盘驱动，可以使用键盘进行有限的交互，但shell以及文件系统没有完成，仍然有很多改进的空间

*基于脑电的多模态情感识别与脑机交互方法研究* #h(1fr) 2020/04 -- 2022/04 \
项目类别:国家自然科学基金项目子项目\
- 设计并完善了EEG等生理信号的采集实验流程，设计并实现实验工具系统。
- 进行情感相关的生理信号的采集实验，获取研究数据。 
- 对采集的生理信号数据进行相关处理，建立多模态情感识别生理信号的数据库。 
- 进行关于情感识别的进一步研究，对情感识别算法的分类效果进行对比研究、对生理信号的特征提取进行研究等。
