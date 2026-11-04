# 第二次作业 · 项目说明


## 0. 基本信息

- 姓名：张一铭
- 学号：2026940210
- 完成日期：2026.11.3
- 是否完成全部基础功能：是

## 1. 类设计

|     类名     |                         职责                                |     继承自  |
| --- | --- | --- |
| TrackingBase |初始化每一个对象，保存基本调用函数，访问id,type,status,snapshot |---          |
| TargetManager|        构造最重要的onFrame函数，以及存储容器targets_          |---          |
| ArmorTarget  |       1.在派生类内调用主类TrackingBase (2.)3帧LOST判定       | TrackingBase|
|SpinningTarget|1.在派生类内调用主类TrackingBase (2.)5帧LOST判定              |TrackingBase |


类之间的关系（文字或图片）：

```
TargetManager
└── vector<unique_ptr<TrackingBase>>
        ├── ArmorTarget
        └── SpinningTarget
```

## 2. 一帧处理流程

按实际执行顺序简述 `onFrame`：

1. 一帧传进来后，`data_types`里`Observation`和`InputFrame`拿到原始数据(`InputFrame`会调用Observation,其实就是在InputFrame里了)

2. `onFrame()`读取`InputFrame`后依次检查
- (1.)是否同一帧内目标编号重复
- (2.)是否观测中存在负数目标编号
- (3.)是否观测类型与已创建目标的类型不一致

3. 接下来判断是否为第一个成功帧
是:
- (1.)标记已有成功帧
- (2.)标记该帧合法
- (3.)更新上一成功帧号及时间戳
否：
- 判断是否`timestamp_ns` 不大于最近一次成功处理的帧时间戳，`frame_id` 是否不大于最近一次成功处理的帧号

4. 若该帧
- 合法 计算`dt`并更新相关参数
- 不合法 标记

5. 创建`Result`函数，判断该帧是否合法
非法：`accepted`标记为`false`,记录`id`
合法:
- (1.)`accepted`标记为`true`,记录`id`
- (2.)遍历接收到的`observation`，拿每一个目标与已知的`targets_`进行比较,若存在调用`update`函数更新状态，若不存在则在`vector`里新建一个目标
- (3.)随后遍历已知目标容器，检查有没有此帧没有出现的已知目标，若有，调用`update`函数

6. 用`sendresulttarget()`函数对更新过状态的`targetresult`制作并上传快照

重点说明：整帧校验失败时，为什么不会留下部分修改。
:项目中修改目标参数全部依赖`update()`函数实现，而整帧校验失败仅返回`false`和旧快照，不会涉及修改

## 3. 附加项

### 3.1 完成情况

| 附加项                     | 是否完成 |
| --- | --- |
| 附加一：`LOST` 目标超时删除 |未完成    |
| 附加二：类型变化时重建目标   | 未完成  |
| 附加三：自主学习并使用 static|未完成   |
| 附加四：自主学习并使用智能指针| 完成   |
| 附加五：目标排序             | 未完成 |


### 3.3 附加四：智能指针
智能指针类型:unique_ptr

使用位置:
- 1.在创建保存历史`Target`的`vector`时，即`TargetManager.hpp`的`std::vector<std::unique_ptr<TrackingBase>> targets_;`
- 2.`TargetManager.cpp`中遍历历史数据，新增目标
理由：考虑到智能指针的特点
- 1.可自动`new`和`delete`内存空间 -> 在每处理一帧时都有可能要扩大`vector`，此时独立地`new`和`delete`过于麻烦，智能指针可以很好的解决这个问题
- 2.`unique_ptr`类型独占所有权的智能指针 -> 在处理`ARMOR`和`SPINNING`时保证唯一性

项目对于需要大量长期动态变化`new`和`delete`的部分好像只有这一处，其他地方没有需求
---

## 4. 遇到的问题 / 未完成的部分

- 遇到的问题和解决过程：
1. `TrackingBase`和`TargetManager`功能划分
解决：咨询AI建议以及后面编写中按需要动态调整
2. 怎么保存已有数据及其调用
解决:问AI，学习了智能指针的容器
3. 在`OnFrame`中直接访问并修改此帧数据，编译失效
解决：通过在`TrackingBase`中构造`update()`函数访问数据
4. `LOST`状态下继续从`observation`中读取不存在的数据参与计算
解决：将参数改为`observation`的指针，`LOST`目标传入`nullptr`，不会污染数据
5. 在`Onframe`中计算`dt_obs`，没法针对`LOST`目标进行规避
解决:移到`TargetManager`中的`update()`函数中，判断后再计算
6. 主体完成后编译错误
解决：AI审校，自己再订正(声明位置，初始化，语法等问题)
- 已知未完成或有缺陷的地方：
原本学习了一下`static`，我的理解是`static`变量会保留上次函数调用产生的局部变量值
然后在`onFrame()`函数中使用，保存`isillegal`，`t_ns`，`has_successful_frame`等每一帧重复使用
的变量，但编译不通过，去掉`static`之后方能运行.
