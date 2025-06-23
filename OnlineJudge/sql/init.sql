drop database if exists `cugoj`;
create database `cugoj`;
USE `cugoj`;
CREATE TABLE `users` (
    username VARCHAR(48) NOT NULL UNIQUE,                # 用户名(唯一)
    password VARCHAR(32) NOT NULL,                       # 密码
    email VARCHAR(100) NOT NULL,                         # 邮箱
    role boolean                                       # 是否是管理员
);
CREATE TABLE `news` (
    id INTEGER(11) NOT NULL AUTO_INCREMENT PRIMARY KEY,    # 新闻ID(唯一)
    time DATETIME NOT NULL,                                # 最后更新时间
    title VARCHAR(255) NOT NULL,                                # 标题
    author VARCHAR(48) NOT NULL,                                # 作者
    importance  BOOLEAN NOT NULL,                               # 是否置顶
    content MEDIUMTEXT NOT NULL,                                # 内容
    foreign key(author) references users(username)          # 外键
);
CREATE TABLE `groups`(
    group_name   VARCHAR(48)  NOT NULL UNIQUE, # 组名(唯一)
    username    VARCHAR(48)  NOT NULL,        # 用户名
    description VARCHAR(255) NOT NULL,        # 描述
    visibility  BOOLEAN      NOT NULL,        # 可见性
    password  VARCHAR(48)      NOT NULL,        # 密码
    create_time  DATETIME     NOT NULL        # 创建时间
);
CREATE TABLE group_members(
    group_name   VARCHAR(48)  NOT NULL,                      # 组名
    username    VARCHAR(48)  NOT NULL,                       # 用户名
    role        BOOLEAN      NOT NULL,                       # 组内权限
    join_time   DATETIME     NOT NULL,                       # 加入时间
    foreign key(group_name) references `groups`(group_name), # 外键
    foreign key(username) references users(username)         # 外键
);
# 创建样例
INSERT INTO `users` (username, password, email, role) VALUES ('1', '2', 'test@
example.com', 1);
INSERT INTO `users` (username, password, email, role) VALUES ('2', '3', 'test@
example.com', 1);
INSERT INTO `news` (time, title, author, importance, content) VALUES (NOW(), 'title', '1', 1, 'content');
INSERT INTO `groups`(group_name, username, description, visibility, password, create_time)VALUES ('group', '1', 'test', 1, 'password', NOW());
INSERT INTO group_members(group_name, username, role, join_time) VALUES ('group', '1', 1, NOW());
# SELECT username, rank FROM `ranks` ORDER BY rank ASC LIMIT 10;
SELECT username, rank FROM `ranks` ORDER BY rank DESC, username ASC LIMIT 10;
set GLOBAL max_connections=9000;