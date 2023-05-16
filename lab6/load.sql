-- 如果原来有同名表，则删除
drop table if exists example;
-- 创建名为example的表
create table example
(
    encoding   bigint,
    ciphertext varchar(512)
);

-- 删除已有的函数
drop function if exists FHInsert;
drop function if exists FHSearch;
drop function if exists FHUpdate;
drop function if exists FHStart;
drop function if exists FHEnd;

-- 创建函数
create function FHInsert RETURNS INTEGER SONAME 'libope.so';
create function FHSearch RETURNS INTEGER SONAME 'libope.so';
create function FHUpdate RETURNS INTEGER SONAME 'libope.so';
create function FHStart RETURNS INTEGER SONAME 'libope.so';
create function FHEnd RETURNS INTEGER SONAME 'libope.so';

-- 创建插入数据的存储过程
drop procedure if exists pro_insert;
delimiter $$
create procedure pro_insert(IN pos int, IN ct varchar(512))
BEGIN
    DECLARE i BIGINT default 0;
    SET i = FHInsert(pos, ct);
    insert into example values (i, ct);
    if i = 0 then
        -- 树结构中更新了编码，同步更新数据库中的信息
        update example
        set encoding = FHUpdate(ciphertext)
        where (encoding >= FHStart() and encoding < FHEnd())
           or (encoding = 0);
    end if;
END $$
delimiter ;