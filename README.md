# Steam_Auto_Login

花了一个多小时简单一写，后续再慢慢完善。

## 配置文件

SteamData.ini放在程序所在目录，注意是ANSI编码。

```
[User]		user1
[Password]	123456
[Remark]	国区

[User]		user2
[Password]	123456
[Remark]	阿区

[User]		user3
[Password]	123456
[Remark]	美区
```

## 编译

编译前需要修改CSteamQuickLoginDlg::LoadUserInfo中的steam的绝对路径。

编译环境vs2019。

## 使用

双击即可登录对应账号。

## TODO

* 密码需要加密存储
* （可选）令牌验证的自动化
* 改用sqlite存储