# MiniSQL

## examples/ex0.cpp

### 创建student表
```
create table student (name varchar(12), grad_year int)
```

### 向student表插入数据
```
insert into student (name, grad_year) values ('xxx', 2000)
insert into student (name, grad_year) values ('yyy', 2000)
insert into student (name, grad_year) values ('zzz', 2000)
insert into student (name, grad_year) values ('aaa', 2001)
insert into student (name, grad_year) values ('bbb', 2001)
insert into student (name, grad_year) values ('ccc', 2001)
```

### 查询所有学生信息
```
select name, grad_year from student
```
查询结果
```
xxx, 2000
yyy, 2000
zzz, 2000
aaa, 2001
bbb, 2001
ccc, 2001
```

### 更新数据
```
update student set grad_year = 2000 where name = 'aaa'
```

### 删除数据
```
delete from student where name = 'zzz'
```

### 查询毕业年份为2000的学生信息
```
select name, grad_year from student where grad_year = 2000
```
查询结果
```
xxx, 2000
yyy, 2000
aaa, 2000
```

### 查询毕业年份为2000且姓名为xxx的学生信息
```
select name, grad_year from student where grad_year = 2000 and name = 'xxx'
```
查询结果
```
xxx, 2000
```


## examples/ex1.cpp

### 创建student表
```
create table student (sid int, sname varchar(12), grad_year int, major_id int)
```

### 向student表插入数据
```
insert into student (sid, sname, grad_year, major_id) values (1, 'joe', 2004, 10)
insert into student (sid, sname, grad_year, major_id) values (2, 'amy', 2004, 20)
insert into student (sid, sname, grad_year, major_id) values (3, 'max', 2005, 10)
insert into student (sid, sname, grad_year, major_id) values (4, 'sue', 2005, 20)
insert into student (sid, sname, grad_year, major_id) values (5, 'bob', 2003, 30)
insert into student (sid, sname, grad_year, major_id) values (6, 'kim', 2001, 20)
insert into student (sid, sname, grad_year, major_id) values (7, 'art', 2004, 30)
insert into student (sid, sname, grad_year, major_id) values (8, 'pat', 2001, 20)
insert into student (sid, sname, grad_year, major_id) values (9, 'lee', 2004, 10)
```

### 创建dept表
```
create table dept (did int, dname varchar(12))
```

### 向dept表插入数据
```
insert into dept (did, dname) values (10, 'compsci')
insert into dept (did, dname) values (20, 'math')
insert into dept (did, dname) values (30, 'drama')
```

### 联表查询
```
select sid, sname, grad_year, did, dname from student, dept where major_id = did
```

查询结果
```
1, joe, 2004, 10, compsci
2, amy, 2004, 20, math
3, max, 2005, 10, compsci
4, sue, 2005, 20, math
5, bob, 2003, 30, drama
6, kim, 2001, 20, math
7, art, 2004, 30, drama
8, pat, 2001, 20, math
9, lee, 2004, 10, compsci
```