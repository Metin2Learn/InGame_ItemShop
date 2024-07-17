/*
Navicat MySQL Data Transfer

Source Server         : localhost_3306
Source Server Version : 50651
Source Host           : localhost:3306
Source Database       : player

Target Server Type    : MYSQL
Target Server Version : 50651
File Encoding         : 65001

Date: 2021-12-23 21:46:48
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for ishop_log
-- ----------------------------
DROP TABLE IF EXISTS `ishop_log`;
CREATE TABLE `ishop_log` (
  `accountID` int(11) NOT NULL DEFAULT '0',
  `playerName` varchar(50) DEFAULT '',
  `buyDate` datetime DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP,
  `buyTime` int(11) DEFAULT '0',
  `ipAdress` varchar(16) DEFAULT NULL,
  `itemVnum` int(11) DEFAULT '0',
  `itemCount` int(11) DEFAULT '0',
  `itemPrice` bigint(55) DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
SET FOREIGN_KEY_CHECKS=1;
