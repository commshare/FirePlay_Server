# FirePlay
NHN NEXT 5학기 프로젝트 FirePlay 리포지토리 입니다. (서버 프로젝트 +  미들웨어 프로젝트)

## Client
Unity 2D, C# (~.Net 2.0) 사용.

## GameServer
C++, IOCP 사용.
다른 서버와의 통신을 위해 MsgPack 사용 (예정)

## LoginServer
C#, ASP.NET, Http 사용.
Unique ID 생성을 위해 twitter의 snowflake 기반 IDGen 사용.

## DBServer
C# 사용.
MongoDB : 아이디 값과 암호화된 비밀번호 값 저장.
Redis : 사용자 권한 토큰 값 저장.
MySQL : 사용자 전적 및 정보 저장.
