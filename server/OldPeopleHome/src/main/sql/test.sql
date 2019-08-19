/*
 Navicat Premium Data Transfer

 Source Server         : localhost
 Source Server Type    : MySQL
 Source Server Version : 80013
 Source Host           : localhost:3306
 Source Schema         : ssmtest

 Target Server Type    : MySQL
 Target Server Version : 80013
 File Encoding         : 65001

 Date: 19/08/2019 14:07:35
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for room
-- ----------------------------
DROP TABLE IF EXISTS `room`;
CREATE TABLE `room`  (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '房间id',
  `location` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '房间位置',
  `name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '房间名称(房间号)',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of room
-- ----------------------------
INSERT INTO `room` VALUES (1, '中北大学9#', '146');

SET FOREIGN_KEY_CHECKS = 1;

-- ----------------------------
-- Table structure for parent
-- ----------------------------
DROP TABLE IF EXISTS `parent`;
CREATE TABLE `parent`  (
  `id` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '老人id',
  `longid` bigint(18) NOT NULL COMMENT '老人身份证',
  `name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci  NOT NULL COMMENT '姓名',
  `sex` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci  NOT NULL COMMENT '性别',
  `account` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '账号',
  `password` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '密码',
  `height` double NULL DEFAULT NULL COMMENT '身高',
  `weight` double NULL DEFAULT NULL COMMENT '体重',
  `birth` date NOT NULL COMMENT '生日',
  `phone` bigint(11) NULL DEFAULT NULL COMMENT '电话号码',
  `room` int(11) NULL DEFAULT NULL COMMENT 'room id',
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `rid`(`room`) USING BTREE,
  CONSTRAINT `rid` FOREIGN KEY (`room`) REFERENCES `room` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of parent
-- ----------------------------
INSERT INTO `parent` VALUES (1, 510703199706330018, 'jiang', '男', 'jiang', '123', 170, 63, '1997-06-23', 19935324869, 1);

SET FOREIGN_KEY_CHECKS = 1;
