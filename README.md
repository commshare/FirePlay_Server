# FirePlay
> NHN NEXT 5학기 프로젝트 FirePlay의 서버 리포지토리 입니다. </br>
> [Client Repository](https://github.com/highfence/FirePlay_Client) </br>
> [플레이 영상](https://www.youtube.com/watch?v=Q3_Uob__OgU&t=25s) </br>

- - -

# 명세
## GameServer
C++, IOCP 사용.  </br>

## LoginServer
C#, ASP.NET, Http 사용.  </br>
Unique ID 생성을 위해 twitter의 snowflake 기반 IDGen 사용. </br>

## DBServer
C# 사용. </br>
MongoDB : 아이디 값과 암호화된 비밀번호 값 저장. </br>
Redis : 사용자 권한 토큰 값 저장. </br>
MySQL : 사용자 전적 및 정보 저장. </br>

- - -

# 구현된 기능
## Network
> DBServer, LoginServer와의 통신 (HappyHttp 라이브러리 이용) </br>

# 고쳐야 될 부분
> Network의 Send부분을 비동기가 아닌 동기로 하고 있음. </br>

# 미구현 기능
> 현재 전적을 저장하지 않음. </br>
> 매치 메이커에 랭킹 시스템을 넣었으면 좋겠음. </br>
> 현재 빠른 매치만 가능하고 Number Match는 진행되지 않음. </br>
> 포트리스 처럼 바람 기능 구현. </br>
