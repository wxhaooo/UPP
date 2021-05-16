<!-- TOC -->

- [Reflection](#reflection)
  - [为什么需要reflection？](#为什么需要reflection)
  - [reflection是什么？](#reflection是什么)
  - [如何实现一个reflection？](#如何实现一个reflection)
  - [reflection in UE4](#reflection-in-ue4)
  - [references](#references)

<!-- /TOC -->

# Reflection

## 为什么需要reflection？

考虑这样一种需求：设计一个UI界面，可以通过这个界面调整一些物理模拟参数从而改变模拟的配置信息。一种直接的实现是使用Qt之类的库，然后实现自己的simulator，simulator暴露一些可以调整参数的接口给UI使用。问题在于 __无论如何管理UI的类都需要一个simulator的对象，这个对象必须存在UI类中。__ 这么做有几个缺点：

* UI类拥有一个simulator对象，但 __逻辑上UI类和simulator类应该是独立无耦合关系的。把simulator对象放在UI类中和逻辑上两者的关系不相符。__
* 如果运行一段时间后想把simulator的信息（simulator的配置信息等，包括simulator的名称，simulator的参数名称，参数大小，参数范围等）都存储起来。如何方便的获得参数的名称、范围？
当然一个参数可以用一对std::string 以及实际类型和确定，比如
```cpp
class simulator
{
    ....
float time_step;
// 可以表示为
std::pair<std::string,float> time_step;
// std::string 记录参数的名称，float记录参数的大小
    ....
};
```
但这样做明显非常的冗余，因为time_step本身已经说明了变量的名称。

对于第一点需求，我们 __如果可以仅仅使用simulator的名称以及参数名称就可以调用调用simulator的对应函数，那自然UI和simulator是解耦的。__
对于第二点需求，如果我们可以通过某种方式直接获得参数/函数/类的名称就可以解决。

但遗憾的是这两种重要的需求在现有的C++中都无法直接解决（C++ 23前）。原因是 __这两者都依赖编译器在编译期间以某种方式来存储类相关的信息。（在编译的时候编译器是知道所有信息的，如果在这期间我们存储类的名字，参数的名字，参数的地址，参数的值等信息，在运行期间可以用过名字得到地址和值，上面的需求就可以直接实现了。）__

而这些需要reflection的支持才能完成。

## reflection是什么？

看完上一小结我们就能给reflection一个通俗的定义，即所谓 __reflection指的是在运行期间程序获取自身信息的能力。__

可能有些小伙伴会有疑问，运行期间我当然可以用过类地址+偏移的方式（比如simulator.time_step）的方式获取自身信息。但如前面所说的，这种方式是不完全的，比如我们缺乏time_step的名字，缺乏依赖从time_step这个名字获取time_step值的方式，使用simulator的成员函数名和参数值就可以调用simulator的成员函数等等。 __究其根本是C++编译器收集的信息不足导致的。__ 一些支持reflection的语言（比如C#）就不存在这些问题。

另外，reflection收集了很多关于对象的信息，对于序列化和反序列化也是非常有帮助的。

总之，在现代面向对象编程中，reflection是一个非常重要的特性。但 __由于C++语言设计遵循零开销原则，在编译期间收集并生成存储信息的属性表是有额外开销的，所以C++标准并没有实现这种特性。__

## 如何实现一个reflection？

既然reflection这么重要但C++又不直接支持这种特性，那么我们非常有必要自己实现一套reflection的机制。这是不是意味着我们要深入编译器来实现呢？可以是，但就学习reflection而言没有必要。

通常而言，reflection有三种实现方式：

1. __在编译期收集信息（C#）__
2. __写外部工具识别收集（UE4）__
3. __程序员手动添加标记__

__其中2，3都是编译前就可以获取相关信息，所以是静态的reflection，而1是在编译期收集，被称作动态的reflection。__

接下来，我们会遵循方式3来实现一个功能比较完备的reflection机制。然后在 [reflection in UE4](#reflection-in-ue4) 这一节，我们整理一下UE4中reflection实现的大体思路。

## reflection in UE4

## references
