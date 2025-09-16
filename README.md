科大讯飞小学版 \- 主板

- 主控

CW32L031C8U6



- 开发问题：

红外遥控器

```c
    uint8_t remote_val = remote_scan(&iremote);
    if (remote_val) // 应该去掉它，否侧一直按同一个按键，值就不会变化了
        dev.id = remote_val;
```

