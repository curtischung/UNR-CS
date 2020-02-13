--CS457 PA1

CREATE DATABASE db_1;
CREATE DATABASE db_1;
CREATE DATABASE db_2;
DROP DATABASE db_2;
DROP DATABASE db_2;
CREATE DATABASE db_2;


USE db_1;
CREATE TABLE tbl_1 (a1 int, a2 varchar(20));
CREATE TABLE tbl_1 (a3 float, a4 char(20));
DROP TABLE tbl_1;
DROP TABLE tbl_1;
CREATE TABLE tbl_1 (a1 int, a2 varchar(20));
SELECT * FROM tbl_1;
ALTER TABLE tbl_1 ADD a3 float;
SELECT * FROM tbl_1;
CREATE TABLE tbl_2 (a3 float, a4 char(20));
SELECT * FROM tbl_2;
USE db_2;
SELECT * FROM tbl_1;
CREATE TABLE tbl_1 (a3 float, a4 char(20));
SELECT * FROM tbl_1;

.EXIT

-- Expected output
--
-- Database db_1 created.
-- !Failed to create database db_1 because it already exists.
-- Database db_2 created.
-- Database db_2 deleted.
-- !Failed to delete db_2 because it does not exist.
-- Database db_2 created.
-- Using database db_1.
-- Table tbl_1 created.
-- !Failed to create table tbl_1 because it already exists.
-- Table tbl_1 deleted.
-- !Failed to delete tbl_1 because it does not exist.
-- Table tbl_1 created.
-- a1 int | a2 varchar(20)
-- Table tbl_1 modified.
-- a1 int | a2 varchar(20) | a3 float
-- Table tbl_2 created.
-- a3 float | a4 char(20)
-- Using Database db_2.
-- !Failed to query table tbl_1 because it does not exist.
-- Table tbl_1 created.
-- a3 float | a4 char(20)
-- All done.