# 标准报文样例

这些十六进制样本对应当前仓库已经确认的标准帧，用于：

- 人工检查是否仍然符合 `ISO 26430-6`
- 单测里的字节级回归断言
- 后续互操作联调时的基线比对

说明：

- 全部基于 Annex A 的标准 ASM UL
- KLV Length 固定使用 4-byte long-form BER
- 十六进制连续书写，不插空格

## 1. GetTime Request

条件：

- `Request_ID = 1`

KLV 帧：

```text
060e2b340205010102070102100000008300000400000001
```

## 2. GetEventList Response

条件：

- `Request_ID = 2`
- `EventIDBatch = [1001, 1002, 1003]`
- `Response = successful`

KLV 帧：

```text
060e2b3402050101020701021300000083000019000000020000000300000004000003e9000003ea000003eb00
```

## 3. LEKeyLoad Request

条件：

- `Request_ID = 3`
- `LEKeyBatch` 只有 1 项
- `LE Key ID = 4001`
- `Key = 00112233445566778899aabbccddeeff`
- `ExpireTime = 3600`
- `AttributeData = 123456789`

KLV 帧：

```text
060e2b340205010102070103200000008300002c00000003000000010000002000000fa100112233445566778899aabbccddeeff00000e1000000000075bcd15
```

## 4. QuerySPB Response

条件：

- `Request_ID = 4`
- `Protocol_Ver = 0x01`
- `Status = not_playing`
- `Response = successful`

KLV 帧：

```text
060e2b340205010102070102170000008300000700000004010000
```

## 5. BadRequest Response

条件：

- `Request Copy` 为第 1 条 `GetTime Request`
- `Response = invalid`

KLV 帧：

```text
060e2b3402050101020701010100000083000019060e2b34020501010207010210000000830000040000000102
```

## 当前覆盖范围

当前标准报文样例已覆盖：

- 通用命令 request
- 通用命令 response
- `LEKeyLoad` 标准 batch request
- `BadRequest` request copy 规则

如果后续继续扩展，建议补齐：

- `GetEventID Response`
- `LEKeyQueryID Response`
- `LEKeyQueryAll Response`
- `LEKeyPurgeID Response`
- `LEKeyPurgeAll Response`
