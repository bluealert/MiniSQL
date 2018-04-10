# MiniSQL

## examples/ex0.cpp

### 创建student表
```
create table student (name varchar(12), grad_year int)
```

### 插入数据
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
运行结果
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
运行结果
```
xxx, 2000
yyy, 2000
aaa, 2000
```

### 查询毕业年份为2000且姓名为xxx的学生信息
```
select name, grad_year from student where grad_year = 2000 and name = 'xxx'
```
运行结果
```
xxx, 2000
```