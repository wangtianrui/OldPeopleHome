

## Server

### 数据库设计

* 老人表 parent

  |  id  | longid | name | sex  | birth | account | password | phone  | height | weight |
  | :--: | :----: | :--: | :--: | :---: | :-----: | :------: | :----: | ------ | ------ |
  | 主键 | 身份证 | 姓名 | 性别 | 生日  |  账号   |   密码   | 电话号 | 身高   | 体重   |

* 子女表 child

  |  id  | longid | name | sex  | account | password | phone  |
  | :--: | :----: | :--: | :--: | :-----: | :------: | :----: |
  | 主键 | 身份证 | 姓名 | 性别 |  账号   |   密码   | 电话号 |

* 子女老人关系表 pc

  | pid       | cid      | relation |
  | --------- | -------- | -------- |
  | parent id | child id | 家属关系 |

* 房间表 room

  |  id  | location |   name   |  pid   |
  | :--: | :------: | :------: | :----: |
  | 主键 | 房间位置 | 房间名称 | 老人id |

* 运动表 motion

  |       pid       | date | count | distance | time |
  | :-------------: | :--: | :---: | :------: | :--: |
  | parent id(外键) | 日期 | 步数  |   距离   | 时长 |

* 睡眠表 sleep

  |    pid    | date |   deep   |  light   |  awake   |
  | :-------: | :--: | :------: | :------: | :------: |
  | parent id | 日期 | 深睡时间 | 浅睡时间 | 清醒时间 |

* 房间实时状态表  rstate

  |  rid   |   time   | temperature | humidity | isin         |
  | :----: | :------: | :---------: | :------: | ------------ |
  | 房间号 | 测量时间 |    温度     |   湿度   | 是否在房间里 |

* 老人心率表 heartrate

  | pid  |   time   | rate |
  | :--: | :------: | :--: |
  | 老人 | 测量时间 | 心率 |

  

### 接口格式

使用实例：http://39.105.65.209:8080/parent/list

基本的业务接口已经完成，如下

| 实体           | url示例                | 请求方式 | 功能概述                                  | 参数格式                                                     |
| -------------- | ---------------------- | -------- | ----------------------------------------- | ------------------------------------------------------------ |
| Admin          | /admin/login           | POST     | 后台管理员账号                            | account:root,password:root                                   |
| Room           | /room/get/1            | GET      | 查询id=1的room的详细信息                  |                                                              |
| Room           | /room/list             | GET      | 查询所有的room信息                        |                                                              |
| Room           | /room/list/empty       | GET      | 查询所有空置room信息(web端给老人分配房间) |                                                              |
| Room           | /room/add              | POST     | 添加room                                  | location:地点，name:房间名                                   |
| Room           | /room/delete/1         | DELETE   | 删除id=1的room信息                        |                                                              |
| Room           | /room/update/1         | POST     | 更新id=1的room信息                        | **要更新的字段严格按照命名格式传参**比如name就传入roomName   |
| Parent         | /parent/get/1          | GET      | 查询id=1的parent的详细信息                |                                                              |
| Parent         | /parent/get_longid/111 | GET      | 查询身份证=111的parent的详细信息          |                                                              |
| Parent         | /parent/get_name/老王  | GET      | 查询姓名=老王的parent的详细信息           |                                                              |
| Parent         | /parent/get_room/1     | GET      | 查询id=1的room对应老人的信息              |                                                              |
| Parent         | /parent/list           | GET      | 查询所有老人信息                          |                                                              |
| Parent         | /parent/add            | POST     | 添加老人                                  | parentSex：老人性别，parentPassword：老人密码，parentAccount：账号，parentName：姓名，parentBirth：生日，parentLongId：身份证 |
| Parent         | /parent/update/1       | POST     | 更新id=1的老人信息                        | **要更新的字段严格按照命名格式传参**比如name就传入parentName |
| Parent         | /parent/delete/1       | DELETE   | 删除id=1的老人                            |                                                              |
| Parent         | /parent/login          | POST     | 老人登陆                                  | 登陆成功返回老人的所有信息，登陆失败返回null（不返回） account:bao,password:123      |
| Child          | /child/get/1           | GET      | 查询id=1的子女                            |                                                              |
| Child          | /child/get_longid/111  | GET      | 查询身份证=111的child的详细信息           |                                                              |
| Child          | /child/get_name/小蒋   | GET      | 查询姓名=小蒋的child的详细信息            |                                                              |
| Child          | /child/list            | GET      | 查询所有子女                              |                                                              |
| Child          | /child/add             | POST     |                                           | childSex，childPassword，childAccount，childName，childLongId，childPhone |
| Child          | /child/update/1        | POST     | 更新id=1的子女信息                        | **要更新的字段严格按照命名格式传参**比如name就传入parentName |
| Child          | /child/delete/1        | DELETE   | 删除id=1的子女信息                        |                                                              |
| Child          | /child/login           | POST     | 登陆子女                                  | 登陆成功返回子女信息，失败返回空(不返回)             account:bao,password:123        |
| ChildAndParent | /child_parent/child/1  | GET      | 查看子女id=1对应的老人                    |                                                              |
| ChildAndParent | /child_parent/parent/1 | GET      | 查看老人id=1对应的子女                    |                                                              |
| ChildAndParent | /child_parent/add      | POST     | 添加老人子女对应关系                      | 子女id：child，老人id：parent，关系：relation                |
| Motion         | /motion/get/1          | GET      | 查找id=1的老人运动情况                    | 参数格式：yyyy-mm-dd  加上start参数可以查看某一时刻。加上start和end参数可以查看某一时间段。 |
| Motion         | /motion/add            | POST     | 添加老人运动情况数据                      | parent，date，count，distance，time                          |
| RoomState      | /rstate/get/1          | GET      | 查找id=1的房间状态                        | 参数格式：yyyy-mm-dd hh-mm-ss  加上start参数可以查看某一时刻。加上start和end参数可以查看某一时间段。 |
| RoomState      | /rstate/add            | POST     | 添加房间状态                              | roomId，time，temperature，humidity，isin(是否在房间内)      |
| Sleep          | /sleep/get/1           | GET      | 查找id=1的老人睡眠信息                    | 参数格式：yyyy-mm-dd  加上start参数可以查看某一时刻。加上start和end参数可以查看某一时间段。 |
| Sleep          | /sleep/add             | POST     | 添加老人睡眠信息                          | parent，date，deep，light，awake                             |
| HeartRate      | /heartrate/get/1       | GET      | 查找id=1的心率信息                        | 参数格式：yyyy-mm-dd hh-mm-ss  加上start参数可以查看某一时刻。加上start和end参数可以查看某一时间段。 |
| HeartRate      | /heartrate/add         | POST     | 添加老人睡眠信息                          | parentId，time，rate1(收缩压)，rate2(舒张压)，oxy(血氧)      |
