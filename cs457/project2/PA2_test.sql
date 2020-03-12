CREATE DATABASE CS457_PA2;
USE CS457_PA2;
CREATE TABLE Product (pid int, name varchar(20), price float);
insert into Product values(1,	'Gizmo',      	19.99);
insert into Product values(2,	'PowerGizmo', 	29.99);
insert into Product values(3,	'SingleTouch', 	149.99);
insert into Product values(4,	'MultiTouch', 	199.99);
insert into Product values(5,	'SuperGizmo', 	49.99);
select * from Product;
update Product set name = 'Gizmo' where name = 'SuperGizmo';
update Product set price = 14.99 where name = 'Gizmo';
select * from Product;
delete from product where name = 'Gizmo';
delete from product where price > 150;
select * from Product;
select name, price from product where pid != 2;
.exit