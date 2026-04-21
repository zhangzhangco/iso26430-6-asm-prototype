# ISO 26430-6 ASM 约束矩阵

这个文件只记录“当前代码真实做到什么”，不再混入旧原型协议。

## 已落实

| 类别 | 约束 | 当前实现 |
| --- | --- | --- |
| 传输 | 默认端口 `1173` | `asm_responder` / `asm_initiator` 默认使用 `1173` |
| 模型 | 同步 `Request-Response Pair` | 单连接单请求单响应 |
| TLS | 双向认证 | server 开启 `SSL_VERIFY_PEER` + `SSL_VERIFY_FAIL_IF_NO_PEER_CERT` |
| TLS | 单一 cipher suite | 默认 `AES128-SHA`，即 `TLS_RSA_WITH_AES_128_CBC_SHA` |
| TLS | 禁用压缩 | `SSL_OP_NO_COMPRESSION` |
| TLS | TLS 1.0 | `SSL_CTX_set_min_proto_version/max_proto_version(TLS1_VERSION)` |
| 证书 | 最小 RSA 约束 | 启动时校验 RSA / 2048-bit / exponent `65537` |
| 编码 | 标准 ASM UL | 每个 Request/Response 使用 Annex A 对应 UL |
| 编码 | 固定 BER Length | KLV Length 固定为 4-byte long-form BER |
| 编码 | big-endian | `Uint32` / `Uint64` / batch header 均按 big-endian |
| 协议 | `Request_ID` | 所有标准 request 必须携带非零 `Request_ID` |
| 协议 | `BadRequest Response` | 解析失败或非法请求时返回原始 `Request Copy` + `RRP Invalid` |
| 命令 | 通用命令 | `GetTime` / `GetEventList` / `GetEventID` / `QuerySPB` |
| 命令 | LE 命令 | `LEKeyLoad` / `LEKeyQueryID` / `LEKeyQueryAll` / `LEKeyPurgeID` / `LEKeyPurgeAll` |
| 架构 | 可注入事件日志源 | `EventLogStore` 支持按时间窗列举和按 `event_id` 取 XML |
| 架构 | 可注入 LE key store | `LeKeyStore` 支持 `JSONL` 持久化与 16-key 容量约束 |
| 验证 | 字节级回归 | 协议黄金向量、服务测试、TLS 集成测试全覆盖 |

## 当前命令语义

### 通用命令

- `GetTime`
  - Request: `Request_ID`
  - Response: `Request_ID + Uint64 Time + Response`
- `GetEventList`
  - Request: `Request_ID + Uint32 TimeStart + Uint32 TimeStop`
  - Response: `Request_ID + EventIDBatch + Response`
- `GetEventID`
  - Request: `Request_ID + Uint32 EventID`
  - Response: `Request_ID + Text LogRecord + Response`
- `QuerySPB`
  - Request: `Request_ID`
  - Response: `Request_ID + Uint8 Protocol_Ver + Uint8 Status + Response`

### LE 命令

- `LEKeyLoad`
  - Request: `Request_ID + LEKeyBatch`
  - Response: `Request_ID + Overflow + Response`
- `LEKeyQueryID`
  - Request: `Request_ID + LE Key ID`
  - Response: `Request_ID + KeyPresent + Response`
- `LEKeyQueryAll`
  - Request: `Request_ID`
  - Response: `Request_ID + LEKeyIDBatch + Response`
- `LEKeyPurgeID`
  - Request: `Request_ID + LE Key ID`
  - Response: `Request_ID + NoKeyID + Response`
- `LEKeyPurgeAll`
  - Request: `Request_ID`
  - Response: `Request_ID + Response`

## 已知差距

| 类别 | 约束 | 当前状态 |
| --- | --- | --- |
| 证书 | D-Cinema compliant `X.509` profile | 仅做最小 RSA 结构约束，不宣称完整 profile 合规 |
| TLS | `512-byte record size` | 未强制控制 |
| TLS | `ChangeCipherSpec` 特殊处理 | 未显式实现 |
| 日志 | `GetEventID` 的完整 SMPTE 430-5 signing proxy | 当前返回未签名 XML 文本 |
| LE | 真实安全模块/真实链路加密 | 当前为 mock key store |

## 测试覆盖

- KLV/UL 编解码
- 固定 BER Length
- 每类命令 payload roundtrip
- 标准帧协议向量
- `Request_ID=0` 非法请求
- `GetEventList` 时间窗过滤
- `GetEventID` XML 返回
- `LEKeyLoad` overflow
- `LEKeyQueryID` present/absent
- 双向 TLS 成功与失败路径
