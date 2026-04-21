# ISO 26430-6 ASM Implementation

Protocol-aligned C++ reference implementation of Auditorium Security Messages (ASM) from `ISO 26430-6:2009(E)` for D-Cinema intra-theater communications.

这个仓库提供一个面向 `ISO 26430-6:2009(E)` 的 ASM 参考实现，重点覆盖 Auditorium Security Messages 在线协议层的关键约束与命令语义，实现重点是：

- 按 Annex A 的命令 UL 编码 Request/Response
- 使用固定 4-byte long-form BER Length
- 在 `1173` 端口上跑同步 `Request-Response Pair`
- 用双向 TLS 1.0 + `TLS_RSA_WITH_AES_128_CBC_SHA`
- 覆盖通用命令和 `LEKey*` 命令的最小标准语义
- 用单元测试、标准报文样例和 TLS 集成测试做回归

当前实现是“协议级对齐原型”，不是生产级影院实现。它已经对齐了标准帧格式、命令字段和主要约束，但仍保留 mock 后端，并且没有宣称完全满足 D-Cinema 证书 profile。

## Repository Summary

This repository is intended for:

- learning and explaining `ISO 26430-6`
- validating ASM wire format and command semantics
- interoperability smoke tests for `TLS + KLV + ASM`
- serving as a small reference prototype for further D-Cinema security tooling

This repository is not intended to claim full production compliance with the complete D-Cinema operational environment.

## 当前实现范围

- `C++20 + OpenSSL + CMake`
- `GetTime`
- `GetEventList`
- `GetEventID`
- `QuerySPB`
- `BadRequest Response`
- `LEKeyLoad`
- `LEKeyQueryID`
- `LEKeyQueryAll`
- `LEKeyPurgeID`
- `LEKeyPurgeAll`
- 可注入事件日志 provider
- 可注入 LE key store，支持 `JSONL` 持久化

## Standard Coverage

当前代码主线主要对应以下标准部分：

- Clause 6: TLS constraints, KLV structure, general command elements, synchronous RRP model
- Clause 7: `BadRequest`, `GetTime`, `GetEventList`, `GetEventID`, `QuerySPB`
- Clause 8: `LEKeyLoad`, `LEKeyQueryID`, `LEKeyQueryAll`, `LEKeyPurgeID`, `LEKeyPurgeAll`
- Annex A: command UL mapping used by Request/Response packets

## 已知差距

- 证书校验目前只强制：
  - RSA
  - 2048-bit
  - exponent `65537`
  还没有实现完整 D-Cinema certificate profile 校验。
- `GetEventID` 目前返回未签名 XML 文本，不包含 SMPTE 430-5 的完整签名链。
- `LEKey*` 目前使用 mock key store，不接真实安全模块。
- `TLS record size = 512 bytes` 还没有在 OpenSSL 层强制控制。
- 除初始握手外 `ChangeCipherSpec` 的特殊处理尚未显式实现。

## 目录结构

```text
iso26430-6-asm-implementation/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── constraints.md
│   ├── iso26430-6-asm-implementation.md
│   └── protocol-vectors.md
├── include/
├── src/
├── tests/
├── scripts/
│   ├── generate_dev_certs.sh
│   └── run_interop_smoke.sh
├── fixtures/
│   ├── certs/dev/
│   ├── events/
│   └── keys/
└── references/
```

## 快速开始

1. 生成开发证书

```bash
./scripts/generate_dev_certs.sh
```

2. 配置并编译

```bash
cmake -S . -B build
cmake --build build
```

3. 跑测试

```bash
ctest --test-dir build --output-on-failure
```

4. 跑一键互操作检查

```bash
./scripts/run_interop_smoke.sh
```

## 手工运行

启动 responder：

```bash
./build/asm_responder \
  fixtures/certs/dev/server.cert.pem \
  fixtures/certs/dev/server.key.pem \
  fixtures/certs/dev/ca.cert.pem \
  fixtures/events/default-events.jsonl \
  fixtures/keys/default-keys.jsonl
```

发起 `GetTime`：

```bash
./build/asm_initiator 127.0.0.1 get-time \
  fixtures/certs/dev/client.cert.pem \
  fixtures/certs/dev/client.key.pem \
  fixtures/certs/dev/ca.cert.pem
```

发起 `QuerySPB`：

```bash
./build/asm_initiator 127.0.0.1 query-spb \
  fixtures/certs/dev/client.cert.pem \
  fixtures/certs/dev/client.key.pem \
  fixtures/certs/dev/ca.cert.pem
```

按时间窗获取事件 ID 列表：

```bash
./build/asm_initiator 127.0.0.1 get-event-list 1776758400 1776759000 \
  fixtures/certs/dev/client.cert.pem \
  fixtures/certs/dev/client.key.pem \
  fixtures/certs/dev/ca.cert.pem
```

获取单条事件 XML：

```bash
./build/asm_initiator 127.0.0.1 get-event-id 1002 \
  fixtures/certs/dev/client.cert.pem \
  fixtures/certs/dev/client.key.pem \
  fixtures/certs/dev/ca.cert.pem
```

加载一条 LE key：

```bash
./build/asm_initiator 127.0.0.1 le-key-load 7001 00112233445566778899aabbccddeeff 3600 123456789 \
  fixtures/certs/dev/client.cert.pem \
  fixtures/certs/dev/client.key.pem \
  fixtures/certs/dev/ca.cert.pem
```

查询全部 LE key ID：

```bash
./build/asm_initiator 127.0.0.1 le-key-query-all \
  fixtures/certs/dev/client.cert.pem \
  fixtures/certs/dev/client.key.pem \
  fixtures/certs/dev/ca.cert.pem
```

## `JSONL` 输入格式

事件文件每行一个对象：

```json
{"event_id":1001,"severity":"info","timestamp":"2026-04-21T08:00:00Z","summary":"Responder started"}
```

LE key 文件每行一个对象：

```json
{"key_id":7001,"key":"00112233445566778899aabbccddeeff","expire_time":3600,"attribute_data":123456789}
```

## 参考文档

- 约束矩阵：`docs/constraints.md`
- 标准报文样例：`docs/protocol-vectors.md`
- 实现说明：`docs/iso26430-6-asm-implementation.md`
- Beamer 讲稿源文件：`docs/beamer/iso26430-6-asm-standard-intro.tex`
- Beamer 讲稿 PDF：[`docs/beamer/iso26430-6-asm-standard-intro.pdf`](docs/beamer/iso26430-6-asm-standard-intro.pdf)
