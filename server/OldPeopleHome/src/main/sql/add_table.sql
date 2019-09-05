SET FOREIGN_KEY_CHECKS=0;
DROP TABLE IF EXISTS `child`;
DROP TABLE IF EXISTS `cp`;
DROP TABLE IF EXISTS `heartrate`;
DROP TABLE IF EXISTS `motion`;
DROP TABLE IF EXISTS `parent`;
DROP TABLE IF EXISTS `room`;
DROP TABLE IF EXISTS `rstate`;
DROP TABLE IF EXISTS `sleep`;
CREATE TABLE `parent` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '老人id',
  `longid` char(18) NOT NULL COMMENT '老人身份证',
  `name` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci  NOT NULL COMMENT '姓名',
  `sex` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci  NOT NULL COMMENT '性别',
  `account` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci  NOT NULL COMMENT '账号',
  `password` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci  NOT NULL COMMENT '密码',
  `height` double DEFAULT NULL COMMENT '身高',
  `weight` double DEFAULT NULL COMMENT '体重',
  `birth` date NOT NULL COMMENT '生日',
  `phone` char(11) DEFAULT NULL COMMENT '电话号码',
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE KEY `index_longid` (`longid`) USING BTREE,
  KEY `index_name` (`name`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=23 DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci  ROW_FORMAT=DYNAMIC;
CREATE TABLE `room` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '房间id',
  `location` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci  NOT NULL COMMENT '房间位置',
  `name` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci  NOT NULL COMMENT '房间名称(房间号)',
  `pid` bigint(20) DEFAULT NULL COMMENT '房间住人id',
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE KEY `index` (`pid`) USING BTREE,
  CONSTRAINT `room_fk_1` FOREIGN KEY (`pid`) REFERENCES `parent` (`id`) ON DELETE SET NULL
) ENGINE=InnoDB AUTO_INCREMENT=19 DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci  ROW_FORMAT=DYNAMIC;

CREATE TABLE `child` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT COMMENT 'id',
  `longid` char(18) NOT NULL COMMENT '身份证',
  `name` varchar(255) NOT NULL COMMENT '姓名',
  `sex` varchar(255) NOT NULL COMMENT '性别',
  `account` varchar(255) NOT NULL COMMENT '账号',
  `password` varchar(255) NOT NULL COMMENT '密码',
  `phone` varchar(255) NOT NULL COMMENT '电话',
  PRIMARY KEY (`id`),
  UNIQUE KEY `index _longid` (`longid`) USING BTREE,
  KEY `index_name` (`name`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci ;
CREATE TABLE `cp` (
  `pid` bigint(20) DEFAULT NULL,
  `cid` bigint(20) DEFAULT NULL,
  `relation` varchar(255) DEFAULT NULL,
  KEY `index_pid` (`pid`) USING BTREE,
  KEY `index_cid` (`cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci ;
CREATE TABLE `heartrate` (
  `pid` bigint(20) DEFAULT NULL,
  `time` datetime DEFAULT NULL,
  `rate` double DEFAULT NULL,
  KEY `index_pid` (`pid`) USING BTREE,
  KEY `time` (`time`) USING BTREE,
  CONSTRAINT `fk_pid2` FOREIGN KEY (`pid`) REFERENCES `parent` (`id`) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci ;

ALTER TABLE `heartrate`
CHANGE COLUMN `rate` `rate1` double NULL DEFAULT NULL COMMENT '收缩压' AFTER `time`,
ADD COLUMN `rate2` double NULL COMMENT '舒张压' AFTER `rate1`,
ADD COLUMN `oxy` double NULL AFTER `rate2`;

ALTER TABLE `heartrate`
ADD COLUMN `rate` double NULL COMMENT '心率' AFTER `time`;

CREATE TABLE `motion` (
  `pid` bigint(20) DEFAULT NULL COMMENT 'parent id',
  `date` date DEFAULT NULL COMMENT '当日日期',
  `count` bigint(20) DEFAULT NULL COMMENT '步数',
  `distance` double DEFAULT NULL COMMENT '距离',
  `time` time DEFAULT NULL COMMENT '运动时长',
  KEY `index_date` (`date`) USING BTREE,
  KEY `index_pid` (`pid`) USING BTREE,
  CONSTRAINT `fk_pid` FOREIGN KEY (`pid`) REFERENCES `parent` (`id`) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci ;

CREATE TABLE `rstate` (
  `rid` int(11) DEFAULT NULL,
  `time` timestamp NULL DEFAULT NULL,
  `temperature` double DEFAULT NULL,
  `humidity` double DEFAULT NULL,
  `isin` int(11) DEFAULT NULL,
  KEY `index_rid` (`rid`) USING BTREE,
  KEY `index_time` (`time`) USING BTREE,
  CONSTRAINT `fk_rid1` FOREIGN KEY (`rid`) REFERENCES `room` (`id`) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci ;
CREATE TABLE `sleep` (
  `pid` bigint(20) DEFAULT NULL COMMENT 'parent id',
  `date` date DEFAULT NULL COMMENT '当天日期',
  `deep` time DEFAULT NULL COMMENT '深睡时长',
  `light` time DEFAULT NULL COMMENT '浅睡时长',
  `awake` time DEFAULT NULL COMMENT '清醒时长',
  KEY `index_date` (`date`) USING BTREE,
  KEY `index_pid` (`pid`) USING BTREE,
  KEY `index_pid_date` (`pid`,`date`) USING BTREE,
  CONSTRAINT `fk_pid1` FOREIGN KEY (`pid`) REFERENCES `parent` (`id`) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci ;
CREATE TABLE `admin`  (
  `account` varchar(255) NOT NULL COMMENT '管理员账号',
  `password` varchar(255) NULL COMMENT '管理员密码',
  PRIMARY KEY (`account`),
  UNIQUE INDEX `index_acc`(`account`) USING BTREE
);

