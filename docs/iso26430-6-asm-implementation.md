# ISO 26430-6 ASM 实现说明

## 目标

当前实现的目标不是做“影院生产系统”，而是做一个**协议级对齐的参考原型**：

- 线协议按 `ISO 26430-6:2009(E)` 的命令 UL 和字段布局编码
- TLS 约束尽量收敛到标准要求
- 用 mock 后端承载 `EventLog` 和 `LEKey*` 行为
- 保证测试和文档都能准确反映当前能力边界

## 当前架构

### 1. `transport`

- `TlsServer`
- `TlsClient`
- 负责 TLS 握手、socket 收发、KLV 帧边界和请求响应往返

### 2. `security`

- `TlsProfile`
- `certificate_utils`
- 负责证书加载、最小 RSA 约束校验、对端 thumbprint 提取

### 3. `klv`

- 负责固定 UL + 固定 4-byte BER Length + Value 的编码解码

### 4. `asm`

- `Codec`
  - 负责命令 UL 和 `StandardRequest` / `StandardResponse` 的相互转换
- `EventLog`
  - 负责 `GetEventList` / `GetEventID` request/response payload
- `QuerySPB`
  - 负责 `Protocol_Ver` / `Status`
- `LEKey`
  - 负责标准 batch、presence、purge 响应等 payload
- `AsmService`
  - 负责 request 分发和语义处理

## 关键实现选择

### 标准命令 UL

每个 Request/Response 都使用 Annex A 的独立 UL，不再复用旧原型的统一 envelope。

### `BadRequest Response`

当前实现把以下情况统一落成 `BadRequest Response`：

- 未知 UL
- 非法 payload 长度
- `Request_ID = 0`
- 其他解码失败

响应携带收到的原始 KLV frame 作为 `Request Copy`，`Response` 固定为 `invalid`。

### `GetEventID`

当前返回未签名 XML 文本，目的是先对齐标准“Text Log Record”字段语义。后续如要继续推进，可把 XML 构造替换为真正符合 SMPTE 430-5 的日志记录。

### `LEKey*`

当前 `LEKey*` 已按标准线协议实现，但底层仍是 mock key store：

- `LEKeyLoad` 使用标准 32-byte item
- `LEKeyQueryID` 返回 `KeyPresent`
- `LEKeyQueryAll` 返回 `LEKeyIDBatch`
- `LEKeyPurgeID` 返回 `NoKeyID`
- `LEKeyPurgeAll` 返回最小成功响应

同时实现了标准要求里“至少 16 个 key/key id buffer”的最小容量约束；超过容量时，`LEKeyLoad` 返回 `Overflow != 0` 且 `Response = failed`。

## 当前不做的事情

- 完整 D-Cinema certificate profile 校验
- `TLS record size = 512 bytes` 强制控制
- `ChangeCipherSpec` 的显式特殊处理
- 真实安全模块或真实链路加密接入
- 完整 SMPTE 430-5 signed log pipeline
