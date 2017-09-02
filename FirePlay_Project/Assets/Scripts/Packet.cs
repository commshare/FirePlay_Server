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
    [System.Serializable]
    public class PacketInfo
    {
        public const int maxUserSize = 16;
    }

    [System.Serializable]
    public class PacketHeader
    {
        public short _id;
        public short _bodySize;
    }

    [System.Serializable]
    public class PacketBase
    {
        public short _error;
    }

    [System.Serializable]
    public class LoginReq
    {
        public char[] _id = new char[PacketInfo.maxUserSize];
        public long _token = 0L;
    }

    [System.Serializable]
    public class LoginRes : PacketBase
    {

    }
}