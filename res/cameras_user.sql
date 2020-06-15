BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS `cameras` (
	`id_camera`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	`camera_make`	TEXT NOT NULL
);

INSERT INTO `cameras` VALUES (1,'acer');
INSERT INTO `cameras` VALUES (2 ,'agfaphoto');
INSERT INTO `cameras` VALUES (3 ,'benq');
INSERT INTO `cameras` VALUES (4 ,'canon');
INSERT INTO `cameras` VALUES (5 ,'casio');
INSERT INTO `cameras` VALUES (6 ,'concord');
INSERT INTO `cameras` VALUES (7 ,'contax');
INSERT INTO `cameras` VALUES (8 ,'epson');
INSERT INTO `cameras` VALUES (9 ,'fujifilm');
INSERT INTO `cameras` VALUES (10,'ge');
INSERT INTO `cameras` VALUES (11,'hp');
INSERT INTO `cameras` VALUES (12,'jenoptik');
INSERT INTO `cameras` VALUES (13,'jvc');
INSERT INTO `cameras` VALUES (14,'kodak');
INSERT INTO `cameras` VALUES (15,'konica');
INSERT INTO `cameras` VALUES (16,'konica-minolta');
INSERT INTO `cameras` VALUES (17,'kyocera');
INSERT INTO `cameras` VALUES (18,'leica');
INSERT INTO `cameras` VALUES (19,'minolta');
INSERT INTO `cameras` VALUES (20,'minox');
INSERT INTO `cameras` VALUES (21,'nikon');
INSERT INTO `cameras` VALUES (22,'nokia');
INSERT INTO `cameras` VALUES (23,'olympus');
INSERT INTO `cameras` VALUES (24,'panasonic');
INSERT INTO `cameras` VALUES (25,'pentax');
INSERT INTO `cameras` VALUES (26,'praktica');
INSERT INTO `cameras` VALUES (27,'ricoh');
INSERT INTO `cameras` VALUES (28,'rollei');
INSERT INTO `cameras` VALUES (29,'samsung');
INSERT INTO `cameras` VALUES (30,'sanyo');
INSERT INTO `cameras` VALUES (31,'sigma');
INSERT INTO `cameras` VALUES (32,'sony');
INSERT INTO `cameras` VALUES (33,'toshiba');
INSERT INTO `cameras` VALUES (34,'vivitar');
INSERT INTO `cameras` VALUES (35,'yakumo');
INSERT INTO `cameras` VALUES (36,'dji');

CREATE TABLE IF NOT EXISTS `models` (
	`id_camera`	INTEGER NOT NULL,
	`camera_model`	TEXT NOT NULL,
	`sensor_width`	NUMERIC
);


COMMIT;
