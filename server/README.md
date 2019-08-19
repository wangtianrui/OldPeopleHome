

## Server

## 数据库

* 老人表 parent

  |  id  | longid | name | sex  | birth | account | password | phone  |  room  | height | weight |
  | :--: | :----: | :--: | :--: | :---: | :-----: | :------: | :----: | :----: | ------ | ------ |
  | 主键 | 身份证 | 姓名 | 性别 | 生日  |  账号   |   密码   | 电话号 | 房间号 | 身高   | 体重   |

* 子女表 child

  |  id  | longid | name | sex  | account | password | phone  | parent |
  | :--: | :----: | :--: | :--: | :-----: | :------: | :----: | :----: |
  | 主键 | 身份证 | 姓名 | 性别 |  账号   |   密码   | 电话号 |  老人  |

* 子女老人关系表 pc

  | pid       | cid      | relation |
  | --------- | -------- | -------- |
  | parent id | child id | 家属关系 |

  

* 房间表 room

  |  id  | location |   name   |
  | :--: | :------: | :------: |
  | 主键 | 房间位置 | 房间名称 |

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

  