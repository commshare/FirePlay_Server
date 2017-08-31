using System.Collections;
using System.Collections.Generic;

public enum PacketId : short
{
    LoginReq = 21,
    LoginRes = 22,

    FastMatchStartReq = 31,
    FastMatchStartRes = 32,

    NotifyMatchSuccess = 41,
    AckMatchSuccess = 42,

    NotifyGameStart = 101,
    AckGameStart = 102,

    NotifyTurnStart = 111,
    AckTurnStart = 112,

    NotifyEnemyTurnStart = 121,
    AckEnemyTurnStart = 122,

    NotifyMove = 131,
    NotifyEnemyMove = 133,

    NotifyFire = 141,
    NotifyEnemyFire = 142
}

namespace Packet
{
    public class PacketInfo
    {
        public const int maxUserSize = 16;
    }

    public class PacketHeader
    {
        short _id;
        short _bodySize;
    }

    public class PacketBase
    {
        short _error;
    }

    public class LoginReq
    {
        char[] _id = new char[PacketInfo.maxUserSize];
        long _token = 0L;
    }

    public class LoginRes : PacketBase
    {

    }
}