## 安装

先安装`judger`,然后

```
npm install
npm run build
npm run test
```

## 已知bug

#### bug_1

`freopen("data.out","w",stdout)`,不能产生"data.out",这个应该是judger代码的锅,:cry:
