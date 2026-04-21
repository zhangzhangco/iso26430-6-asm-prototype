First edition 2009-12-15

# Digital cinema (D-cinema) operations —

Part 6:

# Auditorium security messages for intratheater communications

Opérations du cinéma numérique (cinéma D) —

Partie 6: Messages de sécurité de salle pour les communications à l'intérieur du théâtre

## PDF disclaimer

This PDF file may contain embedded typefaces. In accordance with Adobe's licensing policy, this file may be printed or viewed but shall not be edited unless the typefaces which are embedded are licensed to and installed on the computer performing the editing. In downloading this file, parties accept therein the responsibility of not infringing Adobe's licensing policy. The ISO Central Secretariat accepts no liability in this area.

Adobe is a trademark of Adobe Systems Incorporated.

Details of the software products used to create this PDF file can be found in the General Info relative to the file; the PDF-creation parameters were optimized for printing. Every care has been taken to ensure that the file is suitable for use by ISO member bodies. In the unlikely event that a problem relating to it is found, please inform the Central Secretariat at the address given below.

![](images/25e0ccc89f2919850e6c448c5422c1cd0c2388b29313dd48380478393bfad9ac.jpg)

## COPYRIGHT PROTECTED DOCUMENT

© ISO 2009

All rights reserved. Unless otherwise specified, no part of this publication may be reproduced or utilized in any form or by any means, electronic or mechanical, including photocopying and microfilm, without permission in writing from either ISO at the address below or ISO's member body in the country of the requester.

ISO copyright office

Case postale 56 • CH-1211 Geneva 20

Tel. + 41 22 749 01 11

Fax + 41 22 749 09 47

E-mail copyright@iso.org

Web www.iso.org

Published in Switzerland

## Foreword

ISO (the International Organization for Standardization) is a worldwide federation of national standards bodies (ISO member bodies). The work of preparing International Standards is normally carried out through ISO technical committees. Each member body interested in a subject for which a technical committee has been established has the right to be represented on that committee. International organizations, governmental and non-governmental, in liaison with ISO, also take part in the work. ISO collaborates closely with the International Electrotechnical Commission (IEC) on all matters of electrotechnical standardization.

International Standards are drafted in accordance with the rules given in the ISO/IEC Directives, Part 2.

The main task of technical committees is to prepare International Standards. Draft International Standards adopted by the technical committees are circulated to the member bodies for voting. Publication as an International Standard requires approval by at least 75 % of the member bodies casting a vote.

Attention is drawn to the possibility that some of the elements of this document may be the subject of patent rights. ISO shall not be held responsible for identifying any or all such patent rights.

ISO 26430-6 was prepared by the Society of Motion Picture and Television Engineers (as SMPTE 430-6-2008) and was adopted, under a special “fast-track procedure”, by Technical Committee ISO/TC 36, Cinematography, in parallel with its approval by the ISO member bodies.

ISO 26430 consists of the following parts, under the general title Digital cinema (D-cinema) operations:

⎯ Part 1: Key delivery message [equivalent to SMPTE 430-1]

⎯ Part 2: Digital certificate [equivalent to SMPTE 430-2]

⎯ Part 3: Generic extra-theater message format [equivalent to SMPTE 430-3]

⎯ Part 4: Log record format specification [equivalent to SMPTE 430-4]

⎯ Part 5: Security log event class and constraints [equivalent to SMPTE 430-5]

⎯ Part 6: Auditorium security messages for intra-theater communications [equivalent to SMPTE 430-6]

⎯ Part 9: Key delivery bundle [equivalent to SMPTE 430-9]

## Introduction

This part of ISO 26430 comprises SMPTE 430-6-2008 and Annex ZZ (which provides equivalences between ISO standards and SMPTE standards referenced in the text).

SMPTE 430-6-2008

## SMPTE STANDARD

# D-Cinema Operations — Auditorium Security Messages for Intra-Theater Communications

Page 1 of 18 pages

Table of Contents Page   
Foreword .. 2   
1 Scope .. 3   
2 Conformance Notation . 3   
3 Normative References .. 3   
4 Glossary .. 4   
5 Overview (Informative) .... 4   
6 Message Security, RRP Structure and General Requirements. 5   
6.1 Message Security: Transport Layer Security (TLS).. 5   
6.2 Message Structure: Key-Length-Value (KLV).. 5   
6.3 General ASM Command Elements.. 6   
6.4 General TLS and RRP Requirements for Auditorium Secutiry Messages . 6   
7 General Purpose ASM Commands.. 7   
7.1 BadRequest Response . 8   
7.2 GetTime .. 8   
7.3 GetEventList.. 9   
7.4 GetEventID.. . 10   
7.5 QuerySPB . . 10   
8 Link Encryption ASM Commands . . 11   
8.1 LEKeyLoad... . 12   
8.2 LEKeyQueryID . . 13   
8.3 LEKeyQueryAll. . 14   
8.4 LEKeyPurgeID . . 14   
8.5 LEKeyPurgeAll. .. 15   
Annex A Auditorium Security Messages Variable Length Universal Label (UL) Key (Normative).. ..... 16   
Annex B Bibliography (Informative) .. .. 18

SMPTE 430-6-2008

## Foreword

SMPTE (the Society of Motion Picture and Television Engineers) is an internationally-recognized standards developing organization. Headquartered and incorporated in the United States of America, SMPTE has members in over 80 countries on six continents. SMPTE’s Engineering Documents, including Standards, Recommended Practices and Engineering Guidelines, are prepared by SMPTE’s Technology Committees. Participation in these Committees is open to all with a bona fide interest in their work. SMPTE cooperates closely with other standards-developing organizations, including ISO, IEC and ITU.

SMPTE Engineering Documents are drafted in accordance with the rules given in Part XIII of its Administrative Practices.

SMPTE Standard 430-6 was prepared by Technology Committee DC28.

## 1 Scope

The Auditorium Security Message (ASM) specification enables interoperable communication of securitycritical information (information necessary to ensure security of D-Cinema content) between devices over an intra-theater exhibition network. The specification uses Transport Layer Security (TLS) for authentication and confidentiality, and Key-Length-Value (KLV) coding for message encoding. It defines a protocol, a general purpose request-response message set and a specific message set for link encryption keying.

## 2 Conformance Notation

Normative text is text that describes elements of the design that are indispensable or contains the conformance language keywords: "shall", "should", or "may". Informative text is text that is potentially helpful to the user, but not indispensable, and can be removed, changed, or added editorially without affecting interoperability. Informative text does not contain any conformance keywords.

All text in this document is, by default, normative, except: the Introduction, any section explicitly labeled as "Informative" or individual paragraphs that start with "Note:”

The keywords "shall" and "shall not" indicate requirements strictly to be followed in order to conform to the document and from which no deviation is permitted.

The keywords, "should" and "should not" indicate that, among several possibilities, one is recommended as particularly suitable, without mentioning or excluding others; or that a certain course of action is preferred but not necessarily required; or that (in the negative form) a certain possibility or course of action is deprecated but not prohibited.

The keywords "may" and "need not" indicate courses of action permissible within the limits of the document.

The keyword “reserved” indicates a provision that is not defined at this time, shall not be used, and may be defined in the future. The keyword “forbidden” indicates “reserved” and in addition indicates that the provision will never be defined in the future.

A conformant implementation according to this document is one that includes all mandatory provisions ("shall") and, if implemented, all recommended provisions ("should") as described. A conformant implementation need not implement optional provisions ("may") and need not implement them as described.

## 3 Normative References

The following standards contain provisions which, through reference in this text, constitute provisions of this recommended practice. At the time of publication, the editions indicated were valid. All standards are subject to revision, and parties to agreements based on this recommended practice are encouraged to investigate the possibility of applying the most recent edition of the standards indicated below.

[336M] SMPTE 336M-2007, Data Encoding Protocol Using Key-Length-Value

[Dcert] SMPTE 430-2-2006, D-Cinema Operations — Digital Certificate

[IANA] Internet Assigned Numbers Authority. See www.iana.org/assignments/port-numbers

[KDM] SMPTE 430-1-2006, D-Cinema Operations — Key Delivery Message

[Log] SMPTE 430-5-2008, D-Cinema Packaging — Security Log Event Class and Constraints

[TLS] “The TLS Protocol, Version 1.0” RFC 2246 See www.ietf.org/rfc/rfc2246.txt

[TLS-AES] “AES Cyphersuites for TLS” RFC 3268 See www.ietf.org/rfc/rfc3268.txt

## SMPTE 430-6-2008

## 4 Glossary

The following acronyms are used in this specification:

ASM Auditorium Security Message

AES Advanced Encryption Standard

BER Basic Encoding Rules (ASN.1)

CBC Cipher Block Chaining

IMB Image Media Block

KLV Key Length Value

LDB Link Decryptor Block

LE Link Encryption

RRP Request Response Pair

RSA Rivest Shamir Adleman public key encryption

SHA-1 Secure Hash Algorithm revision 1

SM Security Manager

SPB Secure Processing Block

TLS Transport Layer Security

Uintx Unsigned x bit integer

UL Universal Label

UTC Coordinated Universal Time

UUID Universally Unique Identifier (ISO 11578)

## 5 Overview (Informative)

Exhibition security equipment configurations which employ remote Secure Processing Blocks (SPBs) (i.e., SPBs which are remote from that which contains the Security Manager) require a secure method of communicating with such SPBs. The generic model for this is illustrated in Figure 1.

![](images/cc8b26573f33aaafd995b55336d3480b884e0b51c5d2e4d8bab52f0ed4f3fd09.jpg)  
Figure 1 – Auditorium Security Message Model

The communication security protection mechanism needs to provide (1) confidentiality, (2) integrity, (3) authentication and (4) prevention of replay. In addition, the mechanism needs to be inexpensive to implement, and simple to support in secure silicon processors.

Message descriptions are given in terms of the Initiator and Responder (and this specification makes no distinction between messages emanating from the Security Manager vs. the Image Media Block that contains it). As used herein the generic name for a “block” is SPB.

## 6 Message Security, RRP Structure and General Requirements

The implementation of Auditorium Security Messages (ASM) shall be in the form of a “Request” from the Initiator followed by a “Response” from the Responder (recipient SPB). Each pair of messages is referred to as a Request-Response Pair (RRP).

## 6.1 Message Security: Transport Layer Security (TLS)

Message security shall be provided by communicating ASMs under Transport Layer Security (TLS) (see [TLS]). During TLS session establishment, the Initiator (which contains the Security Manager) and Responder exchange their X.509 certificates as part of the initial TLS handshake. This exchange shall be supported using D-Cinema compliant certificates as defined in the D-Cinema Digital Certificate specification [DCert].

The TLS protocol is constrained to simplify implementation, facilitate interoperability and ensure predictable processing:

The protocol shall be TLS 1.0.

2048-bit RSA using a public exponent value of 65537 shall be the only supported public key algorithm.

 AES-CBC 128-bit shall be the only supported symmetric cipher (see [TLS-AES]).

SHA-1 shall be the only supported hash algorithm.

 The CipherSuite shall be “TLS\_RSA\_WITH\_AES\_128\_CBC\_SHA” (0x00, 0x2F) (see [TLS-AES]).

The TLS record size shall equal 512 bytes.

 The Compression Method shall be “null” (no compression).

 Other than as part of the opening handshake, the ChangeCipherSpec message shall be ignored.

## 6.2 Message Structure: Key Length Value (KLV)

Request and Response ASMs shall be Key Length Value (KLV) encoded using Fixed Length Pack encoding according to SMPTE 336M-2001 [336M]. The Fixed Length Universal Label (UL) Key is given in Annex A of this document. As a Fixed Length Pack, each individual item in the Value field comprises only an item value. The KLV Length field shall be a long-form BER value encoded with a fixed length of 4 bytes total.

Example: For a KLV packet having a Value field that is 12 bytes in length, the Length field would be encoded as the following 4 bytes, 0x83 0x00 0x00 0x0C (hexadecimal).

Each ASM Request-Response Pair (RRP) represents two message types and thus KLV UL “value” registration is required twice for each defined RRP (see Annex A).

Informative Note: The recipient of each RRP Request or Response command is implicit by virtue of the TLS socket (which is known at the applications level) that carries the messages.

## 6.3 General ASM Command Elements

For each message type, the following shall apply:

 The command type is denoted within the opening KLV “Key” field (16 bytes).

 “Length” is a BER-encoded four byte field which describes the length of the message in bytes.

“Request\_ID” shall be an application level tag for the Request, which shall be echoed by the corresponding Response. A non-zero Request\_ID value shall be set by the SM, which should select unique values (e.g. a sequencing counter) for each TLS connection it manages. (Request\_ID generation means is left to implementers and is out of scope of this specification.)

 Multi-byte integer values shall be sent as big-endian data, meaning most significant byte first.

General “Response” elements for each Response command are defined as follows:

General Response Elements
<table><tr><td>Element</td><td>Meaning</td><td>Ulnt8 Value</td><td></td></tr><tr><td>RRP successful</td><td>Request successfully processed</td><td>0</td><td></td></tr><tr><td>RRP failed</td><td>Responder unable to process Request</td><td>1</td><td></td></tr><tr><td>RRP Invalid</td><td>Invalid parameter or command structure</td><td>2</td><td></td></tr><tr><td>ResponderBusy</td><td>Responder too busy to process Request</td><td>3</td><td></td></tr></table>

Messages defined in this document may contain batches. A batch is a compound data type that is created from combinations of simple data types. It is usually preceded by a name (e.g. an EventIDBatch is an unordered batch of Event ID values):

Batch: A compound type comprising multiple individual elements. The elements are unordered, the type is defined, the count of elements is explicit and the size of each element is fixed and explicit.

xxxBatch: A batch of zero or more individual elements of name “xxx” preceded by a header of 8 bytes. The first 4 bytes of the header define the number of elements to follow and the second 4 bytes define the length of each element, both represented as UInt32.
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td><td rowspan=1 colspan=1>Default</td></tr><tr><td rowspan=1 colspan=1>Number of Items</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>The number of Items in the Batch</td><td rowspan=1 colspan=1>n</td></tr><tr><td rowspan=1 colspan=1>Item Length</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>The length of each Item</td><td rowspan=1 colspan=1>L</td></tr><tr><td rowspan=1 colspan=1>First component of firstinstance of xxx</td><td rowspan=1 colspan=1>…</td><td rowspan=1 colspan=1>…</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>First of one or more components describingelement &#x27;xxx&#x27; and having a total length of L</td><td rowspan=1 colspan=1>…</td></tr></table>

## 6.4 General TLS and RRP Requirements for Auditorium Security Messages

This section defines implementation constraints for security assurance, interoperability, RRP contention management and serendipity with other exhibition subsystems which may use network resources shared by these security functions.

1. TLS sessions shall be established by the Initiator following the standard applications level TLS handshake protocol using mutual authentication mode (see [TLS]). Mutual authentication shall exchange both TLS client (Initiator) and server (Responder) D-Cinema compliant certificates.

Informative Note: Certificate utility at each TLS end point is out of scope of this specification; however the purpose of mutual authentication is to enable the Responder (remote SPB) to receive the Initiator’s (Image Media Block) certificate to record its thumbprint for logging purposes.

2. RRP protocols shall be synchronous (i.e., each pairing shall be opened and closed before a new RRP is opened between the same two SPBs). To avoid hang-ups, RRP Responder implementations should be designed to support maximum round-trip Request-to-Response latencies as specified in the message definition sections below. Latency shall be measured from the end of the “Request” message receipt to the start of the “Response” message transmission. Responders unable to transmit the Response within the specified limit because of a “busy” condition should close that RRP duple by issuance of a BadRequest Response with the general Response element indicating “busy” per the General Response Elements table in Section 6.3.

Informative Note: Should the Responder fail to respond (at all) after the specified time limit, the Initiator may consider this a communications failure condition and may, for instance, close and restart the TLS session.

3. SMPTE standardized ASM security messages shall use well-known port 1173, which has been reserved for D-Cinema “security” RRPs by the Internet Assigned Numbers Authority (see [IANA]).

Informative Note: Non-standardized, or non-security related RRPs may exist to support other functionality, however such RRPs should use a different port.

## 7 General Purpose ASM Commands

This section defines ASM commands which support remote SPBs generally (i.e. independently of the specific type of SPB or contained security functions). Table 1 shows these commands together with the names as recorded in the SMPTE UL metadata registries.

Request-Response round trip latency – Per item (2) of Section 6.4, Responder implementations should support a maximum round-trip Request-to-Response latency of 2 seconds for general purpose ASM commands.

Table 1 – General Purpose ASM Command Types
<table><tr><td rowspan=1 colspan=1>General Purpose ASMCommands</td><td rowspan=1 colspan=1>SMPTE Metadata General PurposeASM Command UL Name</td></tr><tr><td rowspan=1 colspan=1>BadRequest Request</td><td rowspan=1 colspan=1>Bad Request Response</td></tr><tr><td rowspan=1 colspan=1>GetTime_Request</td><td rowspan=1 colspan=1>Time Request</td></tr><tr><td rowspan=1 colspan=1>GetTime Response</td><td rowspan=1 colspan=1>Time Response</td></tr><tr><td rowspan=1 colspan=1>GetEventList Request</td><td rowspan=1 colspan=1>Event List Request</td></tr><tr><td rowspan=1 colspan=1>GetEventList Response</td><td rowspan=1 colspan=1>Event List Response</td></tr><tr><td rowspan=1 colspan=1>GetEventID Request</td><td rowspan=1 colspan=1>Event ID Request</td></tr><tr><td rowspan=1 colspan=1>GetEventID Response</td><td rowspan=1 colspan=1>Event ID Response</td></tr><tr><td rowspan=1 colspan=1>QuerySPB Request</td><td rowspan=1 colspan=1>Secure Processing Block Query Request</td></tr><tr><td rowspan=1 colspan=1>QuerySPB Response</td><td rowspan=1 colspan=1>Secure Processing Block Query Response</td></tr></table>

## 7.1 BadRequest Response

Each RRP Response command contains a general “Response” element; however instances may arise where the Responder does not understand the incident Request command that was received. In such case the appropriate Response command would be unknown. The BadRequest Response shall be used when the Recipient cannot otherwise respond with a Response command appropriate to the incident Request. A complete copy of the Request command as received by the Responder is carried in this message (an exception is noted below).

BadRequest Response
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td></tr><tr><td rowspan=1 colspan=1>Bad Request Response UL Name</td><td rowspan=1 colspan=1>Pack Key</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Identifies the BadRequest Response</td></tr><tr><td rowspan=1 colspan=1>Length</td><td rowspan=1 colspan=1>BER Length</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>Pack Length</td></tr><tr><td rowspan=1 colspan=1>Request Copy</td><td rowspan=1 colspan=1>Text</td><td rowspan=1 colspan=1>Var</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Copy of Request</td></tr><tr><td rowspan=1 colspan=1>Response</td><td rowspan=1 colspan=1>Uint8</td><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>General Response info</td></tr></table>

 The Request\_Copy is a complete copy of the Request command that was not understood.

 The length of Request\_Copy can be determined from the Length of the message.

No copy of the incident Request shall be carried in the BadRequest Response in the event that a Responder is “busy” per the General Response Elements table of Section 6.3. In such case the “Request Copy” field shall be null; that is, of length zero.

## 7.2 GetTime

The GetTime command returns a snapshot of the Responder’s absolute UTC time. The units of Time shall be seconds.

GetTime Request
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td></tr><tr><td rowspan=1 colspan=1>Time Request UL Name</td><td rowspan=1 colspan=1>Pack Key</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Identifies the GetTime Request</td></tr><tr><td rowspan=1 colspan=1>Length</td><td rowspan=1 colspan=1>BER Length</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>Pack length</td></tr><tr><td rowspan=1 colspan=1>Request ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>ID of this Request</td></tr></table>

GetTime Response
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td></tr><tr><td rowspan=1 colspan=1>Time Response UL Name</td><td rowspan=1 colspan=1>Pack Key</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Identifies the GetTime Response</td></tr><tr><td rowspan=1 colspan=1>Length</td><td rowspan=1 colspan=1>BER Length</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>Pack length</td></tr><tr><td rowspan=1 colspan=1>Request ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>IDs the Request for which this is the Response</td></tr><tr><td rowspan=1 colspan=1>Time</td><td rowspan=1 colspan=1>Uint64</td><td rowspan=1 colspan=1>8</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Responder&#x27;s time</td></tr><tr><td rowspan=1 colspan=1>Response</td><td rowspan=1 colspan=1>Uint8</td><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>General Response info</td></tr></table>

Time shall be a 64-bit integer representing the number of seconds elapsed since January 1, 1970 00:00:00 UTC. The Time value shall be taken at the instant that the GetTime Response message is queued for transmission to the Initiator.

Informative Note: The Recipient device clock reports absolute time (UTC). The SM will use the GetTime command to determine the difference between true time (the SM’s time) and time in the remote SPB, and remove the delta in log reporting. Accuracy requirements are out of scope of this specification, but it is assumed that corrections for clock drift or other offsets (e.g. leap seconds) are unnecessary.

## 7.3 GetEventList

The GetEventList command identifies a UTC TimeStart and TimeStop period for which the Responder will respond with a list of identified logged event records which it holds.

Informative Note: By appropriate management of start/stop periods, the Initiator is expected to keep track of the collection of log information from a remote SPB and assure no gaps exist across linear time. There are no restrictions placed upon the Initiator regarding the time start/stop times (e.g. book-ending or overlaps) or whether log information has been previously collected. A Responder will simply respond to all Requests. Requirements for when log data is collected or how long it is retained by remote SPBs are out of scope of this specification.

GetEventList Request
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td></tr><tr><td rowspan=1 colspan=1>Event List Request UL Name</td><td rowspan=1 colspan=1>Pack Key</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Identifies the GetEventList Request</td></tr><tr><td rowspan=1 colspan=1>Length</td><td rowspan=1 colspan=1>BER Length</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>Pack length</td></tr><tr><td rowspan=1 colspan=1>Request ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>ID of this Request</td></tr><tr><td rowspan=1 colspan=1>TimeStart</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Event list period start</td></tr><tr><td rowspan=1 colspan=1>TimeStop</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Event list period stop</td></tr></table>

The TimeStart and TimeStop elements define a UTC based time window for which logged event information is being requested.

GetEventList Response
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td></tr><tr><td rowspan=1 colspan=1>Event List Response UL Name</td><td rowspan=1 colspan=1>Pack Key</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Identifies the GetEventList Response</td></tr><tr><td rowspan=1 colspan=1>Length</td><td rowspan=1 colspan=1>BER Length</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>Pack length</td></tr><tr><td rowspan=1 colspan=1>Request ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>IDs the Request for which this is the Response</td></tr><tr><td rowspan=1 colspan=1>EventIDBatch</td><td rowspan=1 colspan=1>Event ID Batch</td><td rowspan=1 colspan=1>8+4n</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>An unordered Batch of Event ID values(see table below)</td></tr><tr><td rowspan=1 colspan=1>Response</td><td rowspan=1 colspan=1>Uint8</td><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>General Response info</td></tr></table>

EventIDBatch
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td><td rowspan=1 colspan=1>Default</td></tr><tr><td rowspan=1 colspan=1>Number of Items</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>The number of Items in the Batch</td><td rowspan=1 colspan=1>n</td></tr><tr><td rowspan=1 colspan=1>Item Length</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>The length of each Item</td><td rowspan=1 colspan=1>4</td></tr><tr><td rowspan=1 colspan=1>Event ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Unique event identifier(s)</td><td rowspan=1 colspan=1></td></tr></table>

The EventIDBatch is a batch of Event IDs. These Event IDs are in no particular order. There shall be only one entry for each Event ID in the EventIDBatch.

The length of the variable portion of EventIDBatch can be determined by multiplying the Number\_of\_Items by the Item\_length.

The batch format returns the Event ID(s) for all the events recorded with time stamps within the specified Request time window (including the TimeStart and TimeStop instances).

## 7.4 GetEventID

The GetEventID command is used to request specific log event records by Event ID (from the list returned in the GetEventList Response). The Responder returns the requested log record.

GetEventID Request
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td></tr><tr><td rowspan=1 colspan=1>Event ID Request UL Name</td><td rowspan=1 colspan=1>Pack Key</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Identifies the GetEventID Request</td></tr><tr><td rowspan=1 colspan=1>Length</td><td rowspan=1 colspan=1>BER Length</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>Pack length</td></tr><tr><td rowspan=1 colspan=1>Request ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>ID of this Request</td></tr><tr><td rowspan=1 colspan=1>Event ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>ID of the requested event</td></tr></table>

The Event ID identifies a specific log record.

GetEventID Response
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td></tr><tr><td rowspan=1 colspan=1>Event ID Response UL Name</td><td rowspan=1 colspan=1>Pack Key</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Identifies the GetEventID Response</td></tr><tr><td rowspan=1 colspan=1>Length</td><td rowspan=1 colspan=1>BER Length</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>Pack length</td></tr><tr><td rowspan=1 colspan=1>Request ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>IDs the Request for which this is the Response</td></tr><tr><td rowspan=1 colspan=1>Log Record</td><td rowspan=1 colspan=1>Text</td><td rowspan=1 colspan=1>Var</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Record being delivered</td></tr><tr><td rowspan=1 colspan=1>Response</td><td rowspan=1 colspan=1>Uint8</td><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>General Response info</td></tr></table>

Log Record is the logged information for the specified event. It is a signed or unsigned XML log data record constructed as defined in [Log] and carried as text within the KLV structure.

 The length of the Log Record item can be determined from the Length of the message.

Informative Note: The utility of the GetEventList and GetEventID messages is to provide a method to identify and move security log data from a remote SPB to the Image Media Block (IMB containing the Security Manager). The TLS connection between Initiator (IMB) and Responder (remote SPB) provides a trusted and secure path for the log record “signing proxy” technique as described in [Log]. Provisioning for log record filtering (as described in [Log]) is unaffected by these RRP messages, and outside the scope of this specification.

## 7.5 QuerySPB

The QuerySPB command is used to interrogate an SPB as to health and status.

QuerySPB Request
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td></tr><tr><td rowspan=1 colspan=1>Secure Processing Block QueryRequest UL Name</td><td rowspan=1 colspan=1>Pack Key</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Identifies the QuerySPB Request</td></tr><tr><td rowspan=1 colspan=1>Length</td><td rowspan=1 colspan=1>BER Length</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>Pack length</td></tr><tr><td rowspan=1 colspan=1>Request ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>ID of this Request</td></tr></table>

QuerySPB Response
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td></tr><tr><td rowspan=1 colspan=1>Secure Processing Block QueryResponse UL Name</td><td rowspan=1 colspan=1>Pack Key</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Identifies the QuerySPB Response</td></tr><tr><td rowspan=1 colspan=1>Length</td><td rowspan=1 colspan=1>BER Length</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>Pack length</td></tr><tr><td rowspan=1 colspan=1>Request ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>IDs the Request for which this is the Response</td></tr><tr><td rowspan=1 colspan=1>Protocol_Ver</td><td rowspan=1 colspan=1>Uint8</td><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>ASM Protocol in Use</td></tr><tr><td rowspan=1 colspan=1>Status</td><td rowspan=1 colspan=1>Uint8</td><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Status information</td></tr><tr><td rowspan=1 colspan=1>Response</td><td rowspan=1 colspan=1>Uint8</td><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>General response info</td></tr></table>

Protocol\_Ver returns a value indicating the ASM protocol suite being used by the Responder. The initial version value of this field shall be 0x01.

Status items are defined as follows:

<table><tr><td>Element</td><td>Meaning</td><td>Value</td><td></td></tr><tr><td>Not playing</td><td>Recipient is not performing a playout security function</td><td>o</td><td></td></tr><tr><td>Playing</td><td>Recipient is performing a playout security function</td><td>1</td><td></td></tr><tr><td>Security Alert</td><td>Recipient is reporting a security alert condition</td><td>2</td><td></td></tr></table>

Informative Note: Examples of security alert conditions would be an open access panel or failed clock.

## 8 Link Encryption ASM Commands

Table 2 lists the ASM commands defined for support of Link Encryption (LE). Table 1 shows these commands together with the names as recorded in the SMPTE UL metadata registries.

Request-Response round trip latency — Per item (2) of Section 6.4, Responder implementations should support a maximum round-trip Request-to-Response latency of 2 seconds for Link Encryption ASM commands.

Responder key buffer — The Link Encryption Responder endpoint shall have a buffer for at least 16 Keys and Key\_IDs.

Informative Note: Other messages (or message sets) may be defined in the future (e.g. to support keying of remote SPBs).

Table 2 – Link Encryption ASM Command Types
<table><tr><td rowspan=1 colspan=1>Link Encryption ASM Commands</td><td rowspan=1 colspan=1>SMPTE Metadata Link Encryption ASM Command UL Name</td></tr><tr><td rowspan=1 colspan=1>LEKeyLoad Request</td><td rowspan=1 colspan=1>Link Encryption Key Load Request</td></tr><tr><td rowspan=1 colspan=1>LEKeyLoad Response</td><td rowspan=1 colspan=1>Link Encryption Key Load Response</td></tr><tr><td rowspan=1 colspan=1>LEKeyQueryID Request</td><td rowspan=1 colspan=1>Link Encryption Key Query ID Request</td></tr><tr><td rowspan=1 colspan=1>LEKeyQueryID Response</td><td rowspan=1 colspan=1>Link Encryption Key Query ID Response</td></tr><tr><td rowspan=1 colspan=1>LEKeyQuery Request</td><td rowspan=1 colspan=1>Link Encryption Key Query All Request</td></tr><tr><td rowspan=1 colspan=1>LEKeyQuery Response</td><td rowspan=1 colspan=1>Link Encryption Key Query All Response</td></tr><tr><td rowspan=1 colspan=1>LEKeyQuery All Request</td><td rowspan=1 colspan=1>Link Encryption Purge ID Request</td></tr><tr><td rowspan=1 colspan=1>LEKeyQuery All Request</td><td rowspan=1 colspan=1>Link Encryption Purge ID Response</td></tr><tr><td rowspan=1 colspan=1>LEKeyPurgeID Request</td><td rowspan=1 colspan=1>Link Encryption Purge All Request</td></tr><tr><td rowspan=1 colspan=1>LEKeyPurgeID Response</td><td rowspan=1 colspan=1>Link Encryption Purge All Response</td></tr></table>

## 8.1 LEKeyLoad

The LEKeyLoad command delivers LE keys to a Link Decryptor Block (LDB). The use of the KLV batch facility enables the command to carry more than one key.

Informative Note: Synchronization of the current decryption key to be used is coordinated through the use of an in-band (i.e., essence-borne) metadata indicator not defined in this document.

LEKeyLoad Request
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td></tr><tr><td rowspan=1 colspan=1>Link Encryption Key Load RequestUL Name</td><td rowspan=1 colspan=1>Pack Key</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Identifies the LEKeyLoad Request command</td></tr><tr><td rowspan=1 colspan=1>Length</td><td rowspan=1 colspan=1>BER Length</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>Pack length</td></tr><tr><td rowspan=1 colspan=1>Request ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>ID of this Request</td></tr><tr><td rowspan=1 colspan=1>LEKeyBatch</td><td rowspan=1 colspan=1>LE Key Batch</td><td rowspan=1 colspan=1>8+(32*n)</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>An unordered batch of LE Key ID to Key,Expire Time and Attribute Data mappings(see table below)</td></tr></table>

LEKeyBatch
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td><td rowspan=1 colspan=1>Default</td></tr><tr><td rowspan=1 colspan=1>Number of Items</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>The number of Items in the Batch</td><td rowspan=1 colspan=1>n</td></tr><tr><td rowspan=1 colspan=1>Item Length</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>The length of each Item</td><td rowspan=1 colspan=1>32</td></tr><tr><td rowspan=1 colspan=1>LE Key ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Unique key identifier</td><td rowspan=1 colspan=1></td></tr><tr><td rowspan=1 colspan=1>Key</td><td rowspan=1 colspan=1>Array of Bytes</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Decryption key</td><td rowspan=1 colspan=1></td></tr><tr><td rowspan=1 colspan=1>Expire Time</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Validity period of key in seconds</td><td rowspan=1 colspan=1></td></tr><tr><td rowspan=1 colspan=1>Attribute Data</td><td rowspan=1 colspan=1>Uint64</td><td rowspan=1 colspan=1>8</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Data used as input to counter mode cipher</td><td rowspan=1 colspan=1></td></tr></table>

Attribute\_Data is used to seed the AES core in counter mode for link decryption. This number shall be a unique random number produced by the SM for each LE key.

The LEKeyBatch is a batch of LE Key ID to Key, Expire Time and Attribute Data mappings. The quadruplets are in no particular order. There shall be only one entry for each LE Key ID in the LEKeyBatch.

 The length of the variable portion of the LEKeyBatch can be determined by multiplying the Number\_of\_Items by the Item\_length.

Expire\_Time is the valid length of time for key use in seconds, after which the SM should purge the key. Upon receipt of the key, the receiving SPB shall track time in seconds.

 LE\_Key\_ID is a unique identifier for each individual key. (The LE\_Key\_ID for LE keys is not a UUID.)

Key is the 128 bit LE key.

LEKeyLoad Response
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td></tr><tr><td rowspan=1 colspan=1>Link Encryption Key Load ResponseUL Name</td><td rowspan=1 colspan=1>Pack Key</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Identifies the LEKeyLoad Response message</td></tr><tr><td rowspan=1 colspan=1>Length</td><td rowspan=1 colspan=1>BER Length</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>Pack length</td></tr><tr><td rowspan=1 colspan=1>Request ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>IDs the Request for which this is the Response</td></tr><tr><td rowspan=1 colspan=1>Overflow</td><td rowspan=1 colspan=1>Uint8</td><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Key buffer full</td></tr><tr><td rowspan=1 colspan=1>Response</td><td rowspan=1 colspan=1>Uint8</td><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>General Response Info</td></tr></table>

A non-zero Overflow value indicates that the LEKeyLoad Request has not been successfully executed because the LDB key buffer would have overflowed. A non-zero value shall also cause the Response element to indicate “failed RRP”.

Informative Note: LDB devices may have different key and key ID memory buffer capacities. The Overflow element allows the Responder to inform the Initiator when attempts to load LE keys would overwrite active keys.

## 8.2 LEKeyQueryID

The LEKeyQueryID command interrogates the LDB for the presence of a key which is identified by KeyID.

LEKeyQueryID Request
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td></tr><tr><td rowspan=1 colspan=1>Link Encryption Key Query ID RequestUL Name</td><td rowspan=1 colspan=1>Pack Key</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Identifies the LEKeyQueryID Request</td></tr><tr><td rowspan=1 colspan=1>Length</td><td rowspan=1 colspan=1>BER Length</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>Pack length</td></tr><tr><td rowspan=1 colspan=1>Request ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>ID of this Request</td></tr><tr><td rowspan=1 colspan=1>LE Key ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Unique LE Key ID</td></tr></table>

 LE Key ID contains the ID of the LE key to be queried.

LEKeyQueryID Response
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td></tr><tr><td rowspan=1 colspan=1>Link Encryption Key Query ID ResponseUL Name</td><td rowspan=1 colspan=1>Pack Key</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Identifies the LEKeyQueryID Response</td></tr><tr><td rowspan=1 colspan=1>Length</td><td rowspan=1 colspan=1>BER Length</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>Pack length</td></tr><tr><td rowspan=1 colspan=1>Request ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>IDs the Request for which this is theResponse</td></tr><tr><td rowspan=1 colspan=1>KeyPresent</td><td rowspan=1 colspan=1>Uint8</td><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>&quot;1&quot; if the key is present, &quot;0&quot; otherwise</td></tr><tr><td rowspan=1 colspan=1>Response</td><td rowspan=1 colspan=1>Uint8</td><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>General Response info</td></tr></table>

 KeyPresent shall have a value of one (1) if the key identified by LE Key ID is present; the value shall otherwise be zero (0).

## 8.3 LEKeyQueryAll

The LEKeyQueryAll command interrogates the LDB to report all of its active LE keys.

LEKeyQueryAll Request
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td></tr><tr><td rowspan=1 colspan=1>Link Encryption Key Query All RequestUL Name</td><td rowspan=1 colspan=1>Pack Key</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Identifies the LEKeyQueryAll Request</td></tr><tr><td rowspan=1 colspan=1>Length UL Name</td><td rowspan=1 colspan=1>BER Length</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>Pack length</td></tr><tr><td rowspan=1 colspan=1>Request ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>ID of this Request</td></tr></table>

LEKeyQueryAll Response
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td></tr><tr><td rowspan=1 colspan=1>Link Encryption Key Query All ResponseUL Name</td><td rowspan=1 colspan=1>Pack Key</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Identifies the LEKeyQueryAll Response</td></tr><tr><td rowspan=1 colspan=1>Length</td><td rowspan=1 colspan=1>BER Length</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>Pack length</td></tr><tr><td rowspan=1 colspan=1>Request ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>IDs the Request for which this is the Response</td></tr><tr><td rowspan=1 colspan=1>LEKeyIDBatch</td><td rowspan=1 colspan=1>LE Key IDBatch</td><td rowspan=1 colspan=1>8+4n</td><td rowspan=1 colspan=1>−</td><td rowspan=1 colspan=1>An unordered batch of LE Key ID values(see table below)</td></tr><tr><td rowspan=1 colspan=1>Response</td><td rowspan=1 colspan=1>Uint8</td><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>General Response info</td></tr></table>

LEKeyID Batch
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td><td rowspan=1 colspan=1>Default</td></tr><tr><td rowspan=1 colspan=1>Number of Items</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>The number of Items in the Batch</td><td rowspan=1 colspan=1>n</td></tr><tr><td rowspan=1 colspan=1>Item Length</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>The length of each Item</td><td rowspan=1 colspan=1>4</td></tr><tr><td rowspan=1 colspan=1>LE Key ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Unique key identifier(s)</td><td rowspan=1 colspan=1></td></tr></table>

LEKeyID Batch returns all the active LE Key IDs within the LDB. A Number\_of\_items of zero in the LEKeyID Batch shall mean no LE keys are present.

The length of the variable portion of the LEKeyID Batch can be determined by multiplying the Number of items by the Item length.

The LEKeyID Batch is a batch of LE Key ID values. The LE Key IDs are in no particular order. There shall be only one entry for each LE Key ID in the LEKeyID Batch.

## 8.4 LEKeyPurgeID

LEKeyPurgeID commands an LDB to purge (zeroize) the key identified by LE Key ID.

LEKeyPurgeID Request
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td></tr><tr><td rowspan=1 colspan=1>Link Encryption Purge ID RequestUL Name</td><td rowspan=1 colspan=1>Pack Key</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Identifies the LEKeyPurgelD Request</td></tr><tr><td rowspan=1 colspan=1>Length</td><td rowspan=1 colspan=1>BER Length</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>Pack length</td></tr><tr><td rowspan=1 colspan=1>Request ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>ID of this Request</td></tr><tr><td rowspan=1 colspan=1>LE Key ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Unique Key ID</td></tr></table>

 LE Key ID contains the ID of the LE key to be purged.

Page 14 of 18 pages

LEKeyPurgeID Response
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td></tr><tr><td rowspan=1 colspan=1>Link Encryption Purge ID ResponseUL Name</td><td rowspan=1 colspan=1>Pack Key</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Identifies the LEKeyPurgelD Response</td></tr><tr><td rowspan=1 colspan=1>Length</td><td rowspan=1 colspan=1>BER Length</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>Pack length</td></tr><tr><td rowspan=1 colspan=1>Request ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>IDs the Request for which this is the Response</td></tr><tr><td rowspan=1 colspan=1>NoKeyID</td><td rowspan=1 colspan=1>Uint8</td><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Key ID not present</td></tr><tr><td rowspan=1 colspan=1>Response</td><td rowspan=1 colspan=1>Uint8</td><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>General Response info</td></tr></table>

A non-zero value for NoKeyID indicates that the identified key to be purged was not present. The value of Response for this condition is “RRP Successful”.

## 8.5 LEKeyPurgeAll

LEKeyPurgeAll commands an LDB to purge (zeroize) all its LE keys.

LEKeyPurgeAll Request
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td></tr><tr><td rowspan=1 colspan=1>Link Encryption Purge All RequestUL Name</td><td rowspan=1 colspan=1>Pack Key</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Identifies the LEKeyPurgeAll Request</td></tr><tr><td rowspan=1 colspan=1>Length</td><td rowspan=1 colspan=1>BER Length</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>Pack length</td></tr><tr><td rowspan=1 colspan=1>Request ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>ID of this Request</td></tr></table>

LEKeyPurgeAll Response
<table><tr><td rowspan=1 colspan=1>Item Name</td><td rowspan=1 colspan=1>Type</td><td rowspan=1 colspan=1>Len</td><td rowspan=1 colspan=1>UL</td><td rowspan=1 colspan=1>Meaning</td></tr><tr><td rowspan=1 colspan=1>Link Encryption Purge All ResponseUL Name</td><td rowspan=1 colspan=1>Pack Key</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Identifies the LEKeyPurgeAll Response</td></tr><tr><td rowspan=1 colspan=1>Length</td><td rowspan=1 colspan=1>BER Length</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>n/a</td><td rowspan=1 colspan=1>Pack length</td></tr><tr><td rowspan=1 colspan=1>Request ID</td><td rowspan=1 colspan=1>Uint32</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>−</td><td rowspan=1 colspan=1>IDs the Request for which this is theResponse</td></tr><tr><td rowspan=1 colspan=1>Response</td><td rowspan=1 colspan=1>Uint8</td><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>General Response info</td></tr></table>

Annex A (Normative)

Auditorium Security Messages Variable Length Universal Label (UL) Key

As a Fixed Length Pack (group of KLV elements), each individual item in the Value field comprises only an item value. All items in a pack are required.

Table A.1 – Common UL Key Value for all ASM Commands
<table><tr><td>Byte No.</td><td>Description</td><td>Value (hex)</td><td>Meaning</td></tr><tr><td>1</td><td>Object Identifier</td><td>06h</td><td>Object ID</td></tr><tr><td>2</td><td>Label size</td><td>0Eh</td><td>Length of UL</td></tr><tr><td>3</td><td>Designator</td><td>2Bh</td><td>Sub Identifier</td></tr><tr><td>4</td><td>Designator</td><td>34h</td><td>SMPTE Identifier</td></tr><tr><td>5</td><td>Registry Category Designator</td><td>02h</td><td>KLV Groups (Sets and Packs)</td></tr><tr><td>6</td><td>Registry Designator</td><td>05h</td><td>Fixed Length Pack</td></tr><tr><td>7</td><td>Structure Designator</td><td>01h</td><td>Groups Dictionary</td></tr><tr><td>∞</td><td>Version Number</td><td>01h</td><td>Registry Version: Dictionary version 1</td></tr><tr><td>9</td><td>Item Designator</td><td>02h</td><td>Administration</td></tr><tr><td>10</td><td>Organization</td><td>07h</td><td>Access Control</td></tr><tr><td>11</td><td>Application</td><td>01h</td><td>Auditorium Security</td></tr><tr><td>12</td><td>Set/Pack Kind (1)</td><td>Xx</td><td>Command Type (see Annex Table A.2)</td></tr><tr><td>13</td><td>Set/Pack Kind (2)</td><td>yy</td><td>Command Type (see Annex Table A.2)</td></tr><tr><td>14</td><td>Reserved</td><td>00h</td><td>Not assigned</td></tr><tr><td>15</td><td>Reserved</td><td>00h</td><td>Not assigned</td></tr><tr><td>16</td><td>Reserved</td><td>00h</td><td>Not assigned</td></tr></table>

The values of bytes 12 and 13 for specified ASM command types are given in Table A.2.

Table A.2 – Key Values for Command Types
<table><tr><td rowspan=1 colspan=1>Message Categoryand byte 12 node names</td><td rowspan=1 colspan=1>SMPTE UL Name Command Typeand byte 13 node names</td><td rowspan=1 colspan=1>Byte 12</td><td rowspan=1 colspan=1>Byte 13</td></tr><tr><td rowspan=1 colspan=1>General Purpose Logs</td><td rowspan=1 colspan=1>Bad Request Response</td><td rowspan=1 colspan=1>01h</td><td rowspan=1 colspan=1>01h</td></tr><tr><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td></tr><tr><td rowspan=1 colspan=1>Temporal Indicators</td><td rowspan=1 colspan=1>Time Request</td><td rowspan=1 colspan=1>02h</td><td rowspan=1 colspan=1>10h</td></tr><tr><td rowspan=1 colspan=1>Temporal Indicators</td><td rowspan=1 colspan=1>Time Response</td><td rowspan=1 colspan=1>02h</td><td rowspan=1 colspan=1>11h</td></tr><tr><td rowspan=1 colspan=1>Temporal Indicators</td><td rowspan=1 colspan=1>Event List Request</td><td rowspan=1 colspan=1>02h</td><td rowspan=1 colspan=1>12h</td></tr><tr><td rowspan=1 colspan=1>Temporal Indicators</td><td rowspan=1 colspan=1>Event List Response</td><td rowspan=1 colspan=1>02h</td><td rowspan=1 colspan=1>13h</td></tr><tr><td rowspan=1 colspan=1>Temporal Indicators</td><td rowspan=1 colspan=1>Event ID Request</td><td rowspan=1 colspan=1>02h</td><td rowspan=1 colspan=1>14h</td></tr><tr><td rowspan=1 colspan=1>Temporal Indicators</td><td rowspan=1 colspan=1>Event ID Response</td><td rowspan=1 colspan=1>02h</td><td rowspan=1 colspan=1>15h</td></tr><tr><td rowspan=1 colspan=1>Temporal Indicators</td><td rowspan=1 colspan=1>Secure Processing Block Query Request</td><td rowspan=1 colspan=1>02h</td><td rowspan=1 colspan=1>16h</td></tr><tr><td rowspan=1 colspan=1>Temporal Indicators</td><td rowspan=1 colspan=1>Secure Processing Block Query Response</td><td rowspan=1 colspan=1>02h</td><td rowspan=1 colspan=1>17h</td></tr><tr><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td></tr><tr><td rowspan=1 colspan=1>Link Encryption</td><td rowspan=1 colspan=1>Link Encryption Key Load Request</td><td rowspan=1 colspan=1>03h</td><td rowspan=1 colspan=1>20h</td></tr><tr><td rowspan=1 colspan=1>Link Encryption</td><td rowspan=1 colspan=1>Link Encryption Key Load Response</td><td rowspan=1 colspan=1>03h</td><td rowspan=1 colspan=1>21h</td></tr><tr><td rowspan=1 colspan=1>Link Encryption</td><td rowspan=1 colspan=1>Link Encryption Key Query ID Request</td><td rowspan=1 colspan=1>03h</td><td rowspan=1 colspan=1>22h</td></tr><tr><td rowspan=1 colspan=1>Link Encryption</td><td rowspan=1 colspan=1>Link Encryption Key Query ID Response</td><td rowspan=1 colspan=1>03h</td><td rowspan=1 colspan=1>23h</td></tr><tr><td rowspan=1 colspan=1>Link Encryption</td><td rowspan=1 colspan=1>Link Encryption Key Query All Request</td><td rowspan=1 colspan=1>03h</td><td rowspan=1 colspan=1>24h</td></tr><tr><td rowspan=1 colspan=1>Link Encryption</td><td rowspan=1 colspan=1>Link Encryption Key Query All Response</td><td rowspan=1 colspan=1>03h</td><td rowspan=1 colspan=1>25h</td></tr><tr><td rowspan=1 colspan=1>Link Encryption</td><td rowspan=1 colspan=1>Link Encryption Purge ID Request</td><td rowspan=1 colspan=1>03h</td><td rowspan=1 colspan=1>26h</td></tr><tr><td rowspan=1 colspan=1>Link Encryption</td><td rowspan=1 colspan=1>Link Encryption Purge ID Response</td><td rowspan=1 colspan=1>03h</td><td rowspan=1 colspan=1>27h</td></tr><tr><td rowspan=1 colspan=1>Link Encryption</td><td rowspan=1 colspan=1>Link Encryption Purge All Request</td><td rowspan=1 colspan=1>03h</td><td rowspan=1 colspan=1>28h</td></tr><tr><td rowspan=1 colspan=1>Link Encryption</td><td rowspan=1 colspan=1>Link Encryption Purge All Response</td><td rowspan=1 colspan=1>03h</td><td rowspan=1 colspan=1>29h</td></tr></table>

SMPTE 430-6-2008

## Annex B (Informative) Bibliography

This section contains informative references that provide helpful background information.

[FIPS-140-2] “Security Requirements for Cryptographic Modules” Version 2, May 25, 2001. FIPS-140-2 http://csrc.nist.gov/publications/fips/fips140-2/fips1402.pdf

[PKCS] “Public Key Cryptography Standards (PKCS) #1” RFC 3447 See www.ietf.org/rfc/rfc3447.txt

[Rescorla] Eric Rescorla. SSL and TLS: Designing and Building Secure Systems. Addison Wesley Professional. ISBN 0201615983. October 2000

[CPL] SMPTE 429-7-2006, D-Cinema Packaging — Composition Playlist

SMPTE 377M-2004, Television – Material Exchange Format (MXF) — File Format Specification

# Annex ZZ (informative)

# Corresponding International Standards for which equivalents are not given in the text

At the time of publication of this part of ISO 26430, the following ISO standards are equivalent to the SMPTE standards referenced in the text.

SMPTE 429-7-2006 ISO 26429-7:2008, Digital cinema (D-cinema) packaging — Part 7: Composition playlist

SMPTE 430-1-2006 ISO 26430-1:2008, Digital cinema (D-cinema) operations — Part 1: Key delivery message

SMPTE 430-2-2006 ISO 26430-2:2008, Digital cinema (D-cinema) operations — Part 2: Digital certificate

SMPTE 430-5-2008 ISO 26430-5:2009, Digital cinema (D-cinema) operations — Part 5: Security log event class and constraints